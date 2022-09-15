#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <stdexcept>

using std::string;

// Tell how much of a substring is in a given string
std::optional<int> substr_in(std::string_view, std::string_view);

