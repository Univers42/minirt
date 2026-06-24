/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENVIRONMENT_H
# define ENVIRONMENT_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include <stdbool.h>

struct	s_texture;

/* Read-only global environment image (equirectangular). Set once before
   the OpenMP render begins; sampled on ray-miss by both engines. */
void					set_scene_environment(struct s_texture *tex);
const struct s_texture	*get_scene_environment(void);
t_color					bg_environment_color(const t_ray *r);
bool					load_scene_environment(const char *path);
void					free_scene_environment(void);

#endif
