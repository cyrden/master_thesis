//
// Created by cyril on 03/04/19.
//

#ifndef MASTER_THESIS_HEAP_MANAGEMENT_H
#define MASTER_THESIS_HEAP_MANAGEMENT_H

#include "ospf_plugins_api.h"


void *my_malloc(heap_t *heap, unsigned int size);
void my_free(heap_t *heap, void *ptr);
void *my_realloc(heap_t *heap, void *ptr, unsigned int size);

#endif //MASTER_THESIS_HEAP_MANAGEMENT_H
