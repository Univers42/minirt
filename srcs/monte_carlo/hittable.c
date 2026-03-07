/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:08:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:17:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"
#include <stdlib.h>

/* set_face_normal: outward_normal is assumed unit length. */
void	set_face_normal(t_hit_record *hit, const t_ray *r,
		const t_vec3 *outward_normal)
{
	bool	front;

	front = (dot(&r->dir, outward_normal) < 0.0);
	hit->front_face = front;
	if (front)
		hit->normal = *outward_normal;
	else
		hit->normal = vec3_neg(outward_normal);
}

t_vec3	rotate_y_vec(const t_vec3 *v, real_t sin_t, real_t cos_t)
{
	return (vec3_create(
			cos_t * v->x + sin_t * v->z,
			v->y,
			-sin_t * v->x + cos_t * v->z));
}

/* translate wrapper */
static __thread const t_translate_wrap	*g_current_translate = NULL;

void	set_current_translate(const void *obj)
{
	g_current_translate = (const t_translate_wrap *)obj;
}

bool	translate_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_translate_wrap	*tr;
	t_vec3					neg_off;
	t_ray					moved;

	tr = g_current_translate;
	if (!tr)
		return (false);
	neg_off = vec3_neg(&tr->offset);
	moved = ray_create(vec3_add(&r->orig, &neg_off), r->dir, r->tm);
	if (!tr->child.set_current || !tr->child.hit_noobj)
		return (false);
	tr->child.set_current(tr->child.object);
	if (!tr->child.hit_noobj(&moved, rayt, rec))
		return (false);
	rec->p = vec3_add(&rec->p, &tr->offset);
	set_face_normal(rec, &moved, &rec->normal);
	return (true);
}

t_translate_wrap	*translate_create(const t_hittable_wrapper *child,
		const t_vec3 *offset)
{
	t_translate_wrap	*tr;

	if (!child || !offset)
		return (NULL);
	tr = (t_translate_wrap *)malloc(sizeof(t_translate_wrap));
	if (!tr)
		return (NULL);
	tr->child = *child;
	tr->offset = *offset;
	tr->bbox = aabb_add_vec3(&child->bbox, offset);
	return (tr);
}
