/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 17:57:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdint.h>
#include "Vector.h" /* defines t_vec3 and X/Y/Z */
#include "types.h"	/* defines real_t */
#include "ray.h"	/* Need t_ray for ray_color */
#include "object.h"
/* Convert a component in [0,1] to an integer byte 0..255 (clamped). */
static inline int component_to_byte(real_t v)
{
	if (v <= (real_t)0.0)
		return (0);
	if (v >= (real_t)1.0)
		return (255);
	return ((int)(255.999 * v));
}

/* Write pixel color to the given FILE stream as "R G B\n". */
static inline void write_color(FILE *out, const t_vec3 *pixel)
{
	int r;
	int g;
	int b;

	r = component_to_byte(pixel->e[X]);
	g = component_to_byte(pixel->e[Y]);
	b = component_to_byte(pixel->e[Z]);
	fprintf(out, "%d %d %d\n", r, g, b);
}

static inline t_vec3 ray_color(const t_ray *r)
{
	/* check sphere hit: create local point and pass its address */
	t_vec3 center = vec3_create((real_t)0, (real_t)0, (real_t)-1);
	if (hit_sphere(&center, (real_t)0.5, r))
		return vec3_create((real_t)1, (real_t)0, (real_t)0);

	/* simple background gradient based on ray direction */
	t_vec3 unit_dir = unit_vector(&r->dir);
	real_t t = 0.5 * (unit_dir.e[Y] + (real_t)1.0); /* map [-1,1] to [0,1] */
	t_vec3 white = vec3_create(1.0, 1.0, 1.0);
	t_vec3 blue = vec3_create(0.5, 0.7, 1.0);
	/* lerp: (1-t)*white + t*blue */
	return vec3_lerp(&white, &blue, t);
}
#endif