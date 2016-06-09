# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/al/wit

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/al/wit

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/al/wit/CMakeFiles /home/al/wit/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/al/wit/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named wPoly

# Build rule for target.
wPoly: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wPoly
.PHONY : wPoly

# fast build rule for target.
wPoly/fast:
	$(MAKE) -f src/CMakeFiles/wPoly.dir/build.make src/CMakeFiles/wPoly.dir/build
.PHONY : wPoly/fast

#=============================================================================
# Target rules for targets named wCommon

# Build rule for target.
wCommon: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wCommon
.PHONY : wCommon

# fast build rule for target.
wCommon/fast:
	$(MAKE) -f src/CMakeFiles/wCommon.dir/build.make src/CMakeFiles/wCommon.dir/build
.PHONY : wCommon/fast

#=============================================================================
# Target rules for targets named wSelect

# Build rule for target.
wSelect: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wSelect
.PHONY : wSelect

# fast build rule for target.
wSelect/fast:
	$(MAKE) -f src/CMakeFiles/wSelect.dir/build.make src/CMakeFiles/wSelect.dir/build
.PHONY : wSelect/fast

#=============================================================================
# Target rules for targets named wFilter

# Build rule for target.
wFilter: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wFilter
.PHONY : wFilter

# fast build rule for target.
wFilter/fast:
	$(MAKE) -f src/CMakeFiles/wFilter.dir/build.make src/CMakeFiles/wFilter.dir/build
.PHONY : wFilter/fast

#=============================================================================
# Target rules for targets named wTex

# Build rule for target.
wTex: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wTex
.PHONY : wTex

# fast build rule for target.
wTex/fast:
	$(MAKE) -f src/CMakeFiles/wTex.dir/build.make src/CMakeFiles/wTex.dir/build
.PHONY : wTex/fast

#=============================================================================
# Target rules for targets named wMat

# Build rule for target.
wMat: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wMat
.PHONY : wMat

# fast build rule for target.
wMat/fast:
	$(MAKE) -f src/CMakeFiles/wMat.dir/build.make src/CMakeFiles/wMat.dir/build
.PHONY : wMat/fast

#=============================================================================
# Target rules for targets named wVerify

# Build rule for target.
wVerify: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wVerify
.PHONY : wVerify

# fast build rule for target.
wVerify/fast:
	$(MAKE) -f src/CMakeFiles/wVerify.dir/build.make src/CMakeFiles/wVerify.dir/build
.PHONY : wVerify/fast

#=============================================================================
# Target rules for targets named wTests

# Build rule for target.
wTests: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wTests
.PHONY : wTests

# fast build rule for target.
wTests/fast:
	$(MAKE) -f src/CMakeFiles/wTests.dir/build.make src/CMakeFiles/wTests.dir/build
.PHONY : wTests/fast

#=============================================================================
# Target rules for targets named wCut

# Build rule for target.
wCut: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 wCut
.PHONY : wCut

# fast build rule for target.
wCut/fast:
	$(MAKE) -f src/CMakeFiles/wCut.dir/build.make src/CMakeFiles/wCut.dir/build
.PHONY : wCut/fast

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... wPoly"
	@echo "... wCommon"
	@echo "... wSelect"
	@echo "... wFilter"
	@echo "... wTex"
	@echo "... wMat"
	@echo "... wVerify"
	@echo "... wTests"
	@echo "... wCut"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
