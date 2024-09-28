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

#ifndef SDA_H
#define SDA_H

#include <stddef.h>
#include <stdbool.h>

typedef void (*sda_clear_callback)(void *);

typedef struct {
	char *data;
	size_t len;
} sda_t;

sda_t *sda_new(size_t element_size);
char *sda_free(sda_t *sda, bool free_segment);
sda_t *sda_append_vals(sda_t *a, void *data, size_t len);
sda_t *sda_prepend_vals(sda_t *a, void *data, size_t len);
sda_t *sda_insert_vals(sda_t *a, size_t index_, void *data, size_t len);
sda_t *sda_set_size(sda_t *a, size_t length);
sda_t *sda_remove_range(sda_t *a, size_t index_, size_t length);
void sda_set_clear_func(sda_t *a, sda_clear_callback func);

#define sda_index(a, t, i)      (((t *)(void *)(a)->data)[(i)])
#define sda_append_val(a, v)    sda_append_vals(a, &(v), 1)
#define sda_prepend_val(a, v)   sda_prepend_vals(a, &(v), 1)
#define sda_insert_val(a, i, v) sda_insert_vals(a, i, &(v), 1)

#endif /* SDA_H */