/**
 * circle_data.h
 *
 * Private Class Data: mutable/immutable attributes live here.
 * Only circle.c should include this header (not clients).
 */
#ifndef __CIRCLE_DATA_H__
#define __CIRCLE_DATA_H__

struct circle_data {
	double radius;      /* immutable after construction */
	char color[32];     /* mutable via public API */
};

void circle_data_init(struct circle_data *data, double radius, const char *color);

#endif /* __CIRCLE_DATA_H__ */
