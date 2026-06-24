/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world);

/* Render one preview frame into lv->buf and blit it to the window. */
static int	live_render(t_live *lv)
{
	const t_hittable_list	*world;

	if (lv->buf)
		free(lv->buf);
	world = (lv->bvh || lv->fbvh) ? &lv->accel : &lv->scene->world;
	lv->buf = render_to_buffer(&lv->cam, world);
	if (!lv->buf)
		return (-1);
	mlx_display_rgb(&lv->ctx, lv->buf);
	return (0);
}

/* Redraw the cached image when the window is exposed/revealed. */
int	live_expose(t_live *lv)
{
	if (lv->ctx.img && lv->ctx.mlx && lv->ctx.win)
	{
		mlx_put_image_to_window(lv->ctx.mlx, lv->ctx.win, lv->ctx.img,
			(t_mlx_point){0, 0});
		mlx_do_sync(lv->ctx.mlx);
	}
	return (0);
}

/* mlx_loop_hook driver: consume held keys, re-render only when dirty. */
int	live_frame(t_live *lv)
{
	if (!lv->running)
		return (0);
	live_apply_keys(lv);
	if (lv->dirty)
	{
		if (live_render(lv) < 0)
			return (lv->running = 0, mlx_loop_end(lv->ctx.mlx), 0);
		lv->dirty = 0;
	}
	return (0);
}

/* Register the editor's own hooks (NOT the one-shot exit(0) handlers). */
static void	live_hooks(t_live *lv)
{
	mlx_do_key_autorepeatoff(lv->ctx.mlx);
	mlx_hook(lv->ctx.win, X_KEY_PRESS, KEY_MASK_PRESS,
		(t_mlx_hcb){(t_mlx_hookfn)live_key_press, lv});
	mlx_hook(lv->ctx.win, X_KEY_RELEASE, KEY_MASK_RELEASE,
		(t_mlx_hcb){(t_mlx_hookfn)live_key_release, lv});
	mlx_hook(lv->ctx.win, X_DESTROY_NOTIFY, 0,
		(t_mlx_hcb){(t_mlx_hookfn)live_close, lv});
	mlx_expose_hook(lv->ctx.win, (int (*)())live_expose, lv);
	mlx_loop_hook(lv->ctx.mlx, (int (*)())live_frame, lv);
}

/* Public entry: open the live editor on an already-built scene.
   CLEAN EXIT: ESC and the red-cross both route through live_teardown. */
int	rt_live_run(t_scene *scene)
{
	t_live	lv;

	if (live_state_init(&lv, scene) < 0)
		return (fprintf(stderr, "Error\nLive editor init failed\n"), 1);
	fprintf(stderr, "Live editor: WASD move, QE down/up, "
		"arrows/IJKL rotate, ESC quit.\n");
	live_hooks(&lv);
	mlx_loop(lv.ctx.mlx);
	mlx_do_key_autorepeaton(lv.ctx.mlx);
	live_teardown(&lv);
	return (0);
}
