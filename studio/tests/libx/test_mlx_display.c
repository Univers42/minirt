/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_mlx_display.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 17:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 17:34:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>

/*
** Self-contained MLX display test — no external dependencies.
** Fills a 400x300 window with a red-green gradient.
** If you see the gradient: MLX display works.
** If blank: compositor / X11 issue.
**
** Build: make build/bin/libx/test_mlx_display
** Run:   ./build/bin/libx/test_mlx_display
*/

#define W	400
#define H	300

typedef struct s_tctx
{
	void	*mlx;
	void	*win;
	void	*img;
	char	*addr;
	int		bpp;
	int		ll;
	int		endian;
}	t_tctx;

static void	put_px(t_tctx *c, int x, int y, int col)
{
	char	*dst;

	dst = c->addr + (y * c->ll + x * (c->bpp / 8));
	*(unsigned int *)dst = col;
}

static int	on_expose(t_tctx *c)
{
	mlx_put_image_to_window(c->mlx, c->win, c->img,
		(t_mlx_point){0, 0});
	mlx_do_sync(c->mlx);
	return (0);
}

static int	on_key(int key, t_tctx *c)
{
	if (key == 65307)
	{
		mlx_destroy_image(c->mlx, c->img);
		mlx_destroy_window(c->mlx, c->win);
		mlx_destroy_display(c->mlx);
		free(c->mlx);
		exit(0);
	}
	return (0);
}

static void	fill_and_show(t_tctx *c)
{
	int	x;
	int	y;

	y = -1;
	while (++y < H)
	{
		x = -1;
		while (++x < W)
			put_px(c, x, y, ((x * 255 / W) << 16)
				| ((y * 255 / H) << 8) | 128);
	}
	mlx_put_image_to_window(c->mlx, c->win, c->img,
		(t_mlx_point){0, 0});
	mlx_do_sync(c->mlx);
	fprintf(stderr, "  bpp=%d endian=%d ll=%d — gradient drawn\n",
		c->bpp, c->endian, c->ll);
}

int	main(void)
{
	t_tctx	c;

	fprintf(stderr, "MLX display test: %dx%d gradient\n", W, H);
	c.mlx = mlx_init();
	if (!c.mlx)
		return (fprintf(stderr, "mlx_init failed\n"), 1);
	c.win = mlx_new_window(c.mlx, W, H, "MLX Test");
	if (!c.win)
		return (fprintf(stderr, "mlx_new_window failed\n"), 1);
	c.img = mlx_new_image(c.mlx, W, H);
	if (!c.img)
		return (fprintf(stderr, "mlx_new_image failed\n"), 1);
	c.addr = mlx_get_data_addr(c.img, &c.bpp, &c.ll, &c.endian);
	fill_and_show(&c);
	mlx_expose_hook(c.win, (int (*)())on_expose, &c);
	mlx_hook(c.win, 2, 1L << 0,
		(t_mlx_hcb){(int (*)())on_key, &c});
	mlx_loop(c.mlx);
	return (0);
}
