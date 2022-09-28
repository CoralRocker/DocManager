# Compiler to use
CXX=g++

# Flags to give to compiler.
# Add or remove flags as needed
CXXFLAGS= --std=c++20 -Wall -Werror -pedantic -ggdb -O0 -Ilib/ -Ilib/backends/

# Flags to give to linker.
# Add or remove flags as needed
LDFLAGS := -lxml2 -Llib/ -Llib/backends/ -limgui -limpl_glfw_opengl2 -lglfw -lGL

# What to name the output executable
TARGET := docmng

# Name of the source code directory
SRC_DIR := src

# Name of the build directory to place object files in
BUILD_DIR := build

#######################################
# DON'T EDIT ANYTHING PAST THIS POINT #
#######################################

# Finds all source files
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

# Creates object targets for each source file
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Sets the default target 
.PHONY: all
all: $(TARGET)
	
# Sets the clean target
.PHONY: clean
clean:
	@echo "Removing all object files ($(OBJS)) and the target executable ($(TARGET))"

	rm $(OBJS) $(TARGET)

# Auto-build the sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Building $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the target executable
$(TARGET): $(OBJS)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


