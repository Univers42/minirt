/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 19:47:51 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
# define COLOR_H

# include <stdio.h>
# include "types.h"
# include "vector.h"
# include "ray.h"
# include "sphere.h"
# include "hittable_list.h"
# include "interval.h"
# include "material.h"

t_vec3	color_create(real_t x, real_t y, real_t z);
real_t	vec3_max_component(const t_vec3 *v);
real_t	linear_to_gamma(real_t v);
int		component_to_byte(real_t v, const t_interval *intensity);
void	color_post_process(real_t *r, real_t *g, real_t *b);
void	write_color(FILE *out, const t_vec3 *pixel);
t_vec3	ray_color_legacy(const t_ray *r);
t_vec3	ray_color_depth(const t_ray *r, const t_hittable_list *world,
			int depth);
t_vec3	ray_color_world(const t_ray *r, const t_hittable_list *world);

#endif
