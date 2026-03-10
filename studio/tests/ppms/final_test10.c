/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test10.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 19:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:30:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "The Chess Endgame" — A dramatic chess-board floor under moody
** overhead lighting.  Kings, queens and pawns represented with
** geometric primitives.  Two armies face each other across the board
** illuminated from above.  Atmospheric haze and reflective pieces.
*/

#include "common.h"
#include "constant_medium.h"
#include "cylinder.h"
#include "bvh.h"

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------ */
/*  Chess-board floor — 8x8 alternating tiles                        */
/* ------------------------------------------------------------------ */
static void	build_board(t_hittable_list *w)
{
	int		x;
	int		z;
	real_t	px;
	real_t	pz;

	z = -1;
	while (++z < 8)
	{
		x = -1;
		while (++x < 8)
		{
			px = (real_t)x - 4.0;
			pz = (real_t)z - 4.0;
			if ((x + z) % 2 == 0)
				add_quad(w, vec3_create(px, 0, pz),
					vec3_create(1, 0, 0), vec3_create(0, 0, 1),
					lambertian_create(vec3_create(0.9, 0.88, 0.82)));
			else
				add_quad(w, vec3_create(px, 0, pz),
					vec3_create(1, 0, 0), vec3_create(0, 0, 1),
					lambertian_create(vec3_create(0.08, 0.07, 0.06)));
		}
	}
}

/* ------------------------------------------------------------------ */
/*  Pawn — cylinder body + sphere head                               */
/* ------------------------------------------------------------------ */
static void	add_pawn(t_hittable_list *w, real_t px,
		real_t pz, t_material *mat)
{
	t_cylinder	body;
	t_sphere	head;
	t_point3	b;
	t_point3	h;

	b = point3_create(px + 0.5, 0.0, pz + 0.5);
	body = cylinder_create_y(&b, 0.18, 0.5, mat);
	hittable_list_add_cylinder(w, &body);
	h = point3_create(px + 0.5, 0.65, pz + 0.5);
	head = create_sphere(&h, 0.16, vec3_create(1, 1, 1), mat);
	hittable_list_add_sphere(w, &head);
}

/* ------------------------------------------------------------------ */
/*  King — taller cylinder + sphere + cross (tiny cylinder on top)   */
/* ------------------------------------------------------------------ */
static void	add_king(t_hittable_list *w, real_t px,
		real_t pz, t_material *mat)
{
	t_cylinder	body;
	t_sphere	head;
	t_cylinder	cross;
	t_point3	b;
	t_point3	h;
	t_point3	cr;

	b = point3_create(px + 0.5, 0.0, pz + 0.5);
	body = cylinder_create_y(&b, 0.22, 0.85, mat);
	hittable_list_add_cylinder(w, &body);
	h = point3_create(px + 0.5, 0.95, pz + 0.5);
	head = create_sphere(&h, 0.2, vec3_create(1, 1, 1), mat);
	hittable_list_add_sphere(w, &head);
	cr = point3_create(px + 0.5, 1.15, pz + 0.5);
	cross = cylinder_create_y(&cr, 0.05, 0.25, mat);
	hittable_list_add_cylinder(w, &cross);
}

/* ------------------------------------------------------------------ */
/*  Queen — cylinder + cone (crown) + sphere                         */
/* ------------------------------------------------------------------ */
static void	add_queen(t_hittable_list *w, real_t px,
		real_t pz, t_material *mat)
{
	t_cylinder	body;
	t_cone		crown;
	t_sphere	head;
	t_point3	b;
	t_point3	h;

	b = point3_create(px + 0.5, 0.0, pz + 0.5);
	body = cylinder_create_y(&b, 0.22, 0.75, mat);
	hittable_list_add_cylinder(w, &body);
	crown = cone_create(
			&(t_point3){px + 0.5, 0.75, pz + 0.5},
			&(t_vec3){0, 1, 0}, 25.0, 0.35, mat);
	hittable_list_add_cone(w, &crown);
	h = point3_create(px + 0.5, 1.12, pz + 0.5);
	head = create_sphere(&h, 0.1, vec3_create(1, 1, 1), mat);
	hittable_list_add_sphere(w, &head);
}

