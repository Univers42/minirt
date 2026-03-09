/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_parse.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:51:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WF_PARSE_H
# define WF_PARSE_H

# include "wf_types.h"
# include <stdio.h>

/* Model lifecycle */
void		wf_model_init(t_wf_model *m);
void		wf_model_free(t_wf_model *m);

/* Parsing */
bool		wf_parse_file(t_wf_model *m, const char *path);
bool		wf_parse_buf(t_wf_model *m, const char *buf, size_t len);

/* Mesh triangulation */
void		wf_mesh_init(t_wf_mesh *mesh);
void		wf_mesh_free(t_wf_mesh *mesh);
bool		wf_triangulate(t_wf_mesh *mesh, const t_wf_model *m);

/* Mesh utilities */
void		wf_mesh_compute_bounds(t_wf_mesh *mesh);
void		wf_mesh_normalize(t_wf_mesh *mesh, float target_size);
void		wf_mesh_center(t_wf_mesh *mesh);
void		wf_mesh_gen_normals(t_wf_mesh *mesh);
void		wf_mesh_translate(t_wf_mesh *mesh, float dx, float dy, float dz);
void		wf_mesh_scale(t_wf_mesh *mesh, float sx, float sy, float sz);

/* Debug */
void		wf_model_print_info(const t_wf_model *m);
void		wf_mesh_print_info(const t_wf_mesh *mesh);

#endif
