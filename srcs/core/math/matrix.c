/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.c                                           :+:      :+:    :+:   */
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

t_matrix	identity(void)
{
	t_matrix	id;
	int			i;
	int			j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			id.matrix[i][j] = (float)(i == j);
			j++;
		}
		i++;
	}
	return (id);
}

t_matrix	t_scale(float factor)
{
	t_matrix	scale;

	scale = identity();
	scale.matrix[0][0] = factor;
	scale.matrix[1][1] = factor;
	scale.matrix[2][2] = factor;
	return (scale);
}

t_matrix	t_scale_xyz(float factor_x, float factor_y, float factor_z)
{
	t_matrix	scale;

	scale = identity();
	scale.matrix[0][0] = factor_x;
	scale.matrix[1][1] = factor_y;
	scale.matrix[2][2] = factor_z;
	return (scale);
}

t_matrix	t_translate(float tx, float ty, float tz)
{
	t_matrix	translate;

	translate = identity();
	translate.matrix[0][3] = tx;
	translate.matrix[1][3] = ty;
	translate.matrix[2][3] = tz;
	return (translate);
}

t_matrix	t_rotate_x(float angle)
{
	t_matrix	rotation_x;

	rotation_x = identity();
	rotation_x.matrix[1][1] = cosf(angle);
	rotation_x.matrix[1][2] = -sinf(angle);
	rotation_x.matrix[2][1] = sinf(angle);
	rotation_x.matrix[2][2] = cosf(angle);
	return (rotation_x);
}

t_matrix	t_rotate_y(float angle)
{
	t_matrix	rotation_y;

	rotation_y = identity();
	rotation_y.matrix[0][0] = cosf(angle);
	rotation_y.matrix[0][2] = sinf(angle);
	rotation_y.matrix[2][0] = -sinf(angle);
	rotation_y.matrix[2][2] = cosf(angle);
	return (rotation_y);
}

t_matrix	t_rotate_z(float angle)
{
	t_matrix	rotation_z;

	rotation_z = identity();
	rotation_z.matrix[0][0] = cosf(angle);
	rotation_z.matrix[0][1] = -sinf(angle);
	rotation_z.matrix[1][0] = sinf(angle);
	rotation_z.matrix[1][1] = cosf(angle);
	return (rotation_z);
}

t_matrix	mat_multiply(t_matrix m1, t_matrix m2)
{
	t_matrix	new_mat;
	int			i;
	int			j;
	int			k;

	new_mat = identity();
	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			k = 0;
			while (k < 4)
			{
				new_mat.matrix[i][j] += m1.matrix[i][k] * m2.matrix[k][j];
				k++;
			}
			j++;
		}
		i++;
	}
	return (new_mat);
}

t_matrix	quaternion_to_matrix(Quaternion q)
{
	t_matrix	m;
	float		w;
	float		x;
	float		y;
	float		z;

	m = identity();
	w = q.w;
	x = q.x;
	y = q.y;
	z = q.z;
	m.matrix[0][0] = 1 - 2 * (y * y + z * z);
	m.matrix[0][1] = 2 * (x * y - z * w);
	m.matrix[0][2] = 2 * (x * z + y * w);
	m.matrix[1][0] = 2 * (x * y + z * w);
	m.matrix[1][1] = 1 - 2 * (x * x + z * z);
	m.matrix[1][2] = 2 * (y * z - x * w);
	m.matrix[2][0] = 2 * (x * z - y * w);
	m.matrix[2][1] = 2 * (y * z + x * w);
	m.matrix[2][2] = 1 - 2 * (x * x + y * y);
	return (m);
}

t_matrix	euler_to_matrix(Euler_Angles euler_angles)
{
	t_matrix	m;
	float		roll;
	float		pitch;
	float		yaw;
	float		c[3];
	float		s[3];

	m = identity();
	roll = euler_angles.roll * M_PI / 180.0;
	pitch = euler_angles.pitch * M_PI / 180.0;
	yaw = euler_angles.yaw * M_PI / 180.0;
	c[0] = cosf(roll);
	s[0] = sinf(roll);
	c[1] = cosf(pitch);
	s[1] = sinf(pitch);
	c[2] = cosf(yaw);
	s[2] = sinf(yaw);
	m.matrix[0][0] = c[2] * c[1];
	m.matrix[0][1] = c[2] * s[1] * s[0] - s[2] * c[0];
	m.matrix[0][2] = c[2] * s[1] * c[0] + s[2] * s[0];
	m.matrix[1][0] = s[2] * c[1];
	m.matrix[1][1] = s[2] * s[1] * s[0] + c[2] * c[0];
	m.matrix[1][2] = s[2] * s[1] * c[0] - c[2] * s[0];
	m.matrix[2][0] = -s[1];
	m.matrix[2][1] = c[1] * s[0];
	m.matrix[2][2] = c[1] * c[0];
	return (m);
}

t_matrix	transpose(t_matrix m)
{
	return ((t_matrix){
		.matrix = {
			{m.matrix[0][0], m.matrix[1][0], m.matrix[2][0], m.matrix[3][0]},
			{m.matrix[0][1], m.matrix[1][1], m.matrix[2][1], m.matrix[3][1]},
			{m.matrix[0][2], m.matrix[1][2], m.matrix[2][2], m.matrix[3][2]},
			{m.matrix[0][3], m.matrix[1][3], m.matrix[2][3], m.matrix[3][3]}}
	});
}

