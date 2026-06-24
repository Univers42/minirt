/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "transformations.h"
#include "vector.h"

static void	fill_euler_trig(t_euler euler_angles, float c[3], float s[3])
{
	float	roll;
	float	pitch;
	float	yaw;

	roll = euler_angles.roll * M_PI / 180.0;
	pitch = euler_angles.pitch * M_PI / 180.0;
	yaw = euler_angles.yaw * M_PI / 180.0;
	c[0] = cosf(roll);
	s[0] = sinf(roll);
	c[1] = cosf(pitch);
	s[1] = sinf(pitch);
	c[2] = cosf(yaw);
	s[2] = sinf(yaw);
}

static void	fill_euler_rows(t_matrix *m, float c[3], float s[3])
{
	m->matrix[0][0] = c[2] * c[1];
	m->matrix[0][1] = c[2] * s[1] * s[0] - s[2] * c[0];
	m->matrix[0][2] = c[2] * s[1] * c[0] + s[2] * s[0];
	m->matrix[1][0] = s[2] * c[1];
	m->matrix[1][1] = s[2] * s[1] * s[0] + c[2] * c[0];
	m->matrix[1][2] = s[2] * s[1] * c[0] - c[2] * s[0];
	m->matrix[2][0] = -s[1];
	m->matrix[2][1] = c[1] * s[0];
	m->matrix[2][2] = c[1] * c[0];
}

t_matrix	euler_to_matrix(t_euler euler_angles)
{
	t_matrix	m;
	float		c[3];
	float		s[3];

	m = identity();
	fill_euler_trig(euler_angles, c, s);
	fill_euler_rows(&m, c, s);
	return (m);
}
