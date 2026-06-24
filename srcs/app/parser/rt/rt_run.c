/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_run.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 17:47:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "studio_config.h"
#include "rt_mlx.h"
#include "camera.h"
#include "bvh.h"
#include "bvh_flat.h"
#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/*  Declarations from scene_build.c                                   */
/* ------------------------------------------------------------------ */

void			setup_camera(t_camera *cam, const t_scene *sc, int width);
int				rt_render_width(void);
bool			add_scene_lights(t_hittable_list *world, const t_scene *sc);

/* ------------------------------------------------------------------ */
/*  Declaration from render.c                                         */
/* ------------------------------------------------------------------ */

unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world);

/* ------------------------------------------------------------------ */
/*  Main entry point                                                  */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/*  Public RT pipeline entry point (called from app/main.c)           */
/* ------------------------------------------------------------------ */

int	rt_run(const char *filepath)
{
	t_scene				scene;
	t_camera			cam;
	t_bvh_node			*bvh;
	t_flat_bvh			*fbvh;
	t_hittable_list		accel;
	t_hittable_wrapper	bvh_wrap;
	unsigned char		*buf;
	t_mlx_ctx			ctx;

	if (!parse_scene(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		fprintf(stderr, "Error\nFailed to add lights\n");
		scene_cleanup(&scene);
		return (1);
	}
	setup_camera(&cam, &scene, rt_render_width());
	bvh = bvh_node_create(&scene.world);
	hittable_list_init(&accel);
	if (bvh)
	{
		bvh_wrap.object = bvh;
		bvh_wrap.owned = false;
		bvh_wrap.set_current = set_current_bvh;
		bvh_wrap.hit_noobj = bvh_node_hit;
		bvh_wrap.bbox = bvh->bbox;
		hittable_list_add_wrapper(&accel, &bvh_wrap);
	}
	fbvh = accel_attach_fast(&accel, &scene.world);
	buf = render_to_buffer(&cam, (bvh || fbvh) ? &accel : &scene.world);
	if (!buf)
	{
		fprintf(stderr, "Error\nRender failed (out of memory)\n");
		hittable_list_clear(&accel);
		flat_bvh_free(fbvh);
		bvh_node_destroy(bvh);
		scene_cleanup(&scene);
		return (1);
	}
	if (mlx_ctx_init(&ctx, cam.image_width, cam.image_height,
			"rt") < 0)
	{
		fprintf(stderr, "Error\nFailed to initialize display\n");
		free(buf);
		hittable_list_clear(&accel);
		flat_bvh_free(fbvh);
		bvh_node_destroy(bvh);
		scene_cleanup(&scene);
		return (1);
	}
	mlx_display_rgb(&ctx, buf);
	free(buf);
	mlx_expose_hook(ctx.win, (int (*)())mlx_expose_handler, &ctx);
	mlx_hook(ctx.win, X_KEY_PRESS, KEY_MASK_PRESS,
		(t_mlx_hcb){mlx_key_handler, &ctx});
	mlx_hook(ctx.win, X_DESTROY_NOTIFY, 0,
		(t_mlx_hcb){mlx_close_handler, &ctx});
	mlx_loop(ctx.mlx);
	mlx_ctx_destroy(&ctx);
	hittable_list_clear(&accel);
	flat_bvh_free(fbvh);
	bvh_node_destroy(bvh);
	scene_cleanup(&scene);
	return (0);
}

