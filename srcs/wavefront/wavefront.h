/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wavefront.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 00:35:00 by marvin            #+#    #+#             */
/*   Updated: 2025/12/30 00:35:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WAVEFRONT_H
# define WAVEFRONT_H

# include <setjmp.h>
# include <stddef.h>
# include <stdint.h>
# include <string.h>

# define new(a, n, t)	(t *)alloc(a, n, sizeof(t), _Alignof(t))
# define S(s)			(Str){s, sizeof(s) - 1}

typedef struct s_arena
{
	char	*beg;
	char	*end;
	jmp_buf	*oom;
}t_arena;

typedef struct s_str
{
	char		*data;
	ptrdiff_t	len;
}	t_str;


typedef struct s_cut
{
	t_str	head;
	t_str	tail;
}	t_cut;

typedef struct s_vert
{
	float[3];
}	t_vert;

typedef struct s_face
{
	int32_t	v[3];
	int32_t	n[3];
}t_face;

typedef struct s_model
{
	t_vert		*verts;
	ptrdiff_t	nverts;
	t_vert		*norms;
	ptrdiff_t	nnorms;
	t_face		*faces;
	ptrdiff_t	nfaces;
}	t_model;

typedef struct
{
    Vert n, v;
}	N3FV3F[3];

typedef struct {
    N3FV3F   *data;
    ptrdiff_t len;
} N3FV3Fs;

static void *alloc(t_arena *a, ptrdiff_t count, ptrdiff_t size, ptrdiff_t align)
{
	ptrdiff_t	pad;

	pad = (uintptr_t)a->end & (align - 1);
	if (count >= (a->end - a->beg - pad) / size)
		longjmp(*a->oom, 1);
	return (memset(a->end -= pad + count * size, 0, count * size));
}

#endif