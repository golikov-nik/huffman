//
// Created by nigo on 30/05/19.
//

#ifndef WRITER_H
#define WRITER_H


#include <ostream>

struct writer {
  explicit writer(std::ostream& out);

  ~writer();

  writer& operator<<(uint8_t ch);

  writer& operator<<(bool x);

 private:
  static size_t const MAX_BITS = 8;
  static size_t const BUFFER_SIZE = 1u << 16u;

  size_t size{};
  uint8_t have_bits{};
  uint8_t byte{};

  char buffer[BUFFER_SIZE];

  std::ostream& out;

  void flush();
};


#endif //WRITER_H
