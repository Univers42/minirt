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

/* XYZ ↔ linear sRGB conversion (CIE 1931 → Rec.709 primaries) */
void	cie_xyz_to_linear_srgb(float x, float y, float z,
			float *r, float *g, float *b);

/* Integrate a sampled spectrum against CIE matching functions.       */
/* `spectrum` has one value per nm from lambda_min to lambda_min+count */
/* Returns XYZ tristimulus values.                                     */
void	spectrum_to_xyz(const float *spectrum, int count,
			int lambda_min, float *x, float *y, float *z);

/* Convert a sampled spectrum directly to engine t_color (linear sRGB) */
t_color	spectrum_to_color(const float *spectrum, int count,
			int lambda_min);

/* Planck's black-body radiance at wavelength (nm) for temperature (K) */
float	planck_radiance(float lambda_nm, float temperature_k);

/* Compute the colour of a black body at temperature T (Kelvin)        */
t_color	blackbody_color(float temperature_k);

#endif
