/**
 * circle_data.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "circle_data.h"

void circle_data_init(struct circle_data *data, double radius, const char *color)
{
	_MY_TRACE_STR("circle_data_init()\n");
	memset(data, 0, sizeof(*data));
	data->radius = radius;
	snprintf(data->color, sizeof(data->color), "%s", color ? color : "black");
}
