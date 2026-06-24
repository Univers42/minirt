/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector6.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vector.h"
#include "random.h"

t_vec3	random_in_unit_sphere(void)
{
	t_vec3	p;

	while (1)
	{
		p = vec3_random_interval((real_t)(-1.0), (real_t)1.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return (p);
	}
}

t_vec3	random_in_unit_disk(void)
{
	t_vec3	p;

	while (1)
	{
		p = vec3_create(
				random_real_interval((real_t)(-1.0), (real_t)1.0),
				random_real_interval((real_t)(-1.0), (real_t)1.0),
				(real_t)0.0);
		if (vec3_length_squared(&p) < (real_t)1.0)
			return (p);
	}
}
