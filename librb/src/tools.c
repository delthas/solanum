/*
 *  ircd-ratbox: A slightly useful ircd.
 *  tools.c: Various functions needed here and there.
 *
 *  Copyright (C) 1996-2002 Hybrid Development Team
 *  Copyright (C) 2002-2005 ircd-ratbox development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 *
 *
 *  Here is the original header:
 *
 *  Useful stuff, ripped from places ..
 *  adrian chadd <adrian@creative.net.au>
 *
 *  The TOOLS_C define builds versions of the functions in tools.h
 *  so that they end up in the resulting object files.  If its not
 *  defined, tools.h will build inlined versions of the functions
 *  on supported compilers
 */

#define _GNU_SOURCE 1
#include <librb_config.h>
#include <rb_lib.h>
#include <rb_tools.h>


/*
 * init_rb_dlink_nodes
 *
 */
static rb_bh *dnode_heap;
void
rb_init_rb_dlink_nodes(size_t dh_size)
{

	dnode_heap = rb_bh_create(sizeof(rb_dlink_node), dh_size, "librb_dnode_heap");
	if(dnode_heap == NULL)
		rb_outofmemory();
}

/*
 * make_rb_dlink_node
 *
 * inputs	- NONE
 * output	- pointer to new rb_dlink_node
 * side effects	- NONE
 */
rb_dlink_node *
rb_make_rb_dlink_node(void)
{
	return (rb_bh_alloc(dnode_heap));
}

/*
 * free_rb_dlink_node
 *
 * inputs	- pointer to rb_dlink_node
 * output	- NONE
 * side effects	- free given rb_dlink_node
 */
void
rb_free_rb_dlink_node(rb_dlink_node *ptr)
{
	assert(ptr != NULL);
	rb_bh_free(dnode_heap, ptr);
}

/* rb_string_to_array()
 *   Changes a given buffer into an array of parameters.
 *   Taken from ircd-ratbox.
 *
 * inputs	- string to parse, array to put in
 * outputs	- number of parameters
 */
int
rb_string_to_array(char *string, char **parv, int maxpara)
{
	char *p, *xbuf = string;
	int x = 0;

	parv[x] = NULL;

	if(string == NULL || string[0] == '\0')
		return x;

	while(*xbuf == ' ')	/* skip leading spaces */
		xbuf++;
	if(*xbuf == '\0')	/* ignore all-space args */
		return x;

	do
	{
		if(*xbuf == ':')	/* Last parameter */
		{
			xbuf++;
			parv[x++] = xbuf;
			parv[x] = NULL;
			return x;
		}
		else
		{
			parv[x++] = xbuf;
			parv[x] = NULL;
			if((p = strchr(xbuf, ' ')) != NULL)
			{
				*p++ = '\0';
				xbuf = p;
			}
			else
				return x;
		}
		while(*xbuf == ' ')
			xbuf++;
		if(*xbuf == '\0')
			return x;
	}
	while(x < maxpara - 1);

	if(*p == ':')
		p++;

	parv[x++] = p;
	parv[x] = NULL;
	return x;
}

#ifndef HAVE_STRCASECMP
#ifndef _WIN32
/* Crummy fallback impl by me. --Elizafox */
int
rb_strcasecmp(const char *s1, const char *s2)
{
	for(; *s1 != '\0' || *s2 != '\0'; s1++, s2++)
	{
		if(tolower(*s1) != tolower(*s2))
			return tolower(*s1) - tolower(*s2);
	}

	if(*s1 == '\0' && *s2 == '\0')
		return 0;

	return tolower(*(--s1)) - tolower(*(--s2));
}
#else /* _WIN32 */
int
rb_strcasecmp(const char *s1, const char *s2)
{
	return stricmp(s1, s2);
}
#endif /* _WIN32 */
#else /* HAVE_STRCASECMP */
int
rb_strcasecmp(const char *s1, const char *s2)
{
	return strcasecmp(s1, s2);
}
#endif

#ifndef HAVE_STRNCASECMP
#ifndef _WIN32
/* Crummy fallback impl by me. --Elizafox */
int
rb_strncasecmp(const char *s1, const char *s2, size_t n)
{
	if(n == 0)
		return 0;

	for(; *s1 != '\0' || *s2 != '\0' || n; s1++, s2++, n--)
	{
		if(tolower(*s1) != tolower(*s2))
			return tolower(*s1) - tolower(*s2);
	}

	if(*s1 == '\0' && *s2 == '\0')
		return 0;

	return tolower(*(--s1)) - tolower(*(--s2));
}
#else /* _WIN32 */
int
rb_strncasecmp(const char *s1, const char *s2, size_t n)
{
	return strnicmp(s1, s2, n);
}
#endif /* _WIN32 */
#else /* HAVE_STRNCASECMP */
int
rb_strncasecmp(const char *s1, const char *s2, size_t n)
{
	return strncasecmp(s1, s2, n);
}
#endif

#ifndef HAVE_STRCASESTR
/* Crummy fallback impl by me. --Elizafox */
char *
rb_strcasestr(const char *s, const char *find)
{
	size_t len_f = strlen(find);

	if(*s == '\0')
		return (char *)s;

	for(char *c = (char *)s; *c != '\0'; c++)
	{
		if(*c == *find && strncasecmp(c, find, len_f) == 0)
			return c;
	}

	return NULL;
}
#else
char *
rb_strcasestr(const char *s, const char *find)
{
	return strcasestr(s, find);
}
#endif

