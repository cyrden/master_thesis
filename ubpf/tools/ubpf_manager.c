/* ubpf_manager.c
   Copyright (c) 2018 Quentin De Coninck
*/

#include "ubpf/tools/ubpf_manager.h"
#include "ospf_plugins_api.h"
#include "heap_management.h"
#include "my_memcpy.h"


static void *readfile(const char *path, size_t maxlen, size_t *len);

/*
 * Register external function to the ubpf vm so that we can use them in the plugins
 */
static int register_functions(struct ubpf_vm *vm) {

    /* Generic functions */
    //if (ubpf_register(vm, 0x0e, "my_print", my_print) == -1) return 0;
    if (ubpf_register(vm, 0x01, "strcpy", strcpy) == -1) return 0;
    if (ubpf_register(vm, 0x02, "gettimeofday", gettimeofday) == -1) return 0;
    if (ubpf_register(vm, 0x0a, "my_ntohl", my_ntohl) == -1) return 0;
    if (ubpf_register(vm, 0x0b, "my_ntohs", my_ntohs) == -1) return 0;
    if (ubpf_register(vm, 0x0c, "print_helper", print_helper) == -1) return 0;
    if (ubpf_register(vm, 0x0d, "lsa_head_dump", lsa_head_dump) == -1) return 0;

    /* Sends data to the monitoring server */
    if (ubpf_register(vm, 0x03, "send_data", send_data) == -1) return 0;

    /* Test functions to try manipulating OSPF var in plugins */
    //if (ubpf_register(vm, 0x04, "set_pointer_toInt", set_pointer_toInt) == -1) return 0;
    //if (ubpf_register(vm, 0x05, "read_int", read_int) == -1) return 0;

    /* Getter functions */
    if (ubpf_register(vm, 0x10, "get_ospf_interface", get_ospf_interface) == -1) return 0;
    if (ubpf_register(vm, 0x11, "get_interface", get_interface) == -1) return 0;
    if (ubpf_register(vm, 0x12, "get_ospf_lsa", get_ospf_lsa) == -1) return 0;
    if (ubpf_register(vm, 0x13, "get_lsa_header", get_lsa_header) == -1) return 0;
    if (ubpf_register(vm, 0x14, "get_ospf_area", get_ospf_area) == -1) return 0;
    if (ubpf_register(vm, 0x15, "get_ospf", get_ospf) == -1) return 0;
    if (ubpf_register(vm, 0x16, "get_vertex", get_vertex) == -1) return 0;
    if (ubpf_register(vm, 0x17, "get_lsa_with_length", get_lsa_with_length) == -1) return 0;
    if (ubpf_register(vm, 0x18, "get_ospf_interface_list", get_ospf_interface_list) == -1) return 0;

    /* Setter functions */
    if (ubpf_register(vm, 0x30, "set_ospf_area_transit", set_ospf_area_transit) == -1) return 0;
    if (ubpf_register(vm, 0x31, "set_ospf_interface", set_ospf_interface) == -1) return 0;


    /* Plugins shared_heap management functions */
    if (ubpf_register(vm, 0x20, "shared_heap_malloc", shared_heap_malloc) == -1) return 0;
    if (ubpf_register(vm, 0x21, "shared_heap_free", shared_heap_free) == -1) return 0;
    if (ubpf_register(vm, 0x22, "shared_heap_get", shared_heap_get) == -1) return 0;
    if (ubpf_register(vm, 0x23, "shared_heap_set", shared_heap_set) == -1) return 0;
    if (ubpf_register(vm, 0x24, "my_malloc", my_malloc) == -1) return 0;
    if (ubpf_register(vm, 0x25, "my_free", my_free) == -1) return 0;
    if (ubpf_register(vm, 0x26, "my_realloc", my_realloc) == -1) return 0;
    if (ubpf_register(vm, 0x27, "my_memcpy", my_memcpy) == -1) return 0;

    /* Functions from OSPF */
    if (ubpf_register(vm, 0x5, "my_ospf_flood_through_area", my_ospf_flood_through_area) == -1) return 0;
    if (ubpf_register(vm, 0x6, "ospf_if_name_string", ospf_if_name_string) == -1) return 0;
    if (ubpf_register(vm, 0x7, "my_ospf_lsa_install", my_ospf_lsa_install) == -1) return 0;
    if (ubpf_register(vm, 0x9, "my_get_lsah", my_get_lsah) == -1) return 0; // TODO: this function is just help to print
    if (ubpf_register(vm, 0x19, "trickle_up", trickle_up) == -1) return 0;
    if (ubpf_register(vm, 0x1a, "ospf_lsa_has_link", ospf_lsa_has_link) == -1) return 0;
    if (ubpf_register(vm, 0x1b, "ospf_lsa_lookup_by_id", ospf_lsa_lookup_by_id) == -1) return 0;
    if (ubpf_register(vm, 0x1c, "ospf_lsa_lookup", ospf_lsa_lookup) == -1) return 0;
    if (ubpf_register(vm, 0x1d, "my_ospf_nexthop_calculation", my_ospf_nexthop_calculation) == -1) return 0;
    if (ubpf_register(vm, 0x1e, "pqueue_enqueue", pqueue_enqueue) == -1) return 0;
    if (ubpf_register(vm, 0x1f, "ospf_vertex_new", ospf_vertex_new) == -1) return 0;
    if (ubpf_register(vm, 0x2a, "my_lsa_link_broadcast_set", my_lsa_link_broadcast_set) == -1) return 0;
    if (ubpf_register(vm, 0x2b, "ospf_my_lsa_new_and_data", ospf_my_lsa_new_and_data) == -1) return 0;



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

    register_functions(plugin->pluglets[pos]->vm);

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
    pthread_mutex_lock(&lock_current_context);

    uint64_t ret;
    if(plugin == NULL) return 0;
    if (plugin->pluglets[pos] == NULL) {
        return 0;
    }

    plugin->pluglets[pos]->pluglet_context->original_arg = mem; // the plugin context has a pointer to the original argument. No need to malloc because it is just a pointer.

    current_context = plugin->pluglets[pos]->pluglet_context; // Set the current_context to the context of the pluglet we want to execute

    ret = ubpf_exec(plugin->pluglets[pos]->vm, mem, mem_len);
    plugin->pluglets[pos]->pluglet_context->original_arg = NULL;
    plugin->pluglets[pos]->pluglet_context->type_arg = -1;
    plugin->pluglets[pos]->pluglet_context->heap = NULL;
    current_context = NULL;

    pthread_mutex_unlock(&lock_current_context);

    return ret;
}