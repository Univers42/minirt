/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transform.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:20:03 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:20:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "transform.h"

t_transform	transform_new(void)
{
    t_transform t;

    t.to_global = matrix4_new();
    t.to_local = matrix4_new();
    return (t);
}

void	transform_init(t_transform *t)
{
    if (!t)
        return ;
    t->to_global = matrix4_new();
    t->to_local = matrix4_new();
}

void	transform_free(t_transform *t)
{
    (void)t;
    /* no dynamic resources */
}

/* local -> global */
t_vec3	transform_local_to_global_vec3(const t_transform *t, const t_vec3 *v)
{
    return (matrix4_mul_vec3(&t->to_global, v));
}

t_point3	transform_local_to_global_point3(const t_transform *t, const t_point3 *p)
{
    return (matrix4_mul_point3(&t->to_global, p));
}

/* follow original logic: normal uses toLocal multiplication */
t_normal	transform_local_to_global_normal(const t_transform *t, const t_normal *n)
{
    return (matrix4_mul_normal(&t->to_local, n));
}

/* global -> local */
t_vec3	transform_global_to_local_vec3(const t_transform *t, const t_vec3 *v)
{
    return (matrix4_mul_vec3(&t->to_local, v));
}

t_point3	transform_global_to_local_point3(const t_transform *t, const t_point3 *p)
{
    return (matrix4_mul_point3(&t->to_local, p));
}

t_normal	transform_global_to_local_normal(const t_transform *t, const t_normal *n)
{
    return (matrix4_mul_normal(&t->to_local, n));
}

/* modifications */
void	transform_translate(t_transform *t, const t_vec3 *translate)
{
    t_matrix4	translation_matrix;

    translation_matrix = matrix4_new();
    translation_matrix.elements[0][3] = -translate->x;
    translation_matrix.elements[1][3] = -translate->y;
    translation_matrix.elements[2][3] = -translate->z;

    t->to_local = matrix4_mul(&t->to_local, &translation_matrix);

    translation_matrix.elements[0][3] = translate->x;
    translation_matrix.elements[1][3] = translate->y;
    translation_matrix.elements[2][3] = translate->z;

    t->to_global = matrix4_mul(&translation_matrix, &t->to_global);
}

void	transform_scale(t_transform *t, const t_vec3 *scale)
{
    t_matrix4	scale_matrix;

    scale_matrix = matrix4_new();
    scale_matrix.elements[0][0] = 1.0 / scale->x;
    scale_matrix.elements[1][1] = 1.0 / scale->y;
    scale_matrix.elements[2][2] = 1.0 / scale->z;

    t->to_local = matrix4_mul(&t->to_local, &scale_matrix);

    scale_matrix.elements[0][0] = scale->x;
    scale_matrix.elements[1][1] = scale->y;
    scale_matrix.elements[2][2] = scale->z;

    t->to_global = matrix4_mul(&scale_matrix, &t->to_global);
}

void	transform_rotate_x(t_transform *t, double degrees)
{
    double		cos_t;
    double		sin_t;
    t_matrix4	rotation;

    cos_t = cos(degrees * PI / 180.0);
    sin_t = sin(degrees * PI / 180.0);

    rotation = matrix4_new();
    rotation.elements[1][1] = cos_t;
    rotation.elements[2][2] = cos_t;
    rotation.elements[1][2] = sin_t;
    rotation.elements[2][1] = -sin_t;

    t->to_local = matrix4_mul(&t->to_local, &rotation);

    rotation.elements[1][2] = -sin_t;
    rotation.elements[2][1] = sin_t;

    t->to_global = matrix4_mul(&rotation, &t->to_global);
}

void	transform_rotate_y(t_transform *t, double degrees)
{
    double		cos_t;
    double		sin_t;
    t_matrix4	rotation;

    cos_t = cos(degrees * PI / 180.0);
    sin_t = sin(degrees * PI / 180.0);

    rotation = matrix4_new();
    rotation.elements[0][0] = cos_t;
    rotation.elements[0][2] = -sin_t;
    rotation.elements[2][0] = sin_t;
    rotation.elements[2][2] = cos_t;

    t->to_local = matrix4_mul(&t->to_local, &rotation);

    rotation.elements[0][2] = sin_t;
    rotation.elements[2][0] = -sin_t;

    t->to_global = matrix4_mul(&rotation, &t->to_global);
}

void	transform_rotate_z(t_transform *t, double degrees)
{
    double		cos_t;
    double		sin_t;
    t_matrix4	rotation;

    cos_t = cos(degrees * PI / 180.0);
    sin_t = sin(degrees * PI / 180.0);

    rotation = matrix4_new();
    rotation.elements[0][0] = cos_t;
    rotation.elements[1][1] = cos_t;
    rotation.elements[1][0] = -sin_t;
    rotation.elements[0][1] = sin_t;

    t->to_local = matrix4_mul(&t->to_local, &rotation);

    rotation.elements[0][1] = -sin_t;
    rotation.elements[1][0] = sin_t;

    t->to_global = matrix4_mul(&rotation, &t->to_global);
}