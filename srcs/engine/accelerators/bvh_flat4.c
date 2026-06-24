/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_flat.h"
#include "shading.h"
#include "studio_config.h"

/* void-typed adapter so a t_hittable_list can store flat_bvh_hit opaquely. */
bool	flat_bvh_hit_v(const void *bvh, const t_ray *r,
			t_interval rayt, t_hit_record *rec)
{
	return (flat_bvh_hit((const t_flat_bvh *)bvh, r, rayt, rec));
}

/* Build a flat BVH over world and install it as accel's fast hit path.
   Only active for the deterministic engine with RT_FAST_BVH enabled; returns
   the bvh (caller frees with flat_bvh_free) or NULL when the fast path is off
   or the build fails (in which case accel keeps its wrapper-loop behaviour). */
t_flat_bvh	*accel_attach_fast(t_hittable_list *accel,
			const t_hittable_list *world)
{
	t_flat_bvh	*bvh;

	if (RT_FAST_BVH == 0 || render_get_engine_mode() != ENGINE_DIRECT)
		return (NULL);
	bvh = flat_bvh_build(world);
	if (!bvh)
		return (NULL);
	accel->fast = bvh;
	accel->fast_hit = flat_bvh_hit_v;
	return (bvh);
}
