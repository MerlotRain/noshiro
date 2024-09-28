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

#ifndef __MINGC_GSTRING_H__
#define __MINGC_GSTRING_H__

#include <stdint.h>

typedef struct {
	uint8_t *data;
	size_t len;
	uint32_t data_ref : 1; // data_ref is used to track whether the data is
			       // owned by the sds
	uint32_t ref : 31; // ref is used to track the number of references to
			   // the data
} sds_t;

#define sds(s) {(uint8_t *)s, sizeof(s) - 1, 1, 1}
sds_t *sds_set(sds_t *str, char *s);
sds_t *sds_nset(sds_t *str, char *s, uint64_t n);
sds_t *sds_ref(sds_t *s);
void sds_free(sds_t *s);

sds_t *sds_new(const char *s);

sds_t *sds_buf_new(unsigned char *buf, uint64_t len);
sds_t *sds_dup(sds_t *str);
sds_t *sds_alloc(size_t size);
sds_t *sds_const_ndup(char *str, size_t size);
sds_t *sds_ref_dup(sds_t *str);
sds_t *sds_const_ref_dup(char *s);
sds_t *sds_concat(sds_t *s1, sds_t *s2, sds_t *sep);

int sds_strseqcmp(sds_t *s1, sds_t *s2);
int sds_strcmp(sds_t *s1, sds_t *s2);
int sds_const_strcmp(sds_t *s1, char *s2);
int sds_strncmp(sds_t *s1, sds_t *s2, uint32_t n);
int sds_const_strncmp(sds_t *s1, char *s2, uint32_t n);
int sds_strcasecmp(sds_t *s1, sds_t *s2);
int sds_const_strcasecmp(sds_t *s1, char *s2);
int sds_const_strncasecmp(sds_t *s1, char *s2, uint32_t n);
int sds_strncasecmp(sds_t *s1, sds_t *s2, uint32_t n);
char *sds_strstr(sds_t *text, sds_t *pattern);
char *sds_const_strstr(sds_t *text, char *pattern);

sds_t *sds_new_strstr(sds_t *text, sds_t *pattern);
sds_t *sds_new_const_strstr(sds_t *text, char *pattern);

char *sds_kmp(sds_t *text, sds_t *pattern);
char *sds_const_kmp(sds_t *text, char *pattern);

sds_t *sds_new_kmp(sds_t *text, sds_t *pattern);
sds_t *sds_new_const_kmp(sds_t *text, char *pattern);

sds_t *sds_slice(sds_t *s, const char *sep_array);
void sds_slice_free(sds_t *array);
sds_t *sds_strcat(sds_t *s1, sds_t *s2);
sds_t *sds_trim(sds_t *s, sds_t *mask);
void sds_upper(sds_t *s);
void sds_lower(sds_t *s);

#endif /* __MINGC_GSTRING_H__ */