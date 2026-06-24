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

/* render_aa.c — adaptive edge anti-aliasing (direct engine, pass B) */
void			render_aa_pass(const t_camera *cam,
					const t_hittable_list *world, t_vec3 *pixels);

/* ray_color_shade.c */
t_vec3			shade_matte(const t_shade_ctx *c);
t_vec3			shade_reflective(const t_shade_ctx *c);

/* ray_color_glass.c */
t_vec3			shade_glass(const t_shade_ctx *c);

/* shade_lights.c */
t_color			direct_lighting(const t_hit_record *rec,
					const t_hittable_list *world, const t_vec3 *view,
					const t_color *albedo);

#endif
