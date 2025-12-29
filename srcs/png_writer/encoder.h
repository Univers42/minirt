/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encoder.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:11:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:11:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENCODER_H
# define ENCODER_H

/*
LZ77-encode the data. Return value is error code. The input are raw bytes, the output
is in the form of unsigned integers with codes representing for example literal bytes, or
length/distance pairs.
It uses a hash table technique to let it encode faster. When doing LZ77 encoding, a
sliding window (of windowsize) is used, and all past bytes in that window can be used as
the "dictionary". A brute force search through all possible distances would be slow, and
this hash technique is one out of several ways to speed this up.
*/
static unsigned encodeLZ77(uivector* out, Hash* hash,
                           const unsigned char* in, size_t inpos, size_t insize, unsigned windowsize,
                           unsigned minmatch, unsigned nicematch, unsigned lazymatching) {
  size_t pos;
  unsigned i, error = 0;
  
  unsigned maxchainlength = windowsize >= 8192 ? windowsize : windowsize / 8;
  unsigned maxlazymatch = windowsize >= 8192 ? MAX_SUPPORTED_DEFLATE_LENGTH : 64;

  unsigned usezeros = 1; 
  unsigned numzeros = 0;

  unsigned offset; 
  unsigned length;
  unsigned lazy = 0;
  unsigned lazylength = 0, lazyoffset = 0;
  unsigned hashval;
  unsigned current_offset, current_length;
  unsigned prev_offset;
  const unsigned char *lastptr, *foreptr, *backptr;
  unsigned hashpos;

  if(windowsize == 0 || windowsize > 32768) return 60; 
  if((windowsize & (windowsize - 1)) != 0) return 90; 

  if(nicematch > MAX_SUPPORTED_DEFLATE_LENGTH) nicematch = MAX_SUPPORTED_DEFLATE_LENGTH;

  for(pos = inpos; pos < insize; ++pos) {
    size_t wpos = pos & (windowsize - 1); 
    unsigned chainlength = 0;

    hashval = getHash(in, insize, pos);

    if(usezeros && hashval == 0) {
      if(numzeros == 0) numzeros = countZeros(in, insize, pos);
      else if(pos + numzeros > insize || in[pos + numzeros - 1] != 0) --numzeros;
    } else {
      numzeros = 0;
    }

    updateHashChain(hash, wpos, hashval, numzeros);

    
    length = 0;
    offset = 0;

    hashpos = hash->chain[wpos];

    lastptr = &in[insize < pos + MAX_SUPPORTED_DEFLATE_LENGTH ? insize : pos + MAX_SUPPORTED_DEFLATE_LENGTH];

    
    prev_offset = 0;
    for(;;) {
      if(chainlength++ >= maxchainlength) break;
      current_offset = (unsigned)(hashpos <= wpos ? wpos - hashpos : wpos - hashpos + windowsize);

      if(current_offset < prev_offset) break; 
      prev_offset = current_offset;
      if(current_offset > 0) {
        
        foreptr = &in[pos];
        backptr = &in[pos - current_offset];

        
        if(numzeros >= 3) {
          unsigned skip = hash->zeros[hashpos];
          if(skip > numzeros) skip = numzeros;
          backptr += skip;
          foreptr += skip;
        }

        while(foreptr != lastptr && *backptr == *foreptr)  {
          ++backptr;
          ++foreptr;
        }
        current_length = (unsigned)(foreptr - &in[pos]);

        if(current_length > length) {
          length = current_length; 
          offset = current_offset; 
          /*jump out once a length of max length is found (speed gain). This also jumps
          out if length is MAX_SUPPORTED_DEFLATE_LENGTH*/
          if(current_length >= nicematch) break;
        }
      }

      if(hashpos == hash->chain[hashpos]) break;

      if(numzeros >= 3 && length > numzeros) {
        hashpos = hash->chainz[hashpos];
        if(hash->zeros[hashpos] != numzeros) break;
      } else {
        hashpos = hash->chain[hashpos];
        
        if(hash->val[hashpos] != (int)hashval) break;
      }
    }

    if(lazymatching) {
      if(!lazy && length >= 3 && length <= maxlazymatch && length < MAX_SUPPORTED_DEFLATE_LENGTH) {
        lazy = 1;
        lazylength = length;
        lazyoffset = offset;
        continue; 
      }
      if(lazy) {
        lazy = 0;
        if(pos == 0) ERROR_BREAK(81);
        if(length > lazylength + 1) {
          
          if(!uivector_push_back(out, in[pos - 1])) ERROR_BREAK(83 );
        } else {
          length = lazylength;
          offset = lazyoffset;
          hash->head[hashval] = -1; 
          hash->headz[numzeros] = -1; 
          --pos;
        }
      }
    }
    if(length >= 3 && offset > windowsize) ERROR_BREAK(86 );

    
    if(length < 3)  {
      if(!uivector_push_back(out, in[pos])) ERROR_BREAK(83 );
    } else if(length < minmatch || (length == 3 && offset > 4096)) {
      /*compensate for the fact that longer offsets have more extra bits, a
      length of only 3 may be not worth it then*/
      if(!uivector_push_back(out, in[pos])) ERROR_BREAK(83 );
    } else {
      addLengthDistance(out, length, offset);
      for(i = 1; i < length; ++i) {
        ++pos;
        wpos = pos & (windowsize - 1);
        hashval = getHash(in, insize, pos);
        if(usezeros && hashval == 0) {
          if(numzeros == 0) numzeros = countZeros(in, insize, pos);
          else if(pos + numzeros > insize || in[pos + numzeros - 1] != 0) --numzeros;
        } else {
          numzeros = 0;
        }
        updateHashChain(hash, wpos, hashval, numzeros);
      }
    }
  } 

  return error;
}

/*
write the lz77-encoded data, which has lit, len and dist codes, to compressed stream using huffman trees.
tree_ll: the tree for lit and len codes.
tree_d: the tree for distance codes.
*/
static void writeLZ77data(size_t* bp, ucvector* out, const uivector* lz77_encoded,
                          const HuffmanTree* tree_ll, const HuffmanTree* tree_d) {
  size_t i = 0;
  for(i = 0; i != lz77_encoded->size; ++i) {
    unsigned val = lz77_encoded->data[i];
    addHuffmanSymbol(bp, out, HuffmanTree_getCode(tree_ll, val), HuffmanTree_getLength(tree_ll, val));
    if(val > 256)  {
      unsigned length_index = val - FIRST_LENGTH_CODE_INDEX;
      unsigned n_length_extra_bits = LENGTHEXTRA[length_index];
      unsigned length_extra_bits = lz77_encoded->data[++i];

      unsigned distance_code = lz77_encoded->data[++i];

      unsigned distance_index = distance_code;
      unsigned n_distance_extra_bits = DISTANCEEXTRA[distance_index];
      unsigned distance_extra_bits = lz77_encoded->data[++i];

      addBitsToStream(bp, out, length_extra_bits, n_length_extra_bits);
      addHuffmanSymbol(bp, out, HuffmanTree_getCode(tree_d, distance_code),
                       HuffmanTree_getLength(tree_d, distance_code));
      addBitsToStream(bp, out, distance_extra_bits, n_distance_extra_bits);
    }
  }
}


# endif