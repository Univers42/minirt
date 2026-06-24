/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_output.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "lode_image.h"
#include <stdio.h>
#include <stdlib.h>

/* Output path for --ppm: $RT_PPM_OUT if set, else render.ppm.        */
/* Lets parallel headless renders write to distinct files.            */
const char	*ppm_out_path(void)
{
	const char	*p;

	p = getenv("RT_PPM_OUT");
	if (p && *p)
		return (p);
	return ("render.ppm");
}

/* Output path for --png: $RT_PNG_OUT if set, else render.png. */
const char	*png_out_path(void)
{
	const char	*p;

	p = getenv("RT_PNG_OUT");
	if (p && *p)
		return (p);
	return ("render.png");
}

static int	save_ppm(const unsigned char *buf, int w, int h, const char *path)
{
	FILE	*fp;

	fp = fopen(path, "wb");
	if (!fp)
	{
		fprintf(stderr, "Error\nCannot open %s for writing\n", path);
		return (1);
	}
	fprintf(fp, "P6\n%d %d\n255\n", w, h);
	fwrite(buf, 1, (size_t)w * (size_t)h * 3, fp);
	fclose(fp);
	fprintf(stderr, "PPM saved to %s (%dx%d)\n", path, w, h);
	return (0);
}

int	display_ppm(t_camera *cam, unsigned char *buf)
{
	int	ret;

	ret = save_ppm(buf, cam->image_width, cam->image_height, ppm_out_path());
	free(buf);
	return (ret);
}

int	display_png(t_camera *cam, unsigned char *buf)
{
	unsigned int	err;
	const char		*path;

	path = png_out_path();
	err = lode_image_save_png(path, buf, cam->image_width, cam->image_height);
	free(buf);
	if (err)
		return (fprintf(stderr, "Error\nPNG write failed (%u)\n", err), 1);
	fprintf(stderr, "PNG saved to %s (%dx%d)\n", path,
		cam->image_width, cam->image_height);
	return (0);
}
