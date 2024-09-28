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

#include "sds.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

static inline int *compute_prefix_function(const char *pattern, size_t m);
static inline char *kmp_string_match(char *text,
				     const char *pattern,
				     size_t text_len,
				     size_t pattern_len);
static sds_t *sds_slice_recursive(char *s,
				  uint64_t len,
				  uint8_t *ascii,
				  int cnt,
				  sds_t *save);
static inline sds_t *sds_assign(char *s, size_t len);

sds_t *
sds_set(sds_t *str, char *s)
{
	str->data = (uint8_t *)s;
	str->len = strlen(s);
	str->data_ref = 1;
	str->ref = 1;
	return str;
}

sds_t *
sds_nset(sds_t *str, char *s, uint64_t n)
{
	str->data = (uint8_t *)s;
	str->len = n;
	str->data_ref = 1;
	str->ref = 1;
	return str;
}

sds_t *
sds_ref(sds_t *s)
{
	++s->ref;
	return s;
}

void
sds_free(sds_t *s)
{
	if (s == NULL)
		return;
	if (s->ref-- > 1)
		return;
	if (!s->data_ref && s->data != NULL)
	{
		free(s->data);
	}

	free(s);
}

sds_t *
sds_new(const char *s)
{
	sds_t *str = (sds_t *)malloc(sizeof(sds_t));
	if (str == NULL)
		return NULL;
	if (s == NULL)
	{
		str->data = NULL;
		str->len = 0;
		str->data_ref = 0;
		str->ref = 1;
		return str;
	}
	size_t len = strlen(s);
	if ((str->data = (uint8_t *)malloc(len + 1)) == NULL)
	{
		free(str);
		return NULL;
	}
	memcpy(str->data, s, len);
	str->data[len] = 0;
	str->len = len;
	str->data_ref = 0;
	str->ref = 1;
	return str;
}

sds_t *
sds_buf_new(unsigned char *buf, uint64_t len)
{
	sds_t *str = (sds_t *)malloc(sizeof(sds_t));
	if (str == NULL)
		return NULL;

	str->data = buf;
	str->len = len;
	str->data_ref = 0;
	str->ref = 1;
	return str;
}

sds_t *
sds_dup(sds_t *str)
{
	sds_t *s = (sds_t *)malloc(sizeof(sds_t));
	if (s == NULL)
		return NULL;
	if ((s->data = (uint8_t *)malloc(str->len + 1)) == NULL)
	{
		free(s);
		return NULL;
	}
	if (str->data != NULL)
		memcpy(s->data, str->data, str->len);
	s->data[str->len] = 0;
	s->len = str->len;
	s->data_ref = 0;
	s->ref = 1;
	return s;
}

sds_t *
sds_alloc(size_t size)
{
	if (size < 0)
		return NULL;
	sds_t *s = (sds_t *)malloc(sizeof(sds_t));
	if (s == NULL)
		return NULL;
	if ((s->data = (uint8_t *)malloc(size + 1)) == NULL)
	{
		free(s);
		return NULL;
	}
	s->len = size;
	s->data_ref = 0;
	s->ref = 1;
	return s;
}

sds_t *
sds_const_ndup(char *str, size_t size)
{
	if (size < 0)
		return NULL;
	sds_t *s = (sds_t *)malloc(sizeof(sds_t));
	if (s == NULL)
		return NULL;
	if ((s->data = (uint8_t *)malloc(size + 1)) == NULL)
	{
		free(s);
		return NULL;
	}
	memcpy(s->data, str, size);
	s->data[size] = 0;
	s->len = size;
	s->data_ref = 0;
	s->ref = 1;
	return s;
}

sds_t *
sds_ref_dup(sds_t *str)
{
	sds_t *s = (sds_t *)malloc(sizeof(sds_t));
	if (s == NULL)
		return NULL;
	s->data = str->data;
	s->len = str->len;
	s->data_ref = 1;
	s->ref = 1;
	return s;
}

sds_t *
sds_const_ref_dup(char *s)
{
	if (s == NULL)
		return NULL;

	sds_t *str = (sds_t *)malloc(sizeof(sds_t));
	if (str == NULL)
		return NULL;

	str->data = (uint8_t *)s;
	str->len = strlen(s);
	str->data_ref = 1;
	str->ref = 1;
	return str;
}

