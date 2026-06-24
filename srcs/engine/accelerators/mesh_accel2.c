/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mesh_accel_internal.h"

t_mesh_ray	mesh_make_ray(const t_ray *r)
{
	t_mesh_ray	mr;
	int			a;
	float		d[3];

	d[0] = (float)r->dir.x;
	d[1] = (float)r->dir.y;
	d[2] = (float)r->dir.z;
	mr.orig[0] = (float)r->orig.x;
	mr.orig[1] = (float)r->orig.y;
	mr.orig[2] = (float)r->orig.z;
	a = 0;
	while (a < 3)
	{
		mr.inv[a] = 1.0f / d[a];
		mr.neg[a] = (mr.inv[a] < 0.0f);
		a++;
	}
	return (mr);
}

/* Slab test against a node's float AABB; true if the ray overlaps the box. */
static bool	node_hit(const t_mesh_node *n, const t_mesh_ray *mr,
				float tmin, float tmax)
{
	int		a;
	float	t0;
	float	t1;
	float	tmp;

	a = 0;
	while (a < 3)
	{
		t0 = (n->bmin[a] - mr->orig[a]) * mr->inv[a];
		t1 = (n->bmax[a] - mr->orig[a]) * mr->inv[a];
		if (t0 > t1)
		{
			tmp = t0;
			t0 = t1;
			t1 = tmp;
		}
		if (t0 > tmin)
			tmin = t0;
		if (t1 < tmax)
			tmax = t1;
		if (tmax <= tmin)
			return (false);
		a++;
	}
	return (true);
}

/* Direct Möller-Trumbore over a leaf's contiguous triangles; narrows tv. */
static void	hit_leaf(const t_mesh_accel *m, const t_mesh_node *n,
				t_mesh_trav *tv)
{
	t_hit_record	tmp;
	int				i;

	i = 0;
	while (i < n->count)
	{
		if (triangle_hit(&m->tris[n->left_first + i], tv->r, tv->rayt, &tmp))
		{
			tv->hit = true;
			tv->rayt.max = tmp.t;
			if (tv->rec)
				*tv->rec = tmp;
		}
		i++;
	}
}

/* Push a node's two children far-first so the near child pops next.
   left child = self_neg[0] + 1; right child = n->left_first.
   self_neg[1] = ray dir along split axis is negative -> right is nearer. */
static int	push_children(int *stack, int sp, const t_mesh_node *n,
				int self_neg[2])
{
	int	left;
	int	right;

	if (sp + 2 > MESH_STACK_MAX)
		return (sp);
	left = self_neg[0] + 1;
	right = n->left_first;
	if (self_neg[1])
	{
		stack[sp++] = left;
		stack[sp++] = right;
	}
	else
	{
		stack[sp++] = right;
		stack[sp++] = left;
	}
	return (sp);
}

void	mesh_accel_walk(const t_mesh_accel *m, const t_mesh_ray *mr,
			t_mesh_trav *tv)
{
	int					stack[MESH_STACK_MAX];
	int					sp;
	int					self;
	const t_mesh_node	*n;

	sp = 0;
	stack[sp++] = 0;
	while (sp > 0)
	{
		self = stack[--sp];
		n = &m->nodes[self];
		if (!node_hit(n, mr, (float)tv->rayt.min, (float)tv->rayt.max))
			continue ;
		if (n->count > 0)
			hit_leaf(m, n, tv);
		else
			sp = push_children(stack, sp, n,
					(int [2]){self, mr->neg[n->axis]});
	}
}
