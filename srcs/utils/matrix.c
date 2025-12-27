/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:17:34 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:17:34 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "matrix.h"
#include "string.h"
#include "math.h"

t_matrix4	matrix4_new(void)
{
    t_matrix4	m;
    int			i;
    int			j;

    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
            m.elements[i][j] = (i == j) ? 1.0 : 0.0;
    }
    return (m);
}

t_matrix4	matrix4_copy(const t_matrix4 *mat)
{
    t_matrix4	m;
    memcpy(&m, mat, sizeof(t_matrix4));
    return (m);
}

t_matrix4	matrix4_assign(t_matrix4 *dst, const t_matrix4 *src)
{
    if (dst == src || src == NULL)
        return (*dst);
    memcpy(dst, src, sizeof(t_matrix4));
    return (*dst);
}

void	matrix4_set_identity(t_matrix4 *m)
{
    int i;
    int j;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            m->elements[i][j] = (i == j) ? 1.0 : 0.0;
}

t_matrix4	matrix4_mul(const t_matrix4 *a, const t_matrix4 *b)
{
    t_matrix4	r;
    int			i;
    int			j;
    int			k;

    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            double sum = 0.0;
            for (k = 0; k < 4; ++k)
                sum += a->elements[i][k] * b->elements[k][j];
            r.elements[i][j] = sum;
        }
    }
    return (r);
}

t_vec3	matrix4_mul_vec3(const t_matrix4 *m, const t_vec3 *v)
{
    t_vec3	r;

    r.x = m->elements[0][0] * v->x + m->elements[0][1] * v->y + m->elements[0][2] * v->z;
    r.y = m->elements[1][0] * v->x + m->elements[1][1] * v->y + m->elements[1][2] * v->z;
    r.z = m->elements[2][0] * v->x + m->elements[2][1] * v->y + m->elements[2][2] * v->z;
    return (r);
}

t_point3	matrix4_mul_point3(const t_matrix4 *m, const t_point3 *p)
{
    t_point3	r;

    r.x = m->elements[0][0] * p->x + m->elements[0][1] * p->y + m->elements[0][2] * p->z + m->elements[0][3];
    r.y = m->elements[1][0] * p->x + m->elements[1][1] * p->y + m->elements[1][2] * p->z + m->elements[1][3];
    r.z = m->elements[2][0] * p->x + m->elements[2][1] * p->y + m->elements[2][2] * p->z + m->elements[2][3];
    return (r);
}

/* follow same mapping as original C++: multiply normal by the transpose of upper-left 3x3 */
t_normal	matrix4_mul_normal(const t_matrix4 *m, const t_normal *n)
{
    t_normal	r;

    r.x = m->elements[0][0] * n->x + m->elements[1][0] * n->y + m->elements[2][0] * n->z;
    r.y = m->elements[0][1] * n->x + m->elements[1][1] * n->y + m->elements[2][1] * n->z;
    r.z = m->elements[0][2] * n->x + m->elements[1][2] * n->y + m->elements[2][2] * n->z;
    return (r);
}

t_matrix4	matrix4_div_scalar(const t_matrix4 *m, double s)
{
    t_matrix4	r;
    int			i;
    int			j;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            r.elements[i][j] = m->elements[i][j] / s;
    return (r);
}