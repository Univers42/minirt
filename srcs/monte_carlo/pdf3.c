/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pdf3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:32 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pdf.h"

real_t	mixture_pdf_value(const t_pdf *pdf, const t_vec3 *direction)
{
	const t_mixture_pdf	*mpdf;

	mpdf = (const t_mixture_pdf *)pdf->data;
	if (!mpdf || !mpdf->p0 || !mpdf->p1)
		return (0.0);
	return (0.5 * mpdf->p0->value(mpdf->p0, direction)
		+ 0.5 * mpdf->p1->value(mpdf->p1, direction));
}

t_vec3	mixture_pdf_generate(const t_pdf *pdf)
{
	const t_mixture_pdf	*mpdf;

	mpdf = (const t_mixture_pdf *)pdf->data;
	if (!mpdf)
		return (vec3_create(0.0, 1.0, 0.0));
	if (random_real() < 0.5)
		return (mpdf->p0->generate(mpdf->p0));
	return (mpdf->p1->generate(mpdf->p1));
}

void	mixture_pdf_destroy(t_pdf *pdf)
{
	t_mixture_pdf	*mpdf;

	if (!pdf || !pdf->data)
		return ;
	mpdf = (t_mixture_pdf *)pdf->data;
	free(mpdf);
	pdf->data = NULL;
}

t_pdf	*mixture_pdf_create(t_pdf *p0, t_pdf *p1)
{
	t_pdf			*pdf;
	t_mixture_pdf	*mpdf;

	if (!p0 || !p1)
		return (NULL);
	pdf = (t_pdf *)malloc(sizeof(t_pdf));
	if (!pdf)
		return (NULL);
	mpdf = (t_mixture_pdf *)malloc(sizeof(t_mixture_pdf));
	if (!mpdf)
		return (free(pdf), NULL);
	mpdf->p0 = p0;
	mpdf->p1 = p1;
	pdf->data = mpdf;
	pdf->value = mixture_pdf_value;
	pdf->generate = mixture_pdf_generate;
	pdf->destroy = mixture_pdf_destroy;
	return (pdf);
}
