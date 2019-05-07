//
// Created by cyril on 03/04/19.
//

#ifndef MASTER_THESIS_HEAP_MANAGEMENT_H
#define MASTER_THESIS_HEAP_MANAGEMENT_H

#include "ospf_plugins_api.h"


void *plugin_malloc(unsigned int size);
void plugin_free(void *ptr);
void *plugin_realloc(void *ptr, unsigned int size);
void *plugin_malloc_with_id(unsigned int id, unsigned int size);
void *plugin_get_memory_with_id(unsigned int id);

#endif //MASTER_THESIS_HEAP_MANAGEMENT_H
