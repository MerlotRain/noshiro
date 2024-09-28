#include "log.h"
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

#include "log.h"
#include <stdio.h>

#define LW_MSG_MAXLEN 256

static log_level_t default_loglevel = debug;

static void default_noticereporter(const char *fmt, va_list ap);
static void default_errorreporter(const char *fmt, va_list ap);
static void default_debuglogger(int level, const char *fmt, va_list ap);
lwreporter lwnotice_var = default_noticereporter;
lwreporter lwerror_var = default_errorreporter;
lwdebuglogger lwdebug_var = default_debuglogger;

void
lwnotice(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	/* Call the supplied function */
	(*lwnotice_var)(fmt, ap);

	va_end(ap);
}

void
lwerror(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	/* Call the supplied function */
	(*lwerror_var)(fmt, ap);

	va_end(ap);
}

void
lwdebug(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	/* Call the supplied function */
	(*lwdebug_var)(level, fmt, ap);

	va_end(ap);
}

void
lwsetloglevel(log_level_t level)
{
	default_loglevel = level;
}

log_level_t
lwgetloglevel(void)
{
	return default_loglevel;
}

static void
default_noticereporter(const char *fmt, va_list ap)
{
	char msg[LW_MSG_MAXLEN + 1];
	vsnprintf(msg, LW_MSG_MAXLEN, fmt, ap);
	msg[LW_MSG_MAXLEN] = '\0';
	fprintf(stderr, "%s\n", msg);
}

static void
default_debuglogger(int level, const char *fmt, va_list ap)
{
	char msg[LW_MSG_MAXLEN + 1];
	if (default_loglevel >= level)
	{
		/* Space pad the debug output */
		int i;
		for (i = 0; i < level; i++)
			msg[i] = ' ';
		vsnprintf(msg + i, LW_MSG_MAXLEN - i, fmt, ap);
		msg[LW_MSG_MAXLEN] = '\0';
		fprintf(stderr, "%s\n", msg);
	}
}

static void
default_errorreporter(const char *fmt, va_list ap)
{
	char msg[LW_MSG_MAXLEN + 1];
	vsnprintf(msg, LW_MSG_MAXLEN, fmt, ap);
	msg[LW_MSG_MAXLEN] = '\0';
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

#undef LW_MSG_MAXLEN