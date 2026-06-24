/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_save.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include "lode_image.h"
#include "mlx.h"
#include <stdio.h>

/* Save the current preview as a PNG (live_render.png) of the live view. */
int	live_save_png(t_live *lv)
{
	unsigned int	err;

	if (!lv->buf)
		return (fprintf(stderr, "Live: nothing rendered to save\n"), 1);
	err = lode_image_save_png("live_render.png", lv->buf,
			lv->cam.image_width, lv->cam.image_height);
	if (err)
		return (fprintf(stderr, "Live: PNG save failed (%u)\n", err), 1);
	fprintf(stderr, "Live: saved live_render.png (%dx%d)\n",
		lv->cam.image_width, lv->cam.image_height);
	return (0);
}

/* Window close (red cross): stop the loop so teardown runs cleanly. */
int	live_close(t_live *lv)
{
	lv->running = 0;
	mlx_loop_end(lv->ctx.mlx);
	return (0);
}
