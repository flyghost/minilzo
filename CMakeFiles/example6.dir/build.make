# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/wzy/miniz

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wzy/miniz

# Include any dependencies generated for this target.
include CMakeFiles/example6.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/example6.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/example6.dir/flags.make

CMakeFiles/example6.dir/examples/example6.c.o: CMakeFiles/example6.dir/flags.make
CMakeFiles/example6.dir/examples/example6.c.o: examples/example6.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wzy/miniz/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/example6.dir/examples/example6.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/example6.dir/examples/example6.c.o   -c /home/wzy/miniz/examples/example6.c

CMakeFiles/example6.dir/examples/example6.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/example6.dir/examples/example6.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/wzy/miniz/examples/example6.c > CMakeFiles/example6.dir/examples/example6.c.i

CMakeFiles/example6.dir/examples/example6.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/example6.dir/examples/example6.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/wzy/miniz/examples/example6.c -o CMakeFiles/example6.dir/examples/example6.c.s

CMakeFiles/example6.dir/examples/example6.c.o.requires:

.PHONY : CMakeFiles/example6.dir/examples/example6.c.o.requires

CMakeFiles/example6.dir/examples/example6.c.o.provides: CMakeFiles/example6.dir/examples/example6.c.o.requires
	$(MAKE) -f CMakeFiles/example6.dir/build.make CMakeFiles/example6.dir/examples/example6.c.o.provides.build
.PHONY : CMakeFiles/example6.dir/examples/example6.c.o.provides

CMakeFiles/example6.dir/examples/example6.c.o.provides.build: CMakeFiles/example6.dir/examples/example6.c.o


# Object files for target example6
example6_OBJECTS = \
"CMakeFiles/example6.dir/examples/example6.c.o"

# External object files for target example6
example6_EXTERNAL_OBJECTS =

bin/example6: CMakeFiles/example6.dir/examples/example6.c.o
bin/example6: CMakeFiles/example6.dir/build.make
bin/example6: libminiz.so.2.1.0
bin/example6: CMakeFiles/example6.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wzy/miniz/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/example6"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example6.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/example6.dir/build: bin/example6

.PHONY : CMakeFiles/example6.dir/build

CMakeFiles/example6.dir/requires: CMakeFiles/example6.dir/examples/example6.c.o.requires

.PHONY : CMakeFiles/example6.dir/requires

CMakeFiles/example6.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/example6.dir/cmake_clean.cmake
.PHONY : CMakeFiles/example6.dir/clean

CMakeFiles/example6.dir/depend:
	cd /home/wzy/miniz && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wzy/miniz /home/wzy/miniz /home/wzy/miniz /home/wzy/miniz /home/wzy/miniz/CMakeFiles/example6.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/example6.dir/depend

