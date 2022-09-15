#include "graph.hpp"
#include "document.hpp"
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>

void docgraph::scan_dir(path dir) {
  for(const std::filesystem::directory_entry &ent : std::filesystem::recursive_directory_iterator(dir) ){
    if( ent.is_regular_file() && !ent.is_directory() ){
      docs.push_back(shared_ptr<document>(new document(ent.path())));
    }
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
vector<shared_ptr<document>> docgraph::getDoc(string docname) {
  using Tsort = std::pair<shared_ptr<document>, int>;
  vector<Tsort> sorted;
  for(auto doc : docs ){
    string name = doc->docname();
    int pos = substr_in(name, docname).value_or(0);
    sorted.push_back(std::make_pair(doc, pos));
  }
  
  sort(sorted.begin(), sorted.end(),
      [](const Tsort &a, const Tsort &b)->bool{return a.second < b.second; });
  
  auto it = std::partition(sorted.begin(), sorted.end(), [](const Tsort &doc){return doc.second != 0; });
  sorted.erase(it, sorted.end());

  vector<shared_ptr<document>> ret;
  for( Tsort& doc : sorted ){
    ret.push_back(doc.first);
  } 

  return ret;
}

