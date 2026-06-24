/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_select.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include <stdio.h>

bool	add_scene_lights(t_hittable_list *world, const t_scene *sc);

/* Position field of a parsed object (every primitive has one). */
t_point3	*obj_pos(t_rt_object *o)
{
	if (o->type == OBJ_SPHERE)
		return (&o->data.sphere.center);
	if (o->type == OBJ_PLANE)
		return (&o->data.plane.point);
	if (o->type == OBJ_CYLINDER)
		return (&o->data.cylinder.center);
	if (o->type == OBJ_CONE)
		return (&o->data.cone.apex);
	if (o->type == OBJ_QUAD)
		return (&o->data.quad.position);
	if (o->type == OBJ_MESH)
		return (&o->data.mesh.position);
	if (o->type == OBJ_TRIANGLE)
		return (&o->data.triangle.v0);
	if (o->type == OBJ_DISK)
		return (&o->data.disk.center);
	if (o->type == OBJ_PARABOLOID)
		return (&o->data.paraboloid.vertex);
	if (o->type == OBJ_HYPERBOLOID)
		return (&o->data.hyperboloid.center);
	return (&o->data.torus.center);
}

/* Orientation axis of a parsed object, or NULL when not rotatable. */
t_vec3	*obj_axis(t_rt_object *o)
{
	if (o->type == OBJ_CYLINDER)
		return (&o->data.cylinder.axis);
	if (o->type == OBJ_CONE)
		return (&o->data.cone.axis);
	if (o->type == OBJ_PLANE)
		return (&o->data.plane.normal);
	if (o->type == OBJ_DISK)
		return (&o->data.disk.normal);
	if (o->type == OBJ_PARABOLOID)
		return (&o->data.paraboloid.axis);
	if (o->type == OBJ_HYPERBOLOID)
		return (&o->data.hyperboloid.axis);
	if (o->type == OBJ_TORUS)
		return (&o->data.torus.axis);
	return (NULL);
}

/* Clear the world + materials and rebuild from the mutated objects[]. */
void	live_rebuild_world(t_live *lv)
{
	live_free_accel(lv);
	hittable_list_clear(&lv->scene->world);
	mat_registry_free_all();
	mat_registry_reset();
	hittable_list_init(&lv->scene->world);
	if (!build_scene_objects(lv->scene)
		|| !add_scene_lights(&lv->scene->world, lv->scene))
		lv->running = 0;
	live_build_accel(lv);
	lv->dirty = 1;
}

/* Number keys 1..9 select objects[idx] (clamped to object_count). */
int	live_select_object(t_live *lv, int keycode)
{
	int	idx;

	if (keycode < LK_1 || keycode > LK_9)
		return (0);
	idx = keycode - LK_1;
	if (idx >= lv->scene->object_count)
		idx = lv->scene->object_count - 1;
	if (idx < 0)
		return (fprintf(stderr, "Live: no objects to select\n"), 1);
	lv->selected = idx;
	fprintf(stderr, "Live: selected object %d (type %d)\n", idx,
		lv->scene->objects[idx].type);
	return (1);
}
