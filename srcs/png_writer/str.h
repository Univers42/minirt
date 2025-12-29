/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:50:30 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:50:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STR_H
# define STR_H
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

static inline void string_cleanup(char** out) {
  free(*out);
  *out = NULL;
}


static inline char* alloc_string(const char* in) {
  size_t insize = strlen(in);
  char* out = (char*)malloc(insize + 1);
  if(out) {
    size_t i;
    for(i = 0; i != insize; ++i) {
      out[i] = in[i];
    }
    out[i] = 0;
  }
  return out;
}
#endif