/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:54:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:58:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "random.h"

real_t	random_real(void)
{
	const uint64_t	rnd = random_u64();
	const uint64_t	mantissa = rnd >> 11;

	return ((real_t)(mantissa * (1.0 / 9007199254740992.0)));
}

real_t	random_real_interval(real_t min, real_t max)
{
	return (min + (max - min) * random_real());
}

int	random_int(int min, int max)
{
	return (min + (int)(random_real() * (real_t)(max - min + 1)));
}

real_t	random_double(void)
{
	return (random_real());
}
