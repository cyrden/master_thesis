//
// Created by cyril on 30/11/18.
//

/*
 * Helper functions are registered to the eBPF vm on creation to be used as external functions
 */

#include "ospf_plugins_api.h"
#include "../../ospfd/plugins_manager/plugins_manager.h"

/*
 * Struct of the message buffer stored in kernel (used to communicate with monitoring server)
 */
struct mesg_buffer {
    long mesg_type;
    char mesg_text[SIZE_MESG];
} message;

/*
 * This function is used to put messages in the queue.
 * The arguments are the type of the plugin and a void* that contains the data the plugin wants to pass to the server (usually a structure specific to the plugin).
 */
uint64_t send_data(int type, void *data) {

    key_t key;
    int msgid; // message queue identifier
    // ftok to generate unique key
    key = ftok("/etc/frr/daemons", 65);
    if (key == -1) {
        printf("sender key error t \n");
        return 0;
    }

    // msgget creates a message queue and returns identifier of this queue
    msgid = msgget(key, 0);
    if (msgid == -1) {
        printf("sender msgget error \n");
        return 0;
    }

    message.mesg_type = type;
    memcpy((void *) message.mesg_text, data, SIZE_MESG*sizeof(char)); //copy the data to the message
    msgsnd(msgid, &message, sizeof(message), 0);

    return 0;
}

/*
 * Function to test reading OSPF variables from a plugin
 */
int read_int(void *pointer) {
    if(pointer == NULL) return -1;
    int *read = (int *) pointer;
    return *read;
}

/*
 * Function to test writing OSPF variables from a plugin
 */
void set_pointer_toInt(void *pointer, int value) {
    *((int *) pointer) = value;
}

/*
 * Simple getter function that
 */
//int interface_get_speed(struct plugin_context *plugin_context, struct ospf_interface *oi, int *speed) {
int interface_get_speed(struct ospf_interface *oi, int *speed) {
    //if(oi == NULL || plugin_context == NULL) { // check that plugin didn't send null pointer
    if(oi == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    // TODO: This doesn't work and moreover it is very dangerous. We still don't know if the context is ok and we already check in it insertion point value
    /*if(plugin_context != plugins_tab.plugins[plugin_context->insertion_point]->plugin_context) {
        printf("The context is not valid \n");
        return 0;
    }*/
    memcpy((void *) &oi->ifp->speed, (void *) speed, sizeof(int)); // TODO: I guess here I should go in the context as well because it is trusted. The oi pointer has not been checked anywhere, it is just used by the plugins
    return 1;
}