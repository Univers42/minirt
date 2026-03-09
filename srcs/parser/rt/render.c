/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "color.h"
#include "interval.h"
#ifdef _OPENMP
# include <omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/*  Render the scene to an RGB buffer (3 bytes/pixel, row-major)      */
/*  Uses stratified sampling identical to camera4.c::render_rows()    */
/*  Returns malloc'd buffer on success, NULL on failure               */
/* ------------------------------------------------------------------ */

static void	pixel_to_rgb(unsigned char *dst, const t_vec3 *pixel)
{
	real_t				r;
	real_t				g;
	real_t				b;
	static const t_interval	intensity = {0.000, 0.999, true};

	r = linear_to_gamma(pixel->x);
	g = linear_to_gamma(pixel->y);
	b = linear_to_gamma(pixel->z);
	dst[0] = (unsigned char)component_to_byte(r, &intensity);
	dst[1] = (unsigned char)component_to_byte(g, &intensity);
	dst[2] = (unsigned char)component_to_byte(b, &intensity);
}

static void	render_pixel(const t_camera *cam, const t_hittable_list *world,
				int x, int y, t_vec3 *out)
{
	t_color	pc;
	t_ray	r;
	t_vec3	sc;
	int		sj;
	int		si;

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
	fprintf(stderr, "Rendering %dx%d (%d spp, depth %d)...\n",
		w, h, (int)cam->samples_per_pixel, cam->max_depth);
	{
		int	rows_done = 0;
		clock_t	start = clock();
#pragma omp parallel for schedule(dynamic, 1)
		for (int j = 0; j < h; ++j)
		{
			for (int i = 0; i < w; ++i)
				render_pixel(cam, world, i, j, &pixels[j * w + i]);
			{
				int done;
#pragma omp atomic capture
				done = ++rows_done;
				if ((done & 7) == 0 || done == h)
					fprintf(stderr, "\rProgress: %5.1f%%",
						100.0 * (double)done / (double)h);
			}
		}
		fprintf(stderr, "\rDone. Elapsed: %.1fs\n",
			(double)(clock() - start) / (double)CLOCKS_PER_SEC);
	}
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
