/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   point3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:07:58 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:07:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "point3.h"

t_point3	point3_new(void)
{
	t_point3	point;

	point.x = 0.0;
	point.y = 0.0;
	point.z = 0.0;
	return (point);
}

t_point3	point3_from_vec3(const t_vec3 *vect)
{
	t_point3	point;

	point.x = vect->x;
	point.y = vect->y;
	point.z = vect->z;
	return (point);
}

t_point3	point3_new_coord(double x, double y, double z)
{
	t_point3	point;

	point.x = x;
	point.y = y;
	point.z = z;
	return (point);
}

t_point3 	point3_copy(const t_point3 *p)
{
	return ((t_point3){p->x, p->y, p->z});
}

void	point3_assign(t_point3 *dst, const t_point3 *src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
}

t_point3	point3_neg(const t_point3 *p)
{
	return ((t_point3){-p->x, -p->y, -p->z});
}