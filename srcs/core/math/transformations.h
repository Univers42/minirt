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

/* ------------------------------------------------------------------ */
/*  Legacy vector / quaternion types (used by matrix.c transforms)    */
/* ------------------------------------------------------------------ */

typedef struct s_vector3
{
	float	x;
	float	y;
	float	z;
}	t_vector3;

typedef struct s_quaternion
{
	float	w;
	float	x;
	float	y;
	float	z;
}	t_quaternion;

typedef struct s_euler_angles
{
	float	roll;
	float	pitch;
	float	yaw;
}	t_euler;

/* ------------------------------------------------------------------ */
/*  4 x 4 transformation matrix                                      */
/* ------------------------------------------------------------------ */

typedef struct s_matrix
{
	float	matrix[4][4];
}	t_matrix;

/* A working matrix and its accumulating inverse (Gauss-Jordan). */
typedef struct s_mat_pair
{
	t_matrix	*mat;
	t_matrix	*inv;
}	t_mat_pair;

/* ------------------------------------------------------------------ */
/*  Matrix operations                                                 */
/* ------------------------------------------------------------------ */

t_matrix		identity(void);
t_matrix		t_scale(float factor);
t_matrix		t_scale_xyz(float fx, float fy, float fz);
t_matrix		t_translate(float tx, float ty, float tz);
t_matrix		t_rotate_x(float angle);
t_matrix		t_rotate_y(float angle);
t_matrix		t_rotate_z(float angle);
t_matrix		mat_multiply(t_matrix m1, t_matrix m2);
t_matrix		quaternion_to_matrix(t_quaternion q);
t_matrix		euler_to_matrix(t_euler e);
t_matrix		transpose(t_matrix m);
t_matrix		inverse(t_matrix m);
void			transform_vector(t_matrix t, t_vector3 *v);
void			transform_point(t_matrix t, t_vector3 *v);

/* ------------------------------------------------------------------ */
/*  Gauss-Jordan inverse stages (matrix4.c, shared with matrix5.c)    */
/* ------------------------------------------------------------------ */

int				pivot_phase(t_mat_pair *p);
void			forward_elim(t_mat_pair *p);
int				normalize_diag(t_mat_pair *p);

/* ------------------------------------------------------------------ */
/*  Legacy vector helpers (kept for matrix compat)                    */
/* ------------------------------------------------------------------ */

t_quaternion	quaternion_from_axis_angle(t_vector3 axis, float angle_deg);
t_quaternion	quaternion_normalize(t_quaternion q);
t_quaternion	quaternion_multiply(t_quaternion q1, t_quaternion q2);

#endif
