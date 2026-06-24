/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera6.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:53:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"
#ifdef _OPENMP
# include <omp.h>
#endif
#include <stdlib.h>

/* Emit one progress line from the job's shared counters. */
static void	report_progress_line(t_render_job *job)
{
	render_progress(job->done, job->total, job->start);
}

/* Bump the shared done-counter (atomically) and emit a progress line. */
static void	report_progress(t_render_job *job)
{
	int	done;

	_Pragma("omp atomic capture")
	done = ++job->done;
	if ((done & 3) == 0 || done == job->total)
		_Pragma("omp critical")
		report_progress_line(job);
}

/* Render every pixel of one work-unit (a tile, or a full row when tiles
   are disabled) identified by its flat index, then report progress. */
static void	render_unit(t_render_job *job, int unit)
{
	const t_camera	*cam;
	t_pix			px;
	int				x1y1[2];

	cam = job->cam;
	px = (t_pix){0, 0, 0, 0};
	render_unit_bounds(cam, unit, &px, x1y1);
	while (px.j < x1y1[1])
	{
		while (px.i < x1y1[0])
		{
			render_tile_pixel(cam, job->world, &px,
				&job->pixels[px.j * cam->image_width + px.i]);
			px.i++;
		}
		px.i = render_unit_x0(cam, unit);
		px.j++;
	}
	report_progress(job);
}

/* One worker: self-schedule work-units via the shared atomic counter. */
static void	tiles_worker(t_render_job *job)
{
	int	unit;

	while (1)
	{
		_Pragma("omp atomic capture")
		unit = job->next++;
		if (unit >= job->total)
			break ;
		render_unit(job, unit);
	}
}

void	render_tiles(t_render_job *job)
{
	job->done = 0;
	job->next = 0;
	_Pragma("omp parallel")
	tiles_worker(job);
}
