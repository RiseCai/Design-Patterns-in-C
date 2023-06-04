/**
 *   2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "margherita_pizza.h"
#include "gourmet_pizza.h"
#include "extra_cheese_topping.h"
#include "jalapeno_topping.h"
#include "mushroom_topping.h"

static int test_test_me(void)
{
	_MY_TRACE_STR("test::test_me()\n");
	/** PSEUDOCODE    Margherita pizza = new Margherita();
     Console.WriteLine('Plain Margherita: ' + pizza.GetPrice().ToString());

     ExtraCheeseTopping moreCheese = new ExtraCheeseTopping(pizza);
     ExtraCheeseTopping someMoreCheese = new ExtraCheeseTopping(moreCheese);
     Console.WriteLine('Plain Margherita with double extra cheese: ' + someMoreCheese.GetPrice().ToString());

     MushroomTopping moreMushroom = new MushroomTopping(someMoreCheese);
     Console.WriteLine('Plain Margherita with double extra cheese with mushroom: ' + moreMushroom.GetPrice().ToString());

     JalapenoTopping moreJalapeno = new JalapenoTopping(moreMushroom);
     Console.WriteLine('Plain Margherita with double extra cheese with mushroom with Jalapeno: ' + moreJalapeno.GetPrice().ToString());

     Console.ReadLine();
	*/
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	TODO(Please add our **test** code here ...)
	return 0;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test decorator_pizza");
}