/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TREE_H
# define TREE_H

# include "common.h"
# include "cylinder.h"
# include "sphere.h"

/* ------------------------------------------------------------------ */
/*  Tree builder                                                      */
/*  Trunk = cylinder, canopy = sphere or cone cluster                 */
/* ------------------------------------------------------------------ */

/* Simple: cylinder trunk + sphere canopy */
void	build_simple_tree(t_hittable_list *world,
			const t_point3 *base,
			real_t trunk_height, real_t trunk_radius,
			real_t canopy_radius,
			t_material *bark_mat, t_material *leaf_mat);

/* Pine-style: cylinder trunk + layered cones */
void	build_pine_tree(t_hittable_list *world,
			const t_point3 *base,
			real_t height, real_t base_radius,
			t_material *bark_mat, t_material *needle_mat);

#endif
