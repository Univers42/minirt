/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:32 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PDF_H
# define PDF_H

# include "types.h"
# include "vector.h"
# include "hittable.h"
# include "settings.h"
# include "random.h"
# include <stdlib.h>
# include <math.h>

typedef struct s_pdf			t_pdf;

typedef real_t					(*t_pdf_value)(const t_pdf *pdf,
		const t_vec3 *direction);
typedef t_vec3					(*t_pdf_generate)(const t_pdf *pdf);
typedef void					(*t_pdf_destroy)(t_pdf *pdf);

struct s_pdf
{
	void			*data;
	t_pdf_value		value;
	t_pdf_generate	generate;
	t_pdf_destroy	destroy;
};

typedef struct s_cosine_pdf
{
	t_vec3	uvw[3];
}	t_cosine_pdf;

typedef struct s_mixture_pdf
{
	t_pdf	*p0;
	t_pdf	*p1;
}	t_mixture_pdf;

void	build_onb_from_w(t_vec3 *uvw, const t_vec3 *n);
t_vec3	onb_local(const t_vec3 *uvw, const t_vec3 *a);
real_t	cosine_pdf_value(const t_pdf *pdf, const t_vec3 *direction);
t_vec3	cosine_pdf_generate(const t_pdf *pdf);
void	cosine_pdf_destroy(t_pdf *pdf);
t_pdf	*cosine_pdf_create(const t_vec3 *normal);
real_t	sphere_pdf_value(const t_pdf *pdf, const t_vec3 *direction);
t_vec3	sphere_pdf_generate(const t_pdf *pdf);
void	sphere_pdf_destroy(t_pdf *pdf);
t_pdf	*sphere_pdf_create(void);
real_t	mixture_pdf_value(const t_pdf *pdf, const t_vec3 *direction);
t_vec3	mixture_pdf_generate(const t_pdf *pdf);
void	mixture_pdf_destroy(t_pdf *pdf);
t_pdf	*mixture_pdf_create(t_pdf *p0, t_pdf *p1);

#endif
