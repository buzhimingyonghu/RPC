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
include rpc/net/CMakeFiles/net.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include rpc/net/CMakeFiles/net.dir/compiler_depend.make

# Include the progress variables for this target.
include rpc/net/CMakeFiles/net.dir/progress.make

# Include the compile flags for this target's objects.
include rpc/net/CMakeFiles/net.dir/flags.make

rpc/net/CMakeFiles/net.dir/eventloop.cpp.o: rpc/net/CMakeFiles/net.dir/flags.make
rpc/net/CMakeFiles/net.dir/eventloop.cpp.o: ../rpc/net/eventloop.cpp
rpc/net/CMakeFiles/net.dir/eventloop.cpp.o: rpc/net/CMakeFiles/net.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpc/net/CMakeFiles/net.dir/eventloop.cpp.o"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT rpc/net/CMakeFiles/net.dir/eventloop.cpp.o -MF CMakeFiles/net.dir/eventloop.cpp.o.d -o CMakeFiles/net.dir/eventloop.cpp.o -c /home/nlove/temp/RPC/rpc/net/eventloop.cpp

rpc/net/CMakeFiles/net.dir/eventloop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net.dir/eventloop.cpp.i"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/rpc/net/eventloop.cpp > CMakeFiles/net.dir/eventloop.cpp.i

rpc/net/CMakeFiles/net.dir/eventloop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net.dir/eventloop.cpp.s"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/rpc/net/eventloop.cpp -o CMakeFiles/net.dir/eventloop.cpp.s

rpc/net/CMakeFiles/net.dir/fd_event.cpp.o: rpc/net/CMakeFiles/net.dir/flags.make
rpc/net/CMakeFiles/net.dir/fd_event.cpp.o: ../rpc/net/fd_event.cpp
rpc/net/CMakeFiles/net.dir/fd_event.cpp.o: rpc/net/CMakeFiles/net.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object rpc/net/CMakeFiles/net.dir/fd_event.cpp.o"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT rpc/net/CMakeFiles/net.dir/fd_event.cpp.o -MF CMakeFiles/net.dir/fd_event.cpp.o.d -o CMakeFiles/net.dir/fd_event.cpp.o -c /home/nlove/temp/RPC/rpc/net/fd_event.cpp

rpc/net/CMakeFiles/net.dir/fd_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net.dir/fd_event.cpp.i"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/rpc/net/fd_event.cpp > CMakeFiles/net.dir/fd_event.cpp.i

rpc/net/CMakeFiles/net.dir/fd_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net.dir/fd_event.cpp.s"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/rpc/net/fd_event.cpp -o CMakeFiles/net.dir/fd_event.cpp.s

rpc/net/CMakeFiles/net.dir/time_event.cpp.o: rpc/net/CMakeFiles/net.dir/flags.make
rpc/net/CMakeFiles/net.dir/time_event.cpp.o: ../rpc/net/time_event.cpp
rpc/net/CMakeFiles/net.dir/time_event.cpp.o: rpc/net/CMakeFiles/net.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object rpc/net/CMakeFiles/net.dir/time_event.cpp.o"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT rpc/net/CMakeFiles/net.dir/time_event.cpp.o -MF CMakeFiles/net.dir/time_event.cpp.o.d -o CMakeFiles/net.dir/time_event.cpp.o -c /home/nlove/temp/RPC/rpc/net/time_event.cpp

rpc/net/CMakeFiles/net.dir/time_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net.dir/time_event.cpp.i"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/rpc/net/time_event.cpp > CMakeFiles/net.dir/time_event.cpp.i

rpc/net/CMakeFiles/net.dir/time_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net.dir/time_event.cpp.s"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/rpc/net/time_event.cpp -o CMakeFiles/net.dir/time_event.cpp.s

rpc/net/CMakeFiles/net.dir/timer.cpp.o: rpc/net/CMakeFiles/net.dir/flags.make
rpc/net/CMakeFiles/net.dir/timer.cpp.o: ../rpc/net/timer.cpp
rpc/net/CMakeFiles/net.dir/timer.cpp.o: rpc/net/CMakeFiles/net.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object rpc/net/CMakeFiles/net.dir/timer.cpp.o"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT rpc/net/CMakeFiles/net.dir/timer.cpp.o -MF CMakeFiles/net.dir/timer.cpp.o.d -o CMakeFiles/net.dir/timer.cpp.o -c /home/nlove/temp/RPC/rpc/net/timer.cpp

rpc/net/CMakeFiles/net.dir/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net.dir/timer.cpp.i"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/rpc/net/timer.cpp > CMakeFiles/net.dir/timer.cpp.i

rpc/net/CMakeFiles/net.dir/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net.dir/timer.cpp.s"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/rpc/net/timer.cpp -o CMakeFiles/net.dir/timer.cpp.s

rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o: rpc/net/CMakeFiles/net.dir/flags.make
rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o: ../rpc/net/wakeup_fd_event.cpp
rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o: rpc/net/CMakeFiles/net.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o -MF CMakeFiles/net.dir/wakeup_fd_event.cpp.o.d -o CMakeFiles/net.dir/wakeup_fd_event.cpp.o -c /home/nlove/temp/RPC/rpc/net/wakeup_fd_event.cpp

rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/net.dir/wakeup_fd_event.cpp.i"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nlove/temp/RPC/rpc/net/wakeup_fd_event.cpp > CMakeFiles/net.dir/wakeup_fd_event.cpp.i

rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/net.dir/wakeup_fd_event.cpp.s"
	cd /home/nlove/temp/RPC/build/rpc/net && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nlove/temp/RPC/rpc/net/wakeup_fd_event.cpp -o CMakeFiles/net.dir/wakeup_fd_event.cpp.s

# Object files for target net
net_OBJECTS = \
"CMakeFiles/net.dir/eventloop.cpp.o" \
"CMakeFiles/net.dir/fd_event.cpp.o" \
"CMakeFiles/net.dir/time_event.cpp.o" \
"CMakeFiles/net.dir/timer.cpp.o" \
"CMakeFiles/net.dir/wakeup_fd_event.cpp.o"

# External object files for target net
net_EXTERNAL_OBJECTS =

../lib/libnet.a: rpc/net/CMakeFiles/net.dir/eventloop.cpp.o
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/fd_event.cpp.o
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/time_event.cpp.o
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/timer.cpp.o
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/wakeup_fd_event.cpp.o
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/build.make
../lib/libnet.a: rpc/net/CMakeFiles/net.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nlove/temp/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library ../../../lib/libnet.a"
	cd /home/nlove/temp/RPC/build/rpc/net && $(CMAKE_COMMAND) -P CMakeFiles/net.dir/cmake_clean_target.cmake
	cd /home/nlove/temp/RPC/build/rpc/net && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/net.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpc/net/CMakeFiles/net.dir/build: ../lib/libnet.a
.PHONY : rpc/net/CMakeFiles/net.dir/build

rpc/net/CMakeFiles/net.dir/clean:
	cd /home/nlove/temp/RPC/build/rpc/net && $(CMAKE_COMMAND) -P CMakeFiles/net.dir/cmake_clean.cmake
.PHONY : rpc/net/CMakeFiles/net.dir/clean

rpc/net/CMakeFiles/net.dir/depend:
	cd /home/nlove/temp/RPC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nlove/temp/RPC /home/nlove/temp/RPC/rpc/net /home/nlove/temp/RPC/build /home/nlove/temp/RPC/build/rpc/net /home/nlove/temp/RPC/build/rpc/net/CMakeFiles/net.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpc/net/CMakeFiles/net.dir/depend
