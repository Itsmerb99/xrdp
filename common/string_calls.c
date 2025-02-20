/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) Jay Sorg 2004-2020
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * generic string handling calls
 */

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>


#include "log.h"
#include "os_calls.h"
#include "string_calls.h"
#include "defines.h"

unsigned int
g_format_info_string(char *dest, unsigned int len,
                     const char *format,
                     const struct info_string_tag map[])
{
    unsigned int result = 0;
    const char *copy_from;  /* Data to add to output */
    unsigned int copy_len;  /* Length of above */
    unsigned int skip;      /* Date to skip over in format string */
    const char *p;
    const struct info_string_tag *m;

    for ( ; *format != '\0'; format += skip)
    {
        if (*format == '%')
        {
            char ch = *(format + 1);
            if (ch == '%')
            {
                /* '%%' in format - replace with single '%' */
                copy_from = format;
                copy_len = 1;
                skip = 2;
            }
            else if (ch == '\0')
            {
                /* Percent at end of string - ignore */
                copy_from = NULL;
                copy_len = 0;
                skip = 1;
            }
            else
            {
                /* Look up the character in the map, assuming failure */
                copy_from = NULL;
                copy_len = 0;
                skip = 2;

                for (m = map ; m->ch != '\0' ; ++m)
                {
                    if (ch == m->ch)
                    {
                        copy_from = m->val;
                        copy_len = strlen(copy_from);
                        break;
                    }
                }
            }
        }
        else if ((p = strchr(format, '%')) != NULL)
        {
            /* Copy up to the next '%' */
            copy_from = format;
            copy_len = p - format;
            skip = copy_len;
        }
        else
        {
            /* Copy the rest of the format string */
            copy_from = format;
            copy_len = strlen(format);
            skip = copy_len;
        }

        /* Update the result before any truncation */
        result += copy_len;

        /* Do we have room in the output buffer for any more data? We
         * must always write a terminator if possible */
        if (len > 1)
        {
            if (copy_len > (len - 1))
            {
                copy_len = len - 1;
            }
            memcpy(dest, copy_from, copy_len);
            dest += copy_len;
            len -= copy_len;
        }
    }

    /* Room for a terminator? */
    if (len > 0)
    {
        *dest = '\0';
    }

    return result;
}

/******************************************************************************/
const char *
g_bool2text(int value)
{
    return value ? "true" : "false";
}

/*****************************************************************************/
int
g_text2bool(const char *s)
{
    if ( (g_atoi(s) != 0) ||
            (0 == g_strcasecmp(s, "true")) ||
            (0 == g_strcasecmp(s, "on")) ||
            (0 == g_strcasecmp(s, "yes")))
    {
        return 1;
    }
    return 0;
}

/*****************************************************************************/
int
g_get_display_num_from_display(const char *display_text)
{
    int rv = -1;
    const char *p;

    /* Skip over the hostname part of the DISPLAY */
    if (display_text != NULL && (p = strchr(display_text, ':')) != NULL)
    {
        ++p; /* Skip the ':' */

        /* Cater for the (still supported) double-colon. See
         * https://www.x.org/releases/X11R7.7/doc/libX11/libX11/libX11.html */
        if (*p == ':')
        {
            ++p;
        }

        /* Check it starts with a digit, to avoid oddities like DISPLAY=":zz.0"
         * being parsed successfully */
        if (isdigit(*p))
        {
            rv = g_atoi(p);
        }
    }

    return rv;
}

/*****************************************************************************/
/* returns length of text */
int
g_strlen(const char *text)
{
    if (text == NULL)
    {
        return 0;
    }

    return strlen(text);
}

/*****************************************************************************/
/* locates char in text */
char *
g_strchr(const char *text, int c)
{
    if (text == NULL)
    {
        return 0;
    }

    /* Cast needed to compile with C++ */
    return (char *)strchr(text, c);
}

/*****************************************************************************/
/* locates char in text */
char *
g_strrchr(const char *text, int c)
{
    if (text == NULL)
    {
        return 0;
    }

    /* Cast needed to compile with C++ */
    return (char *)strrchr(text, c);
}

