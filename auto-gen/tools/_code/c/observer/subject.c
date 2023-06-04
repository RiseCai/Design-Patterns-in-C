/**
 * subject.c  2023-03-14
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
 * The Observer Pattern (sometimes called Listener Pattern) is a design
    pattern which establishes a one-to-many relationship between objects, such
    that changes to one object (the subject) may be communicated to many other
    objects (the observers/listeners). The power of this pattern is that it
    loosely couples objects together in a manner that does not require the
    subject or its listeners understanding anything about the other, beyond the
    fact that they implement a shared interface for communicating.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "subject.h"

static int subject_ops_register_observer(struct subject *subject, struct observer *obs)
{
	_MY_TRACE_STR("subject::register_observer()\n");
	return 0;
}

static int subject_ops_remove_observer(struct subject *subject, struct observer *obs)
{
	_MY_TRACE_STR("subject::remove_observer()\n");
	return 0;
}

static int subject_ops_notify_observers(struct subject *subject, char *operation, char *record)
{
	_MY_TRACE_STR("subject::notify_observers()\n");
	return 0;
}
static struct subject_ops subject_ops = {
	.register_observer = subject_ops_register_observer,
	.remove_observer = subject_ops_remove_observer,
	.notify_observers = subject_ops_notify_observers,
};

/** constructor(). */
void subject_init(struct subject *subject)
{
	_MY_TRACE_STR("subject_init()\n");
	memset(subject, 0, sizeof(*subject));
	subject->ops = &subject_ops;
}