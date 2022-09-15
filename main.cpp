#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

using std::cout;
using std::endl;
using std::filesystem::path;
using std::vector;
using std::string;
using std::shared_ptr;

class document {
  vector<shared_ptr<document>> references;
  path file;
  public:
    document(string file, vector<shared_ptr<document>> references={})
      : references(references), file(file) {
      if( std::filesystem::is_directory(file) )
        throw std::invalid_argument("File path for document cannot be a directory!");
    }

    string docname() {
      return file.filename();
    }

    void printInfo() {
      cout << "Document name: " << file.filename() << endl;
      cout << "References: ";
      for( auto doc : references ){
        cout << doc->docname() << ",";
      }
      cout << endl;
    }

};

class docgraph {
  vector<document> docs; 
  public:
    docgraph() = default;

    void scan_dir(path dir) {
      for(const std::filesystem::directory_entry &ent : std::filesystem::recursive_directory_iterator(dir) ){
        if( ent.is_regular_file() )
          docs.emplace_back(ent.path());
      }
    }

    void printDocs() {
      for( document& doc : docs ){
        doc.printInfo();
      }
    }
    
};

int main() {
  docgraph testdir;
  testdir.scan_dir("test_dir");
  testdir.printDocs();
}
