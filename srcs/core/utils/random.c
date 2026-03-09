/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:54:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:58:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "random.h"

uint64_t	random_seed(uint64_t seed)
{
	if (seed == 0)
		seed = (uint64_t)time(NULL) ^ 0x9e3779b97f4a7c15ULL;
	return (seed);
}

uint64_t	random_u64(void)
{
	static __thread uint64_t	state = 0;

	if (state == 0)
	{
		uint64_t	t;
		uint64_t	addr;
		uint64_t	tid;

		t = (uint64_t)time(NULL);
		addr = (uint64_t)(uintptr_t)&state;
		tid = 0;
#ifdef _OPENMP
		tid = (uint64_t)omp_get_thread_num();
#endif
		state = random_seed(t ^ addr ^ (tid * 0x9e3779b97f4a7c15ULL));
	}
	state ^= state >> 12;
	state ^= state << 25;
	state ^= state >> 27;
	return (state * 0x2545F4914F6CDD1DULL);
}

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
