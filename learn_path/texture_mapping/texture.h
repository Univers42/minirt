/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 17:05:38 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 17:12:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "point.h"
#include <stdlib.h>
#include <math.h>

/* Forward declaration */
typedef struct s_texture t_texture;

/* Abstract texture type with virtual value function */
typedef struct s_texture
{
	/* Private data specific to the texture implementation */
	void *data;

	/* Virtual function: compute color at (u, v, p) */
	t_color (*value)(const struct s_texture *tex, real_t u, real_t v, const t_point3 *p);

	/* Optional destructor for cleanup */
	void (*destroy)(struct s_texture *tex);
} t_texture;

/* Solid color texture implementation */
typedef struct s_solid_color
{
	t_color albedo;
} t_solid_color;

/* Solid color value function */
static inline t_color solid_color_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)u; /* unused */
	(void)v; /* unused */
	(void)p; /* unused */

	const t_solid_color *sc = (const t_solid_color *)tex->data;
	if (!sc)
		return vec3_create(0.0, 0.0, 0.0);
	return sc->albedo;
}

/* Solid color destructor */
static inline void solid_color_destroy(t_texture *tex)
{
	if (tex && tex->data)
	{
		free(tex->data);
		tex->data = NULL;
	}
}

/* Constructor: create a solid color texture from a color */
static inline t_texture *solid_color_create(t_color albedo)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;

	t_solid_color *sc = (t_solid_color *)malloc(sizeof(t_solid_color));
	if (!sc)
	{
		free(tex);
		return NULL;
	}

	sc->albedo = albedo;

	tex->data = sc;
	tex->value = solid_color_value;
	tex->destroy = solid_color_destroy;

	return tex;
}

/* Constructor: create a solid color texture from RGB components */
static inline t_texture *solid_color_create_rgb(real_t red, real_t green, real_t blue)
{
	return solid_color_create(vec3_create(red, green, blue));
}

/* Checker texture implementation */
typedef struct s_checker_texture
{
	real_t inv_scale;
	t_texture *even;
	t_texture *odd;
} t_checker_texture;

/* Checker texture value function */
static inline t_color checker_texture_value(const t_texture *tex, real_t u, real_t v, const t_point3 *p)
{
	(void)u; /* unused */
	(void)v; /* unused */

	const t_checker_texture *checker = (const t_checker_texture *)tex->data;
	if (!checker || !checker->even || !checker->odd)
		return vec3_create(0.0, 0.0, 0.0);

	/* Floor the scaled world coordinates */
	int x_integer = (int)floor((double)(checker->inv_scale * p->x));
	int y_integer = (int)floor((double)(checker->inv_scale * p->y));
	int z_integer = (int)floor((double)(checker->inv_scale * p->z));

	/* Determine if sum is even */
	bool is_even = ((x_integer + y_integer + z_integer) % 2) == 0;

	/* Sample from appropriate texture */
	t_texture *sample_tex = is_even ? checker->even : checker->odd;
	return sample_tex->value(sample_tex, u, v, p);
}

/* Checker texture destructor */
static inline void checker_texture_destroy(t_texture *tex)
{
	if (!tex || !tex->data)
		return;

	t_checker_texture *checker = (t_checker_texture *)tex->data;

	/* Destroy child textures */
	if (checker->even)
	{
		checker->even->destroy(checker->even);
		free(checker->even);
	}
	if (checker->odd)
	{
		checker->odd->destroy(checker->odd);
		free(checker->odd);
	}

	free(tex->data);
	tex->data = NULL;
}

/* Constructor: create a checker texture from two textures */
static inline t_texture *checker_texture_create(real_t scale, t_texture *even, t_texture *odd)
{
	t_texture *tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return NULL;

	t_checker_texture *checker = (t_checker_texture *)malloc(sizeof(t_checker_texture));
	if (!checker)
	{
		free(tex);
		return NULL;
	}

	checker->inv_scale = (scale > 0.0) ? (1.0 / scale) : 1.0;
	checker->even = even;
	checker->odd = odd;

	tex->data = checker;
	tex->value = checker_texture_value;
	tex->destroy = checker_texture_destroy;

	return tex;
}

/* Constructor: create a checker texture from two colors */
static inline t_texture *checker_texture_create_colors(real_t scale, t_color c1, t_color c2)
{
	t_texture *even = solid_color_create(c1);
	t_texture *odd = solid_color_create(c2);

	if (!even || !odd)
	{
		if (even)
		{
			even->destroy(even);
			free(even);
		}
		if (odd)
		{
			odd->destroy(odd);
			free(odd);
		}
		return NULL;
	}

	return checker_texture_create(scale, even, odd);
}

#endif