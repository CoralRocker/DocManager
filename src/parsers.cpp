#include "document.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <ios>
#include <libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlstring.h>

/**
 * @brief A basic RAII wrapper for the xmlChar* strings
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 */
class xmlString {
  xmlChar* ptr = nullptr;
  
  public:
    xmlString() = default;

    // Move constructor from an xmlChar* variable
    xmlString(xmlChar* &p)
      : ptr(p) {
      p = nullptr;
    }

    xmlString(xmlChar* p) : ptr(p) {}


    xmlString& operator=(xmlChar* p){
      if( ptr ) xmlFree(ptr);
      ptr = p;
      return *this;
    }

    ~xmlString() {
      if( ptr )
        xmlFree(ptr);
    }

    xmlChar* get() {
      return ptr;
    }

    operator xmlChar* () {
      return (xmlChar*)ptr;
    }

    operator string () {
      return string((char*)ptr);
    }

    bool contains(string s) const {
      string srch((char*)ptr);
      
      return srch.find(s) != string::npos;

    }

};

/**
 * @brief Get the text run <w:t> in a <w:p> item. Returns the first one found greedily.
 *
 * Searches through the children of the given node.
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param node The node to search through.
 * @returns Nothing if no <w:t> element is found, else the element.
 */
std::optional<xmlNodePtr> getInnerTextRun(xmlNodePtr node) {
  if( !xmlStrcmp(node->name, (const xmlChar*) "t")) {
    return node;
  }
  
  // go through all children of node recursively
  node = node->children;
  while( node != NULL ){
    auto ret = getInnerTextRun(node); 
    if( ret ) return ret;
    
    node = node->next;
  }


  return std::nullopt;
}

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

  // Get Root Node
  cur = xmlDocGetRootElement(doc);

  if( cur == NULL ){
    std::cerr << "Empty XML file: " << *docxml << std::endl;
  }


  /*
   * References are stored in <w:p> paragraphs. They are the last paragpraphs
   * "REFERENCES" header paragraph <w:p><w:r><w:t>Reference</w:t></w:r></w:p>
   * Search for "References" with <w:pStyle w:val="Heading1"/> as the paragraph style
   * ignore bookmarks
   */

  cur = cur->children; // Body node; root node is <document>
  vector<xmlNodePtr> paragraphs;
  cur = cur->children; // First node in body
  
  // Get all paragraph nodes
  while( cur != NULL ){
    if( !xmlStrcmp(cur->name, (const xmlChar*)"p") )
      paragraphs.push_back(cur);

    cur = cur->next;
  }
  

  // Find reference node
  xmlNodePtr ref = NULL;
  for( auto it = paragraphs.rbegin(); it != paragraphs.rend(); ++it ){
    auto txtnode = getInnerTextRun(*it);
    if( txtnode ){
      xmlString str = xmlNodeListGetString(doc, (*txtnode)->xmlChildrenNode, 1);
      if( str.contains("Reference") ){
        ref = *it;
        break;
      }
    }    
  }
  
  // Reference node not found
  if( ref == NULL ){
    std::cerr << "Unable to find references section in word document!" << std::endl;
    return {};
  }

  // Get all actual references
  ref = ref->next;
  vector<string> references;

  while( ref != NULL ){
    auto txtnode = getInnerTextRun(ref);
    if( txtnode ){
      xmlString str = xmlNodeListGetString(doc, (*txtnode)->xmlChildrenNode, 1);
      references.push_back(str);
    }else{
      // End of text nodes in paragraphs, break;
      break;
    }
    ref = ref->next;
  }


  return references;
}
