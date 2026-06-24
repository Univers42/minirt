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
t_xyz	cie_xyz_to_linear_srgb(t_xyz xyz)
{
	t_xyz	rgb;

	rgb.x = 3.2404542f * xyz.x - 1.5371385f * xyz.y - 0.4985314f * xyz.z;
	rgb.y = -0.9692660f * xyz.x + 1.8760108f * xyz.y + 0.0415560f * xyz.z;
	rgb.z = 0.0556434f * xyz.x - 0.2040259f * xyz.y + 1.0572252f * xyz.z;
	return (rgb);
}

/*
** Integrate spectrum × CIE matching functions using the trapezoidal
** rule with 1 nm steps.
*/
t_xyz	spectrum_to_xyz(t_spectrum spec)
{
	t_cie_table	tbl;
	t_xyz		out;
	int			i;
	int			ci;

	tbl = cie_get_table();
	out.x = 0.0f;
	out.y = 0.0f;
	out.z = 0.0f;
	i = 0;
	while (i < spec.count)
	{
		ci = (spec.lambda_min + i) - tbl.lambda_min;
		if (ci >= 0 && ci < tbl.count)
		{
			out.x += spec.samples[i] * tbl.x[ci];
			out.y += spec.samples[i] * tbl.y[ci];
			out.z += spec.samples[i] * tbl.z[ci];
		}
		i++;
	}
	return (out);
}

float	spectrum_clamp_f(float v, float lo, float hi)
{
	if (v < lo)
		return (lo);
	if (v > hi)
		return (hi);
	return (v);
}

t_color	rgb_to_color(t_xyz rgb)
{
	return (vec3_create(
			(real_t)spectrum_clamp_f(rgb.x, 0.0f, 1.0f),
			(real_t)spectrum_clamp_f(rgb.y, 0.0f, 1.0f),
			(real_t)spectrum_clamp_f(rgb.z, 0.0f, 1.0f)));
}

t_color	spectrum_to_color(const float *spectrum, int count,
		int lambda_min)
{
	t_spectrum	spec;
	t_xyz		xyz;

	spec.samples = spectrum;
	spec.count = count;
	spec.lambda_min = lambda_min;
	xyz = spectrum_to_xyz(spec);
	return (rgb_to_color(cie_xyz_to_linear_srgb(xyz)));
}
