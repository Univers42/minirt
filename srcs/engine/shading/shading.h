/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADING_H
# define SHADING_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable_list.h"
# include "camera.h"

/*
** Render engine selector.
**   ENGINE_DIRECT    — deterministic recursive ray tracer (default).
**                      ambient + Lambert diffuse + Blinn-Phong specular +
**                      hard shadows + mirror reflection + Fresnel refraction.
**                      1 primary ray/pixel; subject-compliant; fast.
**   ENGINE_CINEMATIC — the Monte-Carlo path tracer (opt-in via --cinematic).
*/
typedef enum e_engine_mode
{
	ENGINE_DIRECT = 0,
	ENGINE_CINEMATIC = 1
}	t_engine_mode;

/* Per-hit shading context, threaded through the deterministic integrator. */
typedef struct s_shade_ctx
{
	const t_ray				*r;
	const t_hit_record		*rec;
	const t_hittable_list	*world;
	int						depth;
	const t_color			*amb;
	t_color					emission;
}	t_shade_ctx;

/* ray_color_direct.c */
void			render_set_engine_mode(t_engine_mode mode);
t_engine_mode	render_get_engine_mode(void);
t_ray			get_ray_center(const t_camera *cam, int i, int j);
t_vec3			ray_color_direct(const t_ray *r, const t_hittable_list *world,
					int depth, const t_color *ambient);

/* render_aa.c / render_aa2.c — adaptive edge anti-aliasing (direct, pass B) */
typedef struct s_aa_job
{
	const t_camera			*cam;
	const t_hittable_list	*world;
	real_t					*lum;
	t_vec3					*pixels;
	int						total;
	int						next;
}	t_aa_job;

void			render_aa_pass(const t_camera *cam,
					const t_hittable_list *world, t_vec3 *pixels);
real_t			aa_luma(const t_vec3 *p);
t_vec3			aa_supersample(const t_camera *cam,
					const t_hittable_list *world, int x, int y);
int				aa_is_edge(const real_t *lum, int x, int y, int w);

/* ray_color_shade.c */
t_vec3			shade_matte(const t_shade_ctx *c);
t_vec3			shade_reflective(const t_shade_ctx *c);

/* ray_color_glass.c */
t_vec3			shade_glass(const t_shade_ctx *c);

/* shade_ao.c — deterministic ambient occlusion.
   Returns the unoccluded fraction in [0,1] of N fixed hemisphere probe
   rays of length RT_AO_RADIUS around the surface normal.  Used to
   modulate the ambient/fill term only (never the direct-light term). */
real_t			ambient_occlusion(const t_hit_record *rec,
					const t_hittable_list *world);

/* shade_lights.c */
typedef struct s_lctx
{
	const t_hit_record		*rec;
	const t_vec3			*view;
	const t_hittable_list	*world;
}	t_lctx;

t_color			direct_lighting(const t_hit_record *rec,
					const t_hittable_list *world, const t_vec3 *view,
					const t_color *albedo);

/* shade_lights2.c — soft-shadow sampling query (packed to keep args <= 4). */
typedef struct s_shadow_q
{
	const t_point3			*p;
	const t_vec3			*l;
	const t_hittable_list	*world;
	real_t					maxd;
	int						idx;
	int						count;
}	t_shadow_q;

/* shade_lights2.c — deterministic soft (area-light) shadow visibility.
   Returns the fraction in [0,1] of N stratified shadow rays, aimed at a
   fixed pattern of points across the light's spherical surface, that
   reach the light unoccluded.  1 sample reproduces a hard shadow. */
real_t			soft_shadow_visibility(const t_point3 *p, const t_vec3 *l,
					int idx, const t_hittable_list *world);

#endif
