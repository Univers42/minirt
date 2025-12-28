/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lamberation.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 16:47:04 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 16:47:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LAMBERTIAN_H
# define LAMBERTIAN_H

# include <math.h>
# include <stdlib.h>
# include "brdf.h"
# include "vec3.h"
# include "hitinfo.h"

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

typedef struct s_lambertian
{
	t_brdf	base;
	double	kd; /* diffuse coefficient */
	t_vec3	cd;  /* diffuse color */
}	t_lambertian;

/* lifecycle */
void		lambertian_init(t_lambertian *l, double kd, t_vec3 cd, const t_sampler *sampler);
t_lambertian *lambertian_new(double kd, t_vec3 cd, const t_sampler *sampler);
t_lambertian *lambertian_copy(const t_lambertian *src);
void		lambertian_free(t_lambertian *l);

/* implementations */

static inline t_vec3 lambertian_f_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)wi; (void)wo;
	t_lambertian *l = (t_lambertian *)mat;
	if (!l || !hitinfo) return vec3_init(0.0,0.0,0.0);
	return vec3_scale(vec3_scale(l->cd, l->kd), 1.0 / M_PI);
}

static inline t_vec3 lambertian_sample_f_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wo)
{
	(void)wo;
	t_lambertian *l = (t_lambertian *)mat;
	if (!l || !hitinfo) return vec3_init(0.0,0.0,0.0);

	/* build local frame around normal */
	t_vec3 w = vec3_normalize(hitinfo->normal);
	t_vec3 up = vec3_init(0.0034, 1.0, 0.0071);
	t_vec3 v = vec3_cross(up, w);
	v = vec3_normalize(v);
	t_vec3 u = vec3_cross(v, w);

	/* cosine-weighted hemisphere sampling */
	double rnd1 = (double)rand() / (double)RAND_MAX;
	double rnd2 = (double)rand() / (double)RAND_MAX;
	double r = sqrt(rnd1);
	double phi = 2.0 * M_PI * rnd2;
	double x = r * cos(phi);
	double y = r * sin(phi);
	double z = sqrt(fmax(0.0, 1.0 - r * r));

	t_vec3 wi = vec3_add(vec3_add(vec3_scale(u, x), vec3_scale(v, y)), vec3_scale(w, z));
	return vec3_normalize(wi);
}

static inline double lambertian_pdf_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)mat; (void)wo;
	if (!hitinfo || !wi) return 0.0;
	double ndotwi = fmax(0.0, vec3_dot(hitinfo->normal, *wi));
	return ndotwi * (1.0 / M_PI);
}

static inline t_vec3 lambertian_eval_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)wi; (void)wo;
	t_lambertian *l = (t_lambertian *)mat;
	if (!l || !hitinfo) return vec3_init(0.0,0.0,0.0);
	return vec3_scale(vec3_scale(l->cd, l->kd), 1.0 / M_PI);
}

static inline t_vec3 lambertian_color_impl(void *mat)
{
	t_lambertian *l = (t_lambertian *)mat;
	if (!l) return vec3_init(0.0,0.0,0.0);
	return vec3_scale(l->cd, l->kd);
}

/* helpers */
static inline void lambertian_init(t_lambertian *l, double kd, t_vec3 cd, const t_sampler *sampler)
{
	if (!l) return;
	brdf_init(&l->base, sampler);
	l->base.f = lambertian_f_impl;
	l->base.sample_f = lambertian_sample_f_impl;
	l->base.pdf = lambertian_pdf_impl;
	l->base.eval = lambertian_eval_impl;
	l->base.color = lambertian_color_impl;
	l->kd = kd;
	l->cd = cd;
}

static inline t_lambertian *lambertian_new(double kd, t_vec3 cd, const t_sampler *sampler)
{
	t_lambertian *l = (t_lambertian *)malloc(sizeof(*l));
	if (!l) return NULL;
	lambertian_init(l, kd, cd, sampler);
	return l;
}

static inline t_lambertian *lambertian_copy(const t_lambertian *src)
{
	if (!src) return NULL;
	t_lambertian *dst = (t_lambertian *)malloc(sizeof(*dst));
	if (!dst) return NULL;
	*dst = *src;
	return dst;
}

static inline void lambertian_free(t_lambertian *l)
{
	if (!l) return;
	free(l);
}

# endif