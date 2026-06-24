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
#include <stdlib.h>

void	render_progress(int done, int total, double start_time)
{
	double	elapsed;
	double	per_unit;
	double	remain;
	char	elapsed_buf[32];
	char	eta_buf[32];

	elapsed = rt_wtime() - start_time;
	per_unit = 0.0;
	if (done > 0)
		per_unit = elapsed / (double)done;
	remain = -1.0;
	if (per_unit > 0.0)
		remain = per_unit * (double)(total - done);
	format_time(elapsed, elapsed_buf, sizeof(elapsed_buf));
	format_time(remain, eta_buf, sizeof(eta_buf));
	fprintf(stderr, "\rRendering: %5.1f%% | units left: %4d"
		" | elapsed: %s | ETA: %s ",
		(100.0 * (double)done) / (double)total, (total - done),
		elapsed_buf, eta_buf);
}

void	render_tile_pixel(const t_camera *cam,
			const t_hittable_list *world, const t_pix *px, t_vec3 *out)
{
	t_color	pc;
	t_pix	s;
	t_ray	r;
	t_vec3	sc;

	pc = vec3_zero();
	s = (t_pix){px->i, px->j, 0, 0};
	while (s.s_j < cam->sqrt_spp)
	{
		s.s_i = 0;
		while (s.s_i < cam->sqrt_spp)
		{
			r = get_ray_stratified(cam, &s);
			sc = ray_color_with_background(&r, world,
					cam->max_depth, &cam->background);
			pc = vec3_add(&pc, &sc);
			s.s_i++;
		}
		s.s_j++;
	}
	*out = vec3_mul_scalar(&pc, cam->pixel_samples_scale);
}

void	write_rows(const t_camera *cam, FILE *ppm, t_vec3 *pixels)
{
	size_t			rowbuf_sz;
	unsigned char	*rowbuf;
	unsigned char	*ptr;
	int				i;
	int				j;

	rowbuf_sz = (size_t)cam->image_width * 3;
	rowbuf = (unsigned char *)malloc(rowbuf_sz);
	if (!rowbuf)
		return ;
	j = 0;
	while (j < cam->image_height)
	{
		ptr = rowbuf;
		i = 0;
		while (i < cam->image_width)
		{
			ptr = write_color_to_buf_bin(ptr,
					&pixels[j * cam->image_width + i]);
			i++;
		}
		fwrite(rowbuf, 1, rowbuf_sz, ppm);
		j++;
	}
	free(rowbuf);
}
