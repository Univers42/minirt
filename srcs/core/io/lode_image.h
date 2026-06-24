/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lode_image.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:44 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LODE_IMAGE_H
# define LODE_IMAGE_H

# ifndef LODEPNG_COMPILE_DISK
#  define LODEPNG_COMPILE_DISK
# endif

# include <stdlib.h>
# include "../png_writer/all.h"
# include "../png_writer/error.h"

typedef struct s_lode_image
{
	unsigned char	*rgba;
	unsigned int	w;
	unsigned int	h;
	unsigned int	stride;
}	t_lode_image;

void				lode_image_init(t_lode_image *img);
void				lode_image_cleanup(t_lode_image *img);
unsigned int		lode_image_load_png(t_lode_image *img,
						const char *filename);
const unsigned char	*lode_image_pixel_rgb(const t_lode_image *img,
						int x, int y);
unsigned int		lode_image_save_png(const char *path,
						const unsigned char *rgb, int w, int h);

#endif
