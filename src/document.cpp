#include "document.hpp"
#include <filesystem>
#include <memory>

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
}

/**
 * @brief Print some information about the document. 
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 */
void document::printInfo() const {
      std::cout << "Document name: " << file.filename() << std::endl;
      std::cout << "References: ";
      for( auto doc : references ){
        std::cout << doc->docname() << ",";
      }
      std::cout << std::endl;
}

/**
 * @brief Add a reference to a document, if it's not already there.
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @returns True if the reference was added, or false. 
 */
bool addReference(shared_ptr<document> doc) {return false;}
bool addReference(document& doc) { return addReference(shared_ptr<document>(&doc)); }