sds_t *
sds_concat(sds_t *s1, sds_t *s2, sds_t *sep)
{
	sds_t *str = (sds_t *)malloc(sizeof(sds_t));
	if (str == NULL)
		return NULL;

	uint8_t *p;
	size_t size = 0;

	if (s1 != NULL)
	{
		size += s1->len;
		if (s2 != NULL && sep != NULL)
			size += sep->len;
	}
	if (s2 != NULL)
		size += s2->len;

	if ((p = str->data = (uint8_t *)malloc(size + 1)) == NULL)
	{
		free(str);
		return NULL;
	}

	if (s1 != NULL)
	{
		memcpy(p, s1->data, s1->len);
		p += s1->len;
		if (s2 != NULL && sep != NULL)
		{
			memcpy(p, sep->data, sep->len);
			p += sep->len;
		}
	}
	if (s2 != NULL)
	{
		memcpy(p, s2->data, s2->len);
		p += s2->len;
	}
	*p = 0;

	str->len = size;
	str->data_ref = 0;
	str->ref = 1;

	return str;
}

int
sds_strseqcmp(sds_t *s1, sds_t *s2)
{
	int ret;
	if (s1->len > s2->len)
	{
		if (s2->len == 0)
			return 1;
		ret = memcmp(s1->data, s2->data, s2->len);
		return ret ? ret : 1;
	}
	else if (s1->len < s2->len)
	{
		if (s1->len == 0)
			return 1;
		ret = memcmp(s1->data, s2->data, s1->len);
		return ret ? ret : -1;
	}
	if (s1->len == 0)
		return 0;
	return memcmp(s1->data, s2->data, s1->len);
}

int
sds_strcmp(sds_t *s1, sds_t *s2)
{
	if (s1 == s2 || s1->data == s2->data)
		return 0;
	if (s1->len > s2->len)
		return 1;
	if (s1->len < s2->len)
		return -1;
	if (s1->len > 280 || (s1->len % sizeof(uint32_t)))
		return memcmp(s1->data, s2->data, s1->len);
	uint32_t *i1 = (uint32_t *)(s1->data), *i2 = (uint32_t *)(s2->data), i;
	size_t res;
	for (i = 0; i < s1->len;)
	{
		if ((res = (*i1++ - *i2++)) != 0)
			return (int)res;
		i += sizeof(uint32_t);
	}
	return 0;
}

int
sds_const_strcmp(sds_t *s1, char *s2)
{
	if (s1->data == (uint8_t *)s2)
		return 0;
	size_t len = strlen(s2);
	if (s1->len > len)
		return 1;
	if (s1->len < len)
		return -1;
	if (s1->len > 280 || (len % sizeof(uint32_t)))
		return memcmp(s1->data, s2, len);

	uint32_t *i1 = (uint32_t *)(s1->data), *i2 = (uint32_t *)s2, i;
	size_t res;
	for (i = 0; i < len;)
	{
		if ((res = (*i1++ - *i2++)) != 0)
			return (int)res;
		i += sizeof(uint32_t);
	}
	return 0;
}

int
sds_strncmp(sds_t *s1, sds_t *s2, uint32_t n)
{
	if (s1 == s2 || s1->data == s2->data)
		return 0;
	if (s1->len < n || s2->len < n)
		return -1;
	if (n > 280 || (n % sizeof(uint32_t)))
		return memcmp(s1->data, s2->data, n);

	uint32_t *i1 = (uint32_t *)(s1->data), *i2 = (uint32_t *)(s2->data), i;
	size_t res;
	for (i = 0; i < n;)
	{
		if ((res = (*i1++ - *i2++)) != 0)
			return (int)res;
		i += sizeof(uint32_t);
	}
	return 0;
}

int
sds_const_strncmp(sds_t *s1, char *s2, uint32_t n)
{
	if (s1->data == (uint8_t *)s2)
		return 0;
	if (s1->len < n || strlen(s2) < n)
		return -1;
	if (n > 280 || (n % sizeof(uint32_t)))
		return memcmp(s1->data, s2, n);

	uint32_t *i1 = (uint32_t *)(s1->data), *i2 = (uint32_t *)s2, i;
	size_t res;
	for (i = 0; i < n;)
	{
		if ((res = (*i1++ - *i2++)) != 0)
			return (int)res;
		i += sizeof(uint32_t);
	}
	return 0;
}

int
sds_strcasecmp(sds_t *s1, sds_t *s2)
{
	if (s1 == s2 || s1->data == s2->data)
		return 0;
	if (s1->len > s2->len)
		return 1;
	if (s1->len < s2->len)
		return -1;

	return strncasecmp((char *)(s1->data), (char *)(s2->data), s1->len);
}