/*****************************************************************************/
/* locates char in text with length */
char *
g_strnchr(const char *text, int c, int len)
{
    if (text == NULL || len <= 0)
    {
        return NULL;
    }

    return (char *)memchr(text, c, len);
}

/*****************************************************************************/
/* returns dest */
char *
g_strcpy(char *dest, const char *src)
{
    if (src == 0 && dest != 0)
    {
        dest[0] = 0;
        return dest;
    }

    if (dest == 0 || src == 0)
    {
        return 0;
    }

    return strcpy(dest, src);
}

/*****************************************************************************/
/* returns dest */
char *
g_strncpy(char *dest, const char *src, int len)
{
    char *rv;

    if (src == 0 && dest != 0)
    {
        dest[0] = 0;
        return dest;
    }

    if (dest == 0 || src == 0)
    {
        return 0;
    }

    rv = strncpy(dest, src, len);
    dest[len] = 0;
    return rv;
}

/*****************************************************************************/
/* returns dest */
char *
g_strcat(char *dest, const char *src)
{
    if (dest == 0 || src == 0)
    {
        return dest;
    }

    return strcat(dest, src);
}

/*****************************************************************************/
/* returns dest */
char *
g_strncat(char *dest, const char *src, int len)
{
    if (dest == 0 || src == 0)
    {
        return dest;
    }

    return strncat(dest, src, len);
}

/*****************************************************************************/
/* if in = 0, return 0 else return newly alloced copy of in */
char *
g_strdup(const char *in)
{
    int len;
    char *p;

    if (in == 0)
    {
        return 0;
    }

    len = g_strlen(in);
    p = (char *)g_malloc(len + 1, 0);

    if (p != NULL)
    {
        g_strcpy(p, in);
    }

    return p;
}

/*****************************************************************************/
/* if in = 0, return 0 else return newly alloced copy of input string
 * if the input string is larger than maxlen the returned string will be
 * truncated. All strings returned will include null termination*/
char *
g_strndup(const char *in, const unsigned int maxlen)
{
    unsigned int len;
    char *p;

    if (in == 0)
    {
        return 0;
    }

    len = g_strlen(in);

    if (len > maxlen)
    {
        len = maxlen - 1;
    }

    p = (char *)g_malloc(len + 2, 0);

    if (p != NULL)
    {
        g_strncpy(p, in, len + 1);
    }

    return p;
}

/*****************************************************************************/
int
g_strcmp(const char *c1, const char *c2)
{
    return strcmp(c1, c2);
}

/*****************************************************************************/
int
g_strncmp(const char *c1, const char *c2, int len)
{
    return strncmp(c1, c2, len);
}

/*****************************************************************************/
/* compare up to delim */
int
g_strncmp_d(const char *s1, const char *s2, const char delim, int n)
{
    char c1;
    char c2;

    c1 = 0;
    c2 = 0;
    while (n > 0)
    {
        c1 = *(s1++);
        c2 = *(s2++);
        if ((c1 == 0) || (c1 != c2) || (c1 == delim) || (c2 == delim))
        {
            return c1 - c2;
        }
        n--;
    }
    return c1 - c2;
}

/*****************************************************************************/
int
g_strcasecmp(const char *c1, const char *c2)
{
#if defined(_WIN32)
    return stricmp(c1, c2);
#else
    return strcasecmp(c1, c2);
#endif
}

/*****************************************************************************/
int
g_strncasecmp(const char *c1, const char *c2, int len)
{
#if defined(_WIN32)
    return strnicmp(c1, c2, len);
#else
    return strncasecmp(c1, c2, len);
#endif
}

/*****************************************************************************/
int
g_atoi(const char *str)
{
    if (str == 0)
    {
        return 0;
    }

    return atoi(str);
}

/*****************************************************************************/
/* As g_atoi() but allows for hexadecimal too */
int
g_atoix(const char *str)
{
    int base = 10;
    if (str == NULL)
    {
        str = "0";
    }

    while (isspace(*str))
    {
        ++str;
    }

    if (*str == '0' && tolower(*(str + 1)) == 'x')
    {
        str += 2;
        base = 16;
    }
    return strtol(str, NULL, base);
}

