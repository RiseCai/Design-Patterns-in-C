/**
 * light.h  2022-12-24
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * Need to issue requests to objects without knowing anything about the operation being requested or the receiver of the request.
    Encapsulate a request as an object, thereby letting you parameterize clients with different requests, queue or log requests, and support undoable operations.
    Promote "invocation of a method on an object" to full object status
    An object-oriented callback
 */
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <mycommon.h>
#include <mytrace.h>

struct light {
};

/** constructor(). */
void light_init(struct light *light);

void light_turnon(struct light *light);

void light_turnoff(struct light *light);

#endif /* __LIGHT_H__ */