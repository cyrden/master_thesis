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
 * Definition of a plugin (for the moment just encapsulate an ubpf vm, but I will probably add some more fields when needed)
 */
typedef struct plugin {
    struct ubpf_vm *vm;
} plugin_t;

/*
 * Function used by load_elf_file
 */
plugin_t *load_elf(void *code, size_t code_len);

/*
 * Loads an elf file in a ubpf virtual machine. The elf file should but output of clang.
 */
plugin_t *load_elf_file(const char *code_filename);

/*
 * Release all the ressources
 */
int release_elf(plugin_t *plugin);

/*
 * Executes the code loaded in the ubpf virtual machine
 */
uint64_t exec_loaded_code(plugin_t *plugin, void *mem, size_t mem_len);

#endif //FRR_THESIS_UBPF_MANAGER_H
