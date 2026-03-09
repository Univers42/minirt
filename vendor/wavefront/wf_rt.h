/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_rt.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WF_RT_H
# define WF_RT_H

# include "wf_parse.h"
# include "triangle.h"
# include "hittable_list.h"
# include "material.h"

/*
** Load a wavefront OBJ file and convert to ray tracer mesh.
** The mesh is centered, normalized to target_size, then translated
** to the given position.
** Returns true on success, fills 'mesh' with ray tracer triangles.
*/
bool	wf_obj_to_rt_mesh(const char *path, t_mesh *mesh,
			t_material *mat, float target_size);

/*
** Same as above but also translates the mesh to position (px, py, pz).
*/
bool	wf_obj_to_rt_mesh_at(const char *path, t_mesh *mesh,
			t_material *mat, float target_size,
			float px, float py, float pz);

/*
** Convert an already-parsed wf_mesh to ray tracer mesh.
*/
bool	wf_mesh_to_rt_mesh(const t_wf_mesh *wf, t_mesh *rt,
			t_material *mat);

/*
** Load OBJ and add directly to hittable list (convenience).
*/
bool	wf_obj_to_hittable_list(const char *path, t_hittable_list *list,
			t_material *mat, float target_size,
			float px, float py, float pz);

#endif
