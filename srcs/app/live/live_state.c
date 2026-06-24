/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_state.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include <string.h>

void	setup_camera(t_camera *cam, const t_scene *sc, int width);

/* Wrap the world BVH root into the accel list (same shape as rt_run). */
static void	live_wrap_bvh(t_live *lv)
{
	t_hittable_wrapper	w;

	w.object = lv->bvh;
	w.owned = false;
	w.set_current = set_current_bvh;
	w.hit_noobj = bvh_node_hit;
	w.bbox = lv->bvh->bbox;
	hittable_list_add_wrapper(&lv->accel, &w);
}

/* (Re)build the per-render BVH + flat accel over scene->world. */
void	live_build_accel(t_live *lv)
{
	lv->bvh = bvh_node_create(&lv->scene->world);
	hittable_list_init(&lv->accel);
	if (lv->bvh)
		live_wrap_bvh(lv);
	lv->fbvh = accel_attach_fast(&lv->accel, &lv->scene->world);
}

/* Free only the acceleration structures (kept separate so a future
   geometry edit can rebuild without tearing down the window). */
void	live_free_accel(t_live *lv)
{
	hittable_list_clear(&lv->accel);
	flat_bvh_free(lv->fbvh);
	bvh_node_destroy(lv->bvh);
	lv->fbvh = NULL;
	lv->bvh = NULL;
}

int	live_state_init(t_live *lv, t_scene *scene)
{
	memset(lv, 0, sizeof(*lv));
	lv->scene = scene;
	lv->dirty = 1;
	lv->running = 1;
	setup_camera(&lv->cam, scene, LIVE_PREVIEW_W);
	live_build_accel(lv);
	if (mlx_ctx_init(&lv->ctx, lv->cam.image_width,
			lv->cam.image_height, "rt live editor") < 0)
		return (live_free_accel(lv), -1);
	return (0);
}

/* CLEAN EXIT: free accel, render buffer, then the MLX context.
   The scene world + material registry are owned by the caller. */
void	live_teardown(t_live *lv)
{
	live_free_accel(lv);
	if (lv->buf)
		free(lv->buf);
	lv->buf = NULL;
	mlx_ctx_destroy(&lv->ctx);
}
