/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test6.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "Solar System" — Orbital arrangement of planets around a glowing sun.
** A huge emissive sphere (the sun) lights the scene. Each planet has a
** unique material: rocky lambertian, gas-giant noise, ice dielectric,
** ringed (cylinder), molten (tinted glass + emissive core).
** Pitch-black space background.
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
/*  The sun — huge emissive sphere                                   */
/* ------------------------------------------------------------------ */
static void	build_sun(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(0.0, 0.0, 0.0);
	s = create_sphere(&c, 4.0, vec3_create(1, 1, 1),
			diffuse_light_create_scaled(
				vec3_create(1.0, 0.85, 0.4), 40.0));
	hittable_list_add_sphere(w, &s);
	/* Corona glow — larger faint emissive shell */
	c = point3_create(0.0, 0.0, 0.0);
	s = create_sphere(&c, 5.0, vec3_create(1, 1, 1),
			diffuse_light_create_scaled(
				vec3_create(1.0, 0.5, 0.1), 5.0));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Mercury — small dark metal sphere                                */
/* ------------------------------------------------------------------ */
static void	build_mercury(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(7.0, 0.5, -2.0);
	s = create_sphere(&c, 0.5, vec3_create(1, 1, 1),
			metal_create_fuzz(vec3_create(0.5, 0.45, 0.4), 0.3));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Venus — thick atmosphere (constant medium + lambertian core)     */
/* ------------------------------------------------------------------ */
static void	build_venus(t_hittable_list *w)
{
	t_point3			c;
	t_sphere			core;
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*atm;

	c = point3_create(9.5, 0.0, 3.0);
	core = create_sphere(&c, 0.9, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(0.85, 0.65, 0.3)));
	hittable_list_add_sphere(w, &core);
	bs = create_sphere(&c, 1.2, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	atm = constant_medium_create_color(&bw, 0.6,
			vec3_create(0.95, 0.8, 0.4));
	if (atm)
		hittable_list_add_nonowned(w, &(t_nonowned){atm, set_current_medium, constant_medium_hit_noobj, &atm->bbox});
}

/* ------------------------------------------------------------------ */
/*  Earth — checker texture (land + ocean)                           */
/* ------------------------------------------------------------------ */
static void	build_earth(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	t_texture	*tex;

	c = point3_create(12.0, 0.3, -1.5);
	tex = checker_texture_create(12.0,
			solid_color_create(vec3_create(0.15, 0.5, 0.2)),
			solid_color_create(vec3_create(0.1, 0.3, 0.7)));
	s = create_sphere(&c, 1.1, vec3_create(1, 1, 1),
			lambertian_create_texture(tex));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Mars — red rocky lambertian                                      */
/* ------------------------------------------------------------------ */
static void	build_mars(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(14.5, -0.2, 4.0);
	s = create_sphere(&c, 0.65, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(0.7, 0.25, 0.1)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Jupiter — large turbulence noise (gas giant)                     */
/* ------------------------------------------------------------------ */
static void	build_jupiter(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(18.0, 1.0, -3.0);
	s = create_sphere(&c, 2.5, vec3_create(1, 1, 1),
			lambertian_create_texture(
				noise_texture_create_turb(2.5, 7)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Saturn — noise sphere + ring (flat cylinder)                     */
/* ------------------------------------------------------------------ */
static void	build_saturn(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	t_cylinder	ring;
	t_point3	base;

	c = point3_create(23.0, 0.5, 2.0);
	s = create_sphere(&c, 1.8, vec3_create(1, 1, 1),
			lambertian_create_texture(
				noise_texture_create_turb(3.0, 5)));
	hittable_list_add_sphere(w, &s);
	/* Ring — very flat cylinder around the planet */
	base = point3_create(23.0, 0.5, 2.0);
	ring = cylinder_create_y(&base, 3.0, 0.05,
			glossy_create(vec3_create(0.8, 0.7, 0.5), 0.1, 0.6));
	hittable_list_add_cylinder(w, &ring);
}

/* ------------------------------------------------------------------ */
/*  Neptune — icy dielectric sphere                                  */
/* ------------------------------------------------------------------ */
static void	build_neptune(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(27.0, -0.3, -2.0);
	s = create_sphere(&c, 1.3, vec3_create(1, 1, 1),
			tinted_glass_create(1.4, vec3_create(0.15, 0.25, 0.9)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Distant stars — many tiny emissive spheres                       */
/* ------------------------------------------------------------------ */
static void	build_stars(t_hittable_list *w)
{
	int			i;
	t_point3	c;
	t_sphere	s;
	real_t		brightness;

	i = 0;
	while (i < 80)
	{
		c = point3_create(random_real_interval(-50.0, 50.0),
				random_real_interval(-30.0, 30.0),
				random_real_interval(-50.0, -20.0));
		brightness = random_real_interval(8.0, 30.0);
		s = create_sphere(&c, 0.15, vec3_create(1, 1, 1),
				diffuse_light_create_scaled(
					vec3_create(1.0, 1.0,
						random_real_interval(0.7, 1.0)),
					brightness));
		hittable_list_add_sphere(w, &s);
		i++;
	}
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_sun(&world);
	build_mercury(&world);
	build_venus(&world);
	build_earth(&world);
	build_mars(&world);
	build_jupiter(&world);
	build_saturn(&world);
	build_neptune(&world);
	build_stars(&world);
	cam.aspect_ratio = 21.0 / 9.0;
	cam.image_width = 1400;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;
	cam.vfov = 45.0;
	cam.lookfrom = point3_create(14.0, 12.0, 25.0);
	cam.lookat = point3_create(14.0, 0.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
