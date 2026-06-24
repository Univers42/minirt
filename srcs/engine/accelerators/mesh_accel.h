/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESH_ACCEL_H
# define MESH_ACCEL_H

# include "triangle.h"
# include "hittable.h"
# include "interval.h"
# include "aabb.h"
# include <stdbool.h>

/* Maximum interior-node depth pushed on the iterative traversal stack. */
# define MESH_STACK_MAX 64

/* One node of the mesh-local flat BVH (32 bytes, float AABB).
   count == 0 -> interior (left child = self+1, right child = left_first).
   count  > 0 -> leaf: triangles tris[left_first .. left_first+count). */
typedef struct s_mesh_node
{
	float	bmin[3];
	float	bmax[3];
	int		left_first;
	int		count;
	int		axis;
}	t_mesh_node;

/* A whole OBJ mesh exposed to the scene as ONE accelerated primitive:
   a contiguous triangle array (owned) + a mesh-local flat BVH over it.
   The hit path runs the BVH and calls triangle_hit directly (no v-calls). */
typedef struct s_mesh_accel
{
	t_triangle	*tris;
	int			tri_count;
	t_mesh_node	*nodes;
	int			node_count;
	t_aabb		bbox;
}	t_mesh_accel;

/* mesh_accel.c: build from a parsed t_mesh (steals its triangle array). */
t_mesh_accel		*mesh_accel_build(t_mesh *mesh);
void				mesh_accel_free(t_mesh_accel *ma);

/* mesh_accel2.c: two-step wrapper trio + a ready-made scene wrapper. */
void				set_current_mesh_accel(const void *obj);
bool				mesh_accel_hit_noobj(const t_ray *r, t_interval rayt,
						t_hit_record *rec);
t_hittable_wrapper	mesh_accel_wrapper(t_mesh_accel *ma);

#endif