int
sds_const_strcasecmp(sds_t *s1, char *s2)
{
	if (s1->data == (uint8_t *)s2)
		return 0;
	size_t len = strlen(s2);
	if (s1->len > len)
		return 1;
	if (s1->len < len)
		return -1;
	return strncasecmp((char *)(s1->data), s2, len);
}

int
sds_const_strncasecmp(sds_t *s1, char *s2, uint32_t n)
{
	if (s1->data == (uint8_t *)s2)
		return 0;
	size_t len = strlen(s2);
	if (s1->len < n || len < n)
		return -1;
	return strncasecmp((char *)(s1->data), s2, n);
}

int
sds_strncasecmp(sds_t *s1, sds_t *s2, uint32_t n)
{
	if (s1 == s2 || s1->data == s2->data)
		return 0;
	if (s1->len < n || s2->len < n)
		return -1;
	return strncasecmp((char *)(s1->data), (char *)(s2->data), n);
}

char *
sds_strstr(sds_t *text, sds_t *pattern)
{
	if (text == pattern || text->data == pattern->data)
		return (char *)(text->data);
	return strstr((char *)(text->data), (char *)(pattern->data));
}

char *
sds_const_strstr(sds_t *text, char *pattern)
{
	if (text->data == (uint8_t *)pattern)
		return (char *)(text->data);
	return strstr((char *)(text->data), pattern);
}

sds_t *
sds_new_strstr(sds_t *text, sds_t *pattern)
{
	char *p = sds_strstr(text, pattern);
	if (p == NULL)
		return NULL;
	return sds_assign(p, text->len - (p - (char *)(text->data)));
}

sds_t *
sds_new_const_strstr(sds_t *text, char *pattern)
{
	char *p = sds_const_strstr(text, pattern);
	if (p == NULL)
		return NULL;
	return sds_assign(p, text->len - (p - (char *)(text->data)));
}

char *
sds_kmp(sds_t *text, sds_t *pattern)
{
	if (text == pattern || text->data == pattern->data)
		return (char *)(text->data);
	return kmp_string_match((char *)(text->data),
				(char *)(pattern->data),
				text->len,
				pattern->len);
}

char *
sds_const_kmp(sds_t *text, char *pattern)
{
	if (text->data == (uint8_t *)pattern)
		return (char *)(text->data);
	return kmp_string_match(
	    (char *)(text->data), pattern, text->len, strlen(pattern));
}

sds_t *
sds_new_kmp(sds_t *text, sds_t *pattern)
{
	char *p = sds_kmp(text, pattern);
	if (p == NULL)
		return NULL;
	return sds_assign(p, text->len - (p - (char *)(text->data)));
}

sds_t *
sds_new_const_kmp(sds_t *text, char *pattern)
{
	char *p = sds_const_kmp(text, pattern);
	if (p == NULL)
		return NULL;
	return sds_assign(p, text->len - (p - (char *)(text->data)));
}

sds_t *
sds_slice(sds_t *s, const char *sep_array)
{
	const char *ps;
	sds_t *tmp = sds_dup(s);
	uint8_t ascii[256] = {0};

	if (tmp == NULL)
		return NULL;

	for (ps = sep_array; *ps != 0; ++ps)
	{
		ascii[(uint8_t)(*ps)] = 1;
	}
	return sds_slice_recursive(
	    (char *)(tmp->data), tmp->len, ascii, 1, tmp);
}

void
sds_slice_free(sds_t *array)
{
	sds_t *s = &array[0];
	for (; s->len; ++s)
		;
	sds_free((sds_t *)(s->data));
	free(array);
}

sds_t *
sds_strcat(sds_t *s1, sds_t *s2)
{
	sds_t *ret = (sds_t *)malloc(sizeof(sds_t));
	if (ret == NULL)
		return NULL;
	uint64_t len = s1->len + s2->len;
	if (len == 0)
	{
		ret->data = NULL;
		ret->len = 0;
		ret->data_ref = 0;
		ret->ref = 1;
		return ret;
	}
	if ((ret->data = (uint8_t *)malloc(len + 1)) == NULL)
	{
		free(ret);
		return NULL;
	}
	if (s1->len > 0)
		memcpy(ret->data, s1->data, s1->len);
	if (s2->len > 0)
		memcpy(ret->data + s1->len, s2->data, s2->len);
	ret->data[len] = 0;
	ret->len = len;
	ret->data_ref = 0;
	ret->ref = 1;
	return ret;
}

