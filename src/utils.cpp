#include "utils.hpp"


/**
 * @brief Search for the substring in the view, returning the number of equal characters, if any.
 * 
 * The runtime of this function is atrocious, but that's OK. 
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param view The string_view to search in
 * @param cont The string to search for. Length must be at least 3.
 * @returns Nothing if a match of at least length 3 is not found, else the number of contiguous characters matched.
 */
std::optional<int> substr_in(std::string_view view, std::string_view cont) {
  if( cont.size() == 3 ) return (view.find(cont) != string::npos) ? std::make_optional(cont.size()) : std::nullopt;
  if( cont.empty() || cont.size() < 3) throw std::invalid_argument("Substring to search for cannot be empty or < 3!: " + string(cont));

  if( view.find(cont) == string::npos ){
    auto ret = substr_in(view, cont.substr(0, cont.size()-1));
    if( !ret ){
      ret = substr_in(view, cont.substr(1));
    }

    return ret;

  } else {
    return cont.size();
  }
}

