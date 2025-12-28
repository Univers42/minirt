/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   principle_brdf.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 16:08:33 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 16:08:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINCIPLE_BRDF_H
# define PRINCIPLE_BRDF_H

# include "brdf.h"
# include "vec3.h"
# include "hitinfo.h"
# include <math.h>

/* Disney-like material structure (fields used by the BRDF) */
typedef struct s_disney
{
	t_vec3	baseColor;
	double	metallic;
	double	roughness;
	double	specular;
	double	specularTint;
	double	sheen;
	double	sheenTint;
	double	clearcoat;
	double	clearcoatGloss;
	double	subsurface;
}	t_disney;

/* Principled BRDF wrapper */
typedef struct s_principled_brdf
{
	t_brdf	base;
}	t_principled_brdf;

/* lifecycle */
void	principled_init(t_principled_brdf *p, const t_sampler *sampler);
t_principled_brdf *principled_new(const t_sampler *sampler);
void	principled_free(t_principled_brdf *p);

/* BRDF functional interface (match brdf function pointer signatures) */
t_vec3	principled_eval(t_principled_brdf *p, t_disney *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
double	principled_pdf(t_principled_brdf *p, t_disney *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
t_vec3	principled_sample_f(t_principled_brdf *p, t_disney *mat, const t_hit_info *hitinfo, const t_vec3 *wo);

#endif
