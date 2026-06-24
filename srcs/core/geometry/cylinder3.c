/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:05:16 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "hittable_list.h"
#include <stdlib.h>
#include <stddef.h>

static void	cone_compute_bbox(t_cone *cone, const t_point3 *apex)
{
	real_t		br;
	t_vec3		to_base;
	t_point3	bc;
	real_t		e[3];

	br = cone->height * (real_t)tan((double)cone->angle);
	to_base = vec3_mul_scalar(&cone->axis, cone->height);
	bc = vec3_add(apex, &to_base);
	e[0] = br * (real_t)sqrt(1.0 - (double)(cone->axis.x * cone->axis.x));
	e[1] = br * (real_t)sqrt(1.0 - (double)(cone->axis.y * cone->axis.y));
	e[2] = br * (real_t)sqrt(1.0 - (double)(cone->axis.z * cone->axis.z));
	cone->bbox = aabb_from_points(
			&(t_point3){fmin(apex->x, bc.x - e[0]), fmin(apex->y, bc.y - e[1]),
			fmin(apex->z, bc.z - e[2])},
			&(t_point3){fmax(apex->x, bc.x + e[0]), fmax(apex->y, bc.y + e[1]),
			fmax(apex->z, bc.z + e[2])});
}

t_cone	cone_create(const t_point3 *apex, const t_vec3 *axis,
		const t_shape_dims *dims, t_material *mat)
{
	t_cone	cone;

	cone.apex = *apex;
	cone.axis = unit_vector(axis);
	cone.angle = degrees_to_radians(dims->size);
	cone.height = dims->height;
	if (dims->height <= 0)
		cone.height = (real_t)1.0;
	cone.mat = mat;
	cone_compute_bbox(&cone, apex);
	return (cone);
}

static t_vec3	cone_lateral_normal(t_cone_hit *ctx, const t_vec3 *th, real_t h)
{
	t_vec3	ap;
	t_vec3	rad;
	t_vec3	n1;
	t_vec3	n2;

	ap = vec3_mul_scalar(&ctx->cone->axis, h);
	rad = vec3_sub(th, &ap);
	rad = unit_vector(&rad);
	n1 = vec3_mul_scalar(&rad, ctx->trig[0]);
	n2 = vec3_mul_scalar(&ctx->cone->axis, -ctx->trig[1]);
	n1 = vec3_add(&n1, &n2);
	return (unit_vector(&n1));
}

static void	cone_test_sides(t_cone_hit *ctx, const real_t *roots, int i)
{
	t_vec3	hp;
	t_vec3	th;
	real_t	h;

	if (roots[i] < ctx->rayt.min || roots[i] >= ctx->closest_t)
		return ;
	hp = ray_at((t_ray *)ctx->r, roots[i]);
	th = vec3_sub(&hp, &ctx->cone->apex);
	h = dot(&th, &ctx->cone->axis);
	if (h < 0 || h > ctx->cone->height)
		return ;
	ctx->hit_anything = true;
	ctx->closest_t = roots[i];
	ctx->closest_normal = cone_lateral_normal(ctx, &th, h);
}

bool	cone_hit(const t_cone *cone, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_cone_hit	ctx;
	real_t		abc[3];
	real_t		roots[2];

	if (!cone || !r || !rec)
		return (false);
	ctx.cone = cone;
	ctx.r = r;
	ctx.rayt = rayt;
	ctx.closest_t = rayt.max;
	ctx.hit_anything = false;
	if (cone_coeffs(&ctx, abc, roots))
	{
		cone_test_sides(&ctx, roots, 0);
		cone_test_sides(&ctx, roots, 1);
	}
	cone_test_cap(&ctx);
	if (!ctx.hit_anything)
		return (false);
	cone_set_record(&ctx, rec);
	return (true);
}
