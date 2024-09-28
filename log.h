/**
 * Copyright (c) 2023-present Merlot.Rain
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __NOSHIRO_LOG_H__
#define __NOSHIRO_LOG_H__

#include <stdarg.h>

typedef enum
{
	none,
	report,
	debug,
	warn,
	error
} log_level_t;

typedef void (*lwreporter)(const char *fmt, va_list ap);
typedef void (*lwdebuglogger)(int level, const char *fmt, va_list ap);

/* Display a notice at the given debug level */
#define LWDEBUG(level, msg) \
	do \
	{ \
		if (POSTGIS_DEBUG_LEVEL >= level) \
			lwdebug(level, \
				"[%s:%s:%d] " msg, \
				__FILE__, \
				__func__, \
				__LINE__); \
	} while (0);

/* Display a formatted notice at the given debug level
 * (like printf, with variadic arguments) */
#define LWDEBUGF(level, msg, ...) \
	do \
	{ \
		if (POSTGIS_DEBUG_LEVEL >= level) \
			lwdebug(level, \
				"[%s:%s:%d] " msg, \
				__FILE__, \
				__func__, \
				__LINE__, \
				__VA_ARGS__); \
	} while (0);

/**
 * Write a notice out to the notice handler.
 *
 * Uses standard printf() substitutions.
 * Use for messages you always want output.
 * For debugging, use LWDEBUG() or LWDEBUGF().
 * @ingroup logging
 */
void lwnotice(const char *fmt, ...);

/**
 * Write a notice out to the error handler.
 *
 * Uses standard printf() substitutions.
 * Use for errors you always want output.
 * For debugging, use LWDEBUG() or LWDEBUGF().
 * @ingroup logging
 */
void lwerror(const char *fmt, ...);

/**
 * Write a debug message out.
 * Don't call this function directly, use the
 * macros, LWDEBUG() or LWDEBUGF(), for
 * efficiency.
 * @ingroup logging
 */
void lwdebug(int level, const char *fmt, ...);

void lwsetloglevel(log_level_t level);

log_level_t lwgetloglevel(void);

#endif /* __NOSHIRO_LOG_H__ */