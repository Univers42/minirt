/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_flat_internal.h"
#include <stdlib.h>

/* Slab test against a node's float AABB; true if the ray overlaps the box. */
static bool	node_hit(const t_flat_node *n, const t_flat_ray *fr,
				float tmin, float tmax)
{
	int		a;
	float	t0;
	float	t1;
	float	tmp;

	a = 0;
	while (a < 3)
	{
		t0 = (n->bmin[a] - fr->orig[a]) * fr->inv[a];
		t1 = (n->bmax[a] - fr->orig[a]) * fr->inv[a];
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

static t_flat_ray	make_flat_ray(const t_ray *r)
{
	t_flat_ray	fr;
	int			a;
	float		d[3];

	d[0] = (float)r->dir.x;
	d[1] = (float)r->dir.y;
	d[2] = (float)r->dir.z;
	fr.orig[0] = (float)r->orig.x;
	fr.orig[1] = (float)r->orig.y;
	fr.orig[2] = (float)r->orig.z;
	a = 0;
	while (a < 3)
	{
		fr.inv[a] = 1.0f / d[a];
		fr.neg[a] = (fr.inv[a] < 0.0f);
		a++;
	}
	return (fr);
}

/* Intersect every primitive of one leaf; narrows tv->rayt.max on each hit. */
static void	hit_leaf(const t_flat_bvh *bvh, const t_flat_node *n, t_trav *tv)
{
	t_hit_record		tmp;
	const t_flat_leaf	*lf;
	int					i;

	i = 0;
	while (i < n->count)
	{
		lf = &bvh->leaves[n->left_first + i];
		lf->set_current(lf->object);
		if (lf->hit_noobj(tv->r, tv->rayt, &tmp))
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
   neg = ray dir along the split axis is negative -> right child is nearer. */
static int	push_children(int *stack, int sp, const t_flat_node *n,
				int self_neg[2])
{
	int	left;
	int	right;

	if (sp + 2 > FLAT_STACK_MAX)
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

bool	flat_bvh_hit(const t_flat_bvh *bvh, const t_ray *r,
			t_interval rayt, t_hit_record *rec)
{
	t_flat_ray			fr;
	int					stack[FLAT_STACK_MAX];
	int					sp;
	const t_flat_node	*n;
	t_trav				tv;

	if (!bvh || bvh->node_count == 0)
		return (false);
	fr = make_flat_ray(r);
	tv = (t_trav){r, rayt, rec, false};
	sp = 0;
	stack[sp++] = 0;
	while (sp > 0)
	{
		n = &bvh->nodes[stack[--sp]];
		if (!node_hit(n, &fr, (float)tv.rayt.min, (float)tv.rayt.max))
			continue ;
		if (n->count > 0)
			hit_leaf(bvh, n, &tv);
		else
			sp = push_children(stack, sp, n,
					(int [2]){(int)(n - bvh->nodes), fr.neg[n->axis]});
	}
	return (tv.hit);
}
