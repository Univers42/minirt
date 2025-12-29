/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   png_codec.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 17:30:48 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 17:30:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PNG_CODEC_H
# define PNG_CODEC_H

# include <string.h>
# include <limits.h>
# include <stdio.h>
# include <stdlib.h>

# define FIRST_LENGTH_CODE_INDEX	287
# define LAST_LENGTH_CODE_INDEX		285
# define NUM_DEFLATE_CODE_SYMBOLS	288
# define NUM_DISTANCE_SYMBOLS		32
# define NUM_CODE_LENGTH_CODES		19
# define LODEPNG_VERSION_STRING		"20190210"


typedef struct s_ui_vector
{
	unsigned	*data;
	size_t		size;
	size_t		allocsize;
}	t_ui_vector;

typedef struct s_huffman_tree
{
	unsigned *tree2d;
	unsigned	*tree1d;
	unsigned	*lengths;
	unsigned	max_bit_len;
	unsigned	num_codes;
}	t_huffman_tree;

typedef struct s_vector
{
	unsigned char *data;
	size_t			size;
	size_t			allocsize;
}	t_vector;

typedef enum e_png_color_type
{
	PCT_GRAY = 0,	// grayscale: 1, 2, 4, 8, 16
	PCT_RGB = 2,	// RGB: 8, 16 bit
	PCT_PALETTE = 3,	// palette: 1,2,4,8
	PCT_GREY_ALPHA = 4,	// grayscale with alpha: 8,16 bit
	PCT_RGBA = 6,		// RGB with alpha : 8, 16 bit
} t_png_color_type;

typedef struct s_png_color_mode
{
	t_png_color_type	color_type;
	unsigned			bitdepth;
	unsigned char*		palette;
	size_t				palettesize;
	unsigned			key_defined;
	unsigned			key_r;
	unsigned			key_g;
	unsigned			key_b;
}	t_png_color_mode;

typedef struct s_png_info
{
	unsigned	compression_method;
	unsigned	filter_method;
	unsigned	interlace_method;
	t_png_color_mode	color;
	unsigned			background_defined;
	unsigned			bg_r;
	unsigned			bg_g;
	unsigned			bg_b;
	size_t				text_num;
	char				**text_keys;
	char				**text_strings;
	size_t				itext_num;
	char				**itext_keys;
	char				**itext_langtags;
	char				**itext_transkeys;
	char				**itext_strings;
	unsigned			time_defined;
	t_png_time			time;
	unsigned			phys_x;
	unsigned			phys_y;
	unsigned			phys_unit;
	unsigned			gama_defined;
	unsigned			gama_gamma;
	unsigned			chrm_defined;
  unsigned				chrm_defined; /* Whether a cHRM chunk is present (0 = not present, 1 = present). */
  unsigned				chrm_white_x; /* White Point x times 100000 */
  unsigned				chrm_white_y; /* White Point y times 100000 */
  unsigned				chrm_red_x;   /* Red x times 100000 */
  unsigned				chrm_red_y;   /* Red y times 100000 */
  unsigned				chrm_green_x; /* Green x times 100000 */
  unsigned				chrm_green_y; /* Green y times 100000 */
  unsigned				chrm_blue_x;  /* Blue x times 100000 */
  unsigned				chrm_blue_y;  /* Blue y times 100000 */
  unsigned				srgb_defined;
  unsigned				srgb_intent;
  unsigned				iccp_defined;
  char					*iccp_name;
  unsigned char			*iccp_profile;
  unsigned				iccp_profile_size;
  unsigned char			*unknown_chunks_data[3];
  size_t				unknown_chunks_size[3];
}	t_png_info;

typedef struct s_decompress_settings
{
	unsigned	ignore_adler32;
	unsigned	(*zlib){};
	unsigned	(*inflate){};
	const void	*ctx;
}	t_decompress;

typedef struct s_decoder_settings
{

}t_decoder_settings;
typedef struct s_png_time
{
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned min;
	unsigned second;
}	t_png_time;

typedef struct s_filter_strategy
{
	
}t_filter_strategy;
typedef struct s_png_state
{
	t_decoder_settings	decoder;
	t_encoder_settings	encoder;
	t_png_color_type
}t_png_state;
# endif
