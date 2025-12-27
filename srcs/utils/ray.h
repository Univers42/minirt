/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:25:45 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:25:45 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

typedef struct s_ray
{
	t_point3	origin;
	t_vec3		direction;
}t_ray;

static inline t_ray ray_new(const t_point3 *origin, const t_vec3 *direction)
{
	return ((t_ray){*origin, *direction});
}

static inline t_ray	ray_copy(const t_ray *ray)
{
	return ((t_ray){ray->origin, ray->direction});
}

# endif