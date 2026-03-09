/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

# include <stdio.h>
# include "types.h"
# include "point.h"
# include "vector.h"
# include "ray.h"
# include "hittable_list.h"
# include "random.h"
# include "color.h"
# include "settings.h"
# include "interval.h"

typedef struct s_camera
{
	real_t		aspect_ratio;
	real_t		samples_per_pixel;
	real_t		pixel_samples_scale;
	int			image_width;
	int			image_height;
	int			max_depth;
	real_t		vfov;
	t_point3	lookfrom;
	t_point3	lookat;
	t_vec3		vup;
	real_t		defocus_angle;
	real_t		focus_dist;
	t_color		background;
	t_point3	center;
	t_point3	pixel00_loc;
	t_vec3		pixel_delta_u;
	t_vec3		pixel_delta_v;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w;
	t_vec3		defocus_disk_u;
	t_vec3		defocus_disk_v;
	int			sqrt_spp;
	real_t		recip_sqrt_spp;
}	t_camera;

void			camera_init(t_camera *camera, real_t aspect_ratio,
					int image_width);
t_vec3			sample_square(void);
t_vec3			sample_square_stratified(int s_i, int s_j,
					real_t recip_sqrt_spp);
t_vec3			defocus_disk_sample(const t_camera *cam);
t_ray			get_ray_stratified(const t_camera *cam, int i, int j,
					int s_i, int s_j);
t_ray			get_ray(const t_camera *cam, int i, int j);
void			ensure_output_dir(void);
void			get_output_filename(char *filename, size_t size,
					const char *base_name);
t_vec3			ray_color_with_background(const t_ray *r,
					const t_hittable_list *world, int depth,
					const t_color *background);
unsigned char	*write_color_to_buf_bin(unsigned char *dst,
					const t_vec3 *pixel);
void			format_time(double seconds, char *buf, size_t bufsize);
void			camera_render(const t_camera *camera, FILE *out,
					const t_hittable_list *world);

#endif
