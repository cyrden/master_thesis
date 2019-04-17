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
static void *my_sbrk(intptr_t increment) {
    if (current_context->heap->heap_end + increment - current_context->heap->heap_start > MEM_BUFFER) {
        /* Out of memory */
        return (void *) -1;
    }

    current_context->heap->heap_end += increment;
    return current_context->heap->heap_end;
}

/**
 * Goes through the whole heap to find an empty slot.
 */
static meta_data *find_slot(unsigned int size) {
    meta_data *iter = (meta_data*) current_context->heap->heap_start;
    if(current_context->heap->heap_start == current_context->heap->heap_end) return NULL; // otherwise sometime it finds a block but the heap is empty ...
    if(my_sbrk(0) - current_context->heap->heap_start < size) return NULL; // One more guard: if heap is smaller than size -> impossible to find a slot ...
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
static void *extend(unsigned int size) {
    meta_data *new_block = (meta_data*) my_sbrk(0);
    if ((char*) new_block - (char*) current_context->heap->heap_start > MEM_BUFFER) { // No more memory
        return NULL;
    }
    int *flag = (int *) my_sbrk(size + METADATA_SIZE);
    if (*flag == -1) return NULL;
    new_block->size = size;
    new_block->available = 0;
    new_block->next_block = NULL;
    new_block->magic_number = MAGIC_NUMBER;

    if (current_context->heap->heap_last_block) {
        ((meta_data *) current_context->heap->heap_last_block)->next_block = new_block;
    }
    current_context->heap->heap_last_block = new_block;
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
void *my_malloc(unsigned int size) {
    size = align_size(size);
    void *slot;
    if (current_context->heap->heap_start){
        //zlog_notice("Heap starts at: %p and ends at: %p", current_context->heap->heap_start, current_context->heap->heap_end);
        //zlog_notice("Context has arg of type: %d ", current_context->type_arg);
        slot = find_slot(size);
        if (slot) {
            //zlog_notice("Slot found");
            if (((meta_data *) slot)->size > size + METADATA_SIZE) {
                divide_slot(slot, size);
            }
        } else {
            //zlog_notice("Slot not found -> extend");
            slot = extend(size);
        }
    } else {
        current_context->heap->heap_start = my_sbrk(0);
        //zlog_notice("Heap starts at: %p", current_context->heap->heap_start);
        slot = extend(size);
    }

    if (!slot) {
        //zlog_notice("MALLOC FAILED");
        return slot;
    }
    //zlog_notice("Memory assigned from %p to %p (size = %u)", slot, (void *)((char *) slot + METADATA_SIZE + ((meta_data *) slot)->size), size);
    //zlog_notice("Memory ends at: %p", my_sbrk(0));
    //zlog_notice("Size of heap so far: 0x%lx", (unsigned long) ((char *) my_sbrk(0) - (char *) current_context->heap->heap_start));
    return ((char *) slot) + METADATA_SIZE;
}

/**
 * Frees the allocated memory. If first checks if the pointer falls
 * between the allocated heap range. It also checks if the pointer
 * to be deleted is actually allocated. this is done by using the
 * magic number. Due to lack of time i haven't worked on fragmentation.
 */
void my_free(void *ptr) {
    if (!current_context->heap->heap_start) return;
    if (ptr >= current_context->heap->heap_start + METADATA_SIZE && ptr < my_sbrk(0)) {
        meta_data *ptr_metadata = get_metadata(ptr);
        if (ptr_metadata->magic_number == MAGIC_NUMBER) {
            ptr_metadata->available = 1;
            //zlog_notice("Memory freed at: %p", ptr_metadata);
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
void *my_realloc(void *ptr, unsigned int size) {
    /* If no previous ptr, fast-track to my_malloc */
    if (!ptr) return my_malloc(size);
    /* If the previous ptr is invalid, return NULL */
    if (ptr < current_context->heap->heap_start + METADATA_SIZE && ptr >= my_sbrk(0)) return NULL;
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
    void *new_ptr = my_malloc(size);
    if (!new_ptr) {
        my_free(ptr);
        return NULL;
    }
    memcpy(new_ptr, ptr, old_size);
    my_free(ptr);
    return new_ptr;
}