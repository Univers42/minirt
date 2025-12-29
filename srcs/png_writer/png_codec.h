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
# define MAX_SUPPORTED_DEFLATE_LENGTH 258
// 3 bytes of data get encoded intot two bytes, The hash
// cannot use more than 3 bytes as input because 3 is t
// the minimum match length for deflate
# define HASH_NUM_VALUES				65536
# define HASH_BIT_MASK					65535
# define DEFAULT_WINDOWS				2048

typedef struct magic_arrays
{
	const unsigned length_base[29];
	const unsigned length_extra[29];
	const unsigned distance_base[30];
	const unsigned distance_extra[30];
	const unsigned clcl_order[NUM_CODE_LENGTH_CODES];
	const unsigned		lodepng_crc32_table[256];
	const unsigned		adamix[7];
	const unsigned		adam7iy[7];
	const unsigned		adam7dx[7];
	const unsigned		adam7dy[7];
}t_magic_arrays;

t_magic_arrays magic;
magic.length_base[29]
  = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
     67, 83, 99, 115, 131, 163, 195, 227, 258};

magic.length_extra[29]
  = {0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,
      4,  4,  4,   4,   5,   5,   5,   5,   0};

magic.distance_base[30]
  = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
     769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

magic.distance_extra[30]
  = {0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,   6,   6,   7,   7,   8,
       8,    9,    9,   10,   10,   11,   11,   12,    12,    13,    13};

