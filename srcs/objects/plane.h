/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:36:16 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:36:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLANE_H
# define PLANE_H

# include "../includes/utils.h"
# include "../includes/objects.h"

typedef struct s_plane
{
	t_point3	position;
	t_normal	normal;
	void		*shader;
}	t_plane;

void	plane_init(t_plane *p);
void 	plane_init_pos_norm_shader(t_plane *p, t_point3 position, t_normal normal, void *shader);
void	plane_init_pos_norm(t_plane *p, t_point3 position, t_normal normal);
void	plane_copy(t_plane *dst, t_plane const *src);
int		plane_hist(t_plane const *plane, t_ray const *ray, t_posnorm *inter_out_normal, double *tmin);
t_box	plane_calculate_bound(t_plane const *plane);

#endif
