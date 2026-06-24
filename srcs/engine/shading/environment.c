/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "environment.h"
#include "texture.h"
#include <math.h>

#ifndef PI
# define PI 3.14159265358979323846
#endif

/* Global environment image (equirectangular). Loaded once before the
   OpenMP render begins, then read-only across threads — same contract
   as the g_lights cache, so no per-thread storage is needed. */
static t_texture	*g_environment = NULL;

void	set_scene_environment(t_texture *tex)
{
	g_environment = tex;
}

const t_texture	*get_scene_environment(void)
{
	return (g_environment);
}

/* Load an equirectangular PNG once and install it as the environment.
   Owns the texture; free_scene_environment releases it. Returns false
   on load failure (scene then falls back to the graded sky). */
bool	load_scene_environment(const char *path)
{
	free_scene_environment();
	g_environment = image_texture_create_png(path);
	return (g_environment != NULL);
}

void	free_scene_environment(void)
{
	if (g_environment && g_environment->destroy)
		g_environment->destroy(g_environment);
	free(g_environment);
	g_environment = NULL;
}

/* Map a normalized ray direction to equirectangular (u, v) and sample
   the environment image. The image sampler flips v internally, so we
   pass v measured from the bottom (v = 0 at -Y, v = 1 at +Y). */
t_color	bg_environment_color(const t_ray *r)
{
	t_vec3	dir;
	real_t	u;
	real_t	v;
	real_t	y;
	t_point3	p;

	dir = unit_vector(&r->dir);
	y = dir.y;
	if (y < (real_t)-1.0)
		y = (real_t)-1.0;
	if (y > (real_t)1.0)
		y = (real_t)1.0;
	u = (real_t)0.5 + atan2(dir.z, dir.x) / (real_t)(2.0 * PI);
	v = (real_t)0.5 + asin(y) / (real_t)PI;
	p = vec3_zero();
	return (g_environment->value(g_environment, u, v, &p));
}
