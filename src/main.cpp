#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include "graph.hpp"



int main() {
  docgraph testdir;
  testdir.scan_dir("test_dir");

  testdir.printDocs();

  auto vec = testdir.getDoc("Shir");
  testdir.getDoc(" is ");
  testdir.getDoc("GOE");
}
