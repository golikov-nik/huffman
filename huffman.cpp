//
// Created by nigo on 30/05/19.
//

#include <queue>
#include <array>
#include <tuple>
#include <iostream>
#include <vector>

#include "huffman.h"
#include "reader.h"
#include "writer.h"

struct huffman::node {
  static uint16_t const null = -1;

  uint8_t ch;
  uint64_t count;
  uint16_t index;
  uint16_t left;
  uint16_t right;
  uint16_t parent;

  node() = default;

  node(uint8_t _ch, uint16_t _index, uint64_t _count = 0)
          : ch(_ch),
            count(_count),
            index(_index),
            left(null),
            right(null),
            parent(null) {
  }

  node(uint64_t _count, uint16_t _index, uint16_t _left, uint16_t _right) : count(
          _count), index(_index), left(_left), right(_right), parent(null) {
  }
};

void huffman::print_path_to_root(tree_t const& tree, uint16_t v, writer& out) {
  node const& cur = tree[v];
  if (cur.parent != node::null) {
    print_path_to_root(tree, cur.parent, out);
    out << (tree[cur.parent].right == v);
  }
}

std::tuple<huffman::tree_t, huffman::leaf_pointers, uint64_t>
huffman::build_tree(frequencies const& count) {
  auto cmp = [](node const& a, node const& b) {
    return a.count > b.count;
  };
  std::priority_queue<node, std::vector<node>, decltype(cmp)> q(cmp);
  leaf_pointers leaves;
  tree_t tree;
  uint8_t ch = 0;
  uint16_t ptr = 0;
  for (auto cnt : count) {
    node cur(ch, ptr, cnt);
    tree[ptr] = cur;
    leaves[ch++] = ptr++;
    q.push(cur);
  }
  uint64_t total = 0;
  while (q.size() > 1) {
    node first = q.top();
    q.pop();
    node second = q.top();
    q.pop();
    tree[first.index].parent = tree[second.index].parent = ptr;
    node cur(first.count + second.count, ptr, first.index, second.index);
    total += cur.count;
    tree[ptr++] = cur;
    q.push(cur);
  }
  return {tree, leaves, total};
}

void huffman::dump_alphabet(tree_t const& tree, uint16_t root, writer& out) {
  node const& cur = tree[root];
  if (cur.left != node::null) {
    dump_alphabet(tree, cur.left, out);
    dump_alphabet(tree, cur.right, out);
  } else {
    out << cur.ch;
  }
}

void huffman::dump_tree(tree_t const& tree, uint16_t root, writer& out) {
  node const& cur = tree[root];
  if (cur.left != node::null) {
    out << false;
    dump_tree(tree, cur.left, out);
    dump_tree(tree, cur.right, out);
  } else {
    out << true;
  }
}

void huffman::encode(std::istream& istr, std::ostream& ostr) {
  frequencies count{};
  reader in(istr);
  uint8_t ch;
  while (in >> ch) {
    ++count[ch];
  }
  tree_t tree;
  leaf_pointers leaves;
  uint64_t length;
  std::tie(tree, leaves, length) = build_tree(count);
  uint16_t root = tree.size() - 1;
  length += 2 * count.size() - 1;
  writer out(ostr);
  out << static_cast<uint8_t>((reader::MAX_BITS - length % reader::MAX_BITS) %
                              reader::MAX_BITS);
  dump_alphabet(tree, root, out);
  dump_tree(tree, root, out);
  in.reset();
  while (in >> ch) {
    print_path_to_root(tree, leaves[ch], out);
  }
}

inline void fail_decode() {
  throw std::runtime_error("decoding failed");
}

uint16_t huffman::restore_tree(permutation const& p, reader& in, uint16_t& ptr,
                               tree_t& tree, uint8_t& leaf_id) {
  bool leaf;
  if (ptr == tree.size() || !(in >> leaf)) {
    fail_decode();
  }
  if (leaf) {
    tree[ptr] = node(p[leaf_id++], ptr);
  } else {
    auto first = restore_tree(p, in, ptr, tree, leaf_id);
    auto second = restore_tree(p, in, ptr, tree, leaf_id);
    tree[ptr] = node(tree[first].count + tree[second].count, ptr, first,
            second);
  }
  return ptr++;
}

void huffman::decode(std::istream& istr, std::ostream& ostr) {
  reader in(istr);
  in.read_ignore_last();
  permutation p;
  for (auto& x : p) {
    if (!(in >> x)) {
      fail_decode();
    }
  }
  uint16_t ptr = 0;
  uint8_t leaf_id = 0;
  tree_t tree;
  auto root = restore_tree(p, in, ptr, tree, leaf_id);
  if (ptr != tree.size()) {
    fail_decode();
  }
  auto cur = root;
  bool right;
  writer out(ostr);
  while (in >> right) {
    cur = right ? tree[cur].right : tree[cur].left;
    if (cur == node::null) {
      fail_decode();
    }
    if (tree[cur].left == node::null) {
      out << tree[cur].ch;
      cur = root;
    }
  }
  if (cur != root) {
    fail_decode();
  }
}
