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

#include "sda.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SDA_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SDA_MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
	char *data;
	size_t len;
	size_t elt_capacity;
	size_t elt_size;
	sda_clear_callback clear_func;
} sda_t_real;

#define sda_elt_len(array, i) ((size_t)(array)->elt_size * (i))
#define sda_elt_pos(array, i) ((array)->data + sda_elt_len((array), (i)))
#define sda_elt_zero(array, pos, len) \
	(memset(sda_elt_pos((array), pos), 0, sda_elt_len((array), len)))

static void sda_maybe_expand(sda_t_real *array, size_t len);
static size_t sda_nearest_pow(size_t v);

sda_t *
sda_new(size_t element_size)
{
	if (element_size <= 0 || element_size >= 128)
		return NULL;

	sda_t_real *array = (sda_t_real *)malloc(sizeof(sda_t_real));
	if (!array)
		return NULL;

	array->data = NULL;
	array->len = 0;
	array->elt_capacity = 0;
	array->elt_size = element_size;
	array->clear_func = 0;
	return (sda_t *)array;
}

char *
sda_free(sda_t *a, bool free_segment)
{
	sda_t_real *array = (sda_t_real *)a;
	if (!array)
		return NULL;

	char *segment;
	if (free_segment)
	{
		if (array->clear_func != NULL)
		{
			for (size_t i = 0; i < array->len; i++)
				array->clear_func(sda_elt_pos(array, i));
		}

		free(array->data);
		segment = NULL;
	}
	else
	{
		segment = (char *)array->data;
	}

	free(array);
	return segment;
}

sda_t *
sda_append_vals(sda_t *a, void *data, size_t len)
{
	sda_t_real *array = (sda_t_real *)a;
	if (!array)
		return NULL;

	if (len == 0)
		return a;

	sda_maybe_expand(array, len);
	memcpy(sda_elt_pos(array, array->len), data, sda_elt_len(array, len));

	array->len += len;
	return a;
}

sda_t *
sda_prepend_vals(sda_t *a, void *data, size_t len)
{
	sda_t_real *array = (sda_t_real *)a;
	if (array == NULL)
		return NULL;

	if (len == 0)
		return a;

	sda_maybe_expand(array, len);

	memmove(sda_elt_pos(array, len),
		sda_elt_pos(array, 0),
		sda_elt_len(array, array->len));

	memcpy(sda_elt_pos(array, 0), data, sda_elt_len(array, len));

	array->len += len;

	return a;
}

sda_t *
sda_insert_vals(sda_t *a, size_t index_, void *data, size_t len)
{
	sda_t_real *array = (sda_t_real *)a;
	if (!array)
		return NULL;

	if (len == 0)
		return a;

	/* Is the index off the end of the array, and hence do we need to
	 * over-allocate and clear some elements? */
	if (index_ >= array->len)
	{
		sda_maybe_expand(array, index_ - array->len + len);
		return sda_append_vals(sda_set_size(a, index_), data, len);
	}

	sda_maybe_expand(array, len);

	memmove(sda_elt_pos(array, len + index_),
		sda_elt_pos(array, index_),
		sda_elt_len(array, array->len - index_));

	memcpy(sda_elt_pos(array, index_), data, sda_elt_len(array, len));

	array->len += len;

	return a;
}

sda_t *
sda_set_size(sda_t *a, size_t length)
{
	sda_t_real *array = (sda_t_real *)a;
	if (!array)
		return NULL;

	if (length > array->len)
	{
		sda_maybe_expand(array, length - array->len);
	}
	else if (length < array->len)
		sda_remove_range(a, length, array->len - length);

	array->len = length;

	return a;
}

sda_t *
sda_remove_range(sda_t *a, size_t index_, size_t length)
{
	sda_t_real *array = (sda_t_real *)a;
	if (!array)
		return NULL;
	if (index_ > array->len)
		return NULL;
	if (index_ > UINT_MAX - length)
		return NULL;
	if (index_ + length > array->len)
		return NULL;

	if (array->clear_func != NULL)
	{
		size_t i;
		for (i = 0; i < length; i++)
			array->clear_func(sda_elt_pos(array, index_ + i));
	}

	if (index_ + length != array->len)
		memmove(sda_elt_pos(array, index_),
			sda_elt_pos(array, index_ + length),
			(array->len - (index_ + length)) * array->elt_size);

	array->len -= length;
	sda_elt_zero(array, array->len, length);

	return a;
}

void
sda_set_clear_func(sda_t *a, sda_clear_callback func)
{
	sda_t_real *array = (sda_t_real *)(a);
	if (array == NULL)
		return;

	array->clear_func = func;
}

static void
sda_maybe_expand(sda_t_real *array, size_t len)
{
	size_t max_len, want_len;
	max_len = SDA_MIN(SIZE_MAX / 2 / array->elt_size, UINT_MAX);

	/* Detect potential overflow */
	if ((max_len - array->len) < len)
		abort();

	want_len = array->len + len;
	if (want_len > array->elt_capacity)
	{
		size_t want_alloc =
		    sda_nearest_pow(sda_elt_len(array, want_len));
		assert(want_alloc >= sda_elt_len(array, want_len));
		want_alloc = SDA_MAX(want_alloc, 16);

		array->data = realloc(array->data, want_alloc);

		memset(sda_elt_pos(array, array->elt_capacity),
		       0,
		       sda_elt_len(array, want_len - array->elt_capacity));

		array->elt_capacity =
		    SDA_MIN(want_alloc / array->elt_size, UINT_MAX);
	}
}

size_t
sda_nearest_pow(size_t v)
{
	size_t n = 1;

	while (n < v && n > 0)
		n <<= 1;

	return n ? n : v;
}

#undef SDA_MIN
#undef SDA_MAX