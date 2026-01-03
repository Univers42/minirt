/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 02:18:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include "types.h"
#include "vector.h"
#include "ray.h"
#include "object.h"
#include "hittable_list.h"
#include "interval.h"

/* Return a color (t_vec3) by value, not a pointer */
static inline t_vec3 color_create(real_t x, real_t y, real_t z)
{
	return vec3_create(x, y, z);
}

/* helper: return max component of a color/vector */
static inline real_t vec3_max_component(const t_vec3 *v)
{
	real_t m = v->x;
	if (v->y > m)
		m = v->y;
	if (v->z > m)
		m = v->z;
	return m;
}

/* Convert a component in [0,1] to an integer byte 0..255 (clamped) with gamma correction */
static inline int component_to_byte(real_t v, const t_interval *interval)
{
	/* clamp */
	v = clamp(v, interval->min, interval->max);
	/* gamma-2.0 correction */
	v = (real_t)sqrt((double)v);
	return (int)(256.0 * v);
}

/* Write pixel color to the given FILE stream as "R G B\n". */
static inline void write_color(FILE *out, const t_vec3 *pixel)
{
	t_interval intensity = interval(0.000, 0.999);
	int r = component_to_byte(pixel->x, &intensity);
	int g = component_to_byte(pixel->y, &intensity);
	int b = component_to_byte(pixel->z, &intensity);
	fprintf(out, "%d %d %d\n", r, g, b);
}

/* simple ray_color that uses hit_sphere for legacy tests (kept) */
static inline t_vec3 ray_color_legacy(const t_ray *r)
{
	t_vec3 center = vec3_create((real_t)0, (real_t)0, (real_t)-1);
	real_t t = hit_sphere(&center, (real_t)0.5, r);
	if (t > 0.0)
	{
		t_vec3 p = ray_at((t_ray *)r, (real_t)t);
		t_vec3 diff = vec3_sub(&p, &center);
		t_vec3 n = unit_vector(&diff);
		t_vec3 one = vec3_create((real_t)1, (real_t)1, (real_t)1);
		t_vec3 n_plus = vec3_add(&n, &one);
		return vec3_mul_scalar(&n_plus, (real_t)0.5);
	}
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t tt = 0.5 * (unit_dir.y + (real_t)1.0);
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	return vec3_lerp(&white, &blue, tt);
}

/* depth-limited ray-color implemented iteratively to avoid deep recursion
   and excessive multiplicative darkening. */
