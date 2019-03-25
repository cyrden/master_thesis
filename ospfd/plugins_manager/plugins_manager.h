//
// Created by cyril on 15/01/19.
//

#ifndef PROJECT_PLUGINS_MANAGER_H
#define PROJECT_PLUGINS_MANAGER_H

#include "../../ubpf/tools/ubpf_manager.h"

#define MAX_NBR_PLUGINS 30 // Defines the maximum number of plugins that can be loaded (=size of the tab)
#define SIZE_MESG 300

typedef struct plugins_tab {
    plugin_t *plugins[MAX_NBR_PLUGINS];
} plugins_tab_t;

typedef struct contexts_tab {
    plugin_context_t *contexts[MAX_NBR_PLUGINS];
} contexts_tab_t;

int contexts_tab_init(contexts_tab_t *tab);

int plugins_tab_init(plugins_tab_t *tab);

void *plugins_manager(void *tab);

void release_all_plugins(void);

#endif //PROJECT_PLUGINS_MANAGER_H
