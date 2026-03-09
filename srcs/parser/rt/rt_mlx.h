/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_mlx.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:25:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_MLX_H
# define RT_MLX_H

/* ------------------------------------------------------------------ */
/*  X11 event codes                                                   */
/* ------------------------------------------------------------------ */

# define X_KEY_PRESS		2
# define X_DESTROY_NOTIFY	17
# define KEY_ESC			65307
# define KEY_MASK_PRESS		(1L << 0)

/* ------------------------------------------------------------------ */
/*  MinilibX context                                                  */
/* ------------------------------------------------------------------ */

typedef struct s_mlx_ctx
{
	void	*mlx;
	void	*win;
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
	int		width;
	int		height;
}	t_mlx_ctx;

/* ------------------------------------------------------------------ */
/*  MLX display API                                                   */
/* ------------------------------------------------------------------ */

int		mlx_ctx_init(t_mlx_ctx *ctx, int width, int height, char *title);
void	mlx_ctx_destroy(t_mlx_ctx *ctx);
void	mlx_put_pixel(t_mlx_ctx *ctx, int x, int y, int color);
void	mlx_display_rgb(t_mlx_ctx *ctx, const unsigned char *rgb);
int		mlx_close_handler(t_mlx_ctx *ctx);
int		mlx_key_handler(int keycode, t_mlx_ctx *ctx);

#endif