static inline t_vec3 ray_color_depth(const t_ray *r, const t_hittable_list *world, int depth)
{
	/* debug: print details for the first N sampled rays to stderr */
	static int debug_rays = 0;
	const int debug_limit = 20;
	bool do_debug = (debug_rays < debug_limit);

	if (do_debug)
	{
		fprintf(stderr, "DEBUG: ray_color_depth called (ray #%d) depth=%d\n", debug_rays, depth);
		fprintf(stderr, "  ray.orig = (%f, %f, %f)\n", r->orig.x, r->orig.y, r->orig.z);
		fprintf(stderr, "  ray.dir  = (%f, %f, %f)\n", r->dir.x, r->dir.y, r->dir.z);
	}

	/* current ray and accumulated attenuation (start with 1,1,1) */
	t_ray cur = *r;
	t_vec3 attenuation = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);

	/* maximum allowed bounces (kept but we will early-terminate via Russian roulette) */
	for (int i = 0; i < depth; ++i)
	{
		t_hit_record rec;
		/* use small t_min to avoid self intersections */
		if (hittable_list_hit(world, &cur, interval((real_t)1e-4, INFINITY), &rec))
		{
			if (do_debug)
			{
				fprintf(stderr, "  bounce %d: HIT t=%f\n", i, rec.t);
				fprintf(stderr, "    hit.p     = (%f, %f, %f)\n", rec.p.x, rec.p.y, rec.p.z);
				fprintf(stderr, "    hit.normal= (%f, %f, %f) front_face=%d\n", rec.normal.x, rec.normal.y, rec.normal.z, rec.front_face);
				fprintf(stderr, "    attenuation BEFORE = (%f, %f, %f)\n", attenuation.x, attenuation.y, attenuation.z);
			}

			/* cosine-weighted hemisphere sampling for Lambertian BRDF */
			t_vec3 direction = random_cosine_direction(&rec.normal);

			/* spawn next ray slightly offset along normal to avoid self-hit */
			t_vec3 eps = vec3_mul_scalar(&rec.normal, (real_t)1e-4);
			t_vec3 scattered_origin = vec3_add(&rec.p, &eps);
			cur = ray_create(scattered_origin, direction);

			/* use per-surface albedo from hit record */
			attenuation = vec3_mul_elem(&attenuation, &rec.albedo);

			/* Russian roulette after a few bounces to avoid extremely long/low-throughput paths */
			if (i >= 5)
			{
				real_t p = vec3_max_component(&attenuation);
				if (p <= (real_t)0.0)
				{
					if (do_debug)
						fprintf(stderr, "    terminated: p=0\n");
					if (do_debug)
						debug_rays++;
					return vec3_zero();
				}
				if (random_real() > p)
				{
					if (do_debug)
						fprintf(stderr, "    russian-roulette: terminated (p=%g)\n", (double)p);
					if (do_debug)
						debug_rays++;
					return vec3_zero();
				}
				/* survived: compensate */
				attenuation = vec3_div_scalar(&attenuation, p);
			}

			if (do_debug)
			{
				fprintf(stderr, "    albedo = (%f, %f, %f)\n", rec.albedo.x, rec.albedo.y, rec.albedo.z);
				fprintf(stderr, "    attenuation AFTER  = (%f, %f, %f)\n", attenuation.x, attenuation.y, attenuation.z);
			}
			continue;
		}

		/* Miss: evaluate background and apply accumulated attenuation */
		t_vec3 unit_dir = unit_vector(&cur.dir);

		/* compute both candidate blend factors so we can inspect which is correct */
		real_t t_orig = (real_t)0.5 * (unit_dir.y + (real_t)1.0);
		real_t t_flipped = (real_t)0.5 * ((real_t)1.0 - unit_dir.y);

		/* clamp both */
		if (t_orig < (real_t)0.0) t_orig = (real_t)0.0;
		if (t_orig > (real_t)1.0) t_orig = (real_t)1.0;
		if (t_flipped < (real_t)0.0) t_flipped = (real_t)0.0;
		if (t_flipped > (real_t)1.0) t_flipped = (real_t)1.0;

		/* choose the original mapping; use no extra brighten bias so sky doesn't wash out objects */
		real_t t_used = t_orig; /* no pow bias */

		/* tone down the white and make the blue slightly darker so objects are more visible */
		t_vec3 white = vec3_create((real_t)0.8, (real_t)0.8, (real_t)0.8);
		t_vec3 blue = vec3_create((real_t)0.2, (real_t)0.45, (real_t)0.85);
		t_vec3 background = vec3_lerp(&white, &blue, t_used);
		t_vec3 final = vec3_mul_elem(&attenuation, &background);

		if (do_debug)
		{
			fprintf(stderr, "  MISS at bounce %d: background = (%f, %f, %f)\n", i, background.x, background.y, background.z);
			fprintf(stderr, "  FINAL color = attenuation * background = (%f, %f, %f)\n", final.x, final.y, final.z);
			debug_rays++;
		}

		return final;
	}

	/* exceeded depth: return black (no more contribution) */
	if (do_debug)
	{
		fprintf(stderr, "  exceeded max depth (%d). attenuation = (%f, %f, %f)\n", depth, attenuation.x, attenuation.y, attenuation.z);
		debug_rays++;
	}
	return vec3_zero();
}

/* Public API: keep previous signature; use moderate recursion depth (25). */
static inline t_vec3 ray_color_world(const t_ray *r, const t_hittable_list *world)
{
	return ray_color_depth(r, world, 25);
}

#endif