//
// Created by nigo on 30/05/19.
//

#ifndef READER_H
#define READER_H


#include <istream>

struct reader {
  static size_t const MAX_BITS = 8;

  explicit reader(std::istream& _in);

  bool operator>>(uint8_t& ch);

  bool operator>>(bool& x);

  void reset();
  void read_ignore_last();
 private:

  static size_t const BUFFER_SIZE = 1u << 16u;

  size_t size{};
  size_t ptr{};
  uint8_t read_bits{};

  uint8_t ignore_last{};

  char buffer[BUFFER_SIZE];

  std::istream& in;

  void fill();
};


#endif //READER_H
