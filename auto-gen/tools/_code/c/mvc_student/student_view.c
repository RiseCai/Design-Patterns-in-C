/**
 * student_view.c  2023-06-03
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
 * Student object acting as a model.StudentView will be a view class which can
    print student details on console and StudentController is the controller
    class responsible to store data in Student object and update view
    StudentView accordingly.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>

#include "student_view.h"

/** constructor(). */
void student_view_init(struct student_view *student_view)
{
	_MY_TRACE_STR("student_view_init()\n");
	memset(student_view, 0, sizeof(*student_view));
}

void student_view_show_me(struct student_view *student_view, struct student *stu)
{
	_MY_TRACE_STR("student_view::show_me()\n");
}