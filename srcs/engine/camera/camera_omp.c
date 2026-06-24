/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_omp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:53:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"
#ifdef _OPENMP
# include <omp.h>
#else
# include <time.h>
#endif

/*
** OpenMP-vs-sequential wrappers.  The whole function is selected at the
** global scope by #ifdef so no preprocessor lives inside a function body
** (42 norm).  RT_NUM_THREADS is a compile constant, so the runtime `if`
** that pins the team size is folded away by the optimiser.
*/
#ifdef _OPENMP

double	rt_wtime(void)
{
	return (omp_get_wtime());
}

void	rt_configure_threads(void)
{
	if (RT_NUM_THREADS > 0)
		omp_set_num_threads(RT_NUM_THREADS);
	fprintf(stderr, "Threads: %d (OpenMP)\n", omp_get_max_threads());
}

#else

double	rt_wtime(void)
{
	return ((double)clock() / (double)CLOCKS_PER_SEC);
}

void	rt_configure_threads(void)
{
	fprintf(stderr, "Threads: 1 (sequential, compile with -fopenmp)\n");
}

#endif
