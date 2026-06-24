/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "studio_config.h"
#include "sphere.h"
#include "cylinder.h"
#include "quad.h"
#include "plane.h"
#include "disk.h"
#include "paraboloid.h"
#include "hyperboloid.h"
#include "torus.h"
#include "triangle.h"
#include "material.h"
#include "camera.h"
#include "camera_lights.h"
#include "environment.h"
#include "bvh.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

/* scene_build2.c */
t_material	*create_material(const t_mat_spec *ms, t_color clr);
bool		build_quad_obj(t_hittable_list *world, const t_rt_object *obj);
bool		build_mesh_obj(t_hittable_list *world, const t_rt_object *obj);

/* ------------------------------------------------------------------ */
/*  Build a sphere engine object from parsed data                     */
/* ------------------------------------------------------------------ */

static bool	build_sphere(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_sphere	s;

	mat = create_material(&obj->mat, obj->data.sphere.color);
	if (!mat)
		return (false);
	s = create_sphere(&obj->data.sphere.center,
			obj->data.sphere.diameter / 2.0, obj->data.sphere.color, mat);
	return (hittable_list_add_sphere(world, &s));
}

/* ------------------------------------------------------------------ */
/*  Build a true infinite plane (equation-based) from parsed data.     */
/*  Owned by the world, so scene_cleanup frees it.                     */
/* ------------------------------------------------------------------ */

static bool	build_plane(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_plane		p;

	mat = create_material(&obj->mat, obj->data.plane.color);
	if (!mat)
		return (false);
	p = plane_create(&obj->data.plane.point,
			&obj->data.plane.normal, mat);
	return (hittable_list_add_plane(world, &p));
}

/* ------------------------------------------------------------------ */
/*  Build a finite flat disk (equation-based) from parsed data.        */
/*  radius = diameter / 2; owned by the world (scene_cleanup frees it). */
/* ------------------------------------------------------------------ */

static bool	build_disk(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_disk		d;

	mat = create_material(&obj->mat, obj->data.disk.color);
	if (!mat)
		return (false);
	d = disk_create(&obj->data.disk.center, &obj->data.disk.normal,
			obj->data.disk.diameter / 2.0, mat);
	return (hittable_list_add_disk(world, &d));
}

/* ------------------------------------------------------------------ */
/*  Build a cylinder from parsed data                                 */
/*  .rt gives CENTER of cylinder — engine wants BASE                  */
/*  base = center − (height / 2) · axis_unit                          */
/*  radius = diameter / 2                                              */
/* ------------------------------------------------------------------ */

static bool	build_cylinder(t_hittable_list *world, const t_rt_object *obj)
{
	t_material		*mat;
	t_vec3			axis_unit;
	t_point3		base;
	t_cylinder		c;
	t_shape_dims	dims;

	mat = create_material(&obj->mat, obj->data.cylinder.color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&obj->data.cylinder.axis);
	base = vec3_mul_scalar(&axis_unit, obj->data.cylinder.height / 2.0);
	base = vec3_sub(&obj->data.cylinder.center, &base);
	dims.size = obj->data.cylinder.diameter / 2.0;
	dims.height = obj->data.cylinder.height;
	c = cylinder_create(&base, &axis_unit, &dims, mat);
	return (hittable_list_add_cylinder(world, &c));
}

/* ------------------------------------------------------------------ */
/*  Build a cone from parsed data (BONUS)                             */
/*  cone_create(apex, axis, angle_deg, height, mat)                   */
/*  Translate diameter → half-angle:                                   */
/*  angle_deg = atan(radius / height) * 180 / π                       */
/* ------------------------------------------------------------------ */

static bool	build_cone(t_hittable_list *world, const t_rt_object *obj)
{
	t_material		*mat;
	t_vec3			axis_unit;
	double			radius;
	t_cone			c;
	t_shape_dims	dims;

	mat = create_material(&obj->mat, obj->data.cone.color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&obj->data.cone.axis);
	radius = obj->data.cone.diameter / 2.0;
	dims.size = atan(radius / obj->data.cone.height) * (180.0 / PI);
	dims.height = obj->data.cone.height;
	c = cone_create(&obj->data.cone.apex, &axis_unit, &dims, mat);
	return (hittable_list_add_cone(world, &c));
}

/* ------------------------------------------------------------------ */
/*  Build an equation-based paraboloid from parsed data (BONUS)        */
/*  paraboloid_create(vertex, axis, diameter, height, mat)             */
/* ------------------------------------------------------------------ */

static bool	build_paraboloid(t_hittable_list *world, const t_rt_object *obj)
{
	t_material		*mat;
	t_paraboloid	pb;
	t_shape_dims	dims;

	mat = create_material(&obj->mat, obj->data.paraboloid.color);
	if (!mat)
		return (false);
	dims.size = obj->data.paraboloid.diameter;
	dims.height = obj->data.paraboloid.height;
	pb = paraboloid_create(&obj->data.paraboloid.vertex,
			&obj->data.paraboloid.axis, &dims, mat);
	return (hittable_list_add_paraboloid(world, &pb));
}