/*****************************************************************************/
int
g_htoi(char *str)
{
    int len;
    int index;
    int rv;
    int val;
    int shift;

    rv = 0;
    len = strlen(str);
    index = len - 1;
    shift = 0;

    while (index >= 0)
    {
        val = 0;

        switch (str[index])
        {
            case '1':
                val = 1;
                break;
            case '2':
                val = 2;
                break;
            case '3':
                val = 3;
                break;
            case '4':
                val = 4;
                break;
            case '5':
                val = 5;
                break;
            case '6':
                val = 6;
                break;
            case '7':
                val = 7;
                break;
            case '8':
                val = 8;
                break;
            case '9':
                val = 9;
                break;
            case 'a':
            case 'A':
                val = 10;
                break;
            case 'b':
            case 'B':
                val = 11;
                break;
            case 'c':
            case 'C':
                val = 12;
                break;
            case 'd':
            case 'D':
                val = 13;
                break;
            case 'e':
            case 'E':
                val = 14;
                break;
            case 'f':
            case 'F':
                val = 15;
                break;
        }

        rv = rv | (val << shift);
        index--;
        shift += 4;
    }

    return rv;
}

/*****************************************************************************/
/* returns number of bytes copied into out_str */
int
g_bytes_to_hexstr(const void *bytes, int num_bytes, char *out_str,
                  int bytes_out_str)
{
    int rv;
    int index;
    char *lout_str;
    const tui8 *lbytes;

    rv = 0;
    lbytes = (const tui8 *) bytes;
    lout_str = out_str;
    for (index = 0; index < num_bytes; index++)
    {
        if (bytes_out_str < 3)
        {
            break;
        }
        g_snprintf(lout_str, bytes_out_str, "%2.2x", lbytes[index]);
        lout_str += 2;
        bytes_out_str -= 2;
        rv += 2;
    }
    return rv;
}

/*****************************************************************************/
/* convert a byte array into a hex dump */
char *
g_bytes_to_hexdump(const char *src, int len)
{
    unsigned char *line;
    int i;
    int dump_number_lines;
    int dump_line_length;
    int dump_length;
    int dump_offset;
    int thisline;
    int offset;
    char *dump_buffer;

#define HEX_DUMP_SOURCE_BYTES_PER_LINE (16)
#ifdef _WIN32
#define HEX_DUMP_NEWLINE_SIZE (2)
#else
#ifdef _MACOS
#define HEX_DUMP_NEWLINE_SIZE (1)
#else
#define HEX_DUMP_NEWLINE_SIZE (1)
#endif
#endif

    dump_line_length = (4 + 3             /* = 4 offset + 3 space */
                        + ((2 + 1) * HEX_DUMP_SOURCE_BYTES_PER_LINE)  /* + (2 hex char + 1 space) per source byte */
                        + 2 /* + 2 space */
                        + HEX_DUMP_SOURCE_BYTES_PER_LINE
                        + HEX_DUMP_NEWLINE_SIZE);

    dump_number_lines = (len / HEX_DUMP_SOURCE_BYTES_PER_LINE) + 1; /* +1 to round up */
    dump_length = (dump_number_lines * dump_line_length   /* hex dump lines */
                   + 1);    /* terminating NULL */
    dump_buffer = (char *)g_malloc(dump_length, 1);
    if (dump_buffer == NULL)
    {
        LOG_DEVEL(LOG_LEVEL_WARNING,
                  "Failed to allocate buffer for hex dump of size %d",
                  dump_length);
        return NULL;
    }

    line = (unsigned char *)src;
    offset = 0;
    dump_offset = 0;

    while (offset < len)
    {
        g_sprintf(dump_buffer + dump_offset, "%04x   ", offset);
        dump_offset += 7;
        thisline = len - offset;

        if (thisline > HEX_DUMP_SOURCE_BYTES_PER_LINE)
        {
            thisline = HEX_DUMP_SOURCE_BYTES_PER_LINE;
        }

        for (i = 0; i < thisline; i++)
        {
            g_sprintf(dump_buffer + dump_offset, "%02x ", line[i]);
            dump_offset += 3;
        }

        for (; i < HEX_DUMP_SOURCE_BYTES_PER_LINE; i++)
        {
            dump_buffer[dump_offset++] = ' ';
            dump_buffer[dump_offset++] = ' ';
            dump_buffer[dump_offset++] = ' ';
        }

        dump_buffer[dump_offset++] = ' ';
        dump_buffer[dump_offset++] = ' ';

        for (i = 0; i < thisline; i++)
        {
            dump_buffer[dump_offset++] = (line[i] >= 0x20 && line[i] < 0x7f) ? line[i] : '.';
        }

        for (; i < HEX_DUMP_SOURCE_BYTES_PER_LINE; i++)
        {
            dump_buffer[dump_offset++] = ' ';
        }

#ifdef _WIN32
        dump_buffer[dump_offset++] = '\r';
        dump_buffer[dump_offset++] = '\n';
#else
#ifdef _MACOS
        dump_buffer[dump_offset++] = '\r';
#else
        dump_buffer[dump_offset++] = '\n';
#endif
#endif
        offset += thisline;
        line += thisline;


        if (dump_offset % dump_line_length != 0)
        {
            LOG_DEVEL(LOG_LEVEL_WARNING,
                      "BUG: dump_offset (%d) at the end of a line is not a "
                      "multiple of the line length (%d)",
                      dump_offset, dump_line_length);
        }

    }
    if (dump_offset > dump_length)
    {
        LOG_DEVEL(LOG_LEVEL_WARNING,
                  "BUG: dump_offset (%d) is larger than the dump_buffer length (%d)",
                  dump_offset, dump_length);
        dump_buffer[0] = '\0';
        return dump_buffer;
    }

    /* replace the last new line with the end of the string since log_message
       will add a new line */
    dump_buffer[dump_offset - HEX_DUMP_NEWLINE_SIZE] = '\0';
    return dump_buffer;
}

