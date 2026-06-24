/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder6.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

bool	cone_coeffs(t_cone_hit *ctx, real_t *abc, real_t *roots)
{
	t_vec3	co;
	real_t	tan2;
	real_t	disc;

	co = vec3_sub(&ctx->r->orig, &ctx->cone->apex);
	ctx->trig[0] = (real_t)cos((double)ctx->cone->angle);
	ctx->trig[1] = (real_t)sin((double)ctx->cone->angle);
	ctx->dv[0] = dot(&ctx->r->dir, &ctx->cone->axis);
	ctx->dv[1] = dot(&co, &ctx->cone->axis);
	tan2 = (ctx->trig[1] * ctx->trig[1]) / (ctx->trig[0] * ctx->trig[0]);
	abc[0] = dot(&ctx->r->dir, &ctx->r->dir) - (1 + tan2) * ctx->dv[0]
		* ctx->dv[0];
	abc[1] = dot(&ctx->r->dir, &co) - (1 + tan2) * ctx->dv[0] * ctx->dv[1];
	abc[2] = dot(&co, &co) - (1 + tan2) * ctx->dv[1] * ctx->dv[1];
	if (fabsl((long double)abc[0]) <= (long double)1e-8)
		return (false);
	disc = abc[1] * abc[1] - abc[0] * abc[2];
	if (disc < 0)
		return (false);
	roots[0] = (-abc[1] - (real_t)sqrt((double)disc)) / abc[0];
	roots[1] = (-abc[1] + (real_t)sqrt((double)disc)) / abc[0];
	return (true);
}

void	cone_test_cap(t_cone_hit *ctx)
{
	real_t		t;
	t_vec3		hp;
	t_vec3		bc;
	t_vec3		th;
	real_t		br;

	if (fabsl((long double)ctx->dv[0]) <= (long double)1e-8)
		return ;
	t = (ctx->cone->height - ctx->dv[1]) / ctx->dv[0];
	if (t < ctx->rayt.min || t >= ctx->closest_t)
		return ;
	hp = ray_at((t_ray *)ctx->r, t);
	bc = vec3_mul_scalar(&ctx->cone->axis, ctx->cone->height);
	bc = vec3_add(&ctx->cone->apex, &bc);
	th = vec3_sub(&hp, &bc);
	br = ctx->cone->height * (real_t)tan((double)ctx->cone->angle);
	if (vec3_length_squared(&th) > br * br)
		return ;
	ctx->hit_anything = true;
	ctx->closest_t = t;
	ctx->closest_normal = ctx->cone->axis;
}

void	cone_set_record(t_cone_hit *ctx, t_hit_record *rec)
{
	rec->t = ctx->closest_t;
	rec->p = ray_at((t_ray *)ctx->r, ctx->closest_t);
	rec->u = 0;
	rec->v = 0;
	rec->mat = ctx->cone->mat;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	set_face_normal(rec, ctx->r, &ctx->closest_normal);
}
