/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 15:25:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Variables
 */
#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#undef FLOAT_TYPE

/* Constant */
#define X 0
#define Y 1
#define Z 2
#define TOT_COORD3 3
#define W 3
#define TOT_COORD4 4

/* mathematical constants */
#ifndef PI
#define PI 3.1415926535897932385
#endif

/* Ensure math definitions are available before testing/defining INFINITY */
#include <math.h>

/* Define INFINITY only if not already defined by system headers */
#ifndef INFINITY
#if defined(HUGE_VAL)
#define INFINITY HUGE_VAL
#else
#define INFINITY (1.0 / 0.0)
#endif
#endif

/* ================================================================== */
/*  Color profile constants                                           */
/*                                                                    */
/*  Controls the linear → display gamma transfer function.            */
/*  Override at build time with  -DRT_COLOR_PROFILE=RT_COLOR_SRGB     */
/*  or set in studio_config.h.                                        */
/*                                                                    */
/*  RT_COLOR_CIE     – CIE gamma 2.2 power law (default, best        */
/*                     contrast, physically accurate for CRT/LCD)     */
/*  RT_COLOR_SRGB    – IEC 61966-2-1 piecewise sRGB transfer fn      */
/*                     (standard web/photo, brighter shadows)         */
/*  RT_COLOR_GAMMA20 – Simple gamma 2.0 (sqrt), as in the book       */
/*                     "Ray Tracing in One Weekend" series            */
/* ================================================================== */
# define RT_COLOR_CIE		0
# define RT_COLOR_SRGB		1
# define RT_COLOR_GAMMA20	2

# ifndef RT_COLOR_PROFILE
#  define RT_COLOR_PROFILE	RT_COLOR_CIE
# endif

/* Utility function: convert degrees to radians (requires PI and math.h above) */
#include "types.h"
real_t	degrees_to_radians(real_t degrees);

#endif /* SETTINGS_H */