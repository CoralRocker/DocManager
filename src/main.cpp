#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include "document.hpp"
#include "graph.hpp"
#include "utils.hpp"


int main() {
  docgraph testdir;
  testdir.scan_dir("test_dir");
  
  for( auto doc : testdir.getDoc("REGS")) {
    doc->parseReferences<DOCTYPE::WORD_XML>();
  }
}
