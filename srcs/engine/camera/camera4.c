/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera4.c                                          :+:      :+:    :+:   */
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
#include <time.h>

/* ------------------------------------------------------------------ */
/*  rt_wtime — wall-clock seconds (OpenMP if available, else clock()) */
/* ------------------------------------------------------------------ */

static double	rt_wtime(void)
{
#ifdef _OPENMP
	return (omp_get_wtime());
#else
	return ((double)clock() / (double)CLOCKS_PER_SEC);
#endif
}

/* ------------------------------------------------------------------ */
/*  rt_configure_threads — set OpenMP thread count + print info        */
/* ------------------------------------------------------------------ */

static void	rt_configure_threads(void)
{
#ifdef _OPENMP
# if RT_NUM_THREADS > 0
	omp_set_num_threads(RT_NUM_THREADS);
# endif
	fprintf(stderr, "Threads: %d (OpenMP)\n", omp_get_max_threads());
#else
	fprintf(stderr, "Threads: 1 (sequential — compile with -fopenmp)\n");
#endif
}

static void	render_progress(int done, int total, double start_time)
{
	double	elapsed;
	double	per_unit;
	double	remain;
	char	elapsed_buf[32];
	char	eta_buf[32];

	elapsed = rt_wtime() - start_time;
	if (done > 0)
		per_unit = elapsed / (double)done;
	else
		per_unit = 0.0;
	if (per_unit > 0.0)
		remain = per_unit * (double)(total - done);
	else
		remain = -1.0;
	format_time(elapsed, elapsed_buf, sizeof(elapsed_buf));
	format_time(remain, eta_buf, sizeof(eta_buf));
	fprintf(stderr, "\rRendering: %5.1f%% | units left: %4d"
		" | elapsed: %s | ETA: %s ",
		(100.0 * (double)done) / (double)total, (total - done),
		elapsed_buf, eta_buf);
}

static void	render_tile_pixel(const t_camera *cam,
				const t_hittable_list *world,
				int i, int j, t_vec3 *out)
{
	t_color	pc;
	int		sj;
	int		si;

	pc = vec3_zero();
	sj = 0;
	while (sj < cam->sqrt_spp)
	{
		si = 0;
		while (si < cam->sqrt_spp)
		{
			t_ray r = get_ray_stratified(cam, i, j, si, sj);
			t_vec3 sc = ray_color_with_background(&r, world,
					cam->max_depth, &cam->background);
			pc = vec3_add(&pc, &sc);
			si++;
		}
		sj++;
	}
	*out = vec3_mul_scalar(&pc, cam->pixel_samples_scale);
}

#if RT_TILE_SIZE > 0

static void	render_tiles(const t_camera *cam, const t_hittable_list *world,
				t_vec3 *pixels, double start_time)
{
	int	w;
	int	h;
	int	tiles_x;
	int	tiles_y;
	int	total_tiles;
	int	tiles_done;

	w = cam->image_width;
	h = cam->image_height;
	tiles_x = (w + RT_TILE_SIZE - 1) / RT_TILE_SIZE;
	tiles_y = (h + RT_TILE_SIZE - 1) / RT_TILE_SIZE;
	total_tiles = tiles_x * tiles_y;
	tiles_done = 0;
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
				render_tile_pixel(cam, world, i, j,
					&pixels[j * w + i]);
		{
			int done;
#pragma omp atomic capture
			done = ++tiles_done;
			if ((done & 3) == 0 || done == total_tiles)
			{
#pragma omp critical
				render_progress(done, total_tiles, start_time);
			}
		}
	}
}

#else /* RT_TILE_SIZE == 0  →  row-based fallback */

static void	render_tiles(const t_camera *cam, const t_hittable_list *world,
				t_vec3 *pixels, double start_time)
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
			render_tile_pixel(cam, world, i, j, &pixels[j * w + i]);
		{
			int done;
#pragma omp atomic capture
			done = ++rows_done;
			if ((done & 7) == 0)
			{
#pragma omp critical
				render_progress(done, h, start_time);
			}
		}
	}
}

#endif /* RT_TILE_SIZE */

static void	write_rows(const t_camera *cam, FILE *ppm,
				t_vec3 *pixels)
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
	rt_configure_threads();
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
	{
		double t0 = rt_wtime();
		fprintf(stderr, "Starting render (%dx%d, %d spp, depth %d)...\n",
			camera->image_width, camera->image_height,
			(int)camera->samples_per_pixel, camera->max_depth);
		render_tiles(camera, world, pixels, t0);
		fprintf(stderr, "\nRender done — %.1fs wall-clock\n",
			rt_wtime() - t0);
	}
	fprintf(stderr, "Writing PPM...\n");
	write_rows(camera, ppm_file, pixels);
	fprintf(stderr, "Rendered image saved to: %s\n", filename);
	free(pixels);
	fclose(ppm_file);
}
