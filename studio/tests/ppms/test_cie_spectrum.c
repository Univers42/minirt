/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_cie_spectrum.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:51 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "spectrum.h"
#include "cie_table.h"
#include <stdio.h>
#include <math.h>

static int	g_pass;
static int	g_fail;

static void	ok(int cond, const char *msg)
{
	if (cond)
	{
		printf("  \033[32m✓\033[0m %s\n", msg);
		g_pass++;
	}
	else
	{
		printf("  \033[31m✗\033[0m %s\n", msg);
		g_fail++;
	}
}

static void	test_cie_table(void)
{
	t_cie_table	t;

	printf("test_cie_table\n");
	t = cie_get_table();
	ok(t.count == CIE_SAMPLES, "count == 471");
	ok(t.x[0] > 0.0f, "x[0] > 0");
	ok(t.y[0] > 0.0f, "y[0] > 0");
}

static void	test_blackbody_d65(void)
{
	t_color	c;

	printf("test_blackbody_d65 (6500K)\n");
	c = blackbody_color(6500.0);
	ok(c.x > 0.0, "r > 0");
	ok(c.y > 0.0, "g > 0");
	ok(c.z > 0.0, "b > 0");
	printf("    color = (%.4f, %.4f, %.4f)\n", c.x, c.y, c.z);
}

static void	test_blackbody_warm(void)
{
	t_color	c;

	printf("test_blackbody_warm (2700K)\n");
	c = blackbody_color(2700.0);
	ok(c.x > c.z, "warm light: r > b");
	printf("    color = (%.4f, %.4f, %.4f)\n", c.x, c.y, c.z);
}

int	main(void)
{
	printf("\n=== CIE Spectrum Tests ===\n\n");
	test_cie_table();
	test_blackbody_d65();
	test_blackbody_warm();
	printf("\n--- %d passed, %d failed ---\n\n", g_pass, g_fail);
	return (g_fail != 0);
}
