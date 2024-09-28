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

#ifndef __NOSHIRO_BITSET_H__
#define __NOSHIRO_BITSET_H__

#include <stddef.h>
#include <stdbool.h>

enum
{
	BITSET_STATE_NONE, ///< All bits are LW_FALSE
	BITSET_STATE_ALL,  ///< All bits are LW_TRUE
	BITSET_STATE_ANY   ///< Some bits are LW_TRUE
};

typedef struct {
	size_t length;
	size_t capacity;
	char data[];
} bitset_t;

bitset_t *bitset_new(size_t size);
void bitset_free(bitset_t *bs);
void bitset_set(bitset_t *bs, size_t index);
void bitset_clear(bitset_t *bs, size_t index);
bool bitset_test(bitset_t *bs, size_t index);
void bitset_flip(bitset_t *bs, size_t index);
int bitset_state(bitset_t *bs);
size_t bitset_count(bitset_t *bs);
size_t bitset_size(bitset_t *bs);

#endif /* __NOSHIRO_BITSET_H__ */