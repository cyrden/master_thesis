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
    if (ubpf_register(vm, 0x0a, "plugin_ntohl", plugin_ntohl) == -1) return 0;
    if (ubpf_register(vm, 0x0b, "plugin_ntohs", plugin_ntohs) == -1) return 0;
    if (ubpf_register(vm, 0x0c, "print_helper", print_helper) == -1) return 0;
    if (ubpf_register(vm, 0x0d, "lsa_head_dump", lsa_head_dump) == -1) return 0;

    /* Sends data to the monitoring server */
    if (ubpf_register(vm, 0x03, "send_data", send_data) == -1) return 0;

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
    if (ubpf_register(vm, 0x9, "get_candidate", get_candidate) == -1) return 0;

    /* Setter functions */
    if (ubpf_register(vm, 0x30, "set_ospf_area_transit", set_ospf_area_transit) == -1) return 0;
    if (ubpf_register(vm, 0x31, "set_ospf_interface", set_ospf_interface) == -1) return 0;


    /* Plugins heap management functions */
    if (ubpf_register(vm, 0x24, "plugin_malloc", plugin_malloc) == -1) return 0;
    if (ubpf_register(vm, 0x25, "plugin_free", plugin_free) == -1) return 0;
    if (ubpf_register(vm, 0x26, "plugin_realloc", plugin_realloc) == -1) return 0;
    if (ubpf_register(vm, 0x27, "my_memcpy", my_memcpy) == -1) return 0;
    if (ubpf_register(vm, 0x28, "plugin_malloc_with_id", plugin_malloc_with_id) == -1) return 0;
    if (ubpf_register(vm, 0x29, "plugin_get_memory_with_id", plugin_get_memory_with_id) == -1) return 0;


    /* Functions from OSPF */
    if (ubpf_register(vm, 0x5, "my_ospf_flood_through_area", my_ospf_flood_through_area) == -1) return 0;
    if (ubpf_register(vm, 0x6, "ospf_if_name_string", ospf_if_name_string) == -1) return 0;
    if (ubpf_register(vm, 0x7, "my_ospf_lsa_install", my_ospf_lsa_install) == -1) return 0;
    if (ubpf_register(vm, 0x8, "my_get_lsah", my_get_lsah) == -1) return 0; // TODO: this function is just help to print
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

static pluglet_t *load_elf(pluglet_t *pluglet, void *code, size_t code_len) {

    if(pluglet == NULL) {
        fprintf(stderr, "NULL pluglet in load elf function \n");
        return NULL;
    }
    pluglet->pluglet_context = NULL;
    pluglet->vm = ubpf_create();
    if (!pluglet->vm) {
        fprintf(stderr, "Failed to create VM\n");
        free(pluglet);
        return NULL;
    }

    register_functions(pluglet->vm);

    int elf = code_len >= SELFMAG && !memcmp(code, ELFMAG, SELFMAG);

    char *errmsg;
    int rv;
    if (elf) {
        rv = ubpf_load_elf(pluglet->vm, code, code_len, &errmsg);
    }else {
        rv = ubpf_load(pluglet->vm, code, code_len, &errmsg);
    }

    if (rv < 0) {
        fprintf(stderr, "Failed to load code: %s\n", errmsg);
        free(errmsg);
        ubpf_destroy(pluglet->vm);
        free(pluglet);
        return NULL;
    }

    free(errmsg);

    return pluglet;
}

pluglet_t *load_elf_file(pluglet_t *pluglet, const char *code_filename) {
    size_t code_len;
    void *code = readfile(code_filename, 1024*1024, &code_len);
    if (code == NULL) {
        return NULL;
    }

    pluglet_t *ret = load_elf(pluglet, code, code_len);
    free(code);
    return ret;
}

int release_elf(pluglet_t *pluglet) {
    if (pluglet != NULL) {
        ubpf_destroy(pluglet->vm);
        if(pluglet->pluglet_context != NULL) {
            free(pluglet->pluglet_context);
        }
        free(pluglet);
        pluglet = NULL;
    }
    return 1;
}

uint64_t exec_loaded_code(pluglet_t *pluglet, void *mem, size_t mem_len) {
    pthread_mutex_lock(&lock_current_context); // Lock current context

    uint64_t ret;
    if (pluglet == NULL) {
        fprintf(stderr, "NULL pluglet in exec_loaded_code \n");
        return 0;
    }

    pluglet->pluglet_context->original_arg = mem; // the plugin context has a pointer to the original argument. No need to malloc because it is just a pointer.
    pluglet->pluglet_context->type_arg = pluglet->pluglet_context->parent_plugin->type_arg;

    current_context = pluglet->pluglet_context; // Set the current_context to the context of the pluglet we want to execute

    ret = ubpf_exec(pluglet->vm, mem, mem_len);
    /* Once the code has been executed by the UBPF VM, we reset the context of the pluglet */
    pluglet->pluglet_context->original_arg = NULL;
    pluglet->pluglet_context->type_arg = -1;
    pluglet->pluglet_context->heap = NULL;
    current_context = NULL;

    pthread_mutex_unlock(&lock_current_context); // unlock current context

    return ret;
}