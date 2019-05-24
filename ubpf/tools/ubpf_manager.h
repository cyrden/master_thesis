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

#define SIZE_PLUGIN_HEAP 64000
#define MAX_NBR_PLUGLETS 10

struct plugin;

/* Structure that represents a heap */
typedef struct heap {
    char mem[SIZE_PLUGIN_HEAP];
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
    struct plugin *parent_plugin;
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
    pluglet_t *pluglets_PRE[MAX_NBR_PLUGLETS];
    pluglet_t *pluglet_REP;
    pluglet_t *pluglets_POST[MAX_NBR_PLUGLETS];
    heap_t *heap;
    void *arguments;
    int type_arg;
} plugin_t;

/*
 * Loads an elf file in a ubpf virtual machine (pos specifies which pluglet it is). The elf file should be BPF bytecode
 */
pluglet_t *load_elf_file(pluglet_t *pluglet, const char *code_filename);

/*
 * Release all the resources of a pluglet
 */
int release_elf(pluglet_t *pluglet);

/*
 * Executes the code loaded in a pluglet
 */
uint64_t exec_loaded_code(pluglet_t *pluglet, void *mem, size_t mem_len);

uint64_t compile_and_exec_jit(pluglet_t *pluglet, void *mem, size_t mem_len);

#endif //FRR_THESIS_UBPF_MANAGER_H
