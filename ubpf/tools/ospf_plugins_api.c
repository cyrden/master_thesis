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
 * Simple getter function that sets the number of hello out and interface speed of the interface in the structure s.
 */
int interface_get_count_speed(struct plugin_context *plugin_context, struct hello_struct *s) {
    if(plugin_context == NULL) { // check that plugin didn't send null pointer
        printf("NULL pointer \n");
        return 0;
    }
    // TODO: This check is hardcoded. What I need to do: store all context pointers somewhere and check here if the context pointer we received is ok.
    if(plugin_context != plugins_tab.plugins[4]->plugin_context) {
        printf("The context is not valid \n");
        return 0;
    }
    struct arg_plugin_hello_send *arg = (struct arg_plugin_hello_send *) plugin_context->original_arg;
    struct ospf_interface *oi = arg->oi;
    memcpy((void *) &s->itf_speed, (void *) &oi->ifp->speed, sizeof(int)); // TODO: I guess here I should go in the context as well because it is trusted. The oi pointer has not been checked anywhere, it is just used by the plugins
    memcpy((void *) &s->hello_count, (void *) &oi->hello_out, sizeof(int));
    return 1;
}