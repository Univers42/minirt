/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:12:19 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:12:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NORMAL_H
# define NORMAL_H

# include "point3.h"
# include "vec3.h"

typedef struct s_normal
{
	double x;
	double y;
	double z;
}	t_normal;

t_normal	normal_new(void);
t_normal	normal_new_coord(double x, double y, double z);
t_normal	normal_from_point3(const t_point3 *p);
t_normal	normal_from_vec3(const t_vec3 *v);
t_normal	normal_copy(const t_normal *n);
void		normal_assign(t_normal *dst, const t_normal *src);
double		normal_length(const t_normal *n);
t_normal	normal_normalize(const t_normal *n);
t_normal	normal_neg(const t_normal *n);
double		normal_dot(const t_normal *a, const t_normal *b);
t_normal	normal_cross(const t_normal *a, const t_normal *b);
void		normal_print(const t_normal *n);


#endif