#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include "graph.hpp"
#include "utils.hpp"


int main() {
  unzip_file("test_dir/00-Systems/REGS-00-R1-build plan.docx", "word/document.xml");
}
