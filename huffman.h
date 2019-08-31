//
// Created by nigo on 30/05/19.
//

#pragma once

#include <istream>
#include <ostream>
#include <array>
#include <tuple>

#include "writer.h"
#include "reader.h"

using std::array;

struct huffman {
  huffman() = delete;

  static void encode(std::istream& istr, std::ostream& ostr);

  static void decode(std::istream& istr, std::ostream& ostr);

 private:

  struct node;

  static unsigned const MAX = 1u << 8u;

  using frequencies = array<uint64_t, MAX>;
  using leaf_pointers = array<uint16_t, MAX>;
  using tree_t = array<node, 2 * MAX - 1>;
  using permutation = array<uint8_t, MAX>;

  static std::tuple<tree_t, leaf_pointers, uint64_t>
  build_tree(frequencies const& count);

  static void dump_alphabet(tree_t const& root, uint16_t ptr, writer& out);

  static void dump_tree(tree_t const& root, uint16_t ptr, writer& out);

  static uint16_t
  restore_tree(permutation const& p, reader& in, uint16_t& ptr, tree_t& tree,
               uint8_t& leaf_id);

  static void print_path_to_root(tree_t const& root, uint16_t v, writer& out);
};
