/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:52:28 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 01:21:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdint.h>
#include "Vector.h" /* defines t_vec3 and X/Y/Z */
#include "types.h"	/* defines real_t */

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

#endif