/* ------------------------------------------------------------------ */
/*  Build a one-sheet hyperboloid (cooling-tower) from parsed data     */
/*  hyperboloid_create(center, axis, diameter, height, mat)  (BONUS)   */
/* ------------------------------------------------------------------ */

static bool	build_hyperboloid(t_hittable_list *world, const t_rt_object *obj)
{
	t_material		*mat;
	t_hyperboloid	hy;
	t_shape_dims	dims;

	mat = create_material(&obj->mat, obj->data.hyperboloid.color);
	if (!mat)
		return (false);
	dims.size = obj->data.hyperboloid.diameter;
	dims.height = obj->data.hyperboloid.height;
	hy = hyperboloid_create(&obj->data.hyperboloid.center,
			&obj->data.hyperboloid.axis, &dims, mat);
	return (hittable_list_add_hyperboloid(world, &hy));
}

/* ------------------------------------------------------------------ */
/*  Build an equation-based torus (donut, quartic) from parsed data    */
/*  torus_create(center, axis, major_radius, minor_radius, mat) (BONUS)*/
/* ------------------------------------------------------------------ */

static bool	build_torus(t_hittable_list *world, const t_rt_object *obj)
{
	t_material		*mat;
	t_torus			to;
	t_torus_radii	rad;

	mat = create_material(&obj->mat, obj->data.torus.color);
	if (!mat)
		return (false);
	rad.major = obj->data.torus.major;
	rad.minor = obj->data.torus.minor;
	to = torus_create(&obj->data.torus.center, &obj->data.torus.axis,
			&rad, mat);
	return (hittable_list_add_torus(world, &to));
}

static bool	build_object(t_hittable_list *world, const t_rt_object *obj);

/* ------------------------------------------------------------------ */
/*  Build a triangle from parsed data                                 */
/* ------------------------------------------------------------------ */

static bool	build_triangle_obj(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_triangle	t;

	mat = create_material(&obj->mat, obj->data.triangle.color);
	if (!mat)
		return (false);
	t = triangle_create(&obj->data.triangle.v0,
			&obj->data.triangle.v1,
			&obj->data.triangle.v2, mat);
	return (hittable_list_add_triangle(world, &t));
}

/* ------------------------------------------------------------------ */
/*  Build all parsed objects into the engine hittable_list             */
/* ------------------------------------------------------------------ */

static bool	build_object(t_hittable_list *world, const t_rt_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (build_sphere(world, obj));
	if (obj->type == OBJ_PLANE)
		return (build_plane(world, obj));
	if (obj->type == OBJ_CYLINDER)
		return (build_cylinder(world, obj));
	if (obj->type == OBJ_CONE)
		return (build_cone(world, obj));
	if (obj->type == OBJ_QUAD)
		return (build_quad_obj(world, obj));
	if (obj->type == OBJ_MESH)
		return (build_mesh_obj(world, obj));
	if (obj->type == OBJ_TRIANGLE)
		return (build_triangle_obj(world, obj));
	if (obj->type == OBJ_DISK)
		return (build_disk(world, obj));
	if (obj->type == OBJ_PARABOLOID)
		return (build_paraboloid(world, obj));
	if (obj->type == OBJ_HYPERBOLOID)
		return (build_hyperboloid(world, obj));
	if (obj->type == OBJ_TORUS)
		return (build_torus(world, obj));
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

/* scene_build3.c */
void	setup_cam_quality(t_camera *cam, const t_scene *sc, int width);
void	setup_cam_orient(t_camera *cam, const t_scene *sc);

void	setup_camera(t_camera *cam, const t_scene *sc, int width)
{
	setup_cam_quality(cam, sc, width);
	setup_cam_orient(cam, sc);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

/* ------------------------------------------------------------------ */
/*  Populate the direct-lighting cache from parsed lights              */
/* ------------------------------------------------------------------ */

static void	populate_light_cache(const t_scene *sc)
{
	t_light_info	infos[RT_MAX_LIGHTS];
	int				i;

	i = 0;
	while (i < sc->light_count && i < RT_MAX_LIGHTS)
	{
		infos[i].pos = sc->lights[i].pos;
		infos[i].emission = vec3_mul_scalar(&sc->lights[i].color,
				sc->lights[i].brightness * RT_LIGHT_SCALE);
		infos[i].radius = RT_LIGHT_RADIUS;
		i++;
	}
	set_scene_lights(infos, sc->light_count);
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
				sc->lights[i].brightness * RT_LIGHT_SCALE);
		mat = diffuse_light_create(emit);
		if (!mat)
			return (false);
		mat_registry_add(mat);
		s = create_sphere(&sc->lights[i].pos, RT_LIGHT_RADIUS,
				sc->lights[i].color, mat);
		if (!hittable_list_add_sphere(world, &s))
			return (false);
		i++;
	}
	populate_light_cache(sc);
	if (sc->has_environment)
		load_scene_environment(sc->environment);
	return (true);
}
