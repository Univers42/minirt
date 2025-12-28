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
# include "vec3.h"
# include "point3.h"
# include "normal.h"
# include <math.h>

#ifndef PI
# define PI 3.14159265358979323846
#endif

typedef struct s_transform
{
	t_matrix4	to_global;
	t_matrix4	to_local;
}	t_transform;

t_transform	transform_new(void);
void		transform_init(t_transform *t);
void		transform_free(t_transform *t);

// local <-> global
t_vec3		transform_local_to_global_vec3(const t_transform *t, const t_vec3 *v);
t_point3	transform_local_to_global_point3(const t_transform *t, const t_point3 *p);
t_normal	transform_local_to_global_normal(const t_transform *t, const t_normal *n);

t_vec3		transform_global_to_local_vec3(const t_transform *t, const t_vec3 *v);
t_point3	transform_global_to_local_point3(const t_transform *t, const t_point3 *p);
t_normal	transform_global_to_local_normal(const t_transform *t, const t_normal *n);

void		transform_translate(t_transform *t, const t_vec3 *translate);
void		transform_scale(t_transform *t, const t_vec3 *scale);
void		transform_rotate_x(t_transform *t, double degrees);
void		transform_rotate_y(t_transform *t, double degrees);
void		transform_rotate_z(t_transform *t, double degrees);

#endif