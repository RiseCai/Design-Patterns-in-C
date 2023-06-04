/**
 * mortgage.h  2023-06-03
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
 * Provide a unified interface to a set of interfaces in a subsystem. Facade
    defines a higher-level interface that makes the subsystem easier to use.
The classes and/or objects participating in this pattern are:
    Facade   (MortgageApplication)
      - knows which subsystem classes are responsible for a request.
      - delegates client requests to appropriate subsystem objects.
    Subsystem classes   (Bank, Credit, Loan)
      - implement subsystem functionality.
      - handle work assigned by the Facade object.
      - have no knowledge of the facade and keep no reference to it.
 */
#ifndef __MORTGAGE_H__
#define __MORTGAGE_H__

#include <mycommon.h>
#include <mytrace.h>

#include "bank.h"
		
#include "credit.h"
		
#include "loan.h"

/** The facade class */
struct mortgage {
	struct bank * _bank;
	struct credit * _credit;
	struct loan * _loan;
};

/** constructor(). */
void mortgage_init(struct mortgage *mortgage);

int mortgage_is_eligible(struct mortgage *mortgage, char *cust, int amount);

#endif /* __MORTGAGE_H__ */