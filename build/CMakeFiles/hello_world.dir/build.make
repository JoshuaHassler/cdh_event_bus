# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jacob/Nextcloud/SPACEHAUC_code/octopOS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build

# Include any dependencies generated for this target.
include CMakeFiles/hello_world.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hello_world.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hello_world.dir/flags.make

CMakeFiles/hello_world.dir/src/main.cpp.o: CMakeFiles/hello_world.dir/flags.make
CMakeFiles/hello_world.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hello_world.dir/src/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hello_world.dir/src/main.cpp.o -c /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/main.cpp

CMakeFiles/hello_world.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hello_world.dir/src/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/main.cpp > CMakeFiles/hello_world.dir/src/main.cpp.i

CMakeFiles/hello_world.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hello_world.dir/src/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/main.cpp -o CMakeFiles/hello_world.dir/src/main.cpp.s

CMakeFiles/hello_world.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/hello_world.dir/src/main.cpp.o.requires

CMakeFiles/hello_world.dir/src/main.cpp.o.provides: CMakeFiles/hello_world.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/hello_world.dir/build.make CMakeFiles/hello_world.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/hello_world.dir/src/main.cpp.o.provides

CMakeFiles/hello_world.dir/src/main.cpp.o.provides.build: CMakeFiles/hello_world.dir/src/main.cpp.o


CMakeFiles/hello_world.dir/src/octopOS.cpp.o: CMakeFiles/hello_world.dir/flags.make
CMakeFiles/hello_world.dir/src/octopOS.cpp.o: ../src/octopOS.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/hello_world.dir/src/octopOS.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hello_world.dir/src/octopOS.cpp.o -c /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/octopOS.cpp

CMakeFiles/hello_world.dir/src/octopOS.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hello_world.dir/src/octopOS.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/octopOS.cpp > CMakeFiles/hello_world.dir/src/octopOS.cpp.i

CMakeFiles/hello_world.dir/src/octopOS.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hello_world.dir/src/octopOS.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/src/octopOS.cpp -o CMakeFiles/hello_world.dir/src/octopOS.cpp.s

CMakeFiles/hello_world.dir/src/octopOS.cpp.o.requires:

.PHONY : CMakeFiles/hello_world.dir/src/octopOS.cpp.o.requires

CMakeFiles/hello_world.dir/src/octopOS.cpp.o.provides: CMakeFiles/hello_world.dir/src/octopOS.cpp.o.requires
	$(MAKE) -f CMakeFiles/hello_world.dir/build.make CMakeFiles/hello_world.dir/src/octopOS.cpp.o.provides.build
.PHONY : CMakeFiles/hello_world.dir/src/octopOS.cpp.o.provides

CMakeFiles/hello_world.dir/src/octopOS.cpp.o.provides.build: CMakeFiles/hello_world.dir/src/octopOS.cpp.o


# Object files for target hello_world
hello_world_OBJECTS = \
"CMakeFiles/hello_world.dir/src/main.cpp.o" \
"CMakeFiles/hello_world.dir/src/octopOS.cpp.o"

# External object files for target hello_world
hello_world_EXTERNAL_OBJECTS =

bin/hello_world: CMakeFiles/hello_world.dir/src/main.cpp.o
bin/hello_world: CMakeFiles/hello_world.dir/src/octopOS.cpp.o
bin/hello_world: CMakeFiles/hello_world.dir/build.make
bin/hello_world: CMakeFiles/hello_world.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable bin/hello_world"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hello_world.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hello_world.dir/build: bin/hello_world

.PHONY : CMakeFiles/hello_world.dir/build

CMakeFiles/hello_world.dir/requires: CMakeFiles/hello_world.dir/src/main.cpp.o.requires
CMakeFiles/hello_world.dir/requires: CMakeFiles/hello_world.dir/src/octopOS.cpp.o.requires

.PHONY : CMakeFiles/hello_world.dir/requires

CMakeFiles/hello_world.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hello_world.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hello_world.dir/clean

CMakeFiles/hello_world.dir/depend:
	cd /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jacob/Nextcloud/SPACEHAUC_code/octopOS /home/jacob/Nextcloud/SPACEHAUC_code/octopOS /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build /home/jacob/Nextcloud/SPACEHAUC_code/octopOS/build/CMakeFiles/hello_world.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hello_world.dir/depend

