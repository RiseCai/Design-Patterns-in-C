/**
 * viper.c - VIPER Architecture Implementation
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * VIPER Architecture implementation using 5 types of state machines.
 */

#include "viper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Include component-specific headers for structure definitions */
#include "../include/viper_view.h"
#include "../include/viper_presenter.h"
#include "../include/viper_interactor.h"
#include "../include/viper_entity.h"
#include "../include/viper_router.h"

/* Include state machine headers */
#include "../../state_machine_extended/src/mealy_machine.h"
#include "../../state_machine_extended/src/parallel_fsm.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include "../../state_machine_extended/src/efsm_protocol.h"
#include "../../state_machine_extended/src/acceptor_regex.h"

/* VIPER View Implementation */
static struct viper_view *viper_view_create_internal(void)
{
    struct viper_view *view = (struct viper_view *)malloc(sizeof(struct viper_view));
    if (!view) return NULL;
    
    memset(view, 0, sizeof(struct viper_view));
    
    /* Allocate and initialize Mealy FSM */
    view->fsm = (struct mealy_machine *)malloc(sizeof(struct mealy_machine));
    if (!view->fsm) {
        free(view);
        return NULL;
    }
    
    /* Note: Actual state initialization would be done by the application */
    /* For now, just initialize with NULL state */
    memset(view->fsm, 0, sizeof(struct mealy_machine));
    
    return view;
}

static void viper_view_destroy_internal(struct viper_view *view)
{
    if (view) {
        if (view->fsm) {
            free(view->fsm);
        }
        free(view);
    }
}

/* VIPER Presenter Implementation */
static struct viper_presenter *viper_presenter_create_internal(void)
{
    struct viper_presenter *presenter = (struct viper_presenter *)malloc(sizeof(struct viper_presenter));
    if (!presenter) return NULL;
    
    memset(presenter, 0, sizeof(struct viper_presenter));
    
    /* Allocate and initialize Parallel FSM */
    presenter->fsm = (struct parallel_machine *)malloc(sizeof(struct parallel_machine));
    if (!presenter->fsm) {
        free(presenter);
        return NULL;
    }
    
    /* Note: Actual parallel FSM initialization would be done by the application */
    memset(presenter->fsm, 0, sizeof(struct parallel_machine));
    
    return presenter;
}

static void viper_presenter_destroy_internal(struct viper_presenter *presenter)
{
    if (presenter) {
        if (presenter->fsm) {
            free(presenter->fsm);
        }
        free(presenter);
    }
}

/* VIPER Interactor Implementation */
static struct viper_interactor *viper_interactor_create_internal(void)
{
    /* Use the proper initialization function from viper_interactor.c */
    return viper_interactor_init("VIPER_Interactor", 1, NULL);
}

static void viper_interactor_destroy_internal(struct viper_interactor *interactor)
{
    if (interactor) {
        viper_interactor_destroy(interactor);
    }
}

/* VIPER Entity Implementation */
static struct viper_entity *viper_entity_create_internal(void)
{
    struct viper_entity *entity = (struct viper_entity *)malloc(sizeof(struct viper_entity));
    if (!entity) return NULL;
    
    memset(entity, 0, sizeof(struct viper_entity));
    
    /* Allocate and initialize EFSM */
    entity->fsm = (struct efsm_processor *)malloc(sizeof(struct efsm_processor));
    if (!entity->fsm) {
        free(entity);
        return NULL;
    }
    
    /* Note: Actual EFSM initialization would be done by the application */
    memset(entity->fsm, 0, sizeof(struct efsm_processor));
    
    return entity;
}

static void viper_entity_destroy_internal(struct viper_entity *entity)
{
    if (entity) {
        if (entity->fsm) {
            free(entity->fsm);
        }
        free(entity);
    }
}

/* VIPER Router Implementation */
static struct viper_router *viper_router_create_internal(void)
{
    struct viper_router *router = (struct viper_router *)malloc(sizeof(struct viper_router));
    if (!router) return NULL;
    
    memset(router, 0, sizeof(struct viper_router));
    
    /* Allocate and initialize Acceptor/Regex FSM */
    router->fsm = (struct acceptor_machine *)malloc(sizeof(struct acceptor_machine));
    if (!router->fsm) {
        free(router);
        return NULL;
    }
    
