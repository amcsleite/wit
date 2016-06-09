#  groups ALL
cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0015 NEW)

#include_directories(corefw)

######## Setup output Directories  ##############################
SET (
      CMAKE_LIBRARY_OUTPUT_DIRECTORY   ../Bin
      CACHE PATH   "Single Directory for all Libraries"
    )
######## Setup the Executable output Directory ##################
SET (
      CMAKE_RUNTIME_OUTPUT_DIRECTORY   ../Bin
      CACHE PATH  "Single Directory for all Executables."
    )
######## Setup the Executable output Directory ##################
SET (
     CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../Bin
     CACHE PATH   "Single Directory for all static libraries."
   )

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif ()

# compiler flags common to release and debug
IF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS " -- Using GCC -- ")
#   SET(CMAKE_CXX_FLAGS "-pthread -Wextra -Wall -Wreturn-type -pedantic")
#   SET(CMAKE_CXX_FLAGS "-pthread -Wall")
#   SET(CMAKE_CXX_FLAGS "-pthread -Wreturn-type -Wunused-function")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O6")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(CMAKE_COMPILER_IS_CLANGXX)
    MESSAGE(STATUS " -- Using CLANG -- ")
     SET(CMAKE_CXX_FLAGS "-std=c++11 -Wreturn-type -Wunused-function -Wno-format-security")
#    SET(CMAKE_CXX_FLAGS "-pthread -Wreturn-type -Wunused-function -Wno-format-security")
#    SET(CMAKE_AR "/usr/local/bin/llvm-ar" CACHE INTERNAL STRING)
#    SET(CMAKE_LINKER "/usr/local/bin/llvm-link" CACHE INTERNAL STRING)
#    SET(CMAKE_MODULE_LINKER_FLAGS "-rdynamic")
#    SET(CMAKE_EXE_LINKER_FLAGS "-rdynamic")
#    SET(CMAKE_SHARED_LINKER_FLAGS "-rdynamic")
     SET(CMAKE_EXE_LINKER_FLAGS "-v")
     SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
ENDIF(CMAKE_COMPILER_IS_CLANGXX)

#changing type will append eitherCMAKE_CXX_FLAGS_RELEASE or CMAKE_CXX_FLAGS_DEBUG
#below now are in mkallr, mkalld
#SET(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -D _DEBUG")

add_subdirectory(src)

#eof