/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_types.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:14:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WF_TYPES_H
# define WF_TYPES_H

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

typedef struct s_wf_vec3
{
	float	x;
	float	y;
	float	z;
}	t_wf_vec3;

typedef struct s_wf_vec2
{
	float	u;
	float	v;
}	t_wf_vec2;

typedef struct s_wf_face_vert
{
	int32_t	vi;
	int32_t	ti;
	int32_t	ni;
}	t_wf_face_vert;

typedef struct s_wf_face
{
	t_wf_face_vert	verts[8];
	int				nverts;
}	t_wf_face;

typedef struct s_wf_group
{
	char			name[64];
	char			material[64];
	t_wf_face		*faces;
	size_t			nfaces;
	size_t			face_cap;
}	t_wf_group;

typedef struct s_wf_model
{
	t_wf_vec3		*verts;
	size_t			nverts;
	size_t			vert_cap;
	t_wf_vec3		*norms;
	size_t			nnorms;
	size_t			norm_cap;
	t_wf_vec2		*texcoords;
	size_t			ntexcoords;
	size_t			texcoord_cap;
	t_wf_group		*groups;
	size_t			ngroups;
	size_t			group_cap;
	t_wf_vec3		bounds_min;
	t_wf_vec3		bounds_max;
	t_wf_vec3		center;
	float			scale;
	char			mtllib[256];
}	t_wf_model;

/* Triangle produced after triangulation */
typedef struct s_wf_triangle
{
	t_wf_vec3	v[3];
	t_wf_vec3	n[3];
	t_wf_vec2	t[3];
	bool		has_normals;
	bool		has_texcoords;
	int			group_idx;
}	t_wf_triangle;

typedef struct s_wf_mesh
{
	t_wf_triangle	*tris;
	size_t			ntris;
	size_t			tri_cap;
	t_wf_vec3		bounds_min;
	t_wf_vec3		bounds_max;
	t_wf_vec3		center;
	float			scale;
}	t_wf_mesh;

#endif
