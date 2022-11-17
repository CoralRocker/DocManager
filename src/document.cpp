#include "document.hpp"
#include <algorithm>
#include <bits/types/FILE.h>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <istream>
#include <memory>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <string_view>

/**
 * @brief Constructor for a document from a file and a list of possible references
 *
 * @throws invalid_argument given path is only valid if it's a regular file.
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param file The path to the requested file
 * @param references A vector of share_ptrs to documents that the document references.    
 */                                                                                       
document::document(path file, vector<reference> references)                 
  : references(references), file(file) {                                                  
  if( !std::filesystem::is_regular_file(file) )                                            
    throw std::invalid_argument("File path for document must be a regular file!: "+file.string());     

  if( !getFileNameInfo() ){
    std::cerr << "Given file name: " << file.filename() << std::endl;
    throw std::invalid_argument("File name has invalid format. Valid format is: \"REGS-{subsystem number}-R{revision number}-{file name}\"");
  }

}

/**
 * @brief Print some information about the document. 
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 */
void document::printInfo() const {
      std::cout << "Document name: " << docname() << std::endl;
      std::cout << "Subsystem: " << to_string(subsys) << std::endl;
      std::cout << "Revision No.: " << revision << std::endl;

      std::cout << "References: ";
      for( auto doc : references ){
        std::cout << doc.getDoc()->filename() << ",";
      }
      std::cout << std::endl;
}

/**
 * @brief Attempt to get file information from the filename
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @returns True if the file has a valid name, false otherwise.
 */
bool document::getFileNameInfo() {
  const std::regex FINFO("REGS-(\\d+)-R(\\d+)-(.+)$");
  std::smatch m;
  string fname = file.filename();
  
  if( !std::regex_match(fname, m, FINFO ) ){
    return false;
  }else{
    document_name = m[3];
    int sys = std::stoi(m[1]);
    if( sys > (int)SUBSYSTEMS::ATC ) {
      std::cerr << "Invalid system number for file \""<< fname << "\": " << sys << std::endl;
      return false;  
    }
    subsys = SUBSYSTEMS(sys);
    revision = std::stoi(m[2]);
    return true;
  }
  
}

/**
 * @brief Add a reference to a document, if it's not already there.
 *
 * Removes the reference from unfound_references if it's there.
 * References are directional. The reference being added does not 
 * refer to *this.
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @returns True if the reference was added, or false. 
 */
bool document::addReference(shared_ptr<document> doc, string refString) {
  if( !hasReference(doc) ) {
    references.push_back(reference(doc, refString, doc->getDocID()));
    string lower_docname = doc->docname();
    std::transform(lower_docname.begin(), lower_docname.end(), lower_docname.begin(), ::tolower);
    for(auto it = unfound_references.begin(); it != unfound_references.end(); ++it){
      if( doc->docname() == *it ){
        unfound_references.erase(it);
        break;
      }

    }
    
    return true;
  }
  return false;
}

/**
 * @brief Check if document object is in the is in the references
 *
 * This method has a pretty bad runtime: O(n)
 *
 *
 * @author Gaultier Delbarre
 * @date 9/16/2022
 *
 * @param doc The document pointer to look for
 * @returns True if the document is in the references
 */
bool document::hasReference(shared_ptr<document> doc) const {
  return hasReference(doc.get());
}
bool document::hasReference(const document* doc) const {
  for( auto ref : references )
    if( ref.getDoc().get() == doc )
      return true;

  return false;
}

/**
 * @brief Check if the given document name is in the unfound references
 *
 * @author Gaultier Delbarre
 * @date 9/16/2022
 */
bool document::hasUnfoundReference(string doc) const {
  std::transform(doc.begin(), doc.end(), doc.begin(), [](char c){return std::tolower(c);});
  for( auto ref : unfound_references )
    if( ref == doc )
      return true;

  return false;
}

/**
 * @brief Parse the document and save to internal memory. Do nothing if extension/parser pair
 * unknown.
 *
 * @author Gaultier Delbarre
 * @date 9/28/2022
 */
