#include "utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <string>
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
