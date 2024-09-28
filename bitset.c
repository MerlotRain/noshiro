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

#include "bitset.h"
#include <string.h>
#include <stdlib.h>

/// @brief Create a new bitset.
/// @param size The size of the bitset.
/// @return Generate an 8-bit aligned bitset
bitset_t *
bitset_new(size_t size)
{
	size_t cap = 0;
	if (size == 0)
		cap = 8;
	else
	{
		cap = (size + 7) & ~7; // round up to multiple of 8
	}

	bitset_t *bs = (bitset_t *)malloc(sizeof(bitset_t) + cap / 8);
	if (bs == NULL)
	{
		return NULL;
	}
	memset(bs, 0, sizeof(bitset_t) + cap / 8);
	bs->length = size;
	bs->capacity = cap;
	return bs;
}

/// @brief Free a bitset.
/// @param bs The bitset to free.
void
bitset_free(bitset_t *bs)
{
	free(bs);
}

/// @brief Set the \a index bit to LW_TRUE
/// @param bs The bitset
/// @param index The index of the bit
/// @return
void
bitset_set(bitset_t *bs, size_t index)
{
	if (index >= bs->length)
	{
		return;
	}

	bs->data[index / 8] |= (1 << (index % 8)); // set bit
}

/// @brief Clear the \a index bit to LW_FALSE
/// @param bs The bitset
/// @param index The index of the bit
/// @return
void
bitset_clear(bitset_t *bs, size_t index)
{
	if (index >= bs->length)
	{
		return;
	}

	bs->data[index / 8] &= ~(1 << (index % 8)); // clear bit
}

/// @brief Test if the \a index bit is LW_TRUE or LW_FALSE
/// @param bs The bitset
/// @param index The index of the bit
bool
bitset_test(bitset_t *bs, size_t index)
{
	if (index >= bs->length)
	{
		return false;
	}

	return bs->data[index / 8] & (char)(1 << (index % 8)); // test bit
}

/// @brief Flip the \a index bit
/// @param bs The bitset
/// @param index The index of the bit
/// @return
void
bitset_flip(bitset_t *bs, size_t index)
{
	if (index >= bs->length)
	{
		return;
	}

	bs->data[index / 8] ^= (1 << (index % 8));
}

/// @brief Get the state of the bitset
/// @param bs The bitset
/// @return The return value is a BITSET_STATE_* series macro
int
bitset_state(bitset_t *bs)
{
	size_t c = bitset_count(bs);
	if (c == 0)
	{
		return BITSET_STATE_NONE;
	}
	else if (c == bs->length)
	{
		return BITSET_STATE_ALL;
	}
	else
	{
		return BITSET_STATE_ANY;
	}
}

/// @brief Count the number of bits set to LW_TRUE
/// @param bs The bitset
/// @return The number of bits set to LW_TRUE
size_t
bitset_count(bitset_t *bs)
{
	int count = 0;
	for (int i = 0; i < (bs->length / 8) + (bs->length % 8 ? 1 : 0); i++)
	{
		unsigned char byte = bs->data[i];
		while (byte)
		{
			byte &= (byte - 1);
			count++;
		}
	}
	return count;
}

/// @brief Get the size of the bitset
/// @param bs The bitset
/// @return An 8-bit aligned value
size_t
bitset_size(bitset_t *bs)
{
	return bs->length;
}