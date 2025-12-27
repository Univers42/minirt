/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:12:17 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:12:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "normal.h"

t_normal	normal_new(void)
{
    return ((t_normal){0.0, 0.0, 0.0});
}

t_normal	normal_new_coord(double x, double y, double z)
{
    return ((t_normal){x, y, z});
}

t_normal	normal_from_point3(const t_point3 *p)
{
    return ((t_normal){p->x, p->y, p->z});
}

t_normal	normal_from_vec3(const t_vec3 *v)
{
    return ((t_normal){v->x, v->y, v->z});
}

t_normal	normal_copy(const t_normal *n)
{
    return ((t_normal){n->x, n->y, n->z});
}

void	normal_assign(t_normal *dst, const t_normal *src)
{
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

double	normal_length(const t_normal *n)
{
    return (sqrt(n->x * n->x + n->y * n->y + n->z * n->z));
}

t_normal	normal_normalize(const t_normal *n)
{
    double	len;

    len = normal_length(n);
    if (len == 0.0)
        return (normal_new());
    return (normal_new_coord(n->x / len, n->y / len, n->z / len));
}

t_normal	normal_neg(const t_normal *n)
{
    return ((t_normal){-n->x, -n->y, -n->z});
}

double	normal_dot(const t_normal *a, const t_normal *b)
{
    return (a->x * b->x + a->y * b->y + a->z * b->z);
}

t_normal	normal_cross(const t_normal *a, const t_normal *b)
{
    return ((t_normal){
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    });
}

void	normal_print(const t_normal *n)
{
    printf("%g / %g / %g\n", n->x, n->y, n->z);
}