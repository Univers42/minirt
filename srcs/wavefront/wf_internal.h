/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_internal.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:52:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WF_INTERNAL_H
# define WF_INTERNAL_H

# include "wf_types.h"
# include "wf_str.h"

/* Model array helpers (wf_model.c / wf_model2.c / wf_model3.c) */
bool	wf_model_add_vert(t_wf_model *m, t_wf_vec3 v);
bool	wf_model_add_norm(t_wf_model *m, t_wf_vec3 n);
bool	wf_model_add_texcoord(t_wf_model *m, t_wf_vec2 tc);
bool	wf_model_ensure_group(t_wf_model *m);
bool	wf_model_add_group(t_wf_model *m, const char *name);
bool	wf_group_add_face(t_wf_model *m, const t_wf_face *f);
void	wf_model_set_material(t_wf_model *m, const char *name);
void	wf_model_compute_center(t_wf_model *m);

/* Face parsing (wf_face.c) */
bool	wf_parse_face_line(t_wf_model *m, const char *line);

/* Line dispatch (wf_parse.c) */
bool	wf_dispatch_line(t_wf_model *m, const char *line);

#endif
