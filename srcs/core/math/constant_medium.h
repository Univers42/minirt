/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constant_medium.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:04:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"
#include "ray.h"
#include "interval.h"
#include "random.h"
#include <stdlib.h>
#include <math.h>

/* Constant medium (participating media) structure */
typedef struct s_constant_medium
{
	t_hittable_wrapper boundary; /* stored by value (copied), not owned */
	real_t neg_inv_density;		 /* -1/density */
	t_material *phase_function;	 /* isotropic scattering material (owned) */
	t_aabb bbox;				 /* cached bounding box */
} t_constant_medium;

void	constant_medium_destroy(t_constant_medium *medium);
t_aabb	constant_medium_bounding_box(const t_constant_medium *medium);
t_constant_medium	*constant_medium_create_color(const t_hittable_wrapper *boundary, real_t density, t_color albedo);
t_constant_medium	*constant_medium_create_texture(const t_hittable_wrapper *boundary, real_t density, t_texture *tex);
bool	constant_medium_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
bool	constant_medium_hit(const t_constant_medium *medium, const t_ray *r,
									   t_interval rayt, t_hit_record *rec);
void	set_current_medium(const void *obj);

#endif
