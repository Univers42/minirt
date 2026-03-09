/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:45:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "sphere.h"
#include "cylinder.h"
#include "quad.h"
#include "material.h"
#include "camera.h"
#include "bvh.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  Build a sphere engine object from parsed data                     */
/* ------------------------------------------------------------------ */

static bool	build_sphere(t_hittable_list *world, const t_rt_sphere *sp)
{
	t_material	*mat;
	t_sphere	s;

	mat = lambertian_create(sp->color);
	if (!mat)
		return (false);
	s = create_sphere(&sp->center, sp->diameter / 2.0, sp->color, mat);
	return (hittable_list_add_sphere(world, &s));
}

/* ------------------------------------------------------------------ */
/*  Build a plane as a very large quad from parsed data                */
/*  No plane primitive — synthesize from point + normal               */
/*  Tangent vectors computed via cross product with a non-parallel     */
/*  reference vector.                                                 */
/* ------------------------------------------------------------------ */

static t_vec3	compute_tangent(const t_vec3 *normal)
{
	t_vec3	ref;
	t_vec3	tangent;
	double	len;

	if (fabs(normal->x) < 0.9)
		ref = vec3_create(1.0, 0.0, 0.0);
	else
		ref = vec3_create(0.0, 1.0, 0.0);
	tangent = cross(normal, &ref);
	len = vec3_length(&tangent);
	if (len < 1e-10)
		return (vec3_create(1.0, 0.0, 0.0));
	return (vec3_mul_scalar(&tangent, 1.0 / len));
}

static bool	build_plane(t_hittable_list *world, const t_rt_plane *pl)
{
	t_material	*mat;
	t_vec3		n;
	t_vec3		u_dir;
	t_vec3		v_dir;
	t_vec3		u_vec;
	t_vec3		v_vec;
	t_point3	corner;
	t_quad		q;
	t_quad		*qp;

	mat = lambertian_create(pl->color);
	if (!mat)
		return (false);
	n = unit_vector(&pl->normal);
	u_dir = compute_tangent(&n);
	v_dir = cross(&n, &u_dir);
	u_vec = vec3_mul_scalar(&u_dir, RT_PLANE_SIZE);
	v_vec = vec3_mul_scalar(&v_dir, RT_PLANE_SIZE);
	corner = vec3_sub(&pl->point, &u_vec);
	corner = vec3_sub(&corner, &v_vec);
	u_vec = vec3_mul_scalar(&u_dir, RT_PLANE_SIZE * 2.0);
	v_vec = vec3_mul_scalar(&v_dir, RT_PLANE_SIZE * 2.0);
	q = quad_create(&corner, &u_vec, &v_vec, mat);
	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = q;
	return (hittable_list_add_nonowned(world, qp,
			set_current_quad, quad_hit_noobj, &q.bbox));
}

/* ------------------------------------------------------------------ */
/*  Build a cylinder from parsed data                                 */
/*  .rt gives CENTER of cylinder — engine wants BASE                  */
/*  base = center − (height / 2) · axis_unit                          */
/*  radius = diameter / 2                                              */
/* ------------------------------------------------------------------ */

static bool	build_cylinder(t_hittable_list *world, const t_rt_cylinder *cy)
{
	t_material	*mat;
	t_vec3		axis_unit;
	t_vec3		offset;
	t_point3	base;
	t_cylinder	c;

	mat = lambertian_create(cy->color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&cy->axis);
	offset = vec3_mul_scalar(&axis_unit, cy->height / 2.0);
	base = vec3_sub(&cy->center, &offset);
	c = cylinder_create(&base, &axis_unit,
			cy->diameter / 2.0, cy->height, mat);
	return (hittable_list_add_cylinder(world, &c));
}

/* ------------------------------------------------------------------ */
/*  Build a cone from parsed data (BONUS)                             */
/*  cone_create(apex, axis, angle_deg, height, mat)                   */
/*  Translate diameter → half-angle:                                   */
/*  angle_deg = atan(radius / height) * 180 / π                       */
/* ------------------------------------------------------------------ */

static bool	build_cone(t_hittable_list *world, const t_rt_cone *co)
{
	t_material	*mat;
	t_vec3		axis_unit;
	double		radius;
	double		angle_deg;
	t_cone		c;

	mat = lambertian_create(co->color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&co->axis);
	radius = co->diameter / 2.0;
	angle_deg = atan(radius / co->height) * (180.0 / PI);
	c = cone_create(&co->apex, &axis_unit, angle_deg, co->height, mat);
	return (hittable_list_add_cone(world, &c));
}

/* ------------------------------------------------------------------ */
/*  Build all parsed objects into the engine hittable_list             */
/* ------------------------------------------------------------------ */

static bool	build_object(t_hittable_list *world, const t_rt_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (build_sphere(world, &obj->data.sphere));
	if (obj->type == OBJ_PLANE)
		return (build_plane(world, &obj->data.plane));
	if (obj->type == OBJ_CYLINDER)
		return (build_cylinder(world, &obj->data.cylinder));
	if (obj->type == OBJ_CONE)
		return (build_cone(world, &obj->data.cone));
	return (true);
}

bool	build_scene_objects(t_scene *scene)
{
	int	i;

	i = 0;
	while (i < scene->object_count)
	{
		if (!build_object(&scene->world, &scene->objects[i]))
			return (false);
		i++;
	}
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Setup engine camera from parsed scene data                        */
/* ------------------------------------------------------------------ */

void	setup_camera(t_camera *cam, const t_scene *sc, int width)
{
	t_vec3	focus_vec;

	cam->aspect_ratio = 16.0 / 9.0;
	cam->image_width = width;
	cam->samples_per_pixel = 100.0;
	cam->max_depth = 50;
	cam->vfov = sc->camera.fov;
	cam->lookfrom = sc->camera.pos;
	cam->lookat = vec3_add(&sc->camera.pos, &sc->camera.dir);
	cam->vup = vec3_create(0.0, 1.0, 0.0);
	cam->defocus_angle = 0.0;
	focus_vec = vec3_sub(&cam->lookfrom, &cam->lookat);
	cam->focus_dist = vec3_length(&focus_vec);
	if (cam->focus_dist < 1e-6)
		cam->focus_dist = 1.0;
	cam->background = vec3_mul_scalar(&sc->ambient.color,
			sc->ambient.ratio);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

/* ------------------------------------------------------------------ */
/*  Add point lights as small emissive spheres to the world           */
/* ------------------------------------------------------------------ */

bool	add_scene_lights(t_hittable_list *world, const t_scene *sc)
{
	t_material	*mat;
	t_sphere	s;
	t_color		emit;
	int			i;

	i = 0;
	while (i < sc->light_count)
	{
		emit = vec3_mul_scalar(&sc->lights[i].color,
				sc->lights[i].brightness * 20.0);
		mat = diffuse_light_create(emit);
		if (!mat)
			return (false);
		s = create_sphere(&sc->lights[i].pos, 0.5,
				sc->lights[i].color, mat);
		if (!hittable_list_add_sphere(world, &s))
			return (false);
		i++;
	}
	return (true);
}
