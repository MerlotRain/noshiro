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

#ifndef __NOSHIRO_UUID_H__
#define __NOSHIRO_UUID_H__

#include <stdint.h>
#include <string.h>

#define PRI_UUID \
	"%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-" \
	"%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

#define PRI_URN_UUID \
	"urn:uuid:%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-" \
	"%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

#define UUID_ARG(x) \
	(x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], \
	    (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], \
	    (x)[15]

#define UUID_LEN 16

/* Binary representation of a UUID */
typedef uint8_t UUID[UUID_LEN];

/**
 * Parses a string representation of a UUID formatted according to IETF RFC 4122
 * into an UUID. The parsing is case-insensitive. The string must be 37
 * characters long, including the terminating NUL character.
 *
 * Example string representation: "2fceebd0-7017-433d-bafb-d073a7116696"
 *
 * @param[in]  in  String representation of a UUID,
 *                 e.g. 2fceebd0-7017-433d-bafb-d073a7116696
 * @param[out] uu  UUID
 * @return         A non-zero value in case of an error.
 */
int uuid_parse(const char *in, UUID uu);

/**
 * Parses a URN representation of a UUID, as specified at IETF RFC 4122,
 * into an UUID. The parsing is case-insensitive. The string must be 46
 * characters long, including the terminating NUL character.
 *
 * Example string representation:
 * "urn:uuid:2fceebd0-7017-433d-bafb-d073a7116696"
 *
 * @param[in]  in  URN UUID
 * @param[out] uu  UUID
 * @return         A non-zero value in case of an error.
 */
int uuid_urn_parse(const char *in, UUID uu);

/**
 * Parses a string representation of a UUID formatted according to IETF RFC 4122
 * into an UUID. The parsing is case-insensitive.
 *
 * @param[in]  in_start Pointer to the first character of the string
 * representation
 * @param[in]  in_end   Pointer to the character after the last character of the
 *                      string representation. That memory location is never
 *                      accessed. It is an error if `in_end - in_start != 36`.
 * @param[out] uu       UUID
 * @return              A non-zero value in case of an error.
 */
int uuid_parse_range(const char *in_start, const char *in_end, UUID uu);

/**
 * Serializes a UUID into a string representation according to IETF RFC 4122.
 * The string is lowercase and always 37 characters long, including the
 * terminating NUL character.
 *
 * @param[in]  uu  UUID
 * @param[out] out Pointer to an array of no less than 37 characters.
 */
void uuid_unparse(const UUID uu, char *out);

/**
 * Compares two UUIDs for equality.
 *
 * @param[in]  uu1  UUID
 * @param[in]  uu2  UUID
 * @return          Nonzero if uu1 and uu2 are identical, 0 otherwise
 */
int uuid_equal(const UUID uu1, const UUID uu2);

/**
 * Copies the bytes of src into dest.
 *
 * @param[out]  dest  UUID
 * @param[in]   src   UUID
 */
void uuid_copy(UUID dest, const UUID src);

/**
 * Sets a UUID to the nil UUID, i.e. a UUID with have all
 * its 128 bits set to zero.
 *
 * @param[in,out]  uu  UUID to be set to the nil UUID
 */
void uuid_nil(UUID uu);

#endif /* __NOSHIRO_UUID_H__ */