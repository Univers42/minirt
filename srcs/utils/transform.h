/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transform.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:20:15 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:20:15 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRANSFORM_H
# define TRANSFORM_H

# include "matrix.h"
# include "vect3.h"
# include "point3.h"
# include "normal.h"
# include <math.h>

#ifndef PI
# define PI 3.14159265358979323846
#endif

tyepdef struct s_transform
{
	t_matrix4	to_global;
	t_matrix4	to_local;
}	t_transform;

t_transform transform_new(void);
t_transform	*init_transform();
void		free_transform(t_transform *transform);

// local <-> global
t_vect3		transform_local_to_global_vect3(t_transform *transform, t_vect3 const vect);
t_point3	transform_local_to_global_point3(t_transform *transform, t_point3 const point);
t_normal	transform_local_to_global_normal(t_transform *transform, t_normal const normal);

t_vect3		transform_global_to_local_vect3(t_transform *transform, t_vect3 const vect);
t_point3	transform_global_to_local_point3(t_transform *transform, t_point3 const point);
t_normal	transform_global_to_local_normal(t_transform *transform, t_normal const normal);

void	transform_translate(t_transform *transform, t_vect3 const translate);
void	transform_scale(t_transform *transform, t_vect3 const scale);
void	transform_rotate_x(t_transform *transform, double r);
void	transform_rotate_y(t_transform *transform, double r);
void	transform_rotate_z(t_transform *transform, double r);
# endif