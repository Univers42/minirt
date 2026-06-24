/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spectrum.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPECTRUM_H
# define SPECTRUM_H

# include "types.h"
# include "vector.h"
# include "cie_table.h"

/* Tristimulus / linear-RGB triple (same shape, reused for both). */
typedef struct s_xyz
{
	float	x;
	float	y;
	float	z;
}	t_xyz;

/* A sampled spectrum: one value per nm from lambda_min upward.  */
typedef struct s_spectrum
{
	const float	*samples;
	int			count;
	int			lambda_min;
}	t_spectrum;

/* XYZ → linear sRGB conversion (CIE 1931 → Rec.709 primaries). */
t_xyz	cie_xyz_to_linear_srgb(t_xyz xyz);

/* Integrate a sampled spectrum against CIE matching functions.  */
/* Returns XYZ tristimulus values.                               */
t_xyz	spectrum_to_xyz(t_spectrum spec);

/* Convert a sampled spectrum directly to engine t_color (linear sRGB) */
t_color	spectrum_to_color(const float *spectrum, int count,
			int lambda_min);

/* Shared post-conversion helpers (spectrum.c). */
float	spectrum_clamp_f(float v, float lo, float hi);
t_color	rgb_to_color(t_xyz rgb);

/* Planck's black-body radiance at wavelength (nm) for temperature (K) */
float	planck_radiance(float lambda_nm, float temperature_k);

/* Compute the colour of a black body at temperature T (Kelvin)        */
t_color	blackbody_color(float temperature_k);

#endif
