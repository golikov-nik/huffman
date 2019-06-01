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

  node(uint8_t _ch, uint64_t _count = 0) : ch(_ch), count(_count) {
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

void huffman::dump_alphabet(node* v, writer& out) {
  if (v->left) {
    dump_alphabet(v->left, out);
    dump_alphabet(v->right, out);
  } else {
    out << v->ch;
  }
}

void huffman::dump_tree(node* v, writer& out) {
  if (v->left) {
    out << false;
    dump_tree(v->left, out);
    dump_tree(v->right, out);
  } else {
    out << true;
  }
}

void huffman::clear_tree(node* root) {
  if (root->left) {
    clear_tree(root->left);
    clear_tree(root->right);
  }
  delete root;
}

void huffman::encode(std::istream& istr, std::ostream& ostr) {
  frequencies count{};
  reader in(istr);
  uint8_t ch;
  while (in >> ch) {
    ++count[ch];
  }
  node* root;
  leaf_pointers leaves;
  uint64_t length;
  std::tie(root, leaves, length) = build_tree(count);
  length += 2 * count.size() - 1;
  writer out(ostr);
  out << static_cast<uint8_t>((reader::MAX_BITS - length % reader::MAX_BITS) %
                              reader::MAX_BITS);
  dump_alphabet(root, out);
  dump_tree(root, out);
  in.reset();
  while (in >> ch) {
    print_path_to_root(leaves[ch], out);
  }
  clear_tree(root);
}

inline void fail_decode() {
  throw std::runtime_error("decoding failed");
}

huffman::node* huffman::restore_tree(permutation const& p, reader& in,
        uint16_t& ptr) {
  bool leaf;
  if (!(in >> leaf)) {
    fail_decode();
  }
  if (leaf) {
    return new node(p[ptr++]);
  } else {
    node* left = restore_tree(p, in, ptr);
    node* right = restore_tree(p, in, ptr);
    return new node(left, right);
  }
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
  node* root = restore_tree(p, in, ptr);
  if (ptr != p.size()) {
    clear_tree(root);
    fail_decode();
  }
  node* cur = root;
  bool right;
  writer out(ostr);
  while (in >> right) {
    cur = right ? cur->right : cur->left;
    if (!cur) {
      clear_tree(root);
      fail_decode();
    }
    if (!cur->left) {
      out << cur->ch;
      cur = root;
    }
  }
  if (cur != root) {
    clear_tree(root);
    fail_decode();
  }
  clear_tree(root);
}
