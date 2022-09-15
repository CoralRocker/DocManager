#include "document.hpp"


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
  return {};
}
