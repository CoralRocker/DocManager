#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <stdexcept>
#include <filesystem>

using std::string;
using std::filesystem::path;

// Tell how much of a substring is in a given string
std::optional<int> substr_in(std::string_view, std::string_view, decltype(string::npos));

// Strike A Match: http://www.catalysoft.com/articles/StrikeAMatch.html
float string_similarity(std::string, std::string);

// Unzip a given subdocument in a zip file to a temp folder and return the unzipped file path
std::optional<path> unzip_file(path, string); 

std::istream& readCString(std::istream& is, std::string&);
