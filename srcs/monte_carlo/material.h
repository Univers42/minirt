/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:37 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATERIAL_H
# define MATERIAL_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "texture.h"
# include <stdlib.h>

typedef struct s_hit_record	t_hit_record;
typedef struct s_texture	t_texture;

typedef struct s_material
{
	void	*data;
	t_color	(*emitted)(const struct s_material *mat, real_t u, real_t v,
			const t_point3 *p);
	bool	(*scatter)(const struct s_material *mat, const t_ray *r_in,
			const t_hit_record *rec, t_color *attenuation, t_ray *scattered);
	real_t	(*scattering_pdf)(const struct s_material *mat, const t_ray *r_in,
			const t_hit_record *rec, const t_ray *scattered);
	void	(*destroy)(struct s_material *mat);
}	t_material;

typedef struct s_lambertian
{
	t_texture	*tex;
}	t_lambertian;

typedef struct s_metal
{
	t_color	albedo;
	real_t	fuzz;
}	t_metal;

typedef struct s_dielectric
{
	real_t	refraction_index;
}	t_dielectric;

typedef struct s_diffuse_light
{
	t_texture	*tex;
	real_t		scale;
}	t_diffuse_light;

typedef struct s_isotropic
{
	t_texture	*tex;
}	t_isotropic;

typedef struct s_tinted_glass
{
	real_t	refraction_index;
	t_color	tint;
}	t_tinted_glass;

typedef struct s_glossy
{
	t_color	albedo;
	real_t	roughness;
	real_t	metalness;
}	t_glossy;

real_t		reflectance(real_t cosine, real_t refraction_index);
t_color		default_emitted(const t_material *mat, real_t u, real_t v,
				const t_point3 *p);
real_t		default_scattering_pdf(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, const t_ray *scattered);
real_t		lambertian_scattering_pdf(const t_material *mat,
				const t_ray *r_in, const t_hit_record *rec,
				const t_ray *scattered);
bool		lambertian_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
bool		metal_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
bool		dielectric_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
bool		tinted_glass_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
bool		glossy_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
t_color		diffuse_light_emitted(const t_material *mat, real_t u, real_t v,
				const t_point3 *p);
bool		diffuse_light_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
bool		isotropic_scatter(const t_material *mat, const t_ray *r_in,
				const t_hit_record *rec, t_color *attenuation,
				t_ray *scattered);
void		lambertian_destroy(t_material *mat);
void		metal_destroy(t_material *mat);
void		dielectric_destroy(t_material *mat);
void		tinted_glass_destroy(t_material *mat);
void		glossy_destroy(t_material *mat);
void		diffuse_light_destroy(t_material *mat);
void		isotropic_destroy(t_material *mat);
t_material	*lambertian_create_texture(t_texture *tex);
t_material	*lambertian_create(t_color albedo);
t_material	*metal_create_fuzz(t_color albedo, real_t fuzz);
t_material	*metal_create(t_color albedo);
t_material	*dielectric_create(real_t refraction_index);
t_material	*tinted_glass_create(real_t refraction_index, t_color tint);
t_material	*glossy_create(t_color albedo, real_t roughness,
				real_t metalness);
t_material	*diffuse_light_create(t_color emit);
t_material	*diffuse_light_create_scaled(t_color emit, real_t scale);
t_material	*diffuse_light_create_texture(t_texture *tex);
t_material	*diffuse_light_create_texture_scaled(t_texture *tex,
				real_t scale);
t_material	*isotropic_create(t_color albedo);
t_material	*isotropic_create_texture(t_texture *tex);

#endif
