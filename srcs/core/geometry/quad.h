/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:23:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUAD_H
# define QUAD_H

# include "common.h"

typedef struct s_quad
{
	t_point3	q;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w;
	t_material	*mat;
	t_aabb		bbox;
	t_vec3		normal;
	real_t		d;
}	t_quad;

/* Corner + two edge vectors describing one face of an axis-aligned box,
   packed so box_add_face stays within the 42-norm 4-argument cap. */
typedef struct s_quad_face
{
	t_point3	pos;
	t_vec3		u;
	t_vec3		v;
}	t_quad_face;

void	set_bounding_box(t_quad *quad);
t_quad	quad_create(const t_point3 *q_pos, const t_vec3 *u,
			const t_vec3 *v, t_material *mat);
t_aabb	quad_bounding_box(const t_quad *quad);
bool	quad_is_interior(real_t alpha, real_t beta, t_hit_record *rec);
bool	quad_hit(const t_quad *quad, const t_ray *r,
			t_interval rayt, t_hit_record *rec);
void	set_current_quad(const void *obj);
bool	quad_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
void	box(t_hittable_list *world, const t_point3 *a,
			const t_point3 *b, t_material *mat);
void	box_create_list(const t_point3 *a, const t_point3 *b,
			t_material *mat, t_hittable_list *out);

#endif