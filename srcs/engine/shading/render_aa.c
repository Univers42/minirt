/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_aa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 04:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 04:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "camera.h"
#include "interval.h"
#include "studio_config.h"
#ifdef _OPENMP
# include <omp.h>
#endif
#include <stdlib.h>

/* Refine one interior pixel from its flat edge-grid index. */
static void	aa_refine_index(const t_aa_job *job, int idx)
{
	int	w;
	int	x;
	int	y;

	w = job->cam->image_width;
	x = idx % (w - 1);
	y = idx / (w - 1);
	if (aa_is_edge(job->lum, x, y, w))
		job->pixels[y * w + x] = aa_supersample(job->cam, job->world, x, y);
}

/* One worker: grab interior-pixel indices via an atomic shared counter
   (self-scheduling, equivalent to schedule(dynamic, 1)). */
static void	aa_worker(t_aa_job *job)
{
	int	idx;

	while (1)
	{
		_Pragma("omp atomic capture")
		idx = job->next++;
		if (idx >= job->total)
			break ;
		aa_refine_index(job, idx);
	}
}

/* Pass B: refine only silhouette pixels.  Edge flags use a read-only   */
/* luma snapshot so in-place writes never feed back into detection.     */
void	render_aa_pass(const t_camera *cam, const t_hittable_list *world,
			t_vec3 *pixels)
{
	t_aa_job	job;
	int			n;

	if (RT_AA_MODE == 0 || render_get_engine_mode() != ENGINE_DIRECT)
		return ;
	job.cam = cam;
	job.world = world;
	job.pixels = pixels;
	job.next = 0;
	job.total = (cam->image_width - 1) * (cam->image_height - 1);
	job.lum = malloc((size_t)cam->image_width * cam->image_height
			* sizeof(real_t));
	if (!job.lum)
		return ;
	n = -1;
	while (++n < cam->image_width * cam->image_height)
		job.lum[n] = aa_luma(&pixels[n]);
	_Pragma("omp parallel")
	aa_worker(&job);
	free(job.lum);
}
