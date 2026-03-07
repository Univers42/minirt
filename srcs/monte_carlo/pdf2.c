/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pdf2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:32 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pdf.h"

t_pdf	*cosine_pdf_create(const t_vec3 *normal)
{
	t_pdf		*pdf;
	t_cosine_pdf	*cpdf;

	pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return (NULL);
	cpdf = (t_cosine_pdf *)malloc(sizeof(t_cosine_pdf));
	if (!cpdf)
		return (free(pdf), NULL);
	build_onb_from_w(cpdf->uvw, normal);
	pdf->data = cpdf;
	pdf->value = cosine_pdf_value;
	pdf->generate = cosine_pdf_generate;
	pdf->destroy = cosine_pdf_destroy;
	return (pdf);
}

real_t	sphere_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	(void)pdf;
	(void)direction;
	return (1.0 / (4.0 * PI));
}

t_vec3	sphere_pdf_generate(const t_pdf *pdf)
{
	(void)pdf;
	return (random_unit_vector());
}

void	sphere_pdf_destroy(t_pdf *pdf)
{
	(void)pdf;
}

t_pdf	*sphere_pdf_create(void)
{
	t_pdf	*pdf;

	pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return (NULL);
	pdf->data = NULL;
	pdf->value = sphere_pdf_value;
	pdf->generate = sphere_pdf_generate;
	pdf->destroy = sphere_pdf_destroy;
	return (pdf);
}
