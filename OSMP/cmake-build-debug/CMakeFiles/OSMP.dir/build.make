# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /opt/clion-2018.3.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2018.3.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ms/g/go744355/Schreibtisch/g17/OSMP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/OSMP.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OSMP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OSMP.dir/flags.make

CMakeFiles/OSMP.dir/main.c.o: CMakeFiles/OSMP.dir/flags.make
CMakeFiles/OSMP.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/OSMP.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/OSMP.dir/main.c.o   -c /home/ms/g/go744355/Schreibtisch/g17/OSMP/main.c

CMakeFiles/OSMP.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/OSMP.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ms/g/go744355/Schreibtisch/g17/OSMP/main.c > CMakeFiles/OSMP.dir/main.c.i

CMakeFiles/OSMP.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/OSMP.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ms/g/go744355/Schreibtisch/g17/OSMP/main.c -o CMakeFiles/OSMP.dir/main.c.s

# Object files for target OSMP
OSMP_OBJECTS = \
"CMakeFiles/OSMP.dir/main.c.o"

# External object files for target OSMP
OSMP_EXTERNAL_OBJECTS =

OSMP: CMakeFiles/OSMP.dir/main.c.o
OSMP: CMakeFiles/OSMP.dir/build.make
OSMP: CMakeFiles/OSMP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable OSMP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OSMP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OSMP.dir/build: OSMP

.PHONY : CMakeFiles/OSMP.dir/build

CMakeFiles/OSMP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OSMP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OSMP.dir/clean

CMakeFiles/OSMP.dir/depend:
	cd /home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ms/g/go744355/Schreibtisch/g17/OSMP /home/ms/g/go744355/Schreibtisch/g17/OSMP /home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug /home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug /home/ms/g/go744355/Schreibtisch/g17/OSMP/cmake-build-debug/CMakeFiles/OSMP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OSMP.dir/depend

