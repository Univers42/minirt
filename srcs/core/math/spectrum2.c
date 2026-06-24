/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spectrum2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spectrum.h"
#include <math.h>

/*
** Planck's spectral radiance (relative, un-normalised).
** B(λ,T) ∝ 1 / (λ^5 · (exp(hc / λkT) − 1))
** Constants: hc / k ≈ 14388000 nm·K  (using nm for wavelength)
*/
float	planck_radiance(float lambda_nm, float temperature_k)
{
	float	l5;
	float	exponent;

	l5 = lambda_nm * lambda_nm * lambda_nm * lambda_nm * lambda_nm;
	exponent = 14388000.0f / (lambda_nm * temperature_k);
	if (exponent > 80.0f)
		return (0.0f);
	return (1.0f / (l5 * (expf(exponent) - 1.0f)));
}

static void	fill_blackbody(float *spectrum, float temperature_k)
{
	float	peak;
	int		i;

	i = -1;
	peak = 0.0f;
	while (++i < CIE_SAMPLES)
	{
		spectrum[i] = planck_radiance(
				(float)(CIE_LAMBDA_MIN + i), temperature_k);
		if (spectrum[i] > peak)
			peak = spectrum[i];
	}
	if (peak > 0.0f)
	{
		i = -1;
		while (++i < CIE_SAMPLES)
			spectrum[i] /= peak;
	}
}

static t_xyz	normalize_y(t_xyz xyz)
{
	if (xyz.y > 0.0f)
	{
		xyz.x /= xyz.y;
		xyz.z /= xyz.y;
		xyz.y = 1.0f;
	}
	return (xyz);
}

t_color	blackbody_color(float temperature_k)
{
	float		spectrum[CIE_SAMPLES];
	t_spectrum	spec;
	t_xyz		xyz;

	fill_blackbody(spectrum, temperature_k);
	spec.samples = spectrum;
	spec.count = CIE_SAMPLES;
	spec.lambda_min = CIE_LAMBDA_MIN;
	xyz = normalize_y(spectrum_to_xyz(spec));
	return (rgb_to_color(cie_xyz_to_linear_srgb(xyz)));
}
