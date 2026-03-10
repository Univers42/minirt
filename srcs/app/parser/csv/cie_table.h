/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cie_table.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codegen (cie_csv_to_c.sh)                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by codegen           #+#    #+#             */
/*   Updated: 2026/03/09 00:00:00 by codegen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CIE_TABLE_H
# define CIE_TABLE_H

# define CIE_SAMPLES		471
# define CIE_LAMBDA_MIN		360
# define CIE_LAMBDA_MAX		830
# define CIE_LAMBDA_STEP	1

typedef struct s_cie_table
{
	const float	*x;
	const float	*y;
	const float	*z;
	int			count;
	int			lambda_min;
	int			lambda_step;
}	t_cie_table;

t_cie_table	cie_get_table(void);

#endif
