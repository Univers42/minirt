/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adam7.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:50:26 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:50:26 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADAM7_H
# define ADAM7_H

/*
Outputs various dimensions and positions in the image related to the Adam7 reduced images.
passw: output containing the width of the 7 passes
passh: output containing the height of the 7 passes
filter_passstart: output containing the index of the start and end of each
 reduced image with filter bytes
padded_passstart output containing the index of the start and end of each
 reduced image when without filter bytes but with padded scanlines
passstart: output containing the index of the start and end of each reduced
 image without padding between scanlines, but still padding between the images
w, h: width and height of non-interlaced image
bpp: bits per pixel
"padded" is only relevant if bpp is less than 8 and a scanline or image does not
 end at a full byte
*/
static void Adam7_getpassvalues(unsigned passw[7], unsigned passh[7], size_t filter_passstart[8],
                                size_t padded_passstart[8], size_t passstart[8], unsigned w, unsigned h, unsigned bpp) {
  
  unsigned i;

  
  for(i = 0; i != 7; ++i) {
    passw[i] = (w + ADAM7_DX[i] - ADAM7_IX[i] - 1) / ADAM7_DX[i];
    passh[i] = (h + ADAM7_DY[i] - ADAM7_IY[i] - 1) / ADAM7_DY[i];
    if(passw[i] == 0) passh[i] = 0;
    if(passh[i] == 0) passw[i] = 0;
  }

  filter_passstart[0] = padded_passstart[0] = passstart[0] = 0;
  for(i = 0; i != 7; ++i) {
    
    filter_passstart[i + 1] = filter_passstart[i]
                            + ((passw[i] && passh[i]) ? passh[i] * (1 + (passw[i] * bpp + 7) / 8) : 0);
    
    padded_passstart[i + 1] = padded_passstart[i] + passh[i] * ((passw[i] * bpp + 7) / 8);
    
    passstart[i + 1] = passstart[i] + (passh[i] * passw[i] * bpp + 7) / 8;
  }
}

# endif