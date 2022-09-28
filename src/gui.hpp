#pragma once

// Project Includes
#include "document.hpp"
#include "graph.hpp"

// C++ Includes
#include <memory>

// Display The Menu Bar
bool menuBar();

// Prompt User to tell if documents selected are correct
bool correctDocPopUp(std::shared_ptr<document>, string, bool&);

// Display The Reference Graph Created
bool graphWindow(docgraph&);

// Go Through Resolving Reference Issues
bool referenceWindow(docgraph&);
