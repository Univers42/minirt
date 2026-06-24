/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_object_edit.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include "vector.h"
#include <math.h>

t_point3	*obj_pos(t_rt_object *obj);
t_vec3		*obj_axis(t_rt_object *obj);

/* Camera basis used to translate the object relative to the view. */
static void	live_obj_basis(t_live *lv, t_vec3 *fwd, t_vec3 *right, t_vec3 *up)
{
	t_vec3	world_up;

	world_up = vec3_create(0, 1, 0);
	*fwd = unit_vector(&lv->scene->camera.dir);
	*right = cross(fwd, &world_up);
	if (vec3_length_squared(right) < 1e-9)
		*right = vec3_create(1, 0, 0);
	*right = unit_vector(right);
	*up = cross(right, fwd);
}

/* Translate selected object position from held WASD/QE keys. */
int	live_object_move(t_live *lv)
{
	t_vec3		fwd;
	t_vec3		right;
	t_vec3		up;
	t_point3	*p;
	t_vec3		s;

	live_obj_basis(lv, &fwd, &right, &up);
	p = obj_pos(&lv->scene->objects[lv->selected]);
	if (!(lv->keys.fwd | lv->keys.back | lv->keys.left | lv->keys.right
			| lv->keys.up | lv->keys.down))
		return (0);
	s = vec3_mul_scalar(&fwd, LIVE_MOVE_STEP * (lv->keys.fwd - lv->keys.back));
	*p = vec3_add(p, &s);
	s = vec3_mul_scalar(&right,
			LIVE_MOVE_STEP * (lv->keys.right - lv->keys.left));
	*p = vec3_add(p, &s);
	s = vec3_mul_scalar(&up, LIVE_MOVE_STEP * (lv->keys.up - lv->keys.down));
	*p = vec3_add(p, &s);
	return (1);
}

/* Yaw the object's axis around world-up by one rotation step. */
static void	live_yaw_axis(t_vec3 *ax, real_t a)
{
	t_vec3	r;
	real_t	c;
	real_t	s;

	c = cos(a);
	s = sin(a);
	r = vec3_create(ax->x * c + ax->z * s, ax->y,
			-ax->x * s + ax->z * c);
	*ax = unit_vector(&r);
}

/* Rotate the selected object's axis from held [ / ] keys (if rotatable). */
int	live_object_rotate(t_live *lv)
{
	t_vec3	*ax;
	int		dir;

	dir = lv->keys.yaw_l - lv->keys.yaw_r;
	if (dir == 0)
		return (0);
	ax = obj_axis(&lv->scene->objects[lv->selected]);
	if (!ax)
		return (0);
	live_yaw_axis(ax, LIVE_ROT_STEP * dir);
	return (1);
}

/* Object-mode driver: move + rotate the selected object, rebuild on change. */
int	live_object_edit(t_live *lv)
{
	int	changed;

	if (lv->selected < 0 || lv->selected >= lv->scene->object_count)
		return (0);
	changed = live_object_move(lv);
	changed |= live_object_rotate(lv);
	if (changed)
		live_rebuild_world(lv);
	return (changed);
}
