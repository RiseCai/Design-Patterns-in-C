/**
 * command.h  2023-02-18
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
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <mycommon.h>
#include <mytrace.h>

struct command_ops;

struct command {
	struct command_ops *ops;
};
struct command_ops {
	void (*execute)(struct command *);
};

/** constructor(). */
void command_init(struct command *command);

static inline void command_execute(struct command *command)
{
	command->ops->execute(command);
}

#endif /* __COMMAND_H__ */