/* ------------------------------------------------------------------ */
/*  Rook — cylinder + small box on top (simulated with quad cap)     */
/* ------------------------------------------------------------------ */
static void	add_rook(t_hittable_list *w, real_t px,
		real_t pz, t_material *mat)
{
	t_cylinder	body;
	t_cylinder	battlement;
	t_point3	b;

	b = point3_create(px + 0.5, 0.0, pz + 0.5);
	body = cylinder_create_y(&b, 0.2, 0.65, mat);
	hittable_list_add_cylinder(w, &body);
	battlement = cylinder_create_y(
			&(t_point3){px + 0.5, 0.65, pz + 0.5},
			0.25, 0.15, mat);
	hittable_list_add_cylinder(w, &battlement);
}

/* ------------------------------------------------------------------ */
/*  White army (front side, z = 2..3)                                */
/* ------------------------------------------------------------------ */
static void	build_white_army(t_hittable_list *w)
{
	t_material	*wm;
	int			i;

	wm = metal_create_fuzz(vec3_create(0.92, 0.9, 0.85), 0.05);
	i = -1;
	while (++i < 8)
		add_pawn(w, (real_t)i - 4.0, 2.0, wm);
	add_rook(w, -4.0, 3.0, wm);
	add_rook(w, 3.0, 3.0, wm);
	add_king(w, -0.0, 3.0, wm);
	add_queen(w, -1.0, 3.0, wm);
}

/* ------------------------------------------------------------------ */
/*  Black army (back side, z = -3..-4)                               */
/* ------------------------------------------------------------------ */
static void	build_black_army(t_hittable_list *w)
{
	t_material	*bm;
	int			i;

	bm = glossy_create(vec3_create(0.05, 0.04, 0.04), 0.02, 0.95);
	i = -1;
	while (++i < 8)
		add_pawn(w, (real_t)i - 4.0, -3.0, bm);
	add_rook(w, -4.0, -4.0, bm);
	add_rook(w, 3.0, -4.0, bm);
	add_king(w, 0.0, -4.0, bm);
	add_queen(w, -1.0, -4.0, bm);
}

/* ------------------------------------------------------------------ */
/*  Overhead dramatic lighting                                       */
/* ------------------------------------------------------------------ */
static void	build_lights(t_hittable_list *w)
{
	/* Warm main light */
	add_quad(w, vec3_create(-2.0, 6.0, -2.0),
		vec3_create(4.0, 0.0, 0.0), vec3_create(0.0, 0.0, 4.0),
		diffuse_light_create_scaled(
			vec3_create(1.0, 0.85, 0.6), 20.0));
	/* Cool accent left */
	add_quad(w, vec3_create(-5.5, 4.5, -1.0),
		vec3_create(1.0, 0.0, 0.0), vec3_create(0.0, 0.0, 2.0),
		diffuse_light_create_scaled(
			vec3_create(0.5, 0.6, 1.0), 12.0));
	/* Red accent right */
	add_quad(w, vec3_create(4.5, 4.5, -1.0),
		vec3_create(1.0, 0.0, 0.0), vec3_create(0.0, 0.0, 2.0),
		diffuse_light_create_scaled(
			vec3_create(1.0, 0.3, 0.2), 10.0));
}

/* ------------------------------------------------------------------ */
/*  Thin atmospheric haze                                             */
/* ------------------------------------------------------------------ */
static void	build_haze(t_hittable_list *w)
{
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*fog;
	t_point3			c;

	c = point3_create(0.0, 3.0, 0.0);
	bs = create_sphere(&c, 10.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	fog = constant_medium_create_color(&bw, 0.006,
			vec3_create(0.85, 0.82, 0.75));
	if (fog)
		hittable_list_add_nonowned(w, fog,
			set_current_medium, constant_medium_hit_noobj, &fog->bbox);
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_board(&world);
	build_white_army(&world);
	build_black_army(&world);
	build_lights(&world);
	build_haze(&world);
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 10000;
	cam.max_depth = 50;
	cam.vfov = 40.0;
	cam.lookfrom = point3_create(6.0, 5.5, 8.0);
	cam.lookat = point3_create(0.0, 0.5, -0.5);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.3;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
