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

#include <ctype.h>

#include "uuid.h"

int
uuid_parse(const char *in, UUID uu)
{
	if (strlen(in) != 36)
		return -1;

	return uuid_parse_range(in, in + 36, uu);
}

static int
xdigit_to_int(char c)
{
	c = tolower(c);

	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;

	if (c >= '0' && c <= '9')
		return c - '0';

	return -1;
}

int
uuid_parse_range(const char *in_start, const char *in_end, UUID uu)
{
	int i;
	const char *cp;

	if ((in_end - in_start) != 36)
		return -1;

	for (i = 0, cp = in_start; i < 16; i++)
	{
		int hi;
		int lo;

		if (i == 4 || i == 6 || i == 8 || i == 10)
			cp++;

		hi = xdigit_to_int(*cp++);
		lo = xdigit_to_int(*cp++);

		if (hi == -1 || lo == -1)
			return -1;

		uu[i] = (hi << 4) + lo;
	}

	return 0;
}

static const char hexdigits_lower[16] = "0123456789abcdef";

void
uuid_unparse(const UUID uuid, char *out)
{
	char *p = out;

	for (int i = 0; i < 16; i++)
	{
		uint8_t tmp;

		if (i == 4 || i == 6 || i == 8 || i == 10)
			*p++ = '-';

		tmp = uuid[i];
		*p++ = hexdigits_lower[tmp >> 4];
		*p++ = hexdigits_lower[tmp & 15];
	}

	*p = '\0';
}

int
uuid_urn_parse(const char *in, UUID uu)
{
	if (strncmp(in, "urn:uuid:", 9) != 0)
		return -1;

	return uuid_parse(in + 9, uu);
}

int
uuid_equal(const UUID uu1, const UUID uu2)
{
	return memcmp(uu1, uu2, UUID_LEN) == 0;
}

void
uuid_copy(UUID dest, const UUID src)
{
	memcpy(dest, src, UUID_LEN);
}

void
uuid_nil(UUID uu)
{
	memset(uu, 0, UUID_LEN);
}