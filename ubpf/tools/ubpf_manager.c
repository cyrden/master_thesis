/* ubpf_manager.c
   Copyright (c) 2018 Quentin De Coninck
*/

#include "ubpf/tools/ubpf_manager.h"
#include "ospf_plugins_api.h"


static void *readfile(const char *path, size_t maxlen, size_t *len);

/*
 * Register external function to the ubpf vm so that we can use them in the plugins
 */
static int register_functions(struct ubpf_vm *vm) {

    /* Generic functions */
    if (ubpf_register(vm, 0x01, "memcpy", memcpy) == -1) return 0;

    /* Sends data to the monitoring server */
    if (ubpf_register(vm, 0x04, "send_data", send_data) == -1) return 0;

    /* Test functions to try manipulating OSPF var in plugins */
    if (ubpf_register(vm, 0x07, "set_pointer_toInt", set_pointer_toInt) == -1) return 0;
    if (ubpf_register(vm, 0x08, "read_int", read_int) == -1) return 0;

    /* Getter functions */
    if (ubpf_register(vm, 0x09, "get_ospf_interface", get_ospf_interface) == -1) return 0;
    if (ubpf_register(vm, 0x11, "get_interface", get_interface) == -1) return 0;

    /* Functions from OSPF */
    if (ubpf_register(vm, 0x10, "ospf_if_name_string", ospf_if_name_string) == -1) return 0;
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

static plugin_t *load_elf(void *code, size_t code_len) {
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
    if (plugin->plugin_context != NULL) {
        free(plugin->plugin_context);
        plugin->plugin_context = NULL;
    }
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
    if(plugin == NULL) return 0;
    if (plugin->vm == NULL) {
        return 0;
    }
    plugin->arg = malloc(mem_len); // because we will put a copy of the arg here
    if(plugin->arg == NULL) {
        fprintf(stderr, "Malloc of plugin -> arg failed");
        return 0;
    }
    memcpy(plugin->arg, mem, mem_len); // Make a copy of mem to give it as argument field to plugin structure
    plugin->plugin_context->original_arg = mem; // the plugin context has a pointer to the original argument. No need to malloc because it is just a pointer.
    ret = ubpf_exec(plugin->vm, plugin->arg, mem_len);
    return ret;
}