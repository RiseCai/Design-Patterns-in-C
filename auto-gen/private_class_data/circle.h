/**
 * circle.h
 *
 * Private Class Data:
 *   Hide implementation attributes behind an opaque private data pointer.
 *   Clients cannot reach radius/color fields directly; only controlled APIs.
 *
 * Participants:
 *   Circle (main class)  - public API
 *   CircleData (data class) - private attributes
 */
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <mycommon.h>
#include <mytrace.h>

struct circle_data;

struct circle {
	struct circle_data *_data; /* private: opaque to clients */
};

/** constructor(). */
int circle_init(struct circle *circle, double radius, const char *color);

/** destructor helper for allocated private data. */
void circle_uninit(struct circle *circle);

double circle_get_radius(struct circle *circle);
const char *circle_get_color(struct circle *circle);
void circle_set_color(struct circle *circle, const char *color);
double circle_circumference(struct circle *circle);

#endif /* __CIRCLE_H__ */
