#include "document.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <memory>
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
document::document(path file, vector<shared_ptr<document>> references)                 
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
        std::cout << doc->filename() << ",";
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
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @returns True if the reference was added, or false. 
 */
bool document::addReference(shared_ptr<document> doc) {
  if( !hasReference(doc) ) {
    references.push_back(doc);
    string lower_docname = doc->docname();
    std::transform(lower_docname.begin(), lower_docname.end(), lower_docname.begin(), ::tolower);
    for(auto it = unfound_references.begin(); it != unfound_references.end(); ++it){
      if( doc->docname() == *it ){
        unfound_references.erase(it);
        break;
      }

    }
    
    if( !doc->hasReference(this) ){
      
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
    if( ref.get() == doc )
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
