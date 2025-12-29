/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uivector.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:44:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:44:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UIVECTOR_H
# define UIVECTOR_H

typedef struct s_uivector
{
	unsigned	*data;
	size_t		size;
	size_t		allocsize;
}	t_uivector;

static void uivector_cleanup(void* p) {
  ((uivector*)p)->size = ((uivector*)p)->allocsize = 0;
  lodepng_free(((uivector*)p)->data);
  ((uivector*)p)->data = NULL;
}


static unsigned uivector_reserve(uivector* p, size_t allocsize) {
  if(allocsize > p->allocsize) {
    size_t newsize = (allocsize > p->allocsize * 2) ? allocsize : (allocsize * 3 / 2);
    void* data = lodepng_realloc(p->data, newsize);
    if(data) {
      p->allocsize = newsize;
      p->data = (unsigned*)data;
    }
    else return 0; 
  }
  return 1;
}


static unsigned uivector_resize(uivector* p, size_t size) {
  if(!uivector_reserve(p, size * sizeof(unsigned))) return 0;
  p->size = size;
  return 1; 
}


static unsigned uivector_resizev(uivector* p, size_t size, unsigned value) {
  size_t oldsize = p->size, i;
  if(!uivector_resize(p, size)) return 0;
  for(i = oldsize; i < size; ++i) p->data[i] = value;
  return 1;
}

static void uivector_init(uivector* p) {
  p->data = NULL;
  p->size = p->allocsize = 0;
}

static unsigned uivector_push_back(uivector* p, unsigned c) {
  if(!uivector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}
# endif