#include "utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string_view>

#include <set>
#include <algorithm>
#include <vector>

namespace fs = std::filesystem;

/**
 * @brief Search for the substring in the view, returning the number of equal characters, if any.
 * 
 * The runtime of this function is atrocious, but that's OK. 
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param view The string_view to search in
 * @param cont The string to search for. Length must be at least minmatch.
 * @param minmatch The length of match to return at minimum.
 * @returns Nothing if a match of at least length 3 is not found, else the number of contiguous characters matched.
 */
std::optional<int> substr_in(std::string_view view, std::string_view cont, decltype(string::npos) minmatch) {

  // If we're at the recursive base case
  if( cont.size() == minmatch ) return (view.find(cont) != string::npos) ? std::make_optional(cont.size()) : std::nullopt;

  // Misuse of the function
  if( cont.empty() || cont.size() < minmatch) throw std::invalid_argument("Substring to search for cannot be empty or < " + std::to_string(minmatch) + "!: " + string(cont));

  auto ret = substr_in(view, cont.substr(0, cont.size()-1), minmatch);
  if( !ret )
    return {};
  else if( *ret == (int)cont.size()-1 )
    return (view.find(cont) != string::npos) ? std::make_optional(cont.size()) : *ret;
  else
    return ret;

  // if( view.find(cont) == string::npos ){
  //   auto ret = substr_in(view, cont.substr(0, cont.size()-1), minmatch);
  //   if( !ret ){
  //     ret = substr_in(view, cont.substr(1), minmatch);
  //   }

  //   return ret;

  // } else {
  //   return cont.size();
  // }

}


/**
 * @brief Unzip the given file from within a zip archive
 *
 * @author Gaultier Delbarre
 * @date 9/15/2022
 *
 * @param zipfile The path to the zip file to unzip
 * @param subfile The file path within the zipfile to unzip. Leave empty for the whole zip
 * @returns Nothing if the subfile doesn't exist.
 * @throws invalid_argument if the zipfile doesn't exist.
 */
std::optional<path> unzip_file(path zipfile, string subfile) {
  if( !fs::exists(zipfile) )
    throw std::invalid_argument("Unzip error: " + zipfile.string() + " doesn't exist");

  path tmpdir = fs::temp_directory_path();

  string command = "unzip -o \"" + zipfile.string() + "\" \"" + subfile + "\" -d " + tmpdir.string() + " > /dev/null";

  system(command.c_str());

  path enddir = tmpdir / subfile;

  if( !fs::exists(enddir) )
    return std::nullopt;

  return enddir;
}

std::istream& readCString(std::istream& is, std::string& str) {
  char c;
  do {
    c = is.get();
    if( c != 0 )
      str.push_back(c); 
  } while( c != 0 );

  return is;
}

float string_similarity(std::string s1, std::string s2) {
  std::vector<std::string> s1_pairs, s2_pairs;

  for( unsigned i = 0; i < (s1.size()-1); i++ ){
    s1_pairs.push_back(s1.substr(i, 2));
  }

  for( unsigned i = 0; i < (s2.size()-1); i++ ){
    s2_pairs.push_back(s2.substr(i,2));
  }

  std::sort(s1_pairs.begin(), s1_pairs.end());
  std::sort(s2_pairs.begin(), s2_pairs.end());

  std::vector<std::string> intersect;
  std::set_intersection(s1_pairs.begin(), s1_pairs.end(),
                        s2_pairs.begin(), s2_pairs.end(),
                        std::back_inserter(intersect));

  return 2.f * intersect.size() / (s1_pairs.size() + s2_pairs.size());
}
