#include "graph.hpp"
#include "document.hpp"
#include "utils.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>

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

std::istream& operator>>(std::istream& is, docgraph &graph) {
  graph = docgraph();
  
  std::map<uint32_t, shared_ptr<document>> map; 

  // Read Vector Of Documents
  // O(n)
  uint32_t size;
  is.read((char*)&size, sizeof(size));

  printf("Reading file of size %d\n", size);

  for( unsigned i = 0; i < size; i++ ){
    shared_ptr<document> doc(new document());

    // Get Basic Info Abt the File
    DOCID id;
    is >> id;
    doc->subsys = (SUBSYSTEMS)id.SUBSYS;
    doc->document_name = std::move(id.NAME);
    doc->revision = id.REVISION;

    // Set the file name
    std::string filename;
    readCString(is, filename); 
    doc->file = filename;

    printf("Document %s: %s\n", doc->document_name.c_str(), doc->file.c_str());
    
    // Get the unfound references
    std::string ref = "";
    do {
      ref.clear();
      readCString(is, ref);
      if( !ref.empty() )
        doc->unfound_references.push_back(ref);

      printf("Found unfound reference %ld >  %s\n", ref.size(), ref.c_str());
    } while( !ref.empty() );


    // Insert into graph and map
    graph.docs.push_back(doc);
    map.insert({i, doc});
  }

  // Rebuild Graph
  // O(n^2)
  for( unsigned i = 0; i < size; i++ ){
    auto doc = map.at(i);
    uint32_t doc_size;
    is.read((char*)&doc_size, sizeof(doc_size));

    for( unsigned j = 0; j < doc_size; j++ ){
      uint32_t doc_id;
      is.read((char*)&doc_id, sizeof(doc_id));

      auto ref = map.at(doc_id); 
      doc->references.emplace_back(ref);
    }
  }

  return is;
}

std::ostream& operator<<(std::ostream& os, docgraph &graph) {
  uint32_t size = graph.size(); 
  os.write((char*)&size, sizeof(size));
  
  std::map<shared_ptr<document>, uint32_t> id_map;

  // Describe each
  for( unsigned i = 0; i < graph.docs.size(); i++ ){
    // Insert id into the map
    id_map.insert({graph.docs.at(i), i});
    
    // Write to file
    DOCID id = graph.docs.at(i)->getDocID();
    std::filesystem::path path = graph.docs.at(i)->file;
    auto unfound_refs = graph.docs.at(i)->unfound_references;
    
    // Write ID
    os << id;

    // Write path to file
    os.write(path.c_str(), path.generic_string().size()+1);
    
    // Write unfound references 

    // Write a char** = {"xyz", ... , "xyz", NULL};
    // array
    for( string ref : unfound_refs ){
      os.write(ref.c_str(), ref.size()+1);
    }

    char c = 0;
    os.write(&c, 1);

  }

  // Document References 
  for( unsigned i = 0; i < graph.docs.size(); i++ ){
    auto doc = graph.docs.at(i);
    
    // Write Size of References
    size = doc->references.size();
    os.write((char*)&size, sizeof(size));

    for( auto ref : doc->references ){
      // Set size to id of the shared ptr
      size = id_map.at(ref.doc); 

      // Write to disk
      os.write((char*)&size, sizeof(size));
    }

  }

  return os;
}
