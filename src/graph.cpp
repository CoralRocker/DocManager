#include "graph.hpp"

void docgraph::scan_dir(path dir) {
  for(const std::filesystem::directory_entry &ent : std::filesystem::recursive_directory_iterator(dir) ){
    if( ent.is_regular_file() )
      docs.push_back(document(ent.path()));
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
vector<shared_ptr<document>> docgraph::getDoc(string docname) const {
  for( document doc : docs ){
    string name = doc.docname();
    auto pos = substr_in(name, docname);
    if( pos )
      std::cout << "Found document with substr \"" << docname << "\": " << name << " " << *pos << std::endl;
  }
  return {};
}

