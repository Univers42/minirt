/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_mesh5.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:14:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <stdio.h>

void	wf_mesh_print_info(const t_wf_mesh *mesh)
{
	fprintf(stderr, "WF Mesh: %zu triangles\n", mesh->ntris);
	fprintf(stderr, "  bounds: [%.3f,%.3f,%.3f] - [%.3f,%.3f,%.3f]\n",
		mesh->bounds_min.x, mesh->bounds_min.y, mesh->bounds_min.z,
		mesh->bounds_max.x, mesh->bounds_max.y, mesh->bounds_max.z);
	fprintf(stderr, "  center: [%.3f,%.3f,%.3f]\n",
		mesh->center.x, mesh->center.y, mesh->center.z);
}
