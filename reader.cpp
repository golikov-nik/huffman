//
// Created by nigo on 30/05/19.
//

#include <iostream>
#include "reader.h"

reader::reader(std::istream& _in) : in(_in) {
}

void reader::reset() {
  in.clear();
  in.seekg(0, std::istream::beg);
}

bool reader::operator>>(uint8_t& ch) {
  if (ptr == size) {
    fill();
  }
  if (size == 0) {
    return false;
  }
  ch = buffer[ptr++];
  return true;
}

bool reader::operator>>(bool& x) {
  if (ptr == size) {
    fill();
  }
  if (size == 0) {
    return false;
  }
  auto left_ignore_last = read_bits + ignore_last == MAX_BITS;
  if (left_ignore_last && ptr + 1 == size && in.eof()) {
    return false;
  }
  x = buffer[ptr] & (1u << (read_bits++));
  if (read_bits == MAX_BITS) {
    read_bits = 0;
    ptr++;
  }
  return true;
}

void reader::fill() {
  in.read(buffer, BUFFER_SIZE);
  size = in.gcount();
  ptr = 0;
}

void reader::read_ignore_last() {
  if (!(*this >> ignore_last) || ignore_last >= MAX_BITS) {
    throw std::runtime_error("decoding failed");
  }
}
