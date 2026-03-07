/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:00:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:14:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interval.h"

real_t	interval_clamp(const t_interval *it, real_t x)
{
	return (clamp(x, it->min, it->max));
}

real_t	interval_size(const t_interval *it)
{
	return (it->max - it->min);
}

bool	interval_contains(const t_interval *it, real_t x)
{
	return (contains(it->min, it->max, x));
}

bool	interval_surrounds(const t_interval *it, real_t x)
{
	return (surrounds(it->min, it->max, x));
}

t_interval	interval_expand(real_t min, real_t max, real_t delta)
{
	real_t	padding;

	padding = delta / (real_t)2.0;
	return (interval(min - padding, max + padding));
}
