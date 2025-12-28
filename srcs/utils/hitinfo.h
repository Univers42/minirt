/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hitinfo.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 15:05:53 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 15:05:53 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITINFO_H
# define HITINFO_H
# include "vec3.h"
# include "point3.h"
# include <stdlib.h>
# include <string.h>

typedef struct s_hit_info
{
	double		d;
	t_point3	point;
	t_vec3		normal;
	t_vec3		direction;
}	t_hit_info;

/* constructors / helpers */
t_hit_info *hit_info_new(t_vec3 normal, t_vec3 direction, t_point3 point, double distance);
t_hit_info *hit_info_simple(void);
void        hit_info_init(t_hit_info *dst, const t_hit_info *src);
t_hit_info *hit_info_copy(const t_hit_info *src);
void        hit_info_free(t_hit_info *h);

/* implementations */
static inline t_hit_info *hit_info_new(t_vec3 normal, t_vec3 direction, t_point3 point, double distance)
{
    t_hit_info *h = malloc(sizeof(*h));
    if (!h) return NULL;
    h->normal = normal;
    h->direction = direction;
    h->point = point;
    h->d = distance;
    return h;
}

static inline t_hit_info *hit_info_simple(void)
{
	/* use available initializers */
	return hit_info_new(vec3_init(0.0,0.0,0.0), vec3_init(0.0,0.0,0.0), point3_new_coord(0.0,0.0,0.0), 0.0);
}

/* reduced-argument initializer: copy from src into dst */
static inline void hit_info_init(t_hit_info *dst, const t_hit_info *src)
{
    if (!dst || !src) return;
    dst->normal = src->normal;
    dst->direction = src->direction;
    dst->point = src->point;
    dst->d = src->d;
}

static inline t_hit_info *hit_info_copy(const t_hit_info *src)
{
    if (!src) return NULL;
    t_hit_info *h = malloc(sizeof(*h));
    if (!h) return NULL;
    memcpy(h, src, sizeof(*h));
    return h;
}

static inline void hit_info_free(t_hit_info *h)
{
    if (!h) return;
    free(h);
}

# endif