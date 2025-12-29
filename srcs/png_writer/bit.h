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


#endif