# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /mnt/c/Users/Hello/CLionProjects/Deque

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Deque.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Deque.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Deque.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Deque.dir/flags.make

CMakeFiles/Deque.dir/main.cpp.o: CMakeFiles/Deque.dir/flags.make
CMakeFiles/Deque.dir/main.cpp.o: /mnt/c/Users/Hello/CLionProjects/Deque/main.cpp
CMakeFiles/Deque.dir/main.cpp.o: CMakeFiles/Deque.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Deque.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Deque.dir/main.cpp.o -MF CMakeFiles/Deque.dir/main.cpp.o.d -o CMakeFiles/Deque.dir/main.cpp.o -c /mnt/c/Users/Hello/CLionProjects/Deque/main.cpp

CMakeFiles/Deque.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Deque.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Hello/CLionProjects/Deque/main.cpp > CMakeFiles/Deque.dir/main.cpp.i

CMakeFiles/Deque.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Deque.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Hello/CLionProjects/Deque/main.cpp -o CMakeFiles/Deque.dir/main.cpp.s

# Object files for target Deque
Deque_OBJECTS = \
"CMakeFiles/Deque.dir/main.cpp.o"

# External object files for target Deque
Deque_EXTERNAL_OBJECTS =

Deque: CMakeFiles/Deque.dir/main.cpp.o
Deque: CMakeFiles/Deque.dir/build.make
Deque: CMakeFiles/Deque.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Deque"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Deque.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Deque.dir/build: Deque
.PHONY : CMakeFiles/Deque.dir/build

CMakeFiles/Deque.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Deque.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Deque.dir/clean

CMakeFiles/Deque.dir/depend:
	cd /mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/Hello/CLionProjects/Deque /mnt/c/Users/Hello/CLionProjects/Deque /mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug /mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug /mnt/c/Users/Hello/CLionProjects/Deque/cmake-build-debug/CMakeFiles/Deque.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Deque.dir/depend

