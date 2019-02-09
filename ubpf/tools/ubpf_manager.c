/* ubpf_manager.c
   Copyright (c) 2018 Quentin De Coninck
*/

#include "ubpf/tools/ubpf_manager.h"

static void *readfile(const char *path, size_t maxlen, size_t *len);

/*
 * Register external function to the ubpf vm so that we can use them in the plugins
 */
static int register_functions(struct ubpf_vm *vm) {
    if (ubpf_register(vm, 0x00, "printf", printf) == -1) return 0;
    if (ubpf_register(vm, 0x01, "strcmp_ext", strcmp) == -1) return 0;
    if (ubpf_register(vm, 0x02, "gettimeofday", gettimeofday) == -1) return 0;
    if (ubpf_register(vm, 0x03, "inc_hello_count", inc_hello_count) == -1) return 0;
    if (ubpf_register(vm, 0x04, "send_data", send_data) == -1) return 0;
    if (ubpf_register(vm, 0x05, "memcpy", memcpy) == -1) return 0;
    return 1;
}

static void *readfile(const char *path, size_t maxlen, size_t *len)
{
    FILE *file;
    if (!strcmp(path, "-")) {
        file = fdopen(STDIN_FILENO, "r");
    } else {
        file = fopen(path, "r");
    }

    if (file == NULL) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        return NULL;
    }

    void *data = calloc(maxlen, 1);
    size_t offset = 0;
    size_t rv;
    while ((rv = fread(data+offset, 1, maxlen-offset, file)) > 0) {
        offset += rv;
    }

    if (ferror(file)) {
        fprintf(stderr, "Failed to read %s: %s\n", path, strerror(errno));
        fclose(file);
        free(data);
        return NULL;
    }

    if (!feof(file)) {
        fprintf(stderr, "Failed to read %s because it is too large (max %u bytes)\n",
                path, (unsigned)maxlen);
        fclose(file);
        free(data);
        return NULL;
    }

    fclose(file);
    if (len) {
        *len = offset;
    }
    return data;
}

plugin_t *load_elf(void *code, size_t code_len) {
    plugin_t *plugin = (plugin_t *)malloc(sizeof(plugin_t));
    if (!plugin) {
        return NULL;
    }

    plugin->vm = ubpf_create();
    if (!plugin->vm) {
        fprintf(stderr, "Failed to create VM\n");
        free(plugin);
        return NULL;
    }

    register_functions(plugin->vm);

    int elf = code_len >= SELFMAG && !memcmp(code, ELFMAG, SELFMAG);

    char *errmsg;
    int rv;
    if (elf) {
        rv = ubpf_load_elf(plugin->vm, code, code_len, &errmsg);
    }else {
        rv = ubpf_load(plugin->vm, code, code_len, &errmsg);
    }

    if (rv < 0) {
        fprintf(stderr, "Failed to load code: %s\n", errmsg);
        free(errmsg);
        ubpf_destroy(plugin->vm);
        free(plugin);
        return NULL;
    }

    free(errmsg);

    return plugin;
}

plugin_t *load_elf_file(const char *code_filename) {
    size_t code_len;
    void *code = readfile(code_filename, 1024*1024, &code_len);
    if (code == NULL) {
        return NULL;
    }

    plugin_t *ret = load_elf(code, code_len);
    free(code);
    return ret;
}

int release_elf(plugin_t *plugin) {
    if (plugin->vm != NULL) {
        ubpf_destroy(plugin->vm);
        plugin->vm = NULL;
        free(plugin);
        plugin = NULL;
    }
    return 0;
}

uint64_t exec_loaded_code(plugin_t *plugin, void *mem, size_t mem_len) {
    uint64_t ret;
    if (plugin->vm == NULL) {
        return EXIT_FAILURE;
    }
    ret = ubpf_exec(plugin->vm, mem, mem_len);
    return ret;
}