void document::parseReferences() {
  string ext = file.extension();

  DOCTYPE dtype = INVALID;

  if( ext == ".docx" )
    dtype = WORD_XML;

  switch(dtype) {
    case WORD_XML:
      parsed_references = parseReferences<WORD_XML>();
      break;
    case INVALID:
    default:
      break;
  }

}
std::istream& operator>>(std::istream& is, document& doc){
  doc = document();
  uint32_t size;

  // Get Vector<document>
  is.read((char*)&size, sizeof(size));

  for( unsigned i = 0; i < size; i++ ){
    reference ref;
    is >> ref;
    doc.__stream_documents.push_back(ref);
  }

  // Get Vector<String> UnFound References
  is.read((char*)&size, sizeof(size));
  for( unsigned i = 0; i < size; i++ ){
    char c;
    string tmp;
    do {
      tmp.push_back(c = is.get());
    }while( c != 0 );
    doc.unfound_references.push_back(tmp);
  }

  // Get Vector<String> parsed References
  is.read((char*)&size, sizeof(size));
  for( unsigned i = 0; i < size; i++ ){
    char c;
    string tmp;
    do {
      tmp.push_back(c = is.get());
    }while( c != 0 );
    doc.parsed_references.push_back(tmp);
  }

  // Get FilePath
  char c;
  string tmp;
  do {
    tmp.push_back(c = is.get());
  }while( c != 0 );
  doc.file = tmp;

  // Get DOCID
  DOCID id;
  is >> id;
  doc.subsys = (SUBSYSTEMS)id.SUBSYS;
  doc.document_name = id.NAME;
  doc.revision = id.REVISION;

  return is;
}

std::ostream& operator<<(std::ostream& os, document& doc){
  uint32_t size = doc.references.size();
  os.write((char*)&size, sizeof(size));

  // References
  for( auto ref : doc.references ){
    auto id = ref.getDoc()->getDocID();
    os.write((char*)&id.SUBSYS, 1);
    os.write((char*)&id.REVISION, 1);
    os.write(id.NAME.c_str(), id.NAME.size());
    char c = 0x00;
    os.write(&c, 1);
  }


  // Unfound References
  size = doc.unfound_references.size();
  os.write((char*)&size, sizeof(size));

  for( auto ref : doc.unfound_references ){
    os.write(ref.c_str(), ref.size());
  }

  // Parsed References
  size = doc.parsed_references.size();
  os.write((char*)&size, sizeof(size));

  for( auto ref : doc.parsed_references ){
    os.write(ref.c_str(), ref.size());
    char c = 0x00;
    os.write(&c, 1);
  }

  // File Path
  os.write(doc.file.c_str(), doc.file.string().size());
  char c = 0x00;
  os.write(&c, 1);
  
  // DOCID
  os << doc.getDocID();
                                                  
  return os;
}

std::ostream& operator<<(std::ostream& os, reference& ref) {
  os << ref.id;

  if( ref.getRef().empty() ){
    char c = 0x00;
    os.write(&c, 1);
  }else{
    os.write(ref.getRef().c_str(), ref.getRef().size());
    char c = 0x00;
    os.write(&c, 1);
  }

  return os;
}

std::istream& operator>>(std::istream& is, reference& ref) { 
  ref = reference();

  is >> ref.id;
  char c;
  do {
    ref.ref_string.push_back(c = is.get());
  } while( c != 0 );

  return is;
}

std::istream& operator>>(std::istream& is, DOCID& id) {
  is.read((char*)&id.SUBSYS, sizeof(id.SUBSYS));
  is.read((char*)&id.REVISION, sizeof(id.REVISION));
  char c;
  id.NAME.clear();
  do {
    id.NAME.push_back(c = is.get());
  } while( c != 0 );

  return is;
}

std::ostream& operator<<(std::ostream& os, DOCID id) {
  os.write((char*)&id.SUBSYS, sizeof(id.SUBSYS));
  os.write((char*)&id.REVISION, sizeof(id.REVISION));
  os.write(id.NAME.c_str(), id.NAME.size()+1);
  return os;
}
