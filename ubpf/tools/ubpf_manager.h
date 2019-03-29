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


/*
 * Context of a plugin. Contains a pointer to the original version of the arguments and the type of the argument (keeps it invisible in plugin)
 * It also contains a shared_heap shared between the 3 potential bytecodes of this plugin
 */
typedef struct plugin_context {
    void *original_arg;
    int type_arg;
    void *shared_heap;
} plugin_context_t;


/*
 * Definition of a plugin
 * A plugin contains 3 ubpf vm (PRE, REP, POST), a pointer to its context and a pointer to its arguments
 */
typedef struct plugin {
    struct ubpf_vm *vm[3];
    struct plugin_context *plugin_context;
    void *arg;
} plugin_t;

/*
 * All plugin_arg structures start with a pointer to the plugin_context (see ospf_plugins_api.h)
 */
typedef struct general_plugin_arg {
    struct plugin_context *plugin_context;
    void *rest;
} general_plugin_arg_t;

/*
 * Loads an elf file in a ubpf virtual machine. The elf file should but output of clang.
 */
plugin_t *load_elf_file(plugin_t *plugin, const char *code_filename, int pos);

/*
 * Release all the resources
 */
int release_elf(plugin_t *plugin, int pos);

/*
 * Executes the code loaded in the ubpf virtual machine
 */
uint64_t exec_loaded_code(plugin_t *plugin, void *mem, size_t mem_len, int pos);

#endif //FRR_THESIS_UBPF_MANAGER_H