/*****************************************************************************/
int
g_pos(const char *str, const char *to_find)
{
    const char *pp;

    pp = strstr(str, to_find);

    if (pp == 0)
    {
        return -1;
    }

    return (pp - str);
}

/*****************************************************************************/

char *
g_strstr(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL)
    {
        return NULL;
    }

    /* Cast needed to compile with C++ */
    return (char *)strstr(haystack, needle);
}

/*****************************************************************************/
int
g_mbstowcs(twchar *dest, const char *src, int n)
{
    wchar_t *ldest;
    int rv;

    ldest = (wchar_t *)dest;
    rv = mbstowcs(ldest, src, n);
    return rv;
}

/*****************************************************************************/
int
g_wcstombs(char *dest, const twchar *src, int n)
{
    const wchar_t *lsrc;
    int rv;

    lsrc = (const wchar_t *)src;
    rv = wcstombs(dest, lsrc, n);
    return rv;
}

/*****************************************************************************/
/* returns error */
/* trim spaces and tabs, anything <= space */
/* trim_flags 1 trim left, 2 trim right, 3 trim both, 4 trim through */
/* this will always shorten the string or not change it */
int
g_strtrim(char *str, int trim_flags)
{
    int index;
    int len;
    int text1_index;
    int got_char;
    wchar_t *text;
    wchar_t *text1;

    len = mbstowcs(0, str, 0);

    if (len < 1)
    {
        return 0;
    }

    if ((trim_flags < 1) || (trim_flags > 4))
    {
        return 1;
    }

    text = (wchar_t *)malloc(len * sizeof(wchar_t) + 8);
    text1 = (wchar_t *)malloc(len * sizeof(wchar_t) + 8);
    if (text == NULL || text1 == NULL)
    {
        free(text);
        free(text1);
        return 1;
    }
    text1_index = 0;
    mbstowcs(text, str, len + 1);

    switch (trim_flags)
    {
        case 4: /* trim through */

            for (index = 0; index < len; index++)
            {
                if (text[index] > 32)
                {
                    text1[text1_index] = text[index];
                    text1_index++;
                }
            }

            text1[text1_index] = 0;
            break;
        case 3: /* trim both */
            got_char = 0;

            for (index = 0; index < len; index++)
            {
                if (got_char)
                {
                    text1[text1_index] = text[index];
                    text1_index++;
                }
                else
                {
                    if (text[index] > 32)
                    {
                        text1[text1_index] = text[index];
                        text1_index++;
                        got_char = 1;
                    }
                }
            }

            text1[text1_index] = 0;
            len = text1_index;

            /* trim right */
            for (index = len - 1; index >= 0; index--)
            {
                if (text1[index] > 32)
                {
                    break;
                }
            }

            text1_index = index + 1;
            text1[text1_index] = 0;
            break;
        case 2: /* trim right */

            /* copy it */
            for (index = 0; index < len; index++)
            {
                text1[text1_index] = text[index];
                text1_index++;
            }

            /* trim right */
            for (index = len - 1; index >= 0; index--)
            {
                if (text1[index] > 32)
                {
                    break;
                }
            }

            text1_index = index + 1;
            text1[text1_index] = 0;
            break;
        case 1: /* trim left */
            got_char = 0;

            for (index = 0; index < len; index++)
            {
                if (got_char)
                {
                    text1[text1_index] = text[index];
                    text1_index++;
                }
                else
                {
                    if (text[index] > 32)
                    {
                        text1[text1_index] = text[index];
                        text1_index++;
                        got_char = 1;
                    }
                }
            }

            text1[text1_index] = 0;
            break;
    }

    wcstombs(str, text1, text1_index + 1);
    free(text);
    free(text1);
    return 0;
}

/*****************************************************************************/
char *
g_strnjoin(char *dest, int dest_len, const char *joiner, const char *src[], int src_len)
{
    int len = 0;
    int joiner_len;
    int i = 0;
    int dest_remaining;
    char *dest_pos = dest;
    char *dest_end;

    if (dest == NULL || dest_len < 1)
    {
        return dest;
    }
    if (src == NULL || src_len < 1)
    {
        dest[0] = '\0';
        return dest;
    }

    dest[0] = '\0';
    dest_end = dest + dest_len - 1;
    joiner_len = g_strlen(joiner);
    for (i = 0; i < src_len - 1 && dest_pos < dest_end; i++)
    {
        len = g_strlen(src[i]);
        dest_remaining = dest_end - dest_pos;
        g_strncat(dest_pos, src[i], dest_remaining);
        dest_pos += MIN(len, dest_remaining);

        if (dest_pos < dest_end)
        {
            dest_remaining = dest_end - dest_pos;
            g_strncat(dest_pos, joiner, dest_remaining);
            dest_pos += MIN(joiner_len, dest_remaining);
        }
    }

    if (i == src_len - 1 && dest_pos < dest_end)
    {
        g_strncat(dest_pos, src[i], dest_end - dest_pos);
    }

    return dest;
}

int
g_bitmask_to_str(int bitmask, const struct bitmask_string bitdefs[],
                 char delim, char *buff, int bufflen)
{
    int rlen = 0; /* Returned length */

    if (bufflen <= 0) /* Caller error */
    {
        rlen = -1;
    }
    else
    {
        char *p = buff;
        /* Find the last writeable character in the buffer */
        const char *last = buff + (bufflen - 1);

        const struct bitmask_string *b;

        for (b = &bitdefs[0] ; b->mask != 0; ++b)
        {
            if ((bitmask & b->mask) != 0)
            {
                if (p > buff)
                {
                    /* Not first item - append separator */
                    if (p < last)
                    {
                        *p++ = delim;
                    }
                    ++rlen;
                }

                int slen = g_strlen(b->str);
                int copylen = MIN(slen, last - p);
                g_memcpy(p, b->str, copylen);
                p += copylen;
                rlen += slen;

                /* Remove the bit so we can check for undefined bits later*/
                bitmask &= ~b->mask;
            }
        }

        if (bitmask != 0)
        {
            /* Bits left which aren't named by the user */
            if (p > buff)
            {
                if (p < last)
                {
                    *p++ = delim;
                }
                ++rlen;
            }
            /* This call will terminate the return buffer */
            rlen += g_snprintf(p, last - p + 1, "0x%x", bitmask);
        }
        else
        {
            *p = '\0';
        }
    }

    return rlen;
}
