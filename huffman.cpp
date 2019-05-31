//
// Created by nigo on 30/05/19.
//

#include <queue>
#include <array>
#include <tuple>
#include <utility>
#include <iostream>

#include "huffman.h"
#include "reader.h"
#include "writer.h"

struct huffman::node {
  uint8_t ch{};
  uint64_t count;
  node* left{};
  node* right{};
  node* parent{};

  node(uint8_t _ch, uint64_t _count) : ch(_ch), count(_count) {
  }

  node(node* _left, node* _right) : count(_left->count + _right->count),
                                    left(_left),
                                    right(_right) {
    left->parent = this;
    right->parent = this;
  }
};

bool huffman::is_right(node* v) {
  return v == v->parent->right;
}

void huffman::print_path_to_root(node* v, writer& out) {
  if (!v->parent) {
    return;
  }
  print_path_to_root(v->parent, out);
  out << is_right(v);
}

std::tuple<huffman::node*, huffman::leaf_pointers, uint64_t>
huffman::build_tree(frequencies const& count) {
  auto cmp = [](node* a, node* b) {
    return a->count > b->count;
  };
  std::priority_queue<node*, std::vector<node*>, decltype(cmp)> q(cmp);
  leaf_pointers leaves;
  uint8_t ch = 0;
  for (auto cnt : count) {
    node* cur = new node(ch, cnt);
    q.push(leaves[ch++] = cur);
  }
  uint64_t total = 0;
  while (q.size() > 1) {
    node* first = q.top();
    q.pop();
    node* second = q.top();
    q.pop();
    node* cur = new node(first, second);
    total += cur->count;
    q.push(cur);
  }
  return {q.top(), leaves, total};
}

void huffman::clear_tree(node* root) {
  if (root->left) {
    clear_tree(root->left);
    clear_tree(root->right);
  }
  delete root;
}

void huffman::encode(std::istream& in, std::ostream& out) {
  frequencies count{};
  reader rd(in);
  uint8_t ch;
  while (rd >> ch) {
    ++count[ch];
  }
  node* root;
  leaf_pointers leaves;
  uint64_t length;
  std::tie(root, leaves, length) = build_tree(count);
  writer wr(out);
  wr << static_cast<uint8_t>((reader::MAX_BITS - length % reader::MAX_BITS) %
                             reader::MAX_BITS);
  for (auto cnt : count) {
    wr << cnt;
  }
  rd.reset();
  while (rd >> ch) {
    print_path_to_root(leaves[ch], wr);
  }
  clear_tree(root);
}

inline void fail_decode() {
  throw std::runtime_error("decoding failed");
}

void huffman::decode(std::istream& in, std::ostream& out) {
  frequencies count{};
  reader rd(in);
  rd.read_ignore_last();
  for (auto& x : count) {
    if (!(rd >> x)) {
      fail_decode();
    }
  }
  node* root;
  leaf_pointers leaves;
  uint64_t total;
  std::tie(root, leaves, total) = build_tree(count);
  node* cur = root;
  bool right;
  writer wr(out);
  while (rd >> right) {
    cur = right ? cur->right : cur->left;
    if (!cur) {
      fail_decode();
    }
    if (!cur->left) {
      wr << cur->ch;
      cur = root;
    }
  }
  if (cur != root) {
    fail_decode();
  }
  clear_tree(root);
}
