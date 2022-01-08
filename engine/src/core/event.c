//
// Created by 12132 on 2022/1/8.
//
#include "event.h"
#include "kmemory.h"
#include "container/darray.h"

/// todo used map instead vector and multi thread and priority it
typedef struct registered_event{
    void * listener;
    PFN_on_event callback;
}registered_event;


typedef struct event_code_entry{
    registered_event* events;
}event_code_entry;

#define MAX_MESSAGE_CODES 16384

typedef struct event_system_state{
    event_code_entry registered[MAX_MESSAGE_CODES];
}event_system_state;

static b8 is_initialized = false;
static event_system_state  state;

b8 event_initialize()
{
    if(is_initialized == true) {
        return false;
    }
    is_initialized =false;
    kzero_memory(&state, sizeof(state));

    is_initialized = true;
    return true;
}

void event_shutdown()
{
    for (u16 i = 0; i < MAX_EVENT_CODE; ++i) {
        if(state.registered[i].events != 0) {
            darray_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 event_register(u16 code, void* listener, PFN_on_event on_event)
{
    if(is_initialized ==false) {
        return false;
    }

    if(state.registered[code].events == 0){
        state.registered[code].events = darray_create(registered_event);
    }

    u64 register_count = darray_length(state.registered[code].events);
    for (u64 i =0 ;i <register_count; ++i) {
        if(state.registered[code].events[i].listener == listener){
            /// warn not register same twice
            return false;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    darray_push(state.registered[code].events,event);
    return true;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event on_event)
{
    if(is_initialized ==false) {
        return false;
    }

    if(state.registered[code].events == 0){
        /// warn
        return false;
    }

    u64 register_count = darray_length(state.registered[code].events);
    for (u64 i =0 ;i <register_count; ++i) {
        registered_event e = state.registered[code].events[i];
        if(e.listener == listener && e.callback == on_event) {
            ///
            registered_event popped_event;
            darray_pop_at(state.registered[code].events,i,&popped_event);
            return true;
        }
    }
    return false;
}

b8 event_fire(u16 code, void* sender,event_context context)
{
    if(is_initialized ==false) {
        return false;
    }

    if(state.registered[code].events == 0){
        /// warn
        return false;
    }
    u64 register_count = darray_length(state.registered[code].events);
    for (u64 i =0 ;i <register_count; ++i) {
        registered_event e = state.registered[code].events[i];
        if(e.callback(code,sender,e.listener, context)){
            return true;
        }
    }

    return false;
}