magic.clcl_order[NUM_CODE_LENGTH_CODES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

magic.lodepng_crc32_table[256] = {
           0u, 1996959894u, 3993919788u, 2567524794u,  124634137u, 1886057615u, 3915621685u, 2657392035u,
   249268274u, 2044508324u, 3772115230u, 2547177864u,  162941995u, 2125561021u, 3887607047u, 2428444049u,
   498536548u, 1789927666u, 4089016648u, 2227061214u,  450548861u, 1843258603u, 4107580753u, 2211677639u,
   325883990u, 1684777152u, 4251122042u, 2321926636u,  335633487u, 1661365465u, 4195302755u, 2366115317u,
   997073096u, 1281953886u, 3579855332u, 2724688242u, 1006888145u, 1258607687u, 3524101629u, 2768942443u,
   901097722u, 1119000684u, 3686517206u, 2898065728u,  853044451u, 1172266101u, 3705015759u, 2882616665u,
   651767980u, 1373503546u, 3369554304u, 3218104598u,  565507253u, 1454621731u, 3485111705u, 3099436303u,
   671266974u, 1594198024u, 3322730930u, 2970347812u,  795835527u, 1483230225u, 3244367275u, 3060149565u,
  1994146192u,   31158534u, 2563907772u, 4023717930u, 1907459465u,  112637215u, 2680153253u, 3904427059u,
  2013776290u,  251722036u, 2517215374u, 3775830040u, 2137656763u,  141376813u, 2439277719u, 3865271297u,
  1802195444u,  476864866u, 2238001368u, 4066508878u, 1812370925u,  453092731u, 2181625025u, 4111451223u,
  1706088902u,  314042704u, 2344532202u, 4240017532u, 1658658271u,  366619977u, 2362670323u, 4224994405u,
  1303535960u,  984961486u, 2747007092u, 3569037538u, 1256170817u, 1037604311u, 2765210733u, 3554079995u,
  1131014506u,  879679996u, 2909243462u, 3663771856u, 1141124467u,  855842277u, 2852801631u, 3708648649u,
  1342533948u,  654459306u, 3188396048u, 3373015174u, 1466479909u,  544179635u, 3110523913u, 3462522015u,
  1591671054u,  702138776u, 2966460450u, 3352799412u, 1504918807u,  783551873u, 3082640443u, 3233442989u,
  3988292384u, 2596254646u,   62317068u, 1957810842u, 3939845945u, 2647816111u,   81470997u, 1943803523u,
  3814918930u, 2489596804u,  225274430u, 2053790376u, 3826175755u, 2466906013u,  167816743u, 2097651377u,
  4027552580u, 2265490386u,  503444072u, 1762050814u, 4150417245u, 2154129355u,  426522225u, 1852507879u,
  4275313526u, 2312317920u,  282753626u, 1742555852u, 4189708143u, 2394877945u,  397917763u, 1622183637u,
  3604390888u, 2714866558u,  953729732u, 1340076626u, 3518719985u, 2797360999u, 1068828381u, 1219638859u,
  3624741850u, 2936675148u,  906185462u, 1090812512u, 3747672003u, 2825379669u,  829329135u, 1181335161u,
  3412177804u, 3160834842u,  628085408u, 1382605366u, 3423369109u, 3138078467u,  570562233u, 1426400815u,
  3317316542u, 2998733608u,  733239954u, 1555261956u, 3268935591u, 3050360625u,  752459403u, 1541320221u,
  2607071920u, 3965973030u, 1969922972u,   40735498u, 2617837225u, 3943577151u, 1913087877u,   83908371u,
  2512341634u, 3803740692u, 2075208622u,  213261112u, 2463272603u, 3855990285u, 2094854071u,  198958881u,
  2262029012u, 4057260610u, 1759359992u,  534414190u, 2176718541u, 4139329115u, 1873836001u,  414664567u,
  2282248934u, 4279200368u, 1711684554u,  285281116u, 2405801727u, 4167216745u, 1634467795u,  376229701u,
  2685067896u, 3608007406u, 1308918612u,  956543938u, 2808555105u, 3495958263u, 1231636301u, 1047427035u,
  2932959818u, 3654703836u, 1088359270u,  936918000u, 2847714899u, 3736837829u, 1202900863u,  817233897u,
  3183342108u, 3401237130u, 1404277552u,  615818150u, 3134207493u, 3453421203u, 1423857449u,  601450431u,
  3009837614u, 3294710456u, 1567103746u,  711928724u, 3020668471u, 3272380065u, 1510334235u,  755167117u
};


magic.adam7ix[7] = { 0, 4, 0, 2, 0, 1, 0 }; 
magic.ADAM7_IY[7] = { 0, 0, 4, 0, 2, 0, 1 }; 
magic.ADAM7_DX[7] = { 8, 8, 4, 4, 2, 2, 1 }; 
magic.ADAM7_DY[7] = { 8, 8, 8, 4, 4, 2, 2 }; 

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

void lodepng_state_init(LodePNGState* state);
void lodepng_state_cleanup(LodePNGState* state);
void lodepng_state_copy(LodePNGState* dest, const LodePNGState* source);
unsigned lodepng_decode(unsigned char** out, unsigned* w, unsigned* h,
                        LodePNGState* state,
                        const unsigned char* in, size_t insize);
unsigned lodepng_inspect(unsigned* w, unsigned* h,
                         LodePNGState* state,
                         const unsigned char* in, size_t insize);
unsigned lodepng_inspect_chunk(LodePNGState* state, size_t pos,
                               const unsigned char* in, size_t insize)
unsigned lodepng_encode(unsigned char** out, size_t* outsize,
                        const unsigned char* image, unsigned w, unsigned h,
                        LodePNGState* state);
unsigned lodepng_chunk_length(const unsigned char* chunk);
void lodepng_chunk_type(char type[5], const unsigned char* chunk);
unsigned char lodepng_chunk_type_equals(const unsigned char* chunk, const char* type);
unsigned char lodepng_chunk_ancillary(const unsigned char* chunk);
unsigned char lodepng_chunk_private(const unsigned char* chunk);
unsigned char lodepng_chunk_safetocopy(const unsigned char* chunk);
unsigned char* lodepng_chunk_data(unsigned char* chunk);
const unsigned char* lodepng_chunk_data_const(const unsigned char* chunk)
unsigned lodepng_chunk_check_crc(const unsigned char* chunk);
void lodepng_chunk_generate_crc(unsigned char* chunk);
unsigned char* lodepng_chunk_next(unsigned char* chunk);
const unsigned char* lodepng_chunk_next_const(const unsigned char* chunk);
unsigned char* lodepng_chunk_find(unsigned char* chunk, const unsigned char* end, const char type[5]);
const unsigned char* lodepng_chunk_find_const(const unsigned char* chunk, const unsigned char* end, const char type[5]);
unsigned lodepng_chunk_append(unsigned char** out, size_t* outlength, const unsigned char* chunk);
unsigned lodepng_chunk_create(unsigned char** out, size_t* outlength, unsigned length,
                              const char* type, const unsigned char* data);
unsigned lodepng_crc32(const unsigned char* buf, size_t len);
unsigned lodepng_inflate(unsigned char** out, size_t* outsize,
                         const unsigned char* in, size_t insize,
                         const LodePNGDecompressSettings* settings);
unsigned lodepng_zlib_decompress(unsigned char** out, size_t* outsize,
                                 const unsigned char* in, size_t insize,
                                 const LodePNGDecompressSettings* settings);
unsigned lodepng_zlib_compress(unsigned char** out, size_t* outsize,
                               const unsigned char* in, size_t insize,
                               const LodePNGCompressSettings* settings);
unsigned lodepng_huffman_code_lengths(unsigned* lengths, const unsigned* frequencies,
                                      size_t numcodes, unsigned maxbitlen);
unsigned lodepng_deflate(unsigned char** out, size_t* outsize,
                         const unsigned char* in, size_t insize,
                         const LodePNGCompressSettings* settings);
unsigned lodepng_load_file(unsigned char** out, size_t* outsize, const char* filename);
unsigned lodepng_save_file(const unsigned char* buffer, size_t buffersize, const char* filename);



# endif
