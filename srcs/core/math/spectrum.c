/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spectrum.c                                         :+:      :+:    :+:   */
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
** CIE XYZ → linear sRGB (Rec.709 / sRGB primaries).
** 3x3 matrix from CIE 1931 XYZ to linear sRGB.
*/
void	cie_xyz_to_linear_srgb(float x, float y, float z,
		float *r, float *g, float *b)
{
	*r = 3.2404542f * x - 1.5371385f * y - 0.4985314f * z;
	*g = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
	*b = 0.0556434f * x - 0.2040259f * y + 1.0572252f * z;
}

/*
** Integrate spectrum × CIE matching functions using the trapezoidal
** rule with 1 nm steps.
*/
void	spectrum_to_xyz(const float *spectrum, int count,
		int lambda_min, float *x, float *y, float *z)
{
	t_cie_table	tbl;
	int			i;
	int			ci;

	tbl = cie_get_table();
	*x = 0.0f;
	*y = 0.0f;
	*z = 0.0f;
	i = 0;
	while (i < count)
	{
		ci = (lambda_min + i) - tbl.lambda_min;
		if (ci >= 0 && ci < tbl.count)
		{
			*x += spectrum[i] * tbl.x[ci];
			*y += spectrum[i] * tbl.y[ci];
			*z += spectrum[i] * tbl.z[ci];
		}
		i++;
	}
}

static float	clamp_f(float v, float lo, float hi)
{
	if (v < lo)
		return (lo);
	if (v > hi)
		return (hi);
	return (v);
}

t_color	spectrum_to_color(const float *spectrum, int count,
		int lambda_min)
{
	float	x;
	float	y;
	float	z;
	float	r;
	float	g;
	float	b;

	spectrum_to_xyz(spectrum, count, lambda_min, &x, &y, &z);
	cie_xyz_to_linear_srgb(x, y, z, &r, &g, &b);
	return (vec3_create(
			(real_t)clamp_f(r, 0.0f, 1.0f),
			(real_t)clamp_f(g, 0.0f, 1.0f),
			(real_t)clamp_f(b, 0.0f, 1.0f)));
}

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

t_color	blackbody_color(float temperature_k)
{
	float	spectrum[CIE_SAMPLES];
	float	peak;
	int		i;
	float	x;
	float	y;
	float	z;
	float	r;
	float	g;
	float	b;

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
	spectrum_to_xyz(spectrum, CIE_SAMPLES, CIE_LAMBDA_MIN, &x, &y, &z);
	if (y > 0.0f)
	{
		x /= y;
		z /= y;
		y = 1.0f;
	}
	cie_xyz_to_linear_srgb(x, y, z, &r, &g, &b);
	return (vec3_create(
			(real_t)clamp_f(r, 0.0f, 1.0f),
			(real_t)clamp_f(g, 0.0f, 1.0f),
			(real_t)clamp_f(b, 0.0f, 1.0f)));
}
