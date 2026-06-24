/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera7.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:53:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void	ensure_output_dir(void)
{
	const char	*output_dir;
	struct stat	st;

	output_dir = "images";
	if (stat(output_dir, &st) == -1)
		mkdir(output_dir, 0755);
}

void	get_output_filename(char *filename, size_t size,
			const char *base_name)
{
	ensure_output_dir();
	snprintf(filename, size, "images/%s.ppm", base_name);
}

/* Run the parallel render then stream the pixels to the open PPM file. */
static void	camera_render_to(const t_camera *cam, FILE *ppm, t_vec3 *pixels,
				const t_hittable_list *world)
{
	t_render_job	job;

	job.cam = cam;
	job.world = world;
	job.pixels = pixels;
	job.start = rt_wtime();
	job.total = render_total_units(cam);
	fprintf(stderr, "Starting render (%dx%d, %d spp, depth %d)...\n",
		cam->image_width, cam->image_height,
		(int)cam->samples_per_pixel, cam->max_depth);
	render_tiles(&job);
	fprintf(stderr, "\nRender done — %.1fs wall-clock\n",
		rt_wtime() - job.start);
	fprintf(stderr, "Writing PPM...\n");
	write_rows(cam, ppm, pixels);
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
		return ((void)fprintf(stderr,
				"Error: cannot open '%s' for writing\n", filename));
	setvbuf(ppm_file, NULL, _IOFBF, 1 << 20);
	fprintf(ppm_file, "P6\n%d %d\n255\n", camera->image_width,
		camera->image_height);
	pixels = (t_vec3 *)malloc((size_t)camera->image_width
			* (size_t)camera->image_height * sizeof(t_vec3));
	if (!pixels)
		return ((void)fclose(ppm_file));
	camera_render_to(camera, ppm_file, pixels, world);
	fprintf(stderr, "Rendered image saved to: %s\n", filename);
	free(pixels);
	fclose(ppm_file);
}
