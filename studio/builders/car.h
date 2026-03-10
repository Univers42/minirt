/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   car.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAR_H
# define CAR_H

# include "common.h"
# include "cylinder.h"
# include "sphere.h"
# include "quad.h"

/* ------------------------------------------------------------------ */
/*  Car builder (simplified parametric body)                          */
/*  Body = box, wheels = cylinders, windshield = tinted glass quad    */
/* ------------------------------------------------------------------ */

void	build_car_body(t_hittable_list *world,
			const t_point3 *pos,
			real_t length, real_t width, real_t height,
			t_material *body_mat);

void	build_car_wheels(t_hittable_list *world,
			const t_point3 *pos,
			real_t length, real_t width,
			real_t wheel_radius, t_material *tire_mat);

void	build_car(t_hittable_list *world,
			const t_point3 *pos,
			t_material *body_mat, t_material *tire_mat,
			t_material *glass_mat);

#endif
