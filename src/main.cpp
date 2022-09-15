#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

using std::cout;
using std::endl;
using std::filesystem::path;
using std::vector;
using std::string;
using std::shared_ptr;

/**
 * Search for the substring in the view, returning the number of equal characters, if any.
 *
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param view The string_view to search in
 * @param cont The string to search for. Length must be at least 3.
 * @returns Nothing if a match of at least length 3 is not found, else the number of contiguous characters matched.
 */
std::optional<int> substr_in(std::string_view view, std::string_view cont) {
  if( cont.size() == 3 ) return (view.find(cont) != string::npos) ? std::make_optional(cont.size()) : std::nullopt;
  if( cont.empty() || cont.size() < 3) throw std::invalid_argument("Substring to search for cannot be empty or < 3!: " + string(cont));

  if( view.find(cont) == string::npos ){
    auto ret = substr_in(view, cont.substr(0, cont.size()-1));
    if( !ret ){
      ret = substr_in(view, cont.substr(1));
    }

    return ret;

  } else {
    return cont.size();
  }
}

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

    bool addReference(document& doc);
    bool addReference(shared_ptr<document> doc);

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

    /**
     * @brief Search for all documents that have the substring "docname" in them
     * 
     * @author Gaultier Delbarre
     * @date 9/15/2022
     *
     * @param docname Part or all of the name of the document
     * @returns All documents which have that name, in sorted order from best match to worst match
     */
    vector<shared_ptr<document>> getDoc(string docname) {
      for( document doc : docs ){
        string name = doc.docname();
        auto pos = substr_in(name, docname);
        if( pos )
          cout << "Found document with substr \"" << docname << "\": " << name << " " << *pos << endl;
      }
      return {};
    }
    
};

int main() {
  docgraph testdir;
  testdir.scan_dir("test_dir");

  testdir.getDoc("REGS");
  testdir.getDoc(" is ");
  testdir.getDoc("GOE");
}