    /* Note: Actual acceptor FSM initialization would be done by the application */
    memset(router->fsm, 0, sizeof(struct acceptor_machine));
    
    return router;
}

static void viper_router_destroy_internal(struct viper_router *router)
{
    if (router) {
        if (router->fsm) {
            free(router->fsm);
        }
        free(router);
    }
}

/* VIPER Module Implementation */
struct viper_module *viper_module_init(const struct viper_config *config)
{
    if (!config) return NULL;
    
    struct viper_module *module = (struct viper_module *)malloc(sizeof(struct viper_module));
    if (!module) return NULL;
    
    memset(module, 0, sizeof(struct viper_module));
    
    /* Copy module name */
    if (config->module_name[0] != '\0') {
        strncpy(module->module_name, config->module_name, sizeof(module->module_name) - 1);
        module->module_name[sizeof(module->module_name) - 1] = '\0';
    } else {
        strcpy(module->module_name, "VIPER_Module");
    }
    
    /* Create components based on configuration */
    if (config->enable_view) {
        module->view = (struct viper_view *)viper_view_create_internal();
    }
    
    if (config->enable_presenter) {
        module->presenter = (struct viper_presenter *)viper_presenter_create_internal();
    }
    
    if (config->enable_interactor) {
        module->interactor = (struct viper_interactor *)viper_interactor_create_internal();
    }
    
    if (config->enable_entity) {
        module->entity = (struct viper_entity *)viper_entity_create_internal();
    }
    
    if (config->enable_router) {
        module->router = (struct viper_router *)viper_router_create_internal();
    }
    
    /* Generate a simple module ID */
    module->module_id = 1000 + (rand() % 9000);
    
    return module;
}

void viper_module_destroy(struct viper_module *module)
{
    if (!module) return;
    
    /* Destroy components */
    if (module->view) {
        viper_view_destroy_internal((struct viper_view *)module->view);
    }
    
    if (module->presenter) {
        viper_presenter_destroy_internal((struct viper_presenter *)module->presenter);
    }
    
    if (module->interactor) {
        viper_interactor_destroy_internal((struct viper_interactor *)module->interactor);
    }
    
    if (module->entity) {
        viper_entity_destroy_internal((struct viper_entity *)module->entity);
    }
    
    if (module->router) {
        viper_router_destroy_internal((struct viper_router *)module->router);
    }
    
    free(module);
}

