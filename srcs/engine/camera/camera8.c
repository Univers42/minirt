/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera8.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:53:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"

/* Work-unit pixel size into wh[0]=width, wh[1]=height: a square tile when
   RT_TILE_SIZE > 0, else one full image row.  RT_TILE_SIZE is a compile
   constant, so the branch folds away. */
static void	unit_size(const t_camera *cam, int wh[2])
{
	if (RT_TILE_SIZE > 0)
	{
		wh[0] = RT_TILE_SIZE;
		wh[1] = RT_TILE_SIZE;
	}
	else
	{
		wh[0] = cam->image_width;
		wh[1] = 1;
	}
}

/* Number of work-units spanning the image width. */
static int	units_per_row(const t_camera *cam)
{
	int	wh[2];

	unit_size(cam, wh);
	return ((cam->image_width + wh[0] - 1) / wh[0]);
}

int	render_total_units(const t_camera *cam)
{
	int	wh[2];
	int	tiles_y;

	unit_size(cam, wh);
	tiles_y = (cam->image_height + wh[1] - 1) / wh[1];
	return (units_per_row(cam) * tiles_y);
}

int	render_unit_x0(const t_camera *cam, int unit)
{
	int	wh[2];

	unit_size(cam, wh);
	return ((unit % units_per_row(cam)) * wh[0]);
}

/* Set px->i/px->j to the unit's top-left pixel and x1y1[] to its
   exclusive right/bottom bounds (clamped to the image). */
void	render_unit_bounds(const t_camera *cam, int unit,
			t_pix *px, int *x1y1)
{
	int	wh[2];
	int	cols;

	unit_size(cam, wh);
	cols = units_per_row(cam);
	px->i = (unit % cols) * wh[0];
	px->j = (unit / cols) * wh[1];
	x1y1[0] = px->i + wh[0];
	x1y1[1] = px->j + wh[1];
	if (x1y1[0] > cam->image_width)
		x1y1[0] = cam->image_width;
	if (x1y1[1] > cam->image_height)
		x1y1[1] = cam->image_height;
}
