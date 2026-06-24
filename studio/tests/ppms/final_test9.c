/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test9.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "Frozen Tundra" — A cold landscape with an ice lake, frost-covered
** metallic pillars, scattered ice boulders, aurora-like overhead lights,
** and gently falling snowflake particles (tiny white emissive spheres).
*/

#include "common.h"
#include "constant_medium.h"
#include "cylinder.h"
#include "bvh.h"

static bool	add_quad(t_hittable_list *w, t_point3 q,
		t_vec3 u, t_vec3 v, t_material *m)
{
	t_quad	*qp;

	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = quad_create(&q, &u, &v, m);
	return (hittable_list_add_nonowned(w, &(t_nonowned){qp, set_current_quad, quad_hit_noobj, &qp->bbox}));
}

/* ------------------------------------------------------------------ */
/*  Snow-covered ground                                               */
/* ------------------------------------------------------------------ */
static void	build_ground(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(0.0, -200.0, 0.0);
	s = create_sphere(&c, 200.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(0.88, 0.90, 0.95)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Frozen lake — mirror with a blue tint                             */
/* ------------------------------------------------------------------ */
static void	build_ice_lake(t_hittable_list *w)
{
	add_quad(w, vec3_create(-6.0, 0.01, -6.0),
		vec3_create(12.0, 0.0, 0.0), vec3_create(0.0, 0.0, 12.0),
		metal_create_fuzz(vec3_create(0.6, 0.75, 0.95), 0.02));
}

/* ------------------------------------------------------------------ */
/*  Frost pillars — metallic cylinders with ice sheen                 */
/* ------------------------------------------------------------------ */
static void	build_pillars(t_hittable_list *w)
{
	t_cylinder	cyl;
	t_point3	b;
	t_sphere	cap;
	t_point3	tc;

	b = point3_create(-4.0, 0.0, -4.0);
	cyl = cylinder_create_y(&b, 0.35, 3.5,
			metal_create_fuzz(vec3_create(0.7, 0.8, 0.95), 0.05));
	hittable_list_add_cylinder(w, &cyl);
	tc = point3_create(-4.0, 3.5, -4.0);
	cap = create_sphere(&tc, 0.5, vec3_create(1, 1, 1),
			dielectric_create(1.31));
	hittable_list_add_sphere(w, &cap);
	b = point3_create(4.0, 0.0, -4.0);
	cyl = cylinder_create_y(&b, 0.3, 4.0,
			metal_create_fuzz(vec3_create(0.75, 0.82, 0.9), 0.04));
	hittable_list_add_cylinder(w, &cyl);
	tc = point3_create(4.0, 4.0, -4.0);
	cap = create_sphere(&tc, 0.45, vec3_create(1, 1, 1),
			dielectric_create(1.31));
	hittable_list_add_sphere(w, &cap);
	b = point3_create(0.0, 0.0, -7.0);
	cyl = cylinder_create_y(&b, 0.4, 5.0,
			metal_create_fuzz(vec3_create(0.65, 0.78, 0.92), 0.03));
	hittable_list_add_cylinder(w, &cyl);
	tc = point3_create(0.0, 5.0, -7.0);
	cap = create_sphere(&tc, 0.6, vec3_create(1, 1, 1),
			dielectric_create(1.31));
	hittable_list_add_sphere(w, &cap);
}

/* ------------------------------------------------------------------ */
/*  Ice boulders — translucent dielectric spheres                     */
/* ------------------------------------------------------------------ */
static void	build_ice_boulders(t_hittable_list *w)
{
	t_sphere	s;
	t_point3	c;

	c = point3_create(3.0, 0.7, 2.0);
	s = create_sphere(&c, 0.7, vec3_create(1, 1, 1),
			tinted_glass_create(1.31, vec3_create(0.75, 0.88, 1.0)));
	hittable_list_add_sphere(w, &s);
	c = point3_create(-2.5, 0.5, 3.5);
	s = create_sphere(&c, 0.5, vec3_create(1, 1, 1),
			tinted_glass_create(1.31, vec3_create(0.8, 0.9, 1.0)));
	hittable_list_add_sphere(w, &s);
	c = point3_create(5.0, 0.9, -1.0);
	s = create_sphere(&c, 0.9, vec3_create(1, 1, 1),
			dielectric_create(1.31));
	hittable_list_add_sphere(w, &s);
	c = point3_create(-5.5, 0.6, 1.0);
	s = create_sphere(&c, 0.6, vec3_create(1, 1, 1),
			tinted_glass_create(1.31, vec3_create(0.7, 0.85, 0.95)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Aurora borealis — wide emissive quads high overhead               */
/* ------------------------------------------------------------------ */
static void	build_aurora(t_hittable_list *w)
{
	/* Green band */
	add_quad(w, vec3_create(-8.0, 12.0, -10.0),
		vec3_create(16.0, 0.0, 0.0), vec3_create(0.0, 0.0, 5.0),
		diffuse_light_create_scaled(
			vec3_create(0.15, 0.9, 0.35), 8.0));
	/* Teal band — shifted */
	add_quad(w, vec3_create(-6.0, 13.5, -7.0),
		vec3_create(12.0, 0.0, 0.0), vec3_create(0.0, 0.0, 3.0),
		diffuse_light_create_scaled(
			vec3_create(0.1, 0.7, 0.75), 6.0));
	/* Purple fringe */
	add_quad(w, vec3_create(-4.0, 11.0, -12.0),
		vec3_create(8.0, 0.0, 0.0), vec3_create(0.0, 0.0, 3.0),
		diffuse_light_create_scaled(
			vec3_create(0.5, 0.1, 0.7), 5.0));
	/* Pink accent */
	add_quad(w, vec3_create(-10.0, 14.0, -5.0),
		vec3_create(20.0, 0.0, 0.0), vec3_create(0.0, 0.0, 2.0),
		diffuse_light_create_scaled(
			vec3_create(0.9, 0.3, 0.4), 4.0));
}

/* ------------------------------------------------------------------ */
/*  Snowflakes — tiny white emissive spheres scattered in the air     */
/* ------------------------------------------------------------------ */
static void	build_snow(t_hittable_list *w)
{
	int			i;
	t_point3	c;
	t_sphere	s;
	real_t		x;
	real_t		z;

	i = -1;
	while (++i < 120)
	{
		x = random_real_interval(-10.0, 10.0);
		z = random_real_interval(-12.0, 8.0);
		c = point3_create(x,
				random_real_interval(0.5, 10.0), z);
		s = create_sphere(&c, 0.04, vec3_create(1, 1, 1),
				diffuse_light_create_scaled(
					vec3_create(0.95, 0.97, 1.0), 3.0));
		hittable_list_add_sphere(w, &s);
	}
}

/* ------------------------------------------------------------------ */
/*  Thin mist over the lake                                           */
/* ------------------------------------------------------------------ */
static void	build_mist(t_hittable_list *w)
{
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*fog;
	t_point3			c;

	c = point3_create(0.0, 1.5, 0.0);
	bs = create_sphere(&c, 8.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	fog = constant_medium_create_color(&bw, 0.012,
			vec3_create(0.85, 0.9, 1.0));
	if (fog)
		hittable_list_add_nonowned(w, &(t_nonowned){fog, set_current_medium, constant_medium_hit_noobj, &fog->bbox});
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_ground(&world);
	build_ice_lake(&world);
	build_pillars(&world);
	build_ice_boulders(&world);
	build_aurora(&world);
	build_snow(&world);
	build_mist(&world);
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 250;
	cam.max_depth = 50;
	cam.vfov = 55.0;
	cam.lookfrom = point3_create(0.0, 3.0, 15.0);
	cam.lookat = point3_create(0.0, 2.0, -2.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
