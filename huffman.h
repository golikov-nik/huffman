//
// Created by nigo on 30/05/19.
//

#ifndef HUFFMAN_H
#define HUFFMAN_H


#include <istream>
#include <ostream>
#include <array>
#include <utility>

#include "writer.h"
#include "reader.h"

struct huffman {
  huffman() = delete;

  static void encode(std::istream& istr, std::ostream& ostr);

  static void decode(std::istream& istr, std::ostream& ostr);

 private:

  struct node;

  static unsigned const MAX = 1u << 8u;

  using frequencies = std::array<uint64_t, MAX>;
  using leaf_pointers = std::array<node*, MAX>;
  using permutation = std::array<uint8_t, MAX>;

  static std::tuple<node*, leaf_pointers, uint64_t>
  build_tree(frequencies const& count);

  static void dump_alphabet(node* v, writer& out);

  static void dump_tree(node* v, writer& out);

  static node* restore_tree(permutation const& p, reader& in, uint16_t& ptr);

  static void clear_tree(node* root);

  static bool is_right(node* v);

  static void print_path_to_root(node* v, writer& out);
};


#endif //HUFFMAN_H
