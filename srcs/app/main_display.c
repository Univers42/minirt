/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_display.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "rt_mlx.h"
#include "bvh.h"
#include "bvh_flat.h"
#include <stdio.h>
#include <stdlib.h>

void			setup_camera(t_camera *cam, const t_scene *sc, int width);
int				rt_render_width(void);
unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world);

static void	wrap_bvh(t_hittable_list *accel, t_bvh_node *bvh)
{
	t_hittable_wrapper	w;

	w.object = bvh;
	w.owned = false;
	w.set_current = set_current_bvh;
	w.hit_noobj = bvh_node_hit;
	w.bbox = bvh->bbox;
	hittable_list_add_wrapper(accel, &w);
}

/* Build wrapper + flat BVH over scene->world into accel; returns flat bvh. */
static t_flat_bvh	*display_accel(t_hittable_list *accel, t_scene *scene,
						t_bvh_node **bvh)
{
	*bvh = bvh_node_create(&scene->world);
	hittable_list_init(accel);
	if (*bvh)
		wrap_bvh(accel, *bvh);
	return (accel_attach_fast(accel, &scene->world));
}

static void	display_free(t_hittable_list *accel, t_bvh_node *bvh,
				t_flat_bvh *fbvh)
{
	hittable_list_clear(accel);
	flat_bvh_free(fbvh);
	bvh_node_destroy(bvh);
}

/* Open the window and pump the event loop; frees buf, returns 0 on success. */
static int	display_window(t_camera *cam, unsigned char *buf)
{
	t_mlx_ctx	ctx;

	if (mlx_ctx_init(&ctx, cam->image_width, cam->image_height, "rt") < 0)
		return (free(buf), fprintf(stderr, "Error\nDisplay init failed\n"), 1);
	mlx_display_rgb(&ctx, buf);
	free(buf);
	mlx_expose_hook(ctx.win, (int (*)())mlx_expose_handler, &ctx);
	mlx_hook(ctx.win, X_KEY_PRESS, KEY_MASK_PRESS,
		(t_mlx_hcb){mlx_key_handler, &ctx});
	mlx_hook(ctx.win, X_DESTROY_NOTIFY, 0,
		(t_mlx_hcb){mlx_close_handler, &ctx});
	mlx_loop(ctx.mlx);
	mlx_ctx_destroy(&ctx);
	return (0);
}

int	display_scene(t_scene *scene, t_out_mode out)
{
	t_camera		cam;
	t_bvh_node		*bvh;
	t_flat_bvh		*fbvh;
	t_hittable_list	accel;
	unsigned char	*buf;

	setup_camera(&cam, scene, rt_render_width());
	fbvh = display_accel(&accel, scene, &bvh);
	if (bvh || fbvh)
		buf = render_to_buffer(&cam, &accel);
	else
		buf = render_to_buffer(&cam, &scene->world);
	if (!buf)
		return (display_free(&accel, bvh, fbvh),
			fprintf(stderr, "Error\nRender failed\n"), 1);
	if (out == OUT_PNG)
		return (display_free(&accel, bvh, fbvh), display_png(&cam, buf));
	if (out == OUT_PPM)
		return (display_free(&accel, bvh, fbvh), display_ppm(&cam, buf));
	display_window(&cam, buf);
	display_free(&accel, bvh, fbvh);
	return (0);
}
