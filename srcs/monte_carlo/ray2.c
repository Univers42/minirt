/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 17:09:23 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:13:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ray.h"

t_vec3	ray_at(t_ray *ray, real_t t)
{
	t_vec3	scaled;

	scaled = vec3_mul_scalar(&ray->dir, t);
	return (vec3_add(&ray->orig, &scaled));
}
