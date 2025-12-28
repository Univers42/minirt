/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glossy.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 16:08:19 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 16:08:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOSSY_H
# define GLOSSY_H

# include <math.h>
# include <stdlib.h>
# include "brdf.h"
# include "vec3.h"
# include "hitinfo.h"

/* Glossy BRDF (Phong-like) */
typedef struct s_glossy
{
	t_brdf	base;
	double	e;   /* exponent */
	double	ks;  /* specular coefficient */
	t_vec3	cs;  /* specular color */
}	t_glossy;

/* forward declarations */
void		glossy_init(t_glossy *g, double ks, double e, t_vec3 cs, const t_sampler *sampler);
t_glossy	*glossy_new(double ks, double e, t_vec3 cs, const t_sampler *sampler);
t_glossy	*glossy_copy(const t_glossy *src);
void		glossy_free(t_glossy *g);

// BRDF interface
t_vec3		glossy_f(t_glossy *g, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
t_vec3		glossy_sample_f(t_glossy *g, const t_hit_info *hitinfo, const t_vec3 *wo);
double		glossy_pdf(t_glossy *g, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
t_vec3		glossy_eval(t_glossy *g, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo);
t_vec3		glossy_color(t_glossy *g);

/* implementations (inline for header convenience) */

static inline t_vec3 glossy_f_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	t_glossy *g = (t_glossy *)mat;
	if (!g || !hitinfo || !wi || !wo) return vec3_init(0.0,0.0,0.0);

	t_vec3 N = hitinfo->normal;
	/* r = -wi + 2 * (wi·N) * N */
	double wi_dot_n = vec3_dot(*wi, N);
	t_vec3 r = vec3_add(vec3_neg(*wi), vec3_scale(N, 2.0 * wi_dot_n));
	double RdotV = fmax(0.0, vec3_dot(r, *wo));
	double lobe = pow(RdotV, g->e);
	return vec3_scale(vec3_scale(g->cs, g->ks), lobe);
}

static inline t_vec3 glossy_sample_f_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wo)
{
	t_glossy *g = (t_glossy *)mat;
	if (!g || !hitinfo || !wo) return vec3_init(0.0,0.0,0.0);

	/* reflected direction r = -wo + 2*(wo·N)*N */
	t_vec3 N = hitinfo->normal;
	double ndotwo = vec3_dot(*wo, N);
	t_vec3 r = vec3_add(vec3_neg(*wo), vec3_scale(N, 2.0 * ndotwo));

	/* build orthonormal frame (w = normalize(r)) */
	t_vec3 w = vec3_normalize(r);
	t_vec3 up = vec3_init(0.00424, 1.0, 0.00764);
	t_vec3 u = vec3_cross(up, w);
	u = vec3_normalize(u);
	t_vec3 v = vec3_cross(u, w);

	/* sample on Phong lobe: cos_theta = rand()^(1/(e+1)) */
	double rnd1 = (double)rand() / (double)RAND_MAX;
	double rnd2 = (double)rand() / (double)RAND_MAX;
	double cos_theta = pow(rnd1, 1.0 / (g->e + 1.0));
	double sin_theta = sqrt(fmax(0.0, 1.0 - cos_theta * cos_theta));
	double phi = 2.0 * M_PI * rnd2;
	double x = cos(phi) * sin_theta;
	double y = sin(phi) * sin_theta;
	double z = cos_theta;

	/* local to world */
	t_vec3 wi = vec3_add(vec3_add(vec3_scale(u, x), vec3_scale(v, y)), vec3_scale(w, z));

	/* if below surface, flip */
	if (vec3_dot(N, wi) < 0.0)
		wi = vec3_add(vec3_add(vec3_scale(u, -x), vec3_scale(v, -y)), vec3_scale(w, z));

	return vec3_normalize(wi);
}

static inline double glossy_pdf_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	t_glossy *g = (t_glossy *)mat;
	if (!g || !hitinfo || !wi || !wo) return 0.0;

	t_vec3 N = hitinfo->normal;
	double ndotwo = vec3_dot(*wo, N);
	t_vec3 r = vec3_add(vec3_neg(*wo), vec3_scale(N, 2.0 * ndotwo));
	double lobe_base = fmax(0.0, vec3_dot(r, *wi));
	double lobe = pow(lobe_base, g->e);
	double ndotwi = fmax(0.0, vec3_dot(N, *wi));
	return lobe * ndotwi;
}

static inline t_vec3 glossy_eval_impl(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	t_glossy *g = (t_glossy *)mat;
	if (!g || !hitinfo || !wi || !wo) return vec3_init(0.0,0.0,0.0);

	t_vec3 N = hitinfo->normal;
	double ndotwo = vec3_dot(*wo, N);
	t_vec3 r = vec3_add(vec3_neg(*wo), vec3_scale(N, 2.0 * ndotwo));
	double lobe_base = fmax(0.0, vec3_dot(r, *wi));
	double lobe = pow(lobe_base, g->e);
	return vec3_scale(vec3_scale(g->cs, g->ks), lobe);
}

static inline t_vec3 glossy_color_impl(void *mat)
{
	t_glossy *g = (t_glossy *)mat;
	if (!g) return vec3_init(0.0,0.0,0.0);
	return vec3_scale(g->cs, g->ks);
}

/* helpers */
static inline void glossy_init(t_glossy *g, double ks, double e, t_vec3 cs, const t_sampler *sampler)
{
	if (!g) return;
	brdf_init(&g->base, sampler);
	g->base.f = glossy_f_impl;
	g->base.sample_f = glossy_sample_f_impl;
	g->base.pdf = glossy_pdf_impl;
	g->base.eval = glossy_eval_impl;
	g->base.color = glossy_color_impl;
	g->e = e;
	g->ks = ks;
	g->cs = cs;
}

static inline t_glossy *glossy_new(double ks, double e, t_vec3 cs, const t_sampler *sampler)
{
	t_glossy *g = (t_glossy *)malloc(sizeof(*g));
	if (!g) return NULL;
	glossy_init(g, ks, e, cs, sampler);
	return g;
}

static inline t_glossy *glossy_copy(const t_glossy *src)
{
	if (!src) return NULL;
	t_glossy *dst = (t_glossy *)malloc(sizeof(*dst));
	if (!dst) return NULL;
	*dst = *src;
	return dst;
}

static inline void glossy_free(t_glossy *g)
{
	if (!g) return;
	free(g);
}

# endif
