#ifndef FRR_THESIS_UBPF_MANAGER_H
#define FRR_THESIS_UBPF_MANAGER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <elf.h>
#include "ubpf/vm/inc/ubpf.h"
#include "ubpf/vm/ubpf_int.h"

#define SIZE_EBPF_VM_HEAP 16000
#define MAX_SIZE_SHARED_HEAP 1000 // Stack is ~2000 and this has to be copied on it

struct plugin;

/* Structure that represents a heap */
typedef struct heap {
    char mem[SIZE_EBPF_VM_HEAP];
    void *heap_start;
    void *heap_end;
    void *heap_last_block;
} heap_t;

/*
 * Context of a pluglet. Contains a pointer to the original version of the arguments, the type of the argument given to the pluglet,
 * a pointer to its heap (given as argument so that eBPF code can access it) , and a pointer to the parent plugin (useful for the shared heap).
 */
typedef struct pluglet_context {
    void *original_arg;
    int type_arg;
    heap_t *heap;
    struct plugin *parent_plugin; // This is useful because we need to share the shared_heap among pluglets that belong to the same plugin
} pluglet_context_t;

/*
 * Represents a pluglet. A pluglet is an ubpf vm (running eBPF bytecode) and its context.
 */
typedef struct pluglet {
    struct ubpf_vm *vm;
    pluglet_context_t *pluglet_context;
} pluglet_t;


/*
 * Definition of a plugin
 * A plugin contains 3 pluglets (PRE, REP, POST) and a shared heap (memory shared between all pluglets of a plugin)
 */
typedef struct plugin {
    pluglet_t *pluglets[3];
    void *shared_heap;
} plugin_t;

/*
 * Loads an elf file in a ubpf virtual machine (pos specifies which pluglet it is). The elf file should be BPF bytecode
 */
plugin_t *load_elf_file(plugin_t *plugin, const char *code_filename, int pos);

/*
 * Release all the resources of a pluglet
 */
int release_elf(plugin_t *plugin, int pos);

/*
 * Executes the code loaded in a pluglet
 */
uint64_t exec_loaded_code(plugin_t *plugin, void *mem, size_t mem_len, int pos);

#endif //FRR_THESIS_UBPF_MANAGER_H
