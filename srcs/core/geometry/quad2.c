/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:22:57 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"

static __thread const t_quad	*g_current_quad = NULL;

void	set_current_quad(const void *obj)
{
	g_current_quad = (const t_quad *)obj;
}

bool	quad_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_quad)
		return (false);
	return (quad_hit(g_current_quad, r, rayt, rec));
}

static void	box_add_face(t_hittable_list *world, const t_quad_face *face,
		t_material *mat)
{
	t_quad		q;
	t_quad		*cpy;
	t_nonowned	no;

	q = quad_create(&face->pos, &face->u, &face->v, mat);
	cpy = (t_quad *)malloc(sizeof(t_quad));
	if (cpy)
	{
		*cpy = q;
		no = (t_nonowned){cpy, set_current_quad, quad_hit_noobj, &q.bbox};
		hittable_list_add_nonowned(world, &no);
	}
}

void	box(t_hittable_list *world, const t_point3 *a,
		const t_point3 *b, t_material *mat)
{
	t_point3	mn;
	t_point3	mx;
	t_vec3		d[3];

	if (!world || !a || !b || !mat)
		return ;
	mn = point3_create(fmin(a->x, b->x), fmin(a->y, b->y), fmin(a->z, b->z));
	mx = point3_create(fmax(a->x, b->x), fmax(a->y, b->y), fmax(a->z, b->z));
	d[0] = vec3_create(mx.x - mn.x, 0.0, 0.0);
	d[1] = vec3_create(0.0, mx.y - mn.y, 0.0);
	d[2] = vec3_create(0.0, 0.0, mx.z - mn.z);
	box_add_face(world, &(t_quad_face){{mn.x, mn.y, mx.z}, d[0], d[1]}, mat);
	box_add_face(world, &(t_quad_face){{mx.x, mn.y, mx.z},
		vec3_neg(&d[2]), d[1]}, mat);
	box_add_face(world, &(t_quad_face){{mx.x, mn.y, mn.z},
		vec3_neg(&d[0]), d[1]}, mat);
	box_add_face(world, &(t_quad_face){{mn.x, mn.y, mn.z}, d[2], d[1]}, mat);
	box_add_face(world, &(t_quad_face){{mn.x, mx.y, mx.z}, d[0],
		vec3_neg(&d[2])}, mat);
	box_add_face(world, &(t_quad_face){{mn.x, mn.y, mn.z}, d[0], d[2]}, mat);
}

void	box_create_list(const t_point3 *a, const t_point3 *b,
		t_material *mat, t_hittable_list *out)
{
	if (!out || !a || !b || !mat)
		return ;
	hittable_list_init(out);
	box(out, a, b, mat);
}
