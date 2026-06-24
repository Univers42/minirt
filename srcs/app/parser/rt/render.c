/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:53:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "color.h"
#include "interval.h"
#include "studio_config.h"
#include "shading.h"
#ifdef _OPENMP
# include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/*  rt_buf_wtime — wall-clock seconds (OpenMP if available)            */
/* ------------------------------------------------------------------ */

static double	rt_buf_wtime(void)
{
#ifdef _OPENMP
	return (omp_get_wtime());
#else
	return ((double)clock() / (double)CLOCKS_PER_SEC);
#endif
}

/* ------------------------------------------------------------------ */
/*  Render the scene to an RGB buffer (3 bytes/pixel, row-major)      */
/*  Uses stratified sampling identical to camera4.c                   */
/*  Returns malloc'd buffer on success, NULL on failure               */
/* ------------------------------------------------------------------ */

static void	pixel_to_rgb(unsigned char *dst, const t_vec3 *pixel)
{
	real_t				r;
	real_t				g;
	real_t				b;
	static const t_interval	intensity = {0.000, 0.999, true};

	r = pixel->x;
	g = pixel->y;
	b = pixel->z;
	color_post_process(&r, &g, &b);
	dst[0] = (unsigned char)component_to_byte(r, &intensity);
	dst[1] = (unsigned char)component_to_byte(g, &intensity);
	dst[2] = (unsigned char)component_to_byte(b, &intensity);
}

/* Deterministic engine: one centred primary ray, bounded bounce depth. */
static void	render_pixel_direct(const t_camera *cam,
				const t_hittable_list *world, int x, int y, t_vec3 *out)
{
	t_ray	cr;
	int		d;

	d = cam->max_depth;
	if (d > RT_FAST_MAX_DEPTH)
		d = RT_FAST_MAX_DEPTH;
	cr = get_ray_center(cam, x, y);
	*out = ray_color_direct(&cr, world, d, &cam->background);
}

static void	render_pixel(const t_camera *cam, const t_hittable_list *world,
				int x, int y, t_vec3 *out)
{
	t_color	pc;
	t_ray	r;
	t_vec3	sc;
	int		sj;
	int		si;

	if (render_get_engine_mode() == ENGINE_DIRECT)
	{
		render_pixel_direct(cam, world, x, y, out);
		return ;
	}
	pc = vec3_zero();
	sj = 0;
	while (sj < cam->sqrt_spp)
	{
		si = 0;
		while (si < cam->sqrt_spp)
		{
			r = get_ray_stratified(cam, x, y, si, sj);
			sc = ray_color_with_background(&r, world,
					cam->max_depth, &cam->background);
			pc = vec3_add(&pc, &sc);
			si++;
		}
		sj++;
	}
	*out = vec3_mul_scalar(&pc, cam->pixel_samples_scale);
}

unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world)
{
	unsigned char	*buf;
	t_vec3			*pixels;
	int				w;
	int				h;

	w = cam->image_width;
	h = cam->image_height;
	pixels = (t_vec3 *)malloc((size_t)w * (size_t)h * sizeof(t_vec3));
	if (!pixels)
		return (NULL);
	buf = (unsigned char *)malloc((size_t)w * (size_t)h * 3);
	if (!buf)
	{
		free(pixels);
		return (NULL);
	}
	if (render_get_engine_mode() == ENGINE_DIRECT)
		fprintf(stderr, "Rendering %dx%d (direct engine + AA, depth %d)...\n",
			w, h, cam->max_depth);
	else
		fprintf(stderr, "Rendering %dx%d (cinematic %d spp, depth %d)...\n",
			w, h, (int)cam->samples_per_pixel, cam->max_depth);
#ifdef _OPENMP
# if RT_NUM_THREADS > 0
	omp_set_num_threads(RT_NUM_THREADS);
# endif
	fprintf(stderr, "Threads: %d (OpenMP)\n", omp_get_max_threads());
#endif
	{
		int	units_done = 0;
		double	start = rt_buf_wtime();
#if RT_TILE_SIZE > 0
		int tiles_x = (w + RT_TILE_SIZE - 1) / RT_TILE_SIZE;
		int tiles_y = (h + RT_TILE_SIZE - 1) / RT_TILE_SIZE;
		int total_tiles = tiles_x * tiles_y;
#pragma omp parallel for schedule(dynamic, 1)
		for (int t = 0; t < total_tiles; ++t)
		{
			int tx = t % tiles_x;
			int ty = t / tiles_x;
			int x0 = tx * RT_TILE_SIZE;
			int y0 = ty * RT_TILE_SIZE;
			int x1 = x0 + RT_TILE_SIZE;
			int y1 = y0 + RT_TILE_SIZE;
			if (x1 > w)
				x1 = w;
			if (y1 > h)
				y1 = h;
			for (int j = y0; j < y1; ++j)
				for (int i = x0; i < x1; ++i)
					render_pixel(cam, world, i, j, &pixels[j * w + i]);
			{
				int done;
#pragma omp atomic capture
				done = ++units_done;
				if ((done & 3) == 0 || done == total_tiles)
					fprintf(stderr, "\rProgress: %5.1f%%",
						100.0 * (double)done / (double)total_tiles);
			}
		}
#else
#pragma omp parallel for schedule(dynamic, 1)
		for (int j = 0; j < h; ++j)
		{
			for (int i = 0; i < w; ++i)
				render_pixel(cam, world, i, j, &pixels[j * w + i]);
			{
				int done;
#pragma omp atomic capture
				done = ++units_done;
				if ((done & 7) == 0 || done == h)
					fprintf(stderr, "\rProgress: %5.1f%%",
						100.0 * (double)done / (double)h);
			}
		}
#endif
		fprintf(stderr, "\rDone. Elapsed: %.1fs (wall-clock)\n",
			rt_buf_wtime() - start);
	}
	render_aa_pass(cam, world, pixels);
	{
		int	j = 0;
		while (j < h)
		{
			int	i = 0;
			while (i < w)
			{
				pixel_to_rgb(buf + (j * w + i) * 3, &pixels[j * w + i]);
				i++;
			}
			j++;
		}
	}
	free(pixels);
	return (buf);
}
