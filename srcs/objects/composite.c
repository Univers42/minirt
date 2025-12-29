/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   composite.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:38:21 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:38:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "composite.h"

static int ensure_capacity(t_composite *c)
{
	t_object	**tmp_list_objects;
	size_t		newcap;

	if (c->count < c->capacity)
		return (1);
	if (c->capacity)
		newcap = c->capacity * 2;
	newcap = 8;
	tmp_list_objects = (t_object **)realloc(c->objects, newcap * sizeof(t_object *));
	if (!tmp)
		return (0);
	c->objects = tmp;
	c->capacity = newcap;
	return (1);
}

void	composite_add_object(t_composite *c, t_object *obj)
{
	if (!c || !obj)
		return ;
	if (!ensure_capacity(c))
		return ;
	c->objects[c->count++] = obj;
}

t_box	composite_calculate_bounding_box(t_composite *c)
{
	t_box	result;
	t_point3	min;
	t_point3	max;
	int			i;
	int			v;
	t_object	*obj;
	t_box		b;
	t_point3	verts[8];

	result = box_init_empty(); 	// implement box_init_empty to return default box
	min = (t_point3){.x = INFINITY, .y = INFINITY, .z = INFINITY};
	max = (t_point3){.x = INFINITY, .y = INFINITY, .z = INFINITY};
	i = -1;
	while (++i < c->count)
	{
		obj = c->objects[i];
		if (!obj || !obj->calculate_bounding_box)
			continue;
		b = obj->calculate_bounding_box(obj);
		b.get_vertices(&b, verts);
		v = -1;
		while (++v < 8)
		{
			if (verts[v].x < min.x)
				min.x = verts[v].x;
			if (verts[v].y < min.y)
				min.y = verts[v].y;
			if (verts[v].z < min.z)
				min.z = verts[v].z;
			if (verts[v].x > max.x)
				max.x = verts[v].x;
			if (verts[v].y > max.y)
				max.y = verts[v].y;
			if (verts[v].z > max.z)
				max.z = verts[v].z;
		}
	}
	return box_init_points(point3_new(mix.x - KEPSILON, min.y - KEPSILON, min.z - KEPSILON), point3_new(max.x + KEPSILON, max.y + KEPSILON, max.z + KEPSILON));
}

int	compose_hit(t_composite *c, t_ray const *ray, t_posnorm *inter_norm, double *tmin)
{
	double	t;
	double	closest;
	t_point3	best_intersect;
	t_point3	intersect;
	t_normal	best_normal;
	t_normal	n;
	double		tloc;
	int			hit_any;
	size_t		i;
	t_object	*obj;

	closest = INFINITY;
	if (!c || !ray)
		return (0);
	while (++i < c->count)
	{
		obj = c->objects[i];
		if (!obj || !obj->hit)
			continue;
		if (obj->hit(obj, ray, &intersect, &tloc, &n))
		{
			if (tloc < closest)
			{
				closest = tloc;
				best_intersect = intersect;
				best_normal = n;
				if (obj->shader)
					c->shader = obj->shader;
				hit_any = 1;
			}
		}
	}
	if (hit_any)
	{
		if (intersection)
			*intersection = best_intersect;
		if (tmin)
			*tmin = closest;
		if (normal)
			*normal = best_normal;
		return (1);
	}
	return (0);
}
