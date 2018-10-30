/*
ABOUT:
    A small bit array implementation


LICENSE:

    The MIT License (MIT)

    Copyright (c) 2018 Mathias Westerdahl

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


DISCLAIMER:

    This software is supplied "AS IS" without any warranties and support

*/

#pragma once

#include <stdint.h>

typedef struct Bitarray
{
    uint8_t*    data;
    uint32_t    data_size;
    uint32_t    num_bits;
} Bitarray;


Bitarray*   ba_alloc(uint32_t num_bytes);
int         ba_free(Bitarray* array);

// returns number of free bits
uint32_t    ba_get_num_free_bits(Bitarray* array);

void        ba_write(Bitarray* array, uint32_t value, uint32_t num_bits);
uint32_t    ba_read(Bitarray* array, uint32_t value, uint32_t num_bits);

#if !defined(JCBA_MALLOC)
    #include <malloc.h>
    #define JCBA_MALLOC malloc
#endif

#if defined(JC_BITARRAY_IMPLEMENTATION)

Bitarray* ba_alloc(uint32_t num_bytes)
{
    Bitarray* ba = (Bitarray*)malloc(sizeof(Bitarray) + num_bytes);
    ba.data = (uint8_t*)ba + sizeof(Bitarray);
    ba.data_size = num_bytes;
    ba.num_bits = 0;
    return ba;
}

void ba_free(Bitarray* ba)
{
    free(ba);
}


void ba_write(Bitarray* ba, uint32_t value, uint32_t num_bits)
{
    assert(ba->num_bits + num_bits <= ba->data_size*8);

    uint32_t remainder_bits = ba->num_bits & 7;
    if (remainder_bits) // If the target buffer is unaligned
    {
        value &= 0xFFFFFFFF >> (32 - num_bits); // zero out the top bits
        uint32_t byteoffset = ba->num_bits / 8;
        uint32_t bitoffset = ba->num_bits - byteoffset * 8;

        // store the first bits, until the target buffer is aligned again
        for(uint32_t i = 0; i < remainder_bits; ++i)
        {
            ba->data[offset] |= (value & 1) << (bitoffset + i);
            value >>= 1;
        }

        // Now, the target buffer is aligned

    }
    else // aligned
    {
        if (num_bits == 8)
        {
            uint8_t* dest = (uint8_t*)&ba->data[ba->num_bits/8];
            *dest = (uint8_t)(value & 0xFF);
            ba->num_bits += 8;
        }
        else if (num_bits == 16)
        {
            uint16_t* dest = (uint16_t*)&ba->data[ba->num_bits/8];
            *dest = (uint16_t)(value & 0xFFFF);
            ba->num_bits += 16;
        }
        else if (num_bits == 32)
        {
            uint32_t* dest = (uint32_t*)&ba->data[ba->num_bits/8];
            *dest = value;
            ba->num_bits += 32;
        }
    }
}




#endif // JC_BITARRAY_IMPLEMENTATION
