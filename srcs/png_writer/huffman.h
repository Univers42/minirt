/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   huffman.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:02:18 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:02:18 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUFFMAN_H
# define HUFFMAN_H

typedef struct s_huffman_tree
{
	unsigned	*tree2d;
	unsigned	*tree1d;
	unsigned	*lengths;
	unsigned	max_bit_len;
	unsigned	num_codes;
}	t_huffman_tree;

static inline void HuffmanTree_init(HuffmanTree* tree) {
  tree->tree2d = 0;
  tree->tree1d = 0;
  tree->lengths = 0;
}

static inline void HuffmanTree_cleanup(HuffmanTree* tree) {
  lodepng_free(tree->tree2d);
  lodepng_free(tree->tree1d);
  lodepng_free(tree->lengths);
}


static inline unsigned HuffmanTree_make2DTree(HuffmanTree* tree) {
  unsigned nodefilled = 0; 
  unsigned treepos = 0; 
  unsigned n, i;

  tree->tree2d = (unsigned*)lodepng_malloc(tree->numcodes * 2 * sizeof(unsigned));
  if(!tree->tree2d) return 83; 

  /*
  convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means
  uninited, a value >= numcodes is an address to another bit, a value < numcodes
  is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as
  many columns as codes - 1.
  A good huffman tree has N * 2 - 1 nodes, of which N - 1 are internal nodes.
  Here, the internal nodes are stored (what their 0 and 1 option point to).
  There is only memory for such good tree currently, if there are more nodes
  (due to too long length codes), error 55 will happen
  */
  for(n = 0; n < tree->numcodes * 2; ++n) {
    tree->tree2d[n] = 32767; 
  }

  for(n = 0; n < tree->numcodes; ++n)  {
    for(i = 0; i != tree->lengths[n]; ++i)  {
      unsigned char bit = (unsigned char)((tree->tree1d[n] >> (tree->lengths[n] - i - 1)) & 1);
      
      if(treepos > 2147483647 || treepos + 2 > tree->numcodes) return 55;
      if(tree->tree2d[2 * treepos + bit] == 32767)  {
        if(i + 1 == tree->lengths[n])  {
          tree->tree2d[2 * treepos + bit] = n; 
          treepos = 0;
        } else {
          /*put address of the next step in here, first that address has to be found of course
          (it's just nodefilled + 1)...*/
          ++nodefilled;
          
          tree->tree2d[2 * treepos + bit] = nodefilled + tree->numcodes;
          treepos = nodefilled;
        }
      }
      else treepos = tree->tree2d[2 * treepos + bit] - tree->numcodes;
    }
  }

  for(n = 0; n < tree->numcodes * 2; ++n) {
    if(tree->tree2d[n] == 32767) tree->tree2d[n] = 0; 
  }

  return 0;
}

/*
Second step for the ...makeFromLengths and ...makeFromFrequencies functions.
numcodes, lengths and maxbitlen must already be filled in correctly. return
value is error.
*/
static inline unsigned HuffmanTree_makeFromLengths2(HuffmanTree* tree) {
  uivector blcount;
  uivector nextcode;
  unsigned error = 0;
  unsigned bits, n;

  uivector_init(&blcount);
  uivector_init(&nextcode);

  tree->tree1d = (unsigned*)lodepng_malloc(tree->numcodes * sizeof(unsigned));
  if(!tree->tree1d) error = 83; 

  if(!uivector_resizev(&blcount, tree->maxbitlen + 1, 0)
  || !uivector_resizev(&nextcode, tree->maxbitlen + 1, 0))
    error = 83; 

  if(!error) {
    
    for(bits = 0; bits != tree->numcodes; ++bits) ++blcount.data[tree->lengths[bits]];
    
    for(bits = 1; bits <= tree->maxbitlen; ++bits) {
      nextcode.data[bits] = (nextcode.data[bits - 1] + blcount.data[bits - 1]) << 1;
    }
    
    for(n = 0; n != tree->numcodes; ++n) {
      if(tree->lengths[n] != 0) tree->tree1d[n] = nextcode.data[tree->lengths[n]]++;
    }
  }

  uivector_cleanup(&blcount);
  uivector_cleanup(&nextcode);

  if(!error) return HuffmanTree_make2DTree(tree);
  else return error;
}

/*
given the code lengths (as stored in the PNG file), generate the tree as defined
by Deflate. maxbitlen is the maximum bits that a code in the tree can have.
return value is error.
*/
static inline unsigned HuffmanTree_makeFromLengths(HuffmanTree* tree, const unsigned* bitlen,
                                            size_t numcodes, unsigned maxbitlen) {
  unsigned i;
  tree->lengths = (unsigned*)lodepng_malloc(numcodes * sizeof(unsigned));
  if(!tree->lengths) return 83; 
  for(i = 0; i != numcodes; ++i) tree->lengths[i] = bitlen[i];
  tree->numcodes = (unsigned)numcodes; 
  tree->maxbitlen = maxbitlen;
  return HuffmanTree_makeFromLengths2(tree);
}

