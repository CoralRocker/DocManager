#include "graph.hpp"
#include "document.hpp"
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>

void docgraph::scan_dir(path dir) {
  for(const std::filesystem::directory_entry &ent : std::filesystem::recursive_directory_iterator(dir) ){
    if( ent.is_regular_file() ){
      path p = ent.path();
      // Dont allow hidden files. 
      if( p.filename().string()[0] == '.' ) continue;
      docs.push_back(shared_ptr<document>(new document(p)));
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
 * @param minmatch The minimum number of matched letters to be returned
 * @returns All documents which have that name, in sorted order from best match to worst match
 */
vector<shared_ptr<document>> docgraph::getDoc(string docname, decltype(string::npos) minmatch) {
  using Tsort = std::pair<shared_ptr<document>, int>;
  vector<Tsort> sorted;

  for(auto doc : docs ){
    string name = doc->filename();
    int pos = substr_in(name, docname, minmatch).value_or(0);
    sorted.push_back(std::make_pair(doc, pos));
  }
  
  sort(sorted.begin(), sorted.end(),
      [](const Tsort &a, const Tsort &b)->bool{return a.second > b.second; });
  
  auto it = std::partition(sorted.begin(), sorted.end(), [](const Tsort &doc){return doc.second != 0; });
  sorted.erase(it, sorted.end());

  vector<shared_ptr<document>> ret;
  for( Tsort& doc : sorted ){
    ret.push_back(doc.first);
  } 

  return ret;
}

const shared_ptr<document> docgraph::getChild(size_t c) const {
  return docs.at(c);
}


/**
 * @brief Parse All Document's References And Connect Those References
 */
void docgraph::parseAndConnect() {
  for( auto doc : docs ){
    auto ext = doc->file.extension();
    vector<string> refs;
    if( ext == ".docx" )
      refs = doc->parseReferences<WORD_XML>();
    else
      std::cerr << "Error: " << doc->filename() << " is not a parseable document" << std::endl; 
    std::cout << doc->filename() << std::endl;
    for( auto ref : refs ) {
      std::cout << '\t' << ref << std::endl; 
      auto poss = getDoc(ref, 5);
      for( auto r : poss ){
        std::cout << "\t\tMaybe: " << r->docname() << std::endl;
      }
    }
  }

}
