/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "studio_config.h"

/*
** sky_peak — overall sky energy = brightest background component.
** Scaling the whole gradient by this keeps the sky tied to the scene
** ambient, so a dark scene gets a dark sky (no wash-out).
*/
static real_t	sky_peak(const t_color *background)
{
	real_t	peak;

	peak = background->x;
	if (background->y > peak)
		peak = background->y;
	if (background->z > peak)
		peak = background->z;
	return (peak);
}

/*
** bg_sky_color — physically-plausible sky dome for miss rays.
**
** Two tinted bands (studio_config.h): a brighter, slightly warm
** HORIZON easing up to a deeper, cooler ZENITH.  Both bands are
** scaled by the background peak, so the gradient never blows out
** dark scenes.  Shared by BOTH engines (direct + path tracer), so
** reflective/refractive rays see the same graded environment.
**
**   t ≈ 0  (looking level/down) → warm horizon haze
**   t ≈ 1  (looking up)         → deep blue zenith
*/
t_vec3	bg_sky_color(const t_ray *r, const t_color *background)
{
	t_vec3	unit_dir;
	real_t	t;
	real_t	peak;
	t_vec3	horizon;
	t_vec3	zenith;

	unit_dir = unit_vector(&r->dir);
	t = unit_dir.y;
	if (t < (real_t)0.0)
		t = (real_t)0.0;
	t = sqrt(t);
	peak = sky_peak(background) * (real_t)RT_SKY_HORIZON_GAIN;
	horizon = vec3_create((real_t)RT_SKY_HORIZON_R * peak,
			(real_t)RT_SKY_HORIZON_G * peak, (real_t)RT_SKY_HORIZON_B * peak);
	peak = peak * (real_t)RT_SKY_ZENITH_GAIN;
	zenith = vec3_create((real_t)RT_SKY_ZENITH_R * peak,
			(real_t)RT_SKY_ZENITH_G * peak, (real_t)RT_SKY_ZENITH_B * peak);
	return (vec3_lerp(&horizon, &zenith, t));
}

unsigned char	*write_color_to_buf_bin(unsigned char *dst,
				const t_vec3 *pixel)
{
	real_t					r;
	real_t					g;
	real_t					b;
	static const t_interval	intensity = {0.000, 0.999, true};

	r = pixel->x;
	g = pixel->y;
	b = pixel->z;
	color_post_process(&r, &g, &b);
	*dst++ = (unsigned char)component_to_byte(r, &intensity);
	*dst++ = (unsigned char)component_to_byte(g, &intensity);
	*dst++ = (unsigned char)component_to_byte(b, &intensity);
	return (dst);
}

void	format_time(double seconds, char *buf, size_t bufsize)
{
	int	h;
	int	m;
	int	s;

	if (seconds < 0.0)
	{
		snprintf(buf, bufsize, "--:--:--");
		return ;
	}
	h = (int)(seconds / 3600.0);
	m = (int)((seconds - h * 3600) / 60.0);
	s = (int)(seconds - h * 3600 - m * 60);
	if (h > 0)
		snprintf(buf, bufsize, "%02d:%02d:%02d", h, m, s);
	else
		snprintf(buf, bufsize, "%02d:%02d", m, s);
}
