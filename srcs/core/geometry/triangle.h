/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:51:55 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 20:07:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRIANGLE_H
# define TRIANGLE_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "point.h"
# include "hittable.h"
# include "interval.h"
# include "aabb.h"
# include <math.h>
# include <stdbool.h>
# include <string.h>

typedef struct s_material	t_material;
typedef struct s_hittable_list	t_hittable_list;

typedef struct s_triangle
{
	t_point3	v0;
	t_point3	v1;
	t_point3	v2;
	t_vec3		e1;
	t_vec3		e2;
	t_vec3		normal;
	t_material	*mat;
	t_aabb		bbox;
}	t_triangle;

typedef struct s_mesh
{
	t_triangle	*triangles;
	size_t		count;
	size_t		capacity;
	t_aabb		bbox;
}	t_mesh;

t_triangle	triangle_create(const t_point3 *v0, const t_point3 *v1,
				const t_point3 *v2, t_material *mat);
bool		triangle_hit(const t_triangle *tri, const t_ray *r,
				t_interval rayt, t_hit_record *rec);
void		set_current_triangle(const void *obj);
bool		triangle_hit_noobj(const t_ray *r, t_interval rayt,
				t_hit_record *rec);
bool		hittable_list_add_triangle(t_hittable_list *list,
				const t_triangle *tri);
void		mesh_init(t_mesh *mesh);
bool		mesh_add_triangle(t_mesh *mesh, const t_triangle *tri);
void		mesh_clear(t_mesh *mesh);
bool		mesh_add_to_list(const t_mesh *mesh, t_hittable_list *list);
void		mesh_add_quad(t_mesh *mesh, const t_point3 *a, const t_point3 *b,
				const t_point3 *c, const t_point3 *d, t_material *mat);
void		mesh_add_box(t_mesh *mesh, const t_point3 *min_pt,
				const t_point3 *max_pt, t_material *mat);
void		mesh_add_pyramid(t_mesh *mesh, const t_point3 *base_center,
				real_t base_size, real_t height, t_material *mat);
void		mesh_add_icosahedron(t_mesh *mesh, const t_point3 *center,
				real_t radius, t_material *mat);

#endif
