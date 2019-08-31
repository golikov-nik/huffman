//
// Created by nigo on 30/05/19.
//

#include <iostream>
#include <bitset>
#include "writer.h"

writer::writer(std::ostream& _out) : out(_out) {
}

writer& writer::operator<<(bool x) {
  byte |= (x << (have_bits++));
  if (have_bits == MAX_BITS) {
    *this << byte;
    byte = 0;
    have_bits = 0;
  }
  return *this;
}

writer& writer::operator<<(uint8_t ch) {
  buffer[size++] = ch;
  if (size == BUFFER_SIZE) {
    flush();
  }
  return *this;
}

void writer::flush() {
  out.write(buffer, size);
  size = 0;
}

writer::~writer() {
  if (have_bits != 0) {
    *this << byte;
  }
  flush();
}
