/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mesh_accel_internal.h"

static __thread const t_mesh_accel	*g_current_mesh = NULL;

void	set_current_mesh_accel(const void *obj)
{
	g_current_mesh = (const t_mesh_accel *)obj;
}

/* Two-step wrapper hit: traverse the mesh-local BVH stashed by set_current,
   testing triangles directly (no per-node/per-primitive indirect calls). */
bool	mesh_accel_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_mesh_accel	*m;
	t_mesh_ray			mr;
	t_mesh_trav			tv;

	m = g_current_mesh;
	if (!m || m->node_count == 0)
		return (false);
	mr = mesh_make_ray(r);
	tv = (t_mesh_trav){r, rayt, rec, false};
	mesh_accel_walk(m, &mr, &tv);
	return (tv.hit);
}

/* Ready-made scene wrapper: one primitive standing for the whole mesh. */
t_hittable_wrapper	mesh_accel_wrapper(t_mesh_accel *ma)
{
	t_hittable_wrapper	w;

	w.object = ma;
	w.owned = false;
	w.set_current = set_current_mesh_accel;
	w.hit_noobj = mesh_accel_hit_noobj;
	w.bbox = ma->bbox;
	return (w);
}
