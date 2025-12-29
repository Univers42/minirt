/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ucvector.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:14:47 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:14:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UCVECTOR_H
# define UCVECTOR_H

typedef struct s_ucvector
{
	unsigned char	*data;
	size_t			size;
	size_t			allocsize;
}	t_ucvector;

static inline unsigned ucvector_reserve(t_ucvector *p, size_t allocsize)
{
	size_t	newsize;
	void	*data;

	if (allocsize <= p->allocsize)
		return (1);
	if (allocsize > p->allocsize * 2)
		newsize = allocsize;
	else
		newsize = allocsize * 3 / 2;
	data = realloc(p->data, newsize);
	if (!data)
		return (0);
	
}

static inline unsigned ucvector_resize(ucvector* p, size_t size) {
  if(!ucvector_reserve(p, size * sizeof(unsigned char))) return 0;
  p->size = size;
  return 1; 
}

static inline void ucvector_cleanup(void* p) {
  ((ucvector*)p)->size = ((ucvector*)p)->allocsize = 0;
  lodepng_free(((ucvector*)p)->data);
  ((ucvector*)p)->data = NULL;
}

static inline void ucvector_init(ucvector* p) {
  p->data = NULL;
  p->size = p->allocsize = 0;
}


static inline void ucvector_init_buffer(ucvector* p, unsigned char* buffer, size_t size)
{
  p->data = buffer;
  p->allocsize = p->size = size;
}

static inline unsigned ucvector_push_back(ucvector* p, unsigned char c)
{
  if(!ucvector_resize(p, p->size + 1)) return 0;
  p->data[p->size - 1] = c;
  return 1;
}
