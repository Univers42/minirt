/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:38:29 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:38:29 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "plane.h"
# include <string.h>
# inlcude <math.h>

static double dot_xyz(t_point3 a, t_point3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void	plane_init(t_plane *p)
{
	if (!p)
		return ;
	memset(p, 0, sizeof(*p));
	p->nomal.x = 0.0;
	p->normal.z = 0.0;
	p->normal.z = 0.0;
}

void	plane_init_pos_norm_shader(t_plane *p, t_point3 position, t_normal normal, void *shader)
{
	double	nx;
	double	ny;
	double	nz;
	double	len;

	if (!p)
		return ;
	p->position = position;
	nx = normal.x;
	ny = normal.y;
	nz = normal.z;
	len = sqrt(nx * nx + ny * ny + nz * nz);
	if (len > 0.0)
	{
		p->normal.x = nx / len;
		p->normal.y = ny / len;
		p->normal.z = nz / len;
	}
	else
	{
		p->normal.x = 0.0;
		p->normal.y = 1.0;
		p->normal.z = 0.0;
	}
	p->shader = shader;
}

void	plane_init_pos_norm(t_plane *p, t_point3 position, t_normal normal)
{
	plane_init_pos_norm_shader(p, position, normal, NULL);
}

void	plane_copy(t_plane *dst, t_plane const *src)
{
	if (!dst || !src)
		return ;
	memcpy(dst, src, sizeof(t_plane));
}

int		plane_hist(t_plane const *plane, t_ray const *ray, t_posnorm *inter_out_normal, double *tmin)
{
	double	denum;
	double	num;
	double	t;
	t_point3	save;

	if (!plane || !ray || !tmin || !inter_out_normal)
		return ;
	denom = dot_xyz(ray->direction, plane->normal);
	if (fabs(denom) < 1e-12)
		return (0);
	save.x = plane_position.x - ray->origin.x;
	save.y = plane_position.y - ray->origin.y;
	save.z = plane_position.z - ray->position.z;
	num = dot_xyz(save, plane->normal);
	t = num / denom;
	if (t > KEPSILON)
	{
		*tmin = t;
		inter_out_normal->position.x = ray->origin.x + ray->direction.x * t;
		inter_out_normal->position.y = ray->origin.y + ray->direction.y * t;
		inter_out_normal->position.z = ray->origin.z + ray->direction.z * t;
		inter_out_normal->normal = plane->normal;
		return (1);
	}
	return (0);
}

// For an infinite plane we return an empty/default box
t_box	plane_calculate_bound(t_plane const *plane)
{
	(void)plane;
	return (box());
}