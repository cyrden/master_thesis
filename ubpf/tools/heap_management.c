//
// Created by cyril on 03/04/19.
//

/*
 * This code is highly inspired from https://bitbucket.org/qdeconinck/tftp_plugins/src/master/memcpy.c
 *
 * It is used to manage the heap that we give to eBPF codes. The heap is a fixed size memory area that we pass to the eBPF bytecode in its arguments.
 * That way we are sure the user cannot allocate huge memory areas and make the RAM overflow or so on.
 */

#include "heap_management.h"

/* This code is highly inspired from https://github.com/fouady/simple-malloc/blob/master/mem_management.cpp */

/**
 * MEM_BUFFER determines size of RAM
 * METADATA_SIZE is the fixed size of metadata block
 * ALIGNMENT_FACTOR determines the smallest chunk of memory in bytes.
 * MAGIC_NUMBER is used to check if the pointer to be freed is valid.
 */
#define MEM_BUFFER SIZE_EBPF_VM_HEAP
#define METADATA_SIZE (sizeof(meta_data))
#define ALIGNMENT_FACTOR 4
#define MAGIC_NUMBER 0123

/**
 * This structure contains the metadata.
 * Size determines the size of data excuding the size of metadata
 * next block is the pointer to next slot of memory in heap.
 */
typedef struct meta_data {
    unsigned int size;
    unsigned int available;
    struct meta_data *next_block;
    unsigned int magic_number;
} meta_data;

/**
 * Adjusts the requested size so that the allocated space is always a multiple of alighment factor
 */
static unsigned int align_size(unsigned int size) {
    return (size % ALIGNMENT_FACTOR) ? size + ALIGNMENT_FACTOR - (size % ALIGNMENT_FACTOR) : size;
}

/**
 * Home-made implementation of sbrk within a given context_t.
 */
static void *my_sbrk(heap_t *heap, intptr_t increment) {
    if (heap->heap_end + increment - heap->heap_start > MEM_BUFFER) {
        /* Out of memory */
        return (void *) -1;
    }

    heap->heap_end += increment;
    return heap->heap_end;
}

/**
 * Goes through the whole heap to find an empty slot.
 */
static meta_data *find_slot(heap_t *heap, unsigned int size) {
    meta_data *iter = (meta_data*) heap->heap_start;
    while(iter) {
        if (iter->available && iter->size >= size) {
            iter->available = 0;
            return iter;
        }
        iter = iter->next_block;
    }
    return NULL;
}

/**
 * If a free slot can accommodate atleast 1 more (METADATA_SIZE + ALIGNMENT FACTOR)
 * apart from the requested size, then the slot is divided to save space.
 */
static void divide_slot(void *slot, unsigned int size) {
    meta_data *slot_to_divide = (meta_data *) slot;
    meta_data *new_slot= (meta_data*) ((char *) slot_to_divide + METADATA_SIZE + size);

    new_slot->size=slot_to_divide->size - size - METADATA_SIZE;
    new_slot->available = 1;
    new_slot->next_block = slot_to_divide->next_block;
    new_slot->magic_number = MAGIC_NUMBER;

    slot_to_divide->size = size;
    slot_to_divide->next_block = new_slot;
}

/**
 * Extends the heap using ou own sbrk function.
 */
static void *extend(heap_t *heap, unsigned int size) {
    meta_data *new_block = (meta_data*) my_sbrk(heap, 0);
    if ((char*) new_block - (char*) heap->heap_start > MEM_BUFFER) return NULL;
    int *flag = (int *) my_sbrk(heap, size + METADATA_SIZE);
    if (*flag == -1) return NULL;
    new_block->size = size;
    new_block->available = 0;
    new_block->next_block = NULL;
    new_block->magic_number = MAGIC_NUMBER;

    if (heap->heap_last_block) {
        ((meta_data *) heap->heap_last_block)->next_block = new_block;
    }
    heap->heap_last_block = new_block;
    return new_block;
}

/**
 * Returns the metadata from heap corresponding to a data pointer.
 */
static meta_data* get_metadata(void *ptr) {
    return (meta_data *)((char *) ptr - METADATA_SIZE);
}

/**
* Search for big enough free space on heap.
* Split the free space slot if it is too big, else space will be wasted.
* Return the pointer to this slot.
* If no adequately large free slot is available, extend the heap and return the pointer.
*/
void *my_malloc(heap_t *heap, unsigned int size) {
    size = align_size(size);
    void *slot;
    if (heap->heap_start){
        slot = find_slot(heap, size);
        if (slot) {
            if (((meta_data *) slot)->size > size + METADATA_SIZE) {
                divide_slot(slot, size);
            }
        } else {
            slot = extend(heap, size);
        }
    } else {
        heap->heap_start = my_sbrk(heap, 0);
        slot = extend(heap, size);
    }

    if (!slot) { return slot; }

    return ((char *) slot) + METADATA_SIZE;
}

/**
 * Frees the allocated memory. If first checks if the pointer falls
 * between the allocated heap range. It also checks if the pointer
 * to be deleted is actually allocated. this is done by using the
 * magic number. Due to lack of time i haven't worked on fragmentation.
 */
void my_free(heap_t *heap, void *ptr) {
    if (!heap->heap_start) return;
    if (ptr >= heap->heap_start + METADATA_SIZE && ptr < my_sbrk(heap, 0)) {
        meta_data *ptr_metadata = get_metadata(ptr);
        if (ptr_metadata->magic_number == MAGIC_NUMBER) {
            ptr_metadata->available = 1;
        }
    }
}

/**
 * Reallocate the allocated memory to change its size. Three cases are possible.
 * 1) Asking for lower or equal size, or larger size without any block after.
 *    The block is left untouched, we simply increase its size.
 * 2) Asking for larger size, and another block is behind.
 *    We need to request another larger block, then copy the data and finally free it.
 * 3) Asking for larger size, without being able to have free space.
 *    Free the pointer and return NULL.
 * If an invalid pointer is provided, it returns NULL without changing anything.
 */
void *my_realloc(heap_t *heap, void *ptr, unsigned int size) {
    /* If no previous ptr, fast-track to my_malloc */
    if (!ptr) return my_malloc(heap, size);
    /* If the previous ptr is invalid, return NULL */
    if (ptr < heap->heap_start + METADATA_SIZE && ptr >= my_sbrk(heap, 0)) return NULL;
    /* Now take metadata */
    meta_data *ptr_metadata = get_metadata(ptr);
    if (ptr_metadata->magic_number != MAGIC_NUMBER) {
        /* Invalid pointer */
        return NULL;
    }
    /* Case 1a and 1b */
    unsigned int old_size = ptr_metadata->size;
    if (size <= old_size) {
        ptr_metadata->size = size;
        return ptr;
    }

    /* This is clearly not the most optimized way, but it will always work */
    void *new_ptr = my_malloc(heap, size);
    if (!new_ptr) {
        my_free(heap, ptr);
        return NULL;
    }
    memcpy(new_ptr, ptr, old_size);
    my_free(heap, ptr);
    return new_ptr;
}