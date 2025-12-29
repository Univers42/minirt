/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:38:19 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:38:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "box.h"

t_point3	box_get_vertices(const t_box *b, t_point3 v[8])
{
	if (!b || !v)
		return (NULL);
	v[0] = b->p1;
    v[1] = (t_point3){ b->p2.x_, b->p1.y_, b->p1.z_ };
    v[2] = (t_point3){ b->p2.x_, b->p2.y_, b->p1.z_ };
    v[3] = (t_point3){ b->p1.x_, b->p2.y_, b->p1.z_ };
    v[4] = b->p2;
    v[5] = (t_point3){ b->p1.x_, b->p2.y_, b->p2.z_ };
    v[6] = (t_point3){ b->p1.x_, b->p1.y_, b->p2.z_ };
    v[7] = (t_point3){ b->p2.x_, b->p2.y_, b->p2.z_ };
	return (v);
}

int	box_inside(const t_box *b, const t_point3 *p)
{
	if (!b || !p)
		return (0);
	return ((p->x >= b->p1.x && p->x <= b->p2.x)
			&& (p->y >= b->p1.y && p->y <= b->p2.y)
			&& (p->z >= b->p1.z && p->z <= b->p2.z));
}

int	box_hit(const t_box *b, const t_ray *ray, double *t)
{
	t_point3	o;
	t_point3	d;
	t_point3	tmin;
	t_point3	tmax;
	double		a;
	double		binv;
	double		c;
	double		t0;
	double		t1;
	int			hit;

	if (!b || !ray || !t)
		return (0);
	o = (t_point3){.x = ray->origin.x, .y = ray->origin.y, .z = ray->origin.z};
	d = (t_point3){.x = ray->direction.x, .y = ray->direction.y, .z = ray->direction.z};
	a = 1.0 / d.x;
	if (a >= 0.0)
	{
		t_min.x = (b->p1.x - o.x) * a;
		t_max.x = (b->p2.x - o.x) * a;
	}
	else
	{
		t_min.x = (b->p2.x - o.x) * a;
		t_max.x = (b->p1.x - o.x) * a;
	}
	binv = 1.0 / d.y;
	if (binv >= 0.0)
	{
		tmin.y = (b->p1.y - o.y) *binv;
		tmax.y = (b->p2.y - o.y) * binv;
	}
	else
	{
		tmin.y = (b->p2.y - o.y) * binv;
		tmax.y = (b->p1.y - o.y) * binv;
	}
	c = 1.0 / dz;
	if (c >= 0.0)
	{
		tmin.z = (b->p1.z - o.z) * c;
		tmax.z = (b->p2.z - o.z) * c;
	}
	else
	{
		tmin.z = (b->p2.z - o.z) * c;
		tmax.z = (b->p1.z - o.z) * c;
	}
	if (tmin.x > tmin.y)
		t0 = tmin.x;
	else
		t0 = tmin.y;
	if (tmax.x < tmax.y)
		t1 = tmax.x;
	else
		t1 = tmax.y;
	hit = 0;
	if (t0 < t1 && t1 > KEPSILON)
		hit = 1;
	*t = t1;
	return (hit);
}

