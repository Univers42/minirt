/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix8.c                                          :+:      :+:    :+:   */
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

t_quaternion	quaternion_from_axis_angle(t_vector3 axis, float angle_deg)
{
	float			angle;
	float			half_angle;
	t_quaternion	q;

	angle = angle_deg * (float)M_PI / 180.0f;
	half_angle = angle / 2.0f;
	q.w = cosf(half_angle);
	q.x = axis.x * sinf(half_angle);
	q.y = axis.y * sinf(half_angle);
	q.z = axis.z * sinf(half_angle);
	return (quaternion_normalize(q));
}

t_quaternion	quaternion_normalize(t_quaternion q)
{
	float	magnitude;

	magnitude = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	q.w /= magnitude;
	q.x /= magnitude;
	q.y /= magnitude;
	q.z /= magnitude;
	return (q);
}

t_quaternion	quaternion_multiply(t_quaternion q1, t_quaternion q2)
{
	t_quaternion	result;

	result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
	return (result);
}
