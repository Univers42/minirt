/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test5.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "Neon Alley" — A dark corridor lit by intense neon-colored lights.
** Two walls of boxes form an alley, with colored light panels between.
** A mirror floor reflects the neon glow.  A glass sphere and fog add depth.
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
	return (hittable_list_add_nonowned(w, qp,
			set_current_quad, quad_hit_noobj, &qp->bbox));
}

static void	add_box_rt(t_hittable_list *w, t_point3 a, t_point3 b,
		t_material *mat, real_t deg, t_vec3 off)
{
	t_hittable_list		*bx;
	t_hittable_wrapper	wrap;
	t_rotate_y_wrap		*rot;
	t_translate_wrap	*tr;
	t_hittable_wrapper	rw;
	t_hittable_wrapper	tw;

	bx = (t_hittable_list *)malloc(sizeof(t_hittable_list));
	if (!bx)
		return ;
	hittable_list_init(bx);
	box_create_list(&a, &b, mat, bx);
	wrap = hittable_list_wrapper(bx);
	rot = rotate_y_create(&wrap, deg);
	if (!rot)
		return ;
	rw = (t_hittable_wrapper){.object = rot, .owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj, .bbox = rot->bbox};
	tr = translate_create(&rw, &off);
	if (!tr)
		return ;
	tw = (t_hittable_wrapper){.object = tr, .owned = true,
		.set_current = set_current_translate,
		.hit_noobj = translate_hit_noobj, .bbox = tr->bbox};
	hittable_list_add_wrapper(w, &tw);
}

/* ------------------------------------------------------------------ */
/*  Mirror floor                                                     */
/* ------------------------------------------------------------------ */
static void	build_floor(t_hittable_list *w)
{
	add_quad(w, vec3_create(-20.0, 0.0, -20.0),
		vec3_create(40.0, 0.0, 0.0), vec3_create(0.0, 0.0, 40.0),
		metal_create_fuzz(vec3_create(0.15, 0.15, 0.18), 0.02));
}

/* ------------------------------------------------------------------ */
/*  Alley walls — boxes along both sides                             */
/* ------------------------------------------------------------------ */
static void	build_walls(t_hittable_list *w)
{
	int		i;
	real_t	z;

	i = 0;
	while (i < 8)
	{
		z = -12.0 + (real_t)i * 3.5;
		add_box_rt(w, point3_create(0, 0, 0),
			point3_create(1.5, 3.0 + random_real_interval(0, 2), 1.5),
			lambertian_create(vec3_create(0.04, 0.04, 0.05)),
			random_real_interval(-10, 10),
			vec3_create(-4.0, 0.0, z));
		add_box_rt(w, point3_create(0, 0, 0),
			point3_create(1.5, 3.0 + random_real_interval(0, 2), 1.5),
			lambertian_create(vec3_create(0.04, 0.04, 0.05)),
			random_real_interval(-10, 10),
			vec3_create(4.0, 0.0, z));
		i++;
	}
}

/* ------------------------------------------------------------------ */
/*  Neon light panels — vivid colors, high intensity                 */
/* ------------------------------------------------------------------ */
static void	build_neon_lights(t_hittable_list *w)
{
	t_material	*l;

	/* Hot pink left strip */
	l = diffuse_light_create_scaled(vec3_create(1.0, 0.08, 0.5), 35.0);
	add_quad(w, vec3_create(-3.8, 1.5, -10.0),
		vec3_create(0.0, 1.0, 0.0), vec3_create(0.0, 0.0, 20.0), l);
	/* Cyan right strip */
	l = diffuse_light_create_scaled(vec3_create(0.05, 0.9, 1.0), 35.0);
	add_quad(w, vec3_create(3.8, 2.0, -10.0),
		vec3_create(0.0, 0.8, 0.0), vec3_create(0.0, 0.0, 20.0), l);
	/* Purple overhead strip */
	l = diffuse_light_create_scaled(vec3_create(0.6, 0.1, 1.0), 22.0);
	add_quad(w, vec3_create(-2.0, 6.0, -8.0),
		vec3_create(4.0, 0.0, 0.0), vec3_create(0.0, 0.0, 16.0), l);
	/* Yellow accent at the end of the alley */
	l = diffuse_light_create_scaled(vec3_create(1.0, 0.9, 0.2), 40.0);
	add_quad(w, vec3_create(-2.5, 0.5, -13.0),
		vec3_create(5.0, 0.0, 0.0), vec3_create(0.0, 3.0, 0.0), l);
	/* Green floor accent */
	l = diffuse_light_create_scaled(vec3_create(0.15, 1.0, 0.3), 12.0);
	add_quad(w, vec3_create(-1.5, 0.01, 2.0),
		vec3_create(3.0, 0.0, 0.0), vec3_create(0.0, 0.0, 4.0), l);
}

/* ------------------------------------------------------------------ */
/*  Feature objects in the alley                                     */
/* ------------------------------------------------------------------ */
static void	build_objects(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	t_cylinder	cyl;
	t_point3	base;

	/* Large glass sphere — center of alley */
	c = point3_create(0.0, 1.5, 0.0);
	s = create_sphere(&c, 1.5, vec3_create(1, 1, 1),
			dielectric_create(1.5));
	hittable_list_add_sphere(w, &s);
	/* Inner hollow */
	c = point3_create(0.0, 1.5, 0.0);
	s = create_sphere(&c, -1.35, vec3_create(1, 1, 1),
			dielectric_create(1.5));
	hittable_list_add_sphere(w, &s);
	/* Chrome sphere behind */
	c = point3_create(0.0, 0.8, -6.0);
	s = create_sphere(&c, 0.8, vec3_create(1, 1, 1),
			metal_create_fuzz(vec3_create(0.95, 0.95, 0.95), 0.0));
	hittable_list_add_sphere(w, &s);
	/* Tinted glass spheres — left and right */
	c = point3_create(-2.0, 0.6, 4.0);
	s = create_sphere(&c, 0.6, vec3_create(1, 1, 1),
			tinted_glass_create(1.5, vec3_create(1.0, 0.1, 0.4)));
	hittable_list_add_sphere(w, &s);
	c = point3_create(2.0, 0.6, 4.0);
	s = create_sphere(&c, 0.6, vec3_create(1, 1, 1),
			tinted_glass_create(1.5, vec3_create(0.1, 0.6, 1.0)));
	hittable_list_add_sphere(w, &s);
	/* Golden pillar behind glass sphere */
	base = point3_create(0.0, 0.0, -10.0);
	cyl = cylinder_create_y(&base, 0.3, 5.0,
			metal_create_fuzz(vec3_create(0.92, 0.78, 0.35), 0.01));
	hittable_list_add_cylinder(w, &cyl);
}

/* ------------------------------------------------------------------ */
/*  Atmospheric fog filling the alley                                */
/* ------------------------------------------------------------------ */
static void	build_alley_fog(t_hittable_list *w)
{
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*fog;
	t_point3			c;

	c = point3_create(0.0, 3.0, -2.0);
	bs = create_sphere(&c, 8.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	fog = constant_medium_create_color(&bw, 0.015,
			vec3_create(0.7, 0.7, 0.85));
	if (fog)
		hittable_list_add_nonowned(w, fog,
			set_current_medium, constant_medium_hit_noobj, &fog->bbox);
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_floor(&world);
	build_walls(&world);
	build_neon_lights(&world);
	build_objects(&world);
	build_alley_fog(&world);
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;
	cam.vfov = 50.0;
	cam.lookfrom = point3_create(0.0, 3.0, 14.0);
	cam.lookat = point3_create(0.0, 1.5, -4.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
