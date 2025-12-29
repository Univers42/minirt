/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bit.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:52:08 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:52:08 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIT_H
# define BIT_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "ucvector.h"
# include "uivector.h"

static inline unsigned lodepng_read32bitInt(const unsigned char* buffer)
{
  return (unsigned)((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);
}

static inline void lodepng_set32bitInt(unsigned char* buffer, unsigned value) {
  buffer[0] = (unsigned char)((value >> 24) & 0xff);
  buffer[1] = (unsigned char)((value >> 16) & 0xff);
  buffer[2] = (unsigned char)((value >>  8) & 0xff);
  buffer[3] = (unsigned char)((value      ) & 0xff);
}

static inline void lodepng_add32bitInt(ucvector* buffer, unsigned value) {
  ucvector_resize(buffer, buffer->size + 4); 
  lodepng_set32bitInt(&buffer->data[buffer->size - 4], value);
}

static inline void addBitToStream(ucvector* bitstream, size_t* bitpointer, unsigned bit)
{
    if(((*bitpointer) & 7) == 0)
        ucvector_push_back(bitstream, (unsigned char)0);

    bitstream->data[bitstream->size - 1] |= (bit << ((*bitpointer) & 0x7));
    ++(*bitpointer);
}

static void addBitsToStream(size_t* bitpointer, ucvector* bitstream, unsigned value, size_t nbits) {
  size_t i;
  for(i = 0; i != nbits; ++i) addBitToStream(bitpointer, bitstream, (unsigned char)((value >> i) & 1));
}

static void addBitsToStreamReversed(size_t* bitpointer, ucvector* bitstream, unsigned value, size_t nbits) {
  size_t i;
  for(i = 0; i != nbits; ++i) addBitToStream(bitpointer, bitstream, (unsigned char)((value >> (nbits - 1 - i)) & 1));
}

static inline unsigned char readBit(size_t bitpointer, const unsigned char* bitstream)
{
    return (bitstream[bitpointer >> 3] >> (bitpointer & 0x7)) & (unsigned char)1;
}

static unsigned char readBitFromStream(size_t* bitpointer, const unsigned char* bitstream) {
  unsigned char result = (unsigned char)(READBIT(*bitpointer, bitstream));
  ++(*bitpointer);
  return result;
}

static unsigned readBitsFromStream(size_t* bitpointer, const unsigned char* bitstream, size_t nbits) {
  unsigned result = 0, i;
  for(i = 0; i != nbits; ++i) {
    result += ((unsigned)READBIT(*bitpointer, bitstream)) << i;
    ++(*bitpointer);
  }
  return result;
}

#endif