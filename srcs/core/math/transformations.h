/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transformations.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRANSFORMATIONS_H
# define TRANSFORMATIONS_H

# include <math.h>
# include <stdio.h>

# ifndef M_PI
#  define M_PI 3.14159265358979323846
# endif

# define DEG_TO_RAD(a) ((a) * (float)M_PI / 180.0f)

/* ------------------------------------------------------------------ */
/*  Legacy vector / quaternion types (used by matrix.c transforms)    */
/* ------------------------------------------------------------------ */

typedef struct s_vector3
{
	float	x;
	float	y;
	float	z;
}	Vector3;

typedef struct s_quaternion
{
	float	w;
	float	x;
	float	y;
	float	z;
}	Quaternion;

typedef struct s_euler_angles
{
	float	roll;
	float	pitch;
	float	yaw;
}	Euler_Angles;

/* ------------------------------------------------------------------ */
/*  4 x 4 transformation matrix                                      */
/* ------------------------------------------------------------------ */

typedef struct s_matrix
{
	float	matrix[4][4];
}	t_matrix;

/* ------------------------------------------------------------------ */
/*  Matrix operations                                                 */
/* ------------------------------------------------------------------ */

t_matrix	identity(void);
t_matrix	t_scale(float factor);
t_matrix	t_scale_xyz(float fx, float fy, float fz);
t_matrix	t_translate(float tx, float ty, float tz);
t_matrix	t_rotate_x(float angle);
t_matrix	t_rotate_y(float angle);
t_matrix	t_rotate_z(float angle);
t_matrix	mat_multiply(t_matrix m1, t_matrix m2);
t_matrix	quaternion_to_matrix(Quaternion q);
t_matrix	euler_to_matrix(Euler_Angles e);
t_matrix	transpose(t_matrix m);
t_matrix	inverse(t_matrix m);
void		transform_vector(t_matrix t, Vector3 *v);
void		transform_point(t_matrix t, Vector3 *v);

/* ------------------------------------------------------------------ */
/*  Legacy vector helpers (kept for matrix compat)                    */
/* ------------------------------------------------------------------ */

Quaternion	quaternion_from_axis_angle(Vector3 axis, float angle_deg);
Quaternion	quaternion_normalize(Quaternion q);
Quaternion	quaternion_multiply(Quaternion q1, Quaternion q2);

#endif