int viper_module_process_event(struct viper_module *module, 
                               const struct viper_event *event)
{
    if (!module || !event) return -1;
    
    /* Process event based on target component */
    switch (event->target_component) {
        case VIPER_COMPONENT_VIEW:
            if (module->view) {
                /* Map VIPER event to view event */
                viper_view_event_t view_event;
                switch (event->type) {
                    case VIPER_EVENT_VIEW_ACTION:
                        view_event = VIEW_EVENT_USER_INPUT;
                        break;
                    case VIPER_EVENT_DATA_UPDATE:
                        view_event = VIEW_EVENT_DATA_RECEIVED;
                        break;
                    case VIPER_EVENT_ERROR:
                        view_event = VIEW_EVENT_ERROR_OCCURRED;
                        break;
                    case VIPER_EVENT_NAVIGATION:
                        view_event = VIEW_EVENT_NAVIGATE;
                        break;
                    case VIPER_EVENT_PRESENTATION:
                        view_event = VIEW_EVENT_REFRESH;
                        break;
                    default:
                        /* Unsupported event type for view */
                        return -1;
                }
                return viper_view_process_event(module->view, view_event, event->data);
            }
            break;
            
        case VIPER_COMPONENT_PRESENTER:
            if (module->presenter) {
                /* Map VIPER event to presenter event */
                viper_presenter_event_t presenter_event;
                switch (event->type) {
                    case VIPER_EVENT_VIEW_ACTION:
                        presenter_event = PRESENTER_EVENT_VIEW_UPDATE;
                        break;
                    case VIPER_EVENT_DATA_UPDATE:
                        presenter_event = PRESENTER_EVENT_DATA_READY;
                        break;
                    case VIPER_EVENT_ERROR:
                        presenter_event = PRESENTER_EVENT_ERROR;
                        break;
                    case VIPER_EVENT_NAVIGATION:
                        presenter_event = PRESENTER_EVENT_NAVIGATION_REQUEST;
                        break;
                    case VIPER_EVENT_BUSINESS_LOGIC:
                        presenter_event = PRESENTER_EVENT_BUSINESS_LOGIC_COMPLETE;
                        break;
                    case VIPER_EVENT_PRESENTATION:
                        presenter_event = PRESENTER_EVENT_VIEW_UPDATE;
                        break;
                    default:
                        return -1;
                }
                return viper_presenter_process_event(module->presenter, presenter_event, event->data);
            }
            break;
            
        case VIPER_COMPONENT_INTERACTOR:
            if (module->interactor) {
                /* Map VIPER event to interactor event */
                viper_interactor_event_t interactor_event;
                switch (event->type) {
                    case VIPER_EVENT_VIEW_ACTION:
                        interactor_event = INTERACTOR_EVENT_PROCESS_DATA;
                        break;
                    case VIPER_EVENT_BUSINESS_LOGIC:
                        interactor_event = INTERACTOR_EVENT_EXECUTE_BUSINESS_RULE;
                        break;
                    case VIPER_EVENT_DATA_UPDATE:
                        interactor_event = INTERACTOR_EVENT_PROCESS_DATA;
                        break;
                    case VIPER_EVENT_ERROR:
                        interactor_event = INTERACTOR_EVENT_ERROR;
                        break;
                    default:
                        return -1;
                }
                return viper_interactor_process_event(module->interactor, interactor_event, event->data);
            }
            break;
            
        case VIPER_COMPONENT_ENTITY:
            if (module->entity) {
                /* Map VIPER event to entity event */
                viper_entity_event_t entity_event;
                switch (event->type) {
                    case VIPER_EVENT_DATA_UPDATE:
                        entity_event = ENTITY_EVENT_UPDATE_DATA;
                        break;
                    case VIPER_EVENT_BUSINESS_LOGIC:
                        entity_event = ENTITY_EVENT_VALIDATE_DATA;
                        break;
                    case VIPER_EVENT_ERROR:
                        /* Entity doesn't have an error event, use UPDATE_DATA with error flag */
                        entity_event = ENTITY_EVENT_UPDATE_DATA;
                        break;
                    default:
                        return -1;
                }
                return viper_entity_process_event(module->entity, entity_event, event->data);
            }
            break;
            
        case VIPER_COMPONENT_ROUTER:
            if (module->router) {
                /* Map VIPER event to router event */
                viper_router_event_t router_event;
                switch (event->type) {
                    case VIPER_EVENT_NAVIGATION:
                        router_event = ROUTER_EVENT_NAVIGATE;
                        break;
                    case VIPER_EVENT_ERROR:
                        router_event = ROUTER_EVENT_ERROR;
                        break;
                    default:
                        return -1;
                }
                return viper_router_process_event(module->router, router_event, event->data);
            }
            break;
            
        default:
            /* Unknown target component */
            return -1;
    }
    
    return -1; /* Component not enabled or not found */
}

const char *viper_module_get_status(const struct viper_module *module)
{
    static char status[256];
    
    if (!module) return "NULL module";
    
    snprintf(status, sizeof(status), 
             "VIPER Module '%s' (ID: %d) - Components: %s%s%s%s%s",
             module->module_name,
             module->module_id,
             module->view ? "View " : "",
             module->presenter ? "Presenter " : "",
             module->interactor ? "Interactor " : "",
             module->entity ? "Entity " : "",
             module->router ? "Router" : "");
    
    return status;
}

struct viper_event *viper_event_create(viper_event_type_t type,
                                       const void *data,
                                       size_t data_size,
                                       int source,
                                       int target)
{
    struct viper_event *event = (struct viper_event *)malloc(sizeof(struct viper_event));
    if (!event) return NULL;
    
    event->type = type;
    event->source_component = source;
    event->target_component = target;
    
    /* Copy data if provided */
    if (data && data_size > 0) {
        event->data = malloc(data_size);
        if (!event->data) {
            free(event);
            return NULL;
        }
        memcpy(event->data, data, data_size);
        event->data_size = data_size;
    } else {
        event->data = NULL;
        event->data_size = 0;
    }
    
    return event;
}

void viper_event_destroy(struct viper_event *event)
{
    if (!event) return;
    
    if (event->data) {
        free(event->data);
    }
    
    free(event);
}
