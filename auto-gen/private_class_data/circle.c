/**
 * circle.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "circle.h"
#include "circle_data.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** constructor(). */
int circle_init(struct circle *circle, double radius, const char *color)
{
	_MY_TRACE_STR("circle_init()\n");
	memset(circle, 0, sizeof(*circle));
	circle->_data = malloc(sizeof(*circle->_data));
	if (!circle->_data)
		return -1;
	circle_data_init(circle->_data, radius, color);
	return 0;
}

void circle_uninit(struct circle *circle)
{
	_MY_TRACE_STR("circle_uninit()\n");
	if (!circle)
		return;
	free(circle->_data);
	circle->_data = 0;
}

double circle_get_radius(struct circle *circle)
{
	_MY_TRACE_STR("circle::get_radius()\n");
	return circle->_data->radius;
}

const char *circle_get_color(struct circle *circle)
{
	_MY_TRACE_STR("circle::get_color()\n");
	return circle->_data->color;
}

void circle_set_color(struct circle *circle, const char *color)
{
	_MY_TRACE_STR("circle::set_color()\n");
	snprintf(circle->_data->color, sizeof(circle->_data->color), "%s",
		color ? color : "");
}

double circle_circumference(struct circle *circle)
{
	_MY_TRACE_STR("circle::circumference()\n");
	return 2.0 * M_PI * circle->_data->radius;
}
