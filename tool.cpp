#include <bits/stdc++.h>

#include "huffman.h"

void show_usage() {
  std::cout << "Usage: huffman <-e/-d> <in> <out>\n";
}

int main(int argc, char const* argv[]) {
  if (argc != 4) {
    show_usage();
    return 0;
  }
  bool encode;
  if (strcmp(argv[1], "-e") == 0) {
    encode = true;
  } else if (strcmp(argv[1], "-d") == 0) {
    encode = false;
  } else {
    show_usage();
    return 0;
  }
  std::ifstream in{argv[2], std::ifstream::binary};
  if (!in) {
    std::cout << "error opening input file\n";
    return 0;
  }
  std::ofstream out{argv[3], std::ofstream::binary};
  if (!out) {
    std::cout << "error opening output file\n";
    return 0;
  }
  try {
    if (encode) {
      huffman::encode(in, out);
    } else {
      huffman::decode(in, out);
    }
  } catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  return 0;
}
