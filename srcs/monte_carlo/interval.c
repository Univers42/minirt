/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:00:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:14:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interval.h"

t_interval	interval(real_t min, real_t max)
{
	t_interval	between;

	if (min < max)
		between.sense = true;
	else
		between.sense = false;
	between.min = min;
	between.max = max;
	return (between);
}

t_interval	interval_merge(const t_interval *a, const t_interval *b)
{
	real_t	new_min;
	real_t	new_max;

	if (a->min <= b->min)
		new_min = a->min;
	else
		new_min = b->min;
	if (a->max >= b->max)
		new_max = a->max;
	else
		new_max = b->max;
	return (interval(new_min, new_max));
}

bool	contains(real_t min, real_t max, real_t x)
{
	return (min <= x && x <= max);
}

real_t	clamp(real_t x, real_t min, real_t max)
{
	if (x < min)
		return (min);
	else if (x > max)
		return (max);
	return (x);
}

bool	surrounds(real_t min, real_t max, real_t x)
{
	return (min < x && x < max);
}
