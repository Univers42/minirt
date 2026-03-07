/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:18 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "settings.h"
#include "types.h"

uint64_t	random_seed(uint64_t seed);
uint64_t	random_u64(void);
real_t		random_real(void);
real_t		random_real_interval(real_t min, real_t max);
#define random_double random_real
int			random_int(int min, int max);

#endif