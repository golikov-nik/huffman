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

struct huffman {
  huffman() = delete;

  static void encode(std::istream& in, std::ostream& out);

  static void decode(std::istream& in, std::ostream& out);

 private:

  struct node;

  static unsigned const MAX = 1u << 8u;

  using frequencies = std::array<uint8_t, MAX>;
  using leaf_pointers = std::array<node*, MAX>;

  static std::tuple<node*, leaf_pointers, uint64_t>
  build_tree(frequencies const& count);

  static void clear_tree(node* root);

  static bool is_right(node* v);

  static void print_path_to_root(node* v, writer& out);
};


#endif //HUFFMAN_H
