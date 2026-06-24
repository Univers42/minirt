/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_H
# define TEXTURE_H

# include "types.h"
# include "vector.h"
# include "point.h"
# include "lode_image.h"
# include <stdlib.h>
# include <math.h>
# include <stdbool.h>

typedef struct s_texture
{
	void	*data;
	t_color	(*value)(const struct s_texture *tex, real_t u, real_t v,
			const t_point3 *p);
	void	(*destroy)(struct s_texture *tex);
}	t_texture;

typedef struct s_solid_color
{
	t_color	albedo;
}	t_solid_color;

typedef struct s_checker_texture
{
	real_t		inv_scale;
	t_texture	*even;
	t_texture	*odd;
}	t_checker_texture;

typedef struct s_image_texture
{
	t_lode_image	image;
}	t_image_texture;

t_color		solid_color_value(const t_texture *tex, real_t u, real_t v,
				const t_point3 *p);
void		solid_color_destroy(t_texture *tex);
t_texture	*solid_color_create(t_color albedo);
real_t		srgb_to_linear(unsigned char c);
real_t		linear_to_srgb(real_t linear);
t_color		checker_texture_value(const t_texture *tex, real_t u, real_t v,
				const t_point3 *p);
void		checker_texture_destroy(t_texture *tex);
t_texture	*checker_texture_create(real_t scale, t_texture *even,
				t_texture *odd);
void		checker_set_view(const t_point3 *cam);
void		checker_set_hit(const t_vec3 *normal);
int			checker_parity(const t_checker_texture *ck, const t_point3 *p);
real_t		checker_blend(const t_checker_texture *ck, const t_point3 *p);
t_color		image_texture_value(const t_texture *tex, real_t u, real_t v,
				const t_point3 *p);
void		image_texture_destroy(t_texture *tex);
t_texture	*image_texture_create_png(const char *filename);

#endif