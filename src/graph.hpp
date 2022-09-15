#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <iostream>

#include "document.hpp"
#include "utils.hpp"

using std::filesystem::path;
using std::string;

class docgraph {
  vector<document> docs; 
  public:
    docgraph() = default;

    void scan_dir(path);

    /** Print information on all docs in the graph.
     */
    void printDocs() const {
      for( const document& doc : docs ){
        doc.printInfo();
      }
    }

   vector<shared_ptr<document>> getDoc(string) const; 
};
