/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_camera.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include "vector.h"

void	setup_camera(t_camera *cam, const t_scene *sc, int width);

/* Rebuild the render camera + accel after a scene mutation, mark dirty. */
void	live_rebuild_camera(t_live *lv)
{
	live_free_accel(lv);
	setup_camera(&lv->cam, lv->scene, LIVE_PREVIEW_W);
	live_build_accel(lv);
	lv->dirty = 1;
}

/* Orthonormal basis of the camera: fwd (view dir), right, world-ish up. */
static void	live_basis(t_live *lv, t_vec3 *fwd, t_vec3 *right, t_vec3 *up)
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

/* Translate camera position along the basis from held movement keys. */
static int	live_move(t_live *lv, t_vec3 fwd, t_vec3 right, t_vec3 up)
{
	t_vec3		*pos;
	t_vec3		step;
	int			moved;
	t_live_keys	*k;

	pos = &lv->scene->camera.pos;
	k = &lv->keys;
	moved = k->fwd | k->back | k->left | k->right | k->up | k->down;
	step = vec3_mul_scalar(&fwd, LIVE_MOVE_STEP * (k->fwd - k->back));
	*pos = vec3_add(pos, &step);
	step = vec3_mul_scalar(&right, LIVE_MOVE_STEP * (k->right - k->left));
	*pos = vec3_add(pos, &step);
	step = vec3_mul_scalar(&up, LIVE_MOVE_STEP * (k->up - k->down));
	*pos = vec3_add(pos, &step);
	return (moved);
}

/* Yaw around world-up and pitch around right axis from held rotate keys. */
static int	live_rotate(t_live *lv, t_vec3 right, t_vec3 up)
{
	t_vec3		*dir;
	t_vec3		d;
	t_vec3		off;
	t_live_keys	*k;
	real_t		ry;

	k = &lv->keys;
	if (!(k->yaw_l | k->yaw_r | k->pitch_u | k->pitch_d))
		return (0);
	dir = &lv->scene->camera.dir;
	d = unit_vector(dir);
	ry = LIVE_ROT_STEP * (k->yaw_l - k->yaw_r);
	off = vec3_mul_scalar(&right, ry);
	d = vec3_add(&d, &off);
	off = vec3_mul_scalar(&up, LIVE_ROT_STEP * (k->pitch_u - k->pitch_d));
	d = vec3_add(&d, &off);
	*dir = unit_vector(&d);
	return (1);
}

/* Apply all currently held keys; rebuild the camera if anything changed. */
void	live_apply_keys(t_live *lv)
{
	t_vec3	fwd;
	t_vec3	right;
	t_vec3	up;
	int		changed;

	live_basis(lv, &fwd, &right, &up);
	changed = live_move(lv, fwd, right, up);
	changed |= live_rotate(lv, right, up);
	if (changed)
		live_rebuild_camera(lv);
}
