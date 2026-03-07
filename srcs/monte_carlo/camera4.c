/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:30:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#ifdef _OPENMP
# include <omp.h>
#endif
#include <time.h>

static void	render_progress(int done, int h, clock_t start_clock)
{
	double	elapsed;
	double	per_line;
	double	remain;
	char	elapsed_buf[32];
	char	eta_buf[32];

	elapsed = (double)(clock() - start_clock) / (double)CLOCKS_PER_SEC;
	if (done > 0)
		per_line = elapsed / (double)done;
	else
		per_line = 0.0;
	if (per_line > 0.0)
		remain = per_line * (double)(h - done);
	else
		remain = -1.0;
	format_time(elapsed, elapsed_buf, sizeof(elapsed_buf));
	format_time(remain, eta_buf, sizeof(eta_buf));
	fprintf(stderr, "\rRendering: %5.1f%% | rows left: %4d"
		" | elapsed: %s | ETA: %s ",
		(100.0 * (double)done) / (double)h, (h - done),
		elapsed_buf, eta_buf);
}

static void	render_rows(const t_camera *cam, const t_hittable_list *world,
				t_vec3 *pixels, clock_t start_clock)
{
	int	w;
	int	h;
	int	rows_done;

	w = cam->image_width;
	h = cam->image_height;
	rows_done = 0;
#pragma omp parallel for schedule(dynamic, 1)
	for (int j = 0; j < h; ++j)
	{
		for (int i = 0; i < w; ++i)
		{
			t_color pc = vec3_zero();
			for (int sj = 0; sj < cam->sqrt_spp; ++sj)
				for (int si = 0; si < cam->sqrt_spp; ++si)
				{
					t_ray r = get_ray_stratified(cam, i, j, si, sj);
					t_vec3 sc = ray_color_with_background(&r, world,
							cam->max_depth, &cam->background);
					pc = vec3_add(&pc, &sc);
				}
			pixels[j * w + i] = vec3_mul_scalar(&pc,
					cam->pixel_samples_scale);
		}
		{
			int done;
#pragma omp atomic capture
			done = ++rows_done;
			if ((done & 7) == 0)
			{
#pragma omp critical
				render_progress(done, h, start_clock);
			}
		}
	}
}

static void	write_rows(const t_camera *cam, FILE *ppm,
				t_vec3 *pixels, clock_t start_clock)
{
	int				w;
	int				h;
	size_t			rowbuf_sz;
	unsigned char	*rowbuf;
	unsigned char	*ptr;

	w = cam->image_width;
	h = cam->image_height;
	rowbuf_sz = (size_t)w * 3;
	rowbuf = (unsigned char *)malloc(rowbuf_sz);
	if (!rowbuf)
		return ;
	for (int j = 0; j < h; ++j)
	{
		ptr = rowbuf;
		for (int i = 0; i < w; ++i)
			ptr = write_color_to_buf_bin(ptr, &pixels[j * w + i]);
		fwrite(rowbuf, 1, rowbuf_sz, ppm);
	}
	{
		double total = (double)(clock() - start_clock)
			/ (double)CLOCKS_PER_SEC;
		fprintf(stderr, "\rDone.  Elapsed: %.1fs\n", total);
	}
	free(rowbuf);
}

void	camera_render(const t_camera *camera, FILE *out,
			const t_hittable_list *world)
{
	char	filename[256];
	FILE	*ppm_file;
	t_vec3	*pixels;

	(void)out;
	if (!camera)
		return ;
	setvbuf(stderr, NULL, _IONBF, 0);
	get_output_filename(filename, sizeof(filename), "render");
	ppm_file = fopen(filename, "wb");
	if (!ppm_file)
	{
		fprintf(stderr, "Error: cannot open '%s' for writing\n", filename);
		return ;
	}
	setvbuf(ppm_file, NULL, _IOFBF, 1 << 20);
	fprintf(ppm_file, "P6\n%d %d\n255\n", camera->image_width,
		camera->image_height);
	pixels = (t_vec3 *)malloc((size_t)camera->image_width
			* (size_t)camera->image_height * sizeof(t_vec3));
	if (!pixels)
	{
		fclose(ppm_file);
		return ;
	}
	fprintf(stderr, "Starting render...\n");
	render_rows(camera, world, pixels, clock());
	fprintf(stderr, "\nStarting write...\n");
	write_rows(camera, ppm_file, pixels, clock());
	fprintf(stderr, "Rendered image saved to: %s\n", filename);
	free(pixels);
	fclose(ppm_file);
}
