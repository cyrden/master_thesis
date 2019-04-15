/* ubpf_manager.c
   Copyright (c) 2018 Quentin De Coninck
*/

#include "ubpf/tools/ubpf_manager.h"
#include "ospf_plugins_api.h"
#include "heap_management.h"


static void *readfile(const char *path, size_t maxlen, size_t *len);

/*
 * Register external function to the ubpf vm so that we can use them in the plugins
 */
static int register_functions(struct ubpf_vm *vm) {

    /* Generic functions */
    if (ubpf_register(vm, 0x01, "strcpy", strcpy) == -1) return 0;
    if (ubpf_register(vm, 0x02, "gettimeofday", gettimeofday) == -1) return 0;

    /* Plugins shared_heap management functions */
    if (ubpf_register(vm, 0x20, "shared_heap_malloc", shared_heap_malloc) == -1) return 0;
    if (ubpf_register(vm, 0x21, "shared_heap_free", shared_heap_free) == -1) return 0;
    if (ubpf_register(vm, 0x22, "shared_heap_get", shared_heap_get) == -1) return 0;
    if (ubpf_register(vm, 0x23, "shared_heap_set", shared_heap_set) == -1) return 0;
    if (ubpf_register(vm, 0x24, "my_malloc", my_malloc) == -1) return 0;
    if (ubpf_register(vm, 0x25, "my_free", my_free) == -1) return 0;
    if (ubpf_register(vm, 0x26, "my_realloc", my_realloc) == -1) return 0;


    /* Sends data to the monitoring server */
    if (ubpf_register(vm, 0x04, "send_data", send_data) == -1) return 0;

    /* Test functions to try manipulating OSPF var in plugins */
    if (ubpf_register(vm, 0x07, "set_pointer_toInt", set_pointer_toInt) == -1) return 0;
    if (ubpf_register(vm, 0x08, "read_int", read_int) == -1) return 0;

    /* Getter functions */
    if (ubpf_register(vm, 0x09, "get_ospf_interface", get_ospf_interface) == -1) return 0;
    if (ubpf_register(vm, 0x11, "get_interface", get_interface) == -1) return 0;
    if (ubpf_register(vm, 0x12, "get_ospf_lsa", get_ospf_lsa) == -1) return 0;
    if (ubpf_register(vm, 0x13, "get_lsa_header", get_lsa_header) == -1) return 0;
    if (ubpf_register(vm, 0x14, "get_ospf_area", get_ospf_area) == -1) return 0;

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

static plugin_t *load_elf(plugin_t *plugin, void *code, size_t code_len, int pos) {

    plugin->pluglets[pos] = malloc(sizeof(pluglet_t));
    plugin->pluglets[pos]->pluglet_context = NULL;
    plugin->pluglets[pos]->vm = ubpf_create();
    if (!plugin->pluglets[pos]->vm) {
        fprintf(stderr, "Failed to create VM\n");
        free(plugin);
        return NULL;
    }

    int test = register_functions(plugin->pluglets[pos]->vm);
    zlog_notice("register function pos: %d, code_len: %ld,  return val: %d \n",pos, code_len, test);

    int elf = code_len >= SELFMAG && !memcmp(code, ELFMAG, SELFMAG);

    char *errmsg;
    int rv;
    if (elf) {
        rv = ubpf_load_elf(plugin->pluglets[pos]->vm, code, code_len, &errmsg);
    }else {
        rv = ubpf_load(plugin->pluglets[pos]->vm, code, code_len, &errmsg);
    }

    if (rv < 0) {
        fprintf(stderr, "Failed to load code: %s\n", errmsg);
        free(errmsg);
        ubpf_destroy(plugin->pluglets[pos]->vm);
        free(plugin);
        return NULL;
    }

    free(errmsg);

    return plugin;
}

plugin_t *load_elf_file(plugin_t *plugin, const char *code_filename, int pos) {
    size_t code_len;
    void *code = readfile(code_filename, 1024*1024, &code_len);
    if (code == NULL) {
        return NULL;
    }

    plugin_t *ret = load_elf(plugin, code, code_len, pos);
    free(code);
    return ret;
}

int release_elf(plugin_t *plugin, int pos) {
    if (plugin->pluglets[pos] != NULL) {
        ubpf_destroy(plugin->pluglets[pos]->vm);
        if(plugin->pluglets[pos]->pluglet_context != NULL) {
            free(plugin->pluglets[pos]->pluglet_context);
        }
        free(plugin->pluglets[pos]);
        plugin->pluglets[pos] = NULL;
    }
    return 0;
}

uint64_t exec_loaded_code(plugin_t *plugin, void *mem, size_t mem_len, int pos) {
    uint64_t ret;
    if(plugin == NULL) return 0;
    if (plugin->pluglets[pos] == NULL) {
        return 0;
    }

    plugin->pluglets[pos]->pluglet_context->original_arg = mem; // the plugin context has a pointer to the original argument. No need to malloc because it is just a pointer.
    plugin->pluglets[pos]->pluglet_context->shared_heap = plugin->shared_heap; // TODO: I initialized it to NULL --> doesn't work need to fix
    while(1) {
        if (current_context == NULL) { // Sort of mutex ...
            current_context = plugin->pluglets[pos]->pluglet_context; // Set the current_context to the context of the pluglet we want to execute
            break;
        }
        sleep(1);
    }
    ret = ubpf_exec(plugin->pluglets[pos]->vm, mem, mem_len);
    current_context = NULL; // release mutex
    return ret;
}