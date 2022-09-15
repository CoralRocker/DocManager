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

using std::endl, std::cout;

int main() {
  docgraph testdir;
  testdir.scan_dir("test_dir");
  
  for( auto doc : testdir.getDoc("REGS") ){
    auto refs = doc->parseReferences<DOCTYPE::WORD_XML>();
    cout << "References for " << doc->docname() << ":" << endl;
    for( auto ref : refs ){
      cout << "\t" << ref << endl;
    };
  }

}
