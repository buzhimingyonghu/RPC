# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nlove/temp/RPC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nlove/temp/RPC/build

# Include any dependencies generated for this target.
include testcases/CMakeFiles/test_client.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include testcases/CMakeFiles/test_client.dir/compiler_depend.make

# Include the progress variables for this target.
include testcases/CMakeFiles/test_client.dir/progress.make

# Include the compile flags for this target's objects.
include testcases/CMakeFiles/test_client.dir/flags.make

testcases/CMakeFiles/test_client.dir/test_client.cpp.o: testcases/CMakeFiles/test_client.dir/flags.make
testcases/CMakeFiles/test_client.dir/test_client.cpp.o: ../testcases/test_client.cpp
testcases/CMakeFiles/test_client.dir/test_client.cpp.o: testcases/CMakeFiles/test_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object testcases/CMakeFiles/test_client.dir/test_client.cpp.o"
	cd /home/nlove/temp/RPC/build/testcases && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT testcases/CMakeFiles/test_client.dir/test_client.cpp.o -MF CMakeFiles/test_client.dir/test_client.cpp.o.d -o CMakeFiles/test_client.dir/test_client.cpp.o -c /home/nlove/temp/RPC/testcases/test_client.cpp

testcases/CMakeFiles/test_client.dir/test_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_client.dir/test_client.cpp.i"
	cd /home/nlove/temp/RPC/build/testcases && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/testcases/test_client.cpp > CMakeFiles/test_client.dir/test_client.cpp.i

testcases/CMakeFiles/test_client.dir/test_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_client.dir/test_client.cpp.s"
	cd /home/nlove/temp/RPC/build/testcases && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/testcases/test_client.cpp -o CMakeFiles/test_client.dir/test_client.cpp.s

# Object files for target test_client
test_client_OBJECTS = \
"CMakeFiles/test_client.dir/test_client.cpp.o"

# External object files for target test_client
test_client_EXTERNAL_OBJECTS =

../bin/test_client: testcases/CMakeFiles/test_client.dir/test_client.cpp.o
../bin/test_client: testcases/CMakeFiles/test_client.dir/build.make
../bin/test_client: ../lib/libcommon.a
../bin/test_client: ../lib/libnet.a
../bin/test_client: ../lib/libtcp.a
../bin/test_client: ../lib/libnet.a
../bin/test_client: ../lib/libcommon.a
../bin/test_client: testcases/CMakeFiles/test_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/test_client"
	cd /home/nlove/temp/RPC/build/testcases && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
testcases/CMakeFiles/test_client.dir/build: ../bin/test_client
.PHONY : testcases/CMakeFiles/test_client.dir/build

testcases/CMakeFiles/test_client.dir/clean:
	cd /home/nlove/temp/RPC/build/testcases && $(CMAKE_COMMAND) -P CMakeFiles/test_client.dir/cmake_clean.cmake
.PHONY : testcases/CMakeFiles/test_client.dir/clean

testcases/CMakeFiles/test_client.dir/depend:
	cd /home/nlove/temp/RPC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nlove/temp/RPC /home/nlove/temp/RPC/testcases /home/nlove/temp/RPC/build /home/nlove/temp/RPC/build/testcases /home/nlove/temp/RPC/build/testcases/CMakeFiles/test_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : testcases/CMakeFiles/test_client.dir/depend