static void	print_mat(t_matrix m)
{
	int	i;
	int	j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			printf("%0.4f ", m.matrix[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
	printf("\n");
}

static void	swap(float *a, float *b)
{
	float	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

t_matrix	inverse(t_matrix mat)
{
	t_matrix	inv;
	int			i;
	int			j;
	int			k;
	int			max;
	float		c;

	inv = identity();
	j = 0;
	while (j < 4)
	{
		if (mat.matrix[j][j] == 0.0f)
		{
			max = j;
			i = j;
			while (i < 4)
			{
				if (fabs(mat.matrix[i][j]) >= fabs(mat.matrix[max][j]))
					max = i;
				i++;
			}
			if (max == j)
			{
				fprintf(stderr, "Singular matrix, couldn't invert\n");
				return (identity());
			}
			k = 0;
			while (k < 4)
			{
				swap(&mat.matrix[max][k], &mat.matrix[j][k]);
				swap(&inv.matrix[max][k], &inv.matrix[j][k]);
				k++;
			}
		}
		j++;
	}
	i = 0;
	while (i < 3)
	{
		j = i + 1;
		while (j < 4)
		{
			if (mat.matrix[j][i] != 0.0f)
			{
				c = mat.matrix[j][i] / mat.matrix[i][i];
				k = 0;
				while (k < 4)
				{
					mat.matrix[j][k] -= mat.matrix[i][k] * c;
					inv.matrix[j][k] -= inv.matrix[i][k] * c;
					k++;
				}
				mat.matrix[j][i] = 0.f;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (i < 4)
	{
		if (mat.matrix[i][i] == 0.0f)
		{
			fprintf(stderr, "Singular matrix, couldn't invert\n");
			return (identity());
		}
		c = 1 / (float)mat.matrix[i][i];
		j = 0;
		while (j < 4)
		{
			mat.matrix[i][j] *= c;
			inv.matrix[i][j] *= c;
			j++;
		}
		i++;
	}
	i = 0;
	while (i < 3)
	{
		j = i + 1;
		while (j < 4)
		{
			c = mat.matrix[i][j];
			k = 0;
			while (k < 4)
			{
				mat.matrix[i][k] -= mat.matrix[j][k] * c;
				inv.matrix[i][k] -= inv.matrix[j][k] * c;
				k++;
			}
			mat.matrix[i][j] = 0.f;
			j++;
		}
		i++;
	}
	return (inv);
}

void	transform_vector(t_matrix transform, Vector3 *vector)
{
	float	x;
	float	y;
	float	z;

	if (vector == NULL)
	{
		fprintf(stderr, "NULL vector\n");
		return ;
	}
	x = vector->x;
	y = vector->y;
	z = vector->z;
	vector->x = transform.matrix[0][0] * x + transform.matrix[0][1] * y
		+ transform.matrix[0][2] * z;
	vector->y = transform.matrix[1][0] * x + transform.matrix[1][1] * y
		+ transform.matrix[1][2] * z;
	vector->z = transform.matrix[2][0] * x + transform.matrix[2][1] * y
		+ transform.matrix[2][2] * z;
}

void	transform_point(t_matrix transform, Vector3 *vector)
{
	float	x;
	float	y;
	float	z;

	if (vector == NULL)
	{
		fprintf(stderr, "NULL vector\n");
		return ;
	}
	x = vector->x;
	y = vector->y;
	z = vector->z;
	vector->x = transform.matrix[0][0] * x + transform.matrix[0][1] * y
		+ transform.matrix[0][2] * z + transform.matrix[0][3];
	vector->y = transform.matrix[1][0] * x + transform.matrix[1][1] * y
		+ transform.matrix[1][2] * z + transform.matrix[1][3];
	vector->z = transform.matrix[2][0] * x + transform.matrix[2][1] * y
		+ transform.matrix[2][2] * z + transform.matrix[2][3];
}

static float	legacy_dot(Vector3 v1, Vector3 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

static float	legacy_length(Vector3 v)
{
	return (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
}

static Vector3	legacy_normalize(Vector3 v)
{
	float	len;

	len = legacy_length(v);
	return ((Vector3){v.x / len, v.y / len, v.z / len});
}

Quaternion	quaternion_from_axis_angle(Vector3 axis, float angle_deg)
{
	float		angle;
	float		half_angle;
	Quaternion	q;

	angle = DEG_TO_RAD(angle_deg);
	half_angle = angle / 2.0f;
	q.w = cosf(half_angle);
	q.x = axis.x * sinf(half_angle);
	q.y = axis.y * sinf(half_angle);
	q.z = axis.z * sinf(half_angle);
	return (quaternion_normalize(q));
}

Quaternion	quaternion_normalize(Quaternion q)
{
	float	magnitude;

	magnitude = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	q.w /= magnitude;
	q.x /= magnitude;
	q.y /= magnitude;
	q.z /= magnitude;
	return (q);
}

Quaternion	quaternion_multiply(Quaternion q1, Quaternion q2)
{
	Quaternion	result;

	result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
	return (result);
}