#ifndef HAVE_STRLCAT
size_t
rb_strlcat(char *dest, const char *src, size_t count)
{
	size_t dsize = strlen(dest);
	size_t len = strlen(src);
	size_t res = dsize + len;

	dest += dsize;
	count -= dsize;
	if(len >= count)
		len = count - 1;
	memcpy(dest, src, len);
	dest[len] = 0;
	return res;
}
#else
size_t
rb_strlcat(char *dest, const char *src, size_t count)
{
	return strlcat(dest, src, count);
}
#endif

#ifndef HAVE_STRLCPY
size_t
rb_strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if(size)
	{
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}
#else
size_t
rb_strlcpy(char *dest, const char *src, size_t size)
{
	return strlcpy(dest, src, size);
}
#endif


#ifndef HAVE_STRNLEN
size_t
rb_strnlen(const char *s, size_t count)
{
	const char *sc;
	for(sc = s; count-- && *sc != '\0'; ++sc)
		;
	return sc - s;
}
#else
size_t
rb_strnlen(const char *s, size_t count)
{
	return strnlen(s, count);
}
#endif

/*
 * rb_snprintf_append()
 * appends snprintf formatted string to the end of the buffer but not
 * exceeding len
 */
int
rb_snprintf_append(char *str, size_t len, const char *format, ...)
{
	if(len == 0)
		return 0;

	size_t x = strlen(str);

	if(len < x)
	{
		str[len - 1] = '\0';
		return (int)len - 1;
	}

	va_list ap;
	va_start(ap, format);
	int y = (vsnprintf(str + x, len - x, format, ap) + (int)x);
	va_end(ap);

	return (y);
}

/* rb_basename
 *
 * input        -
 * output       -
 * side effects -
 */
char *
rb_basename(const char *path)
{
        const char *s;

        if(!(s = strrchr(path, '/')))
                s = path;
        else
                s++;
	return rb_strdup(s);
}

/*
 * rb_dirname
 */

char *
rb_dirname (const char *path)
{
	char *s;

	s = strrchr(path, '/');
	if(s == NULL)
	{
		return rb_strdup(".");
	}

	/* remove extra slashes */
	while(s > path && *s == '/')
		--s;

	return rb_strndup(path, ((uintptr_t)s - (uintptr_t)path) + 2);
}

size_t rb_zstring_serialized(rb_zstring_t *zs, void **buf, size_t *buflen)
{
        uint8_t *p;
        size_t alloclen = sizeof(uint16_t) + zs->len;

        p = rb_malloc(sizeof(alloclen));
        memcpy(p, &zs->len, sizeof(uint16_t));
        p += sizeof(uint16_t);
        memcpy(p, zs->data, zs->len);
        return alloclen;
}

size_t rb_zstring_deserialize(rb_zstring_t *zs, void *buf)
{
	uint8_t *p = (uint8_t *)buf;

	memcpy(&zs->len, p, sizeof(uint16_t));
	p += sizeof(uint16_t);
	if(zs->len == 0)
	{
		zs->data = NULL;
		return sizeof(uint16_t);
	}
	zs->data = rb_malloc(zs->len);
	memcpy(zs->data, p, zs->len);
	return zs->len + sizeof(uint16_t);
}

void rb_zstring_free(rb_zstring_t *zs)
{
	rb_free(zs->data);
	rb_free(zs);

}

rb_zstring_t *rb_zstring_alloc(void)
{
	rb_zstring_t *zs = rb_malloc(sizeof(rb_zstring_t));
	return zs;
}

rb_zstring_t *rb_zstring_from_c_len(const char *buf, size_t len)
{
	rb_zstring_t *zs;

	if(len > UINT16_MAX-1)
		return NULL;

	zs = rb_zstring_alloc();
	zs->alloclen = zs->len = (uint16_t)len;
	zs->alloclen = (uint16_t)len;
	if(zs->alloclen < 128)
		zs->alloclen = 128;
	zs->data = rb_malloc(zs->alloclen);
	memcpy(zs->data, buf, zs->len);
	return(zs);
}

rb_zstring_t *rb_zstring_from_c(const char *buf)
{
	return rb_zstring_from_c_len(buf, strlen(buf));
}

size_t rb_zstring_len(rb_zstring_t *zs)
{
	return zs->len;
}

void rb_zstring_append_from_zstring(rb_zstring_t *dst_zs, rb_zstring_t *src_zs)
{
	void *ep;
	size_t nlen = dst_zs->len + src_zs->len;

	if(nlen > dst_zs->alloclen)
	{
		dst_zs->alloclen += src_zs->len + 64;
		dst_zs->data = rb_realloc(dst_zs->data, dst_zs->alloclen);
	}

	ep = dst_zs->data + dst_zs->len;
	memcpy(ep, src_zs->data, src_zs->len);
}

void rb_zstring_append_from_c(rb_zstring_t *zs, const char *buf, size_t len)
{
	void *ep;
	size_t nlen = zs->len + len;

	if(nlen > zs->alloclen)
	{
		zs->alloclen += len + 64;
		zs->data = rb_realloc(zs->data, zs->alloclen);
	}
	ep = zs->data + zs->len;
	zs->len += len;
	memcpy(ep, buf, len);
}

char *rb_zstring_to_c(rb_zstring_t *zs, char *buf, size_t len)
{
        size_t cpylen;
        if(len < zs->len)
                cpylen = len - 1;
        else
                cpylen = zs->len;
        buf[cpylen] = '\0';
        memcpy(buf, zs->data, cpylen);
        return buf;
}


char *rb_zstring_to_c_alloc(rb_zstring_t *zs)
{
	char *p;
	p = rb_malloc(zs->len+1);
	memcpy(p, zs->data, zs->len);
	return p;
}

size_t rb_zstring_to_ptr(rb_zstring_t *zs, void **ptr)
{
	*ptr = (void *)zs->data;
	return zs->len;
}