static inline unsigned lodepng_huffman_code_lengths(unsigned* lengths, const unsigned* frequencies,
                                      size_t numcodes, unsigned maxbitlen) {
  unsigned error = 0;
  unsigned i;
  size_t numpresent = 0; 
  BPMNode* leaves; 

  if(numcodes == 0) return 80; 
  if((1u << maxbitlen) < (unsigned)numcodes) return 80; 

  leaves = (BPMNode*)lodepng_malloc(numcodes * sizeof(*leaves));
  if(!leaves) return 83; 

  for(i = 0; i != numcodes; ++i) {
    if(frequencies[i] > 0) {
      leaves[numpresent].weight = (int)frequencies[i];
      leaves[numpresent].index = i;
      ++numpresent;
    }
  }

  for(i = 0; i != numcodes; ++i) lengths[i] = 0;

  /*ensure at least two present symbols. There should be at least one symbol
  according to RFC 1951 section 3.2.7. Some decoders incorrectly require two. To
  make these work as well ensure there are at least two symbols. The
  Package-Merge code below also doesn't work correctly if there's only one
  symbol, it'd give it the theoritical 0 bits but in practice zlib wants 1 bit*/
  if(numpresent == 0) {
    lengths[0] = lengths[1] = 1; 
  } else if(numpresent == 1) {
    lengths[leaves[0].index] = 1;
    lengths[leaves[0].index == 0 ? 1 : 0] = 1;
  } else {
    BPMLists lists;
    BPMNode* node;

    bpmnode_sort(leaves, numpresent);

    lists.listsize = maxbitlen;
    lists.memsize = 2 * maxbitlen * (maxbitlen + 1);
    lists.nextfree = 0;
    lists.numfree = lists.memsize;
    lists.memory = (BPMNode*)lodepng_malloc(lists.memsize * sizeof(*lists.memory));
    lists.freelist = (BPMNode**)lodepng_malloc(lists.memsize * sizeof(BPMNode*));
    lists.chains0 = (BPMNode**)lodepng_malloc(lists.listsize * sizeof(BPMNode*));
    lists.chains1 = (BPMNode**)lodepng_malloc(lists.listsize * sizeof(BPMNode*));
    if(!lists.memory || !lists.freelist || !lists.chains0 || !lists.chains1) error = 83; 

    if(!error) {
      for(i = 0; i != lists.memsize; ++i) lists.freelist[i] = &lists.memory[i];

      bpmnode_create(&lists, leaves[0].weight, 1, 0);
      bpmnode_create(&lists, leaves[1].weight, 2, 0);

      for(i = 0; i != lists.listsize; ++i) {
        lists.chains0[i] = &lists.memory[0];
        lists.chains1[i] = &lists.memory[1];
      }

      
      for(i = 2; i != 2 * numpresent - 2; ++i) boundaryPM(&lists, leaves, numpresent, (int)maxbitlen - 1, (int)i);

      for(node = lists.chains1[maxbitlen - 1]; node; node = node->tail) {
        for(i = 0; i != node->index; ++i) ++lengths[leaves[i].index];
      }
    }

    lodepng_free(lists.memory);
    lodepng_free(lists.freelist);
    lodepng_free(lists.chains0);
    lodepng_free(lists.chains1);
  }

  lodepng_free(leaves);
  return error;
}

static inline unsigned HuffmanTree_makeFromFrequencies(HuffmanTree* tree, const unsigned* frequencies,
                                                size_t mincodes, size_t numcodes, unsigned maxbitlen) {
  unsigned error = 0;
  while(!frequencies[numcodes - 1] && numcodes > mincodes) --numcodes; 
  tree->maxbitlen = maxbitlen;
  tree->numcodes = (unsigned)numcodes; 
  tree->lengths = (unsigned*)lodepng_realloc(tree->lengths, numcodes * sizeof(unsigned));
  if(!tree->lengths) return 83; 
  
  memset(tree->lengths, 0, numcodes * sizeof(unsigned));

  error = lodepng_huffman_code_lengths(tree->lengths, frequencies, numcodes, maxbitlen);
  if(!error) error = HuffmanTree_makeFromLengths2(tree);
  return error;
}

static inline unsigned HuffmanTree_getCode(const HuffmanTree* tree, unsigned index) {
  return tree->tree1d[index];
}

static inline unsigned HuffmanTree_getLength(const HuffmanTree* tree, unsigned index) {
  return tree->lengths[index];
}
#endif