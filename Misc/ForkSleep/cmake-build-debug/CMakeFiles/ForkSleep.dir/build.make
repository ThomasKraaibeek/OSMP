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
CMAKE_SOURCE_DIR = /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ForkSleep.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ForkSleep.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ForkSleep.dir/flags.make

CMakeFiles/ForkSleep.dir/main.c.o: CMakeFiles/ForkSleep.dir/flags.make
CMakeFiles/ForkSleep.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ForkSleep.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ForkSleep.dir/main.c.o   -c /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/main.c

CMakeFiles/ForkSleep.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ForkSleep.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/main.c > CMakeFiles/ForkSleep.dir/main.c.i

CMakeFiles/ForkSleep.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ForkSleep.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/main.c -o CMakeFiles/ForkSleep.dir/main.c.s

# Object files for target ForkSleep
ForkSleep_OBJECTS = \
"CMakeFiles/ForkSleep.dir/main.c.o"

# External object files for target ForkSleep
ForkSleep_EXTERNAL_OBJECTS =

ForkSleep: CMakeFiles/ForkSleep.dir/main.c.o
ForkSleep: CMakeFiles/ForkSleep.dir/build.make
ForkSleep: CMakeFiles/ForkSleep.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ForkSleep"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ForkSleep.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ForkSleep.dir/build: ForkSleep

.PHONY : CMakeFiles/ForkSleep.dir/build

CMakeFiles/ForkSleep.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ForkSleep.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ForkSleep.dir/clean

CMakeFiles/ForkSleep.dir/depend:
	cd /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug /home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/ForkSleep/cmake-build-debug/CMakeFiles/ForkSleep.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ForkSleep.dir/depend

