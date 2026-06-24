/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "studio_config.h"
#include "camera.h"
#include "material.h"
#include "triangle.h"
#include "hittable_list.h"
#include "../obj/obj_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "libft.h"

/* ------------------------------------------------------------------ */
/*  Runtime resolution overrides via env vars (no recompile needed).   */
/*  RT_WIDTH  : render width, clamped 16..7680, else RT_IMAGE_WIDTH.   */
/*  RT_HEIGHT : forced height, clamped 16..4320, else 0 (derive from   */
/*  aspect ratio as before). Returns fallback when unset/invalid.     */
/* ------------------------------------------------------------------ */

int	rt_env_int(const char *name, int lo, int hi, int fallback)
{
	const char	*val;
	int			n;

	val = getenv(name);
	if (!val || !*val)
		return (fallback);
	n = ft_atoi(val);
	if (n < lo || n > hi)
		return (fallback);
	return (n);
}

int	rt_render_width(void)
{
	return (rt_env_int("RT_WIDTH", 16, 7680, RT_IMAGE_WIDTH));
}

/* ------------------------------------------------------------------ */
/*  Resolve render quality: config defaults → JSON overrides          */
/* ------------------------------------------------------------------ */

void	setup_cam_quality(t_camera *cam, const t_scene *sc, int width)
{
	int	w;
	int	spp;
	int	depth;

	w = width;
	if (sc->render.has_width)
		w = sc->render.width;
	spp = RT_SAMPLES_PER_PIXEL;
	if (sc->render.has_spp)
		spp = sc->render.spp;
	depth = RT_MAX_DEPTH;
	if (sc->render.has_depth)
		depth = sc->render.max_depth;
	cam->aspect_ratio = rt_clampd(RT_ASPECT_RATIO, 0.25, 4.0);
	if (sc->render.has_aspect)
		cam->aspect_ratio = rt_clampd(sc->render.aspect_ratio, 0.25, 4.0);
	cam->image_width = rt_clampi(w, 16, 7680);
	cam->samples_per_pixel = rt_clampi(spp, 1, 16384);
	cam->max_depth = rt_clampi(depth, 1, 500);
#if RT_IMAGE_HEIGHT > 0
	cam->image_height = rt_clampi(RT_IMAGE_HEIGHT, 64, 4320);
#else
	cam->image_height = 0;
#endif
	cam->image_height = rt_env_int("RT_HEIGHT", 16, 4320, cam->image_height);
}

/* ------------------------------------------------------------------ */
/*  Resolve camera orientation / DOF: config defaults → JSON          */
/* ------------------------------------------------------------------ */

void	setup_cam_orient(t_camera *cam, const t_scene *sc)
{
	t_vec3	focus_vec;
	t_vec3	norm_dir;

	cam->vfov = rt_clampd(sc->camera.fov, 1.0, 179.0);
	cam->lookfrom = sc->camera.pos;
	norm_dir = unit_vector(&sc->camera.dir);
	cam->lookat = vec3_add(&sc->camera.pos, &norm_dir);
	cam->vup = vec3_create(RT_DEFAULT_VUP_X,
			RT_DEFAULT_VUP_Y, RT_DEFAULT_VUP_Z);
	if (sc->camera.has_vup)
		cam->vup = sc->camera.vup;
	cam->defocus_angle = rt_clampd(RT_DEFAULT_DEFOCUS, 0.0, 30.0);
	if (sc->camera.has_defocus)
		cam->defocus_angle = rt_clampd(sc->camera.defocus_angle,
				0.0, 30.0);
	focus_vec = vec3_sub(&cam->lookfrom, &cam->lookat);
	cam->focus_dist = vec3_length(&focus_vec);
	if (sc->camera.has_focus_dist)
		cam->focus_dist = sc->camera.focus_dist;
	if (cam->focus_dist < 1e-6)
		cam->focus_dist = 1.0;
	/* Scale the ambient color by the ambient ratio so the sky dome
	   brightness actually respects the user-specified ratio (0.0–1.0).
	   Without this, A 0.15 200,210,230 sets a near-white background
	   instead of a dim sky tint. */
	cam->background = vec3_mul_scalar(&sc->ambient.color,
			sc->ambient.ratio);
}

/* ------------------------------------------------------------------ */
/*  Build an OBJ mesh object from JSON spec                           */
/* ------------------------------------------------------------------ */

t_material	*create_material(const t_mat_spec *ms, t_color clr);

bool	build_mesh_obj(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_obj_xform	xf;

	mat = create_material(&obj->mat, obj->data.mesh.color);
	if (!mat)
		return (false);
	xf.target_size = (float)obj->data.mesh.scale;
	xf.px = (float)obj->data.mesh.position.x;
	xf.py = (float)obj->data.mesh.position.y;
	xf.pz = (float)obj->data.mesh.position.z;
	return (obj_parse_to_list(obj->data.mesh.path, world, mat, &xf));
}
