/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_display.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 17:32:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_mlx.h"
#include "mlx.h"
#include <stdlib.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Initialize MLX context: connection, window, image                 */
/* ------------------------------------------------------------------ */

int	mlx_ctx_init(t_mlx_ctx *ctx, int width, int height, char *title)
{
	ctx->width = width;
	ctx->height = height;
	ctx->mlx = mlx_init();
	if (!ctx->mlx)
		return (-1);
	ctx->win = mlx_new_window(ctx->mlx, width, height, title);
	if (!ctx->win)
	{
		mlx_destroy_display(ctx->mlx);
		free(ctx->mlx);
		return (-1);
	}
	ctx->img = mlx_new_image(ctx->mlx, width, height);
	if (!ctx->img)
	{
		mlx_destroy_window(ctx->mlx, ctx->win);
		mlx_destroy_display(ctx->mlx);
		free(ctx->mlx);
		return (-1);
	}
	ctx->addr = mlx_get_data_addr(ctx->img, &ctx->bpp,
			&ctx->line_len, &ctx->endian);
	return (0);
}

/* ------------------------------------------------------------------ */
/*  Destroy MLX context and free resources                            */
/* ------------------------------------------------------------------ */

void	mlx_ctx_destroy(t_mlx_ctx *ctx)
{
	if (!ctx || !ctx->mlx)
		return ;
	if (ctx->img)
		mlx_destroy_image(ctx->mlx, ctx->img);
	if (ctx->win)
		mlx_destroy_window(ctx->mlx, ctx->win);
	mlx_destroy_display(ctx->mlx);
	free(ctx->mlx);
	ctx->mlx = NULL;
}

/* ------------------------------------------------------------------ */
/*  Put a single pixel (ARGB format) into the image buffer            */
/* ------------------------------------------------------------------ */

void	mlx_put_pixel(t_mlx_ctx *ctx, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height)
		return ;
	dst = ctx->addr + (y * ctx->line_len + x * (ctx->bpp / 8));
	*(unsigned int *)dst = color;
}

/* ------------------------------------------------------------------ */
/*  Copy an RGB byte buffer (3 bytes/pixel, row-major) to the MLX     */
/*  image and display it.                                             */
/* ------------------------------------------------------------------ */

void	mlx_display_rgb(t_mlx_ctx *ctx, const unsigned char *rgb)
{
	int					x;
	int					y;
	const unsigned char	*px;
	int					color;

	y = 0;
	while (y < ctx->height)
	{
		x = 0;
		while (x < ctx->width)
		{
			px = rgb + (y * ctx->width + x) * 3;
			color = (px[0] << 16) | (px[1] << 8) | px[2];
			mlx_put_pixel(ctx, x, y, color);
			x++;
		}
		y++;
	}
	mlx_put_image_to_window(ctx->mlx, ctx->win, ctx->img,
		(t_mlx_point){0, 0});
	mlx_do_sync(ctx->mlx);
	fprintf(stderr, "MLX: %dx%d bpp=%d endian=%d llen=%d\n",
		ctx->width, ctx->height, ctx->bpp, ctx->endian, ctx->line_len);
}
