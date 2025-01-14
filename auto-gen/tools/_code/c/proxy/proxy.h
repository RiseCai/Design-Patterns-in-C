/**
 * proxy.h  2023-06-03
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
 * Provides a surrogate or placeholder for another object to control access to
    it.This structural code demonstrates the Proxy pattern which provides a
    representative object (proxy) that controls access to another similar
    object.
 */
#ifndef __PROXY_H__
#define __PROXY_H__

#include <mycommon.h>
#include <mytrace.h>

#include "real_subject.h"
		
#include "subject.h"

/** Proxy can delay creatting of the real-subject case by case. */
struct proxy {
	struct subject subject;
	struct real_subject * _subject;
};

/** constructor(). */
void proxy_init(struct proxy *proxy);

#endif /* __PROXY_H__ */