/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   brdf.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 14:54:09 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 14:54:09 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BRDF_H
# define BRDF_H

# include "utils.h"
# include "material.h"
# include "hitinfo.h"
# include "vec3.h"
# include <math.h>

/* forward-declare sampler type if present elsewhere */
typedef struct s_sampler	t_sampler;

/* Generic BRDF in C: uses function pointers to emulate virtual methods.
   Material/context pointer is provided so implementations can access instance state. */
typedef struct s_brdf
{
	const t_sampler	*sampler;

	/* evaluate BRDF: mat/context, hitinfo, incoming wi, outgoing wo */
	t_vec3	(*f)(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);

	/* sample BRDF: returns sampled wi in world space (vector) */
	t_vec3	(*sample_f)(void *mat, const t_hit_info *hitinfo, const t_vec3 *wo);

	/* material color (optional) */
	t_vec3	(*color)(void *mat);

	/* probability density of sampling wi given wo */
	double	(*pdf)(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);

	/* full evaluation wrapper */
	t_vec3	(*eval)(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
}	t_brdf;

/* default implementations (return black / zero) */
static inline t_vec3	brdf_default_f(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)mat; (void)hitinfo; (void)wi; (void)wo;
	return (vec3_init(0.0, 0.0, 0.0));
}

static inline t_vec3	brdf_default_sample_f(void *mat, const t_hit_info *hitinfo, const t_vec3 *wo)
{
	(void)mat; (void)hitinfo; (void)wo;
	return (vec3_init(0.0, 0.0, 0.0));
}

static inline t_vec3	brdf_default_color(void *mat)
{
	(void)mat;
	return (vec3_init(0.0, 0.0, 0.0));
}

static inline double	brdf_default_pdf(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)mat; (void)hitinfo; (void)wi; (void)wo;
	return 0.0;
}

static inline t_vec3	brdf_default_eval(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)mat; (void)hitinfo; (void)wi; (void)wo;
	return (vec3_init(0.0, 0.0, 0.0));
}

/* helpers */
static inline void	brdf_init(t_brdf *brdf, const t_sampler *sampler)
{
	if (!brdf)
		return;
	brdf->sampler = sampler;
	brdf->f = brdf_default_f;
	brdf->sample_f = brdf_default_sample_f;
	brdf->color = brdf_default_color;
	brdf->pdf = brdf_default_pdf;
	brdf->eval = brdf_default_eval;
}

static inline t_brdf	brdf_copy(const t_brdf *src)
{
	if (!src) return (t_brdf){0};
	return (*src);
}

static inline void	brdf_assign(t_brdf *dst, const t_brdf *src)
{
	if (!dst || !src || dst == src)
		return;
	*dst = *src;
}

#endif