sds_t *
sds_trim(sds_t *s, sds_t *mask)
{
	uint8_t chars[256] = {0};
	sds_t tmp;
	size_t i, j;

	for (i = 0; i < mask->len; ++i)
	{
		chars[mask->data[i]] = 1;
	}
	for (i = 0; i < s->len; ++i)
	{
		if (!chars[s->data[i]])
			break;
	}
	for (j = s->len; j > i; --j)
	{
		if (!chars[s->data[j - 1]])
			break;
	}

	sds_nset(&tmp, &(s->data[i]), j - i);
	return sds_dup(&tmp);
}

void
sds_upper(sds_t *s)
{
	uint8_t chars[256] = {0};
	uint8_t upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint8_t lower[] = "abcdefghijklmnopqrstuvwxyz";
	size_t i;

	for (i = 0; i < sizeof(lower) - 1; ++i)
	{
		chars[lower[i]] = upper[i];
	}
	for (i = 0; i < s->len; ++i)
	{
		if (chars[s->data[i]])
			s->data[i] = chars[s->data[i]];
	}
}

void
sds_lower(sds_t *s)
{
	uint8_t chars[256] = {0};
	uint8_t upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint8_t lower[] = "abcdefghijklmnopqrstuvwxyz";
	size_t i;

	for (i = 0; i < sizeof(upper) - 1; ++i)
	{
		chars[upper[i]] = lower[i];
	}
	for (i = 0; i < s->len; ++i)
	{
		if (chars[s->data[i]])
			s->data[i] = chars[s->data[i]];
	}
}

static inline int *
compute_prefix_function(const char *pattern, size_t m)
{
	int *shift = (int *)malloc(sizeof(int) * m);
	if (shift == NULL)
		return NULL;
	shift[0] = 0;
	int k = 0, q;
	for (q = 1; q < m; ++q)
	{
		while (k > 0 && pattern[k] != pattern[q])
			k = shift[k - 1];
		if (pattern[k] == pattern[q])
			++k;
		shift[q] = k;
	}
	return shift;
}

static inline char *
kmp_string_match(char *text,
		 const char *pattern,
		 size_t text_len,
		 size_t pattern_len)
{
	int *shift = compute_prefix_function(pattern, pattern_len);
	if (shift == NULL)
		return NULL;
	int q = 0, i;
	for (i = 0; i < text_len; ++i)
	{
		while (q > 0 && pattern[q] != text[i])
			q = shift[q - 1];
		if (pattern[q] == text[i])
			++q;
		if (q == pattern_len)
		{
			free(shift);
			return &text[i - pattern_len + 1];
			/*
			 * we just return the first position.
			 */
			/*q = shift[q];*/
		}
	}
	free(shift);
	return NULL;
}

static sds_t *
sds_slice_recursive(char *s, uint64_t len, uint8_t *ascii, int cnt, sds_t *save)
{
	char *jmp_ascii, *end = s + len;
	for (jmp_ascii = s; jmp_ascii < end; ++jmp_ascii)
	{
		if (!ascii[(uint8_t)(*jmp_ascii)])
			break;
		*jmp_ascii = 0;
	}
	if (jmp_ascii >= end)
	{
		sds_t *ret = (sds_t *)malloc(sizeof(sds_t) * cnt);
		if (ret == NULL)
			return NULL;
		ret[cnt - 1].data = (uint8_t *)save;
		ret[cnt - 1].len = 0;
		ret[cnt - 1].data_ref = 0;
		ret[cnt - 1].ref = 1;
		return ret;
	}
	++cnt;
	char *jmp_valid;
	for (jmp_valid = jmp_ascii; jmp_valid < end; ++jmp_valid)
	{
		if (ascii[(uint8_t)(*jmp_valid)])
			break;
	}
	sds_t *array = sds_slice_recursive(
	    jmp_valid, len - (jmp_valid - s), ascii, cnt, save);
	if (array == NULL)
		return NULL;
	array[cnt - 2].data = (uint8_t *)jmp_ascii;
	array[cnt - 2].len = jmp_valid - jmp_ascii;
	array[cnt - 2].data_ref = 1;
	array[cnt - 2].ref = 1;
	return array;
}

static inline sds_t *
sds_assign(char *s, size_t len)
{
	sds_t *str;
	str = (sds_t *)malloc(sizeof(sds_t));
	if (str == NULL)
		return NULL;
	str->data = (uint8_t *)s;
	str->len = len;
	str->data_ref = 1;
	str->ref = 1;
	return str;
}