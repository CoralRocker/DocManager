#include "document.hpp"
#include "utils.hpp"
#include <fstream>
#include <ios>
#include <libxml/parser.h>
#include <libxml2/libxml/tree.h>

/**
 * @brief Special parser for word XML documents
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @returns A vector containing all the refrences in the document
 */
template<> 
vector<string> document::parseReferences<WORD_XML>() const {
  auto docxml = unzip_file(file, "word/document.xml");
  if( !docxml ){
    std::cerr << "Unable to unzip DOCX file " << file.filename() << std::endl;
    return {};
  }

  // Parse XML here
  xmlDocPtr doc;
  xmlNodePtr cur;

  doc = xmlParseFile(docxml->c_str());
  if( doc == NULL ){
    std::cerr << "Document " << *docxml << " not successfully parsed" << std::endl;
    return {};
  }

  cur = xmlDocGetRootElement(doc);

  if( cur == NULL ){
    std::cerr << "Empty XML file: " << *docxml << std::endl;
  }

  std::cout << "Root node name: " << cur->name << std::endl;

  return {};
}
