/**
 * console_logger.h - real Logger that prints to stdout
 */
#ifndef __CONSOLE_LOGGER_H__
#define __CONSOLE_LOGGER_H__

#include "logger.h"

struct console_logger {
	struct logger logger;
};

/** constructor(). */
void console_logger_init(struct console_logger *console_logger);

#endif /* __CONSOLE_LOGGER_H__ */
