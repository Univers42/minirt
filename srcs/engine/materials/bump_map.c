/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bump_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 18:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 18:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"
#include "pdf.h"

/*
** Attach a height/bump image to an existing lambertian material.  No-op for
** any other material kind, so callers can attach unconditionally.  The bump
** texture is owned by the material and freed in lambertian_destroy.
*/
void	lambertian_attach_bump(t_material *mat, t_texture *bump,
			real_t strength)
{
	t_lambertian	*lamb;

	if (!mat || !bump || mat->scatter != lambertian_scatter)
		return ;
	lamb = (t_lambertian *)mat->data;
	lamb->bump = bump;
	if (strength <= (real_t)0.0)
		strength = (real_t)1.0;
	lamb->bump_strength = strength;
}

/* Perceptual luminance of a linear-RGB bump sample at (u, v). */
static real_t	bump_height(const t_texture *b, real_t u, real_t v,
					const t_point3 *p)
{
	t_color	c;

	c = b->value(b, u, v, p);
	return ((real_t)0.299 * c.x + (real_t)0.587 * c.y
		+ (real_t)0.114 * c.z);
}

/*
** Central-difference slope of the height field in u and v.  The raw
** luminance difference across a small UV step is the bump amplitude;
** bump_strength then scales the tangent-plane tilt directly, so a value
** around 1 gives subtle relief and a few units gives bold relief.
*/
static void	bump_gradient(const t_lambertian *lamb, const t_hit_record *rec,
					real_t grad[2])
{
	const real_t	d = (real_t)0.0015;
	real_t			hu0;
	real_t			hu1;
	real_t			hv0;
	real_t			hv1;

	hu0 = bump_height(lamb->bump, rec->u - d, rec->v, &rec->p);
	hu1 = bump_height(lamb->bump, rec->u + d, rec->v, &rec->p);
	hv0 = bump_height(lamb->bump, rec->u, rec->v - d, &rec->p);
	hv1 = bump_height(lamb->bump, rec->u, rec->v + d, &rec->p);
	grad[0] = (hu1 - hu0);
	grad[1] = (hv1 - hv0);
}

/*
** Perturb rec->normal from the bump height gradient.  A tangent basis is
** built from the surface normal (an ONB — consistent relief without needing
** per-primitive UV-aligned tangents).  Tilting N against the gradient makes
** lighter pixels read as raised bumps.  Read-only on the material, so the
** call is safe inside the OpenMP render region.
*/
void	bump_perturb_normal(const t_material *mat, t_hit_record *rec)
{
	t_lambertian	*lamb;
	t_vec3			uvw[3];
	real_t			grad[2];
	t_vec3			tilt;
	t_vec3			n;

	if (!mat || mat->scatter != lambertian_scatter)
		return ;
	lamb = (t_lambertian *)mat->data;
	if (!lamb->bump || lamb->bump_strength <= (real_t)0.0)
		return ;
	build_onb_from_w(uvw, &rec->normal);
	bump_gradient(lamb, rec, grad);
	tilt = vec3_mul_scalar(&uvw[0], -grad[0] * lamb->bump_strength);
	n = vec3_mul_scalar(&uvw[1], -grad[1] * lamb->bump_strength);
	tilt = vec3_add(&tilt, &n);
	n = vec3_add(&rec->normal, &tilt);
	rec->normal = unit_vector(&n);
}
