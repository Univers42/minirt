/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   point3.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:05:38 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:05:38 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POINT3_H
# define POINT3_H
# include "vec3.h"
typedef struct s_point3
{
	double	x;
	double	y;
	double	z;
}			t_point3;

t_point3	point3_new(void);
t_point3	point3_from_vec3(const t_vec3 *vect);
t_point3	point3_new_coord(double x, double y, double z);
t_point3 	point3_copy(const t_point3 *p);
t_point3	point_assign(t_point3 *dst, const t_point3 *src);
t_point3	point3_neg(const t_point3 *p);

# endif