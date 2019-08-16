#include <stdlib.h>
#include <string.h>

#include "xstring.h"

static void ensure_capacity(xstr_t* xs, size_t size)
{
    size_t new_cap = xs->capacity * 2;
    void* new_data;

    size += xs->size + 1;   /* leave 1 null-terminated */
    if (size > xs->capacity)
    {
        if (new_cap < size)
        {
            new_cap = size;
        }
        new_data = realloc(xs->data, new_cap * sizeof(xchar));
        if (new_data)
        {
            xs->data = (xchar*)new_data;
        }
        /* ignore realloc failed */
    }
}

xstr_t* xstr_new(size_t capacity)
{
    xstr_t* r = malloc(sizeof(xstr_t));

    if (r)
    {
        r->size = 0;
        r->capacity = capacity > 0 ? capacity : XSTR_DEFAULT_CAPACITY;
        r->data = malloc(r->capacity * sizeof(xchar));
        r->data[0] = (xchar)'\0';
    }

    return r;
}

xstr_t* xstr_new_with(const char* cstr, int size)
{
    xstr_t* r = malloc(sizeof(xstr_t));

    if (r)
    {
        if (size < 0)
        {
            for (size = 0; cstr[size]; ++size) { }
        }

        r->size = size;
        r->capacity = size + 1;
        r->data = malloc(r->capacity * sizeof(xchar));
        r->data[size] = (xchar)'\0';

        memcpy(r->data, cstr, size);
    }

    return r;
}

void xstr_free(xstr_t* xs)
{
    if (xs)
    {
        free(xs->data);
        free(xs);
    }
}

void xstr_clear(xstr_t* xs)
{
    xs->size = 0;
    xs->data[0] = (xchar)'\0';
}

void xstr_append(xstr_t* xs, const xchar* cstr, int size)
{
    if (size < 0)
    {
        for (size = 0; cstr[size]; ++size) { }
    }

    ensure_capacity(xs, size);
    memcpy(xs->data + xs->size, cstr, size * sizeof(xchar));
    xs->size += size;
    xs->data[xs->size] = (xchar)'\0';
}

void xstr_append_at(xstr_t* xs, int pos, const xchar* cstr, int size)
{
    if (pos > xs->size)
    {
        pos = xs->size;
    }
    else if (pos < 0) pos = 0;

    xs->size = pos;
    xstr_append(xs, cstr, size);
}

void xstr_assign(xstr_t* xs, const xchar* cstr, int size)
{
    xs->size = 0;
    xstr_append(xs, cstr, size);
}

void xstr_push_back(xstr_t* xs, xchar ch)
{
    ensure_capacity(xs, 1);
    xs->data[xs->size++] = ch;
    xs->data[xs->size] = (xchar)'\0';
}

xchar xstr_pop_back(xstr_t* xs)
{
    xchar c = xs->data[--xs->size];

    xs->data[xs->size] = (xchar)'\0';
    return c;
}

void xstr_erase(xstr_t* xs, int pos, int count)
{
    int endpos = pos + count;

    if (pos > 0 && pos < xs->size)
    {
        if (count < 0 || endpos >= xs->size)
        {
            xs->size = pos;
            xs->data[pos] = (xchar)'\0';
        }
        else memmove(xs->data + pos, xs->data + endpos, xs->size - endpos);
    }
}

static const char i2c[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

char* uitoa(char* buf, unsigned int val, int radix)
{
    int off = 0;
    unsigned int v = val;

    if (!v) { ++off; }
    else do
    {
        v /= radix;
        ++off;
    } while (v);

    buf[off] = '\0';
    while (off)
    {
        buf[--off] = i2c[val % radix];
        val /= radix;
    }

    return buf;
}

char* uctoa_hex(char buf[2], unsigned char val)
{
    buf[0] = i2c[val >> 4];
    buf[1] = i2c[val & 15];

    return buf;
}

static const char c2i[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*0~15*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*16~31*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*32~47*/
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, /*48~63*/
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, /*64~79*/
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, /*80~95*/
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, /*64~79*/
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, /*112~127*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

unsigned int atoui(const char* str, int slen, int base)
{
    unsigned int acc = 0;
    int v;

    while (slen-- > 0)
    {
        v = c2i[(unsigned char)*str++];

        if (v < 0 || v > base) break;

        acc = acc * base + v;
    }

    return acc;
}

unsigned char atouc_hex(const char str[2])
{
    return (c2i[(unsigned char)str[0]] << 4) | (c2i[(unsigned char)str[1]]);
}