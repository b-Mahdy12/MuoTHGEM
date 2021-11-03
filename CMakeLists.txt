# #########################################################
# Use the code in this section for standalone projects
#
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(garfieldpp)

if(NOT TARGET Garfield::Garfield)
find_package(Garfield REQUIRED)
endif()

# ###########################################################
# Find ROOT variables if the variable G4ANALYSIS_USE_ROOT is set
#
#find_package(ROOT REQUIRED)
#message("${ROOT_INCLUDE_DIR}")

# Setup ROOT
#
# include(${ROOT_USE_FILE})

# ###########################################################
# ---Build executable----------------------------

add_executable(performance THGem.cc)
target_link_libraries(performance Garfield::Garfield)

add_executable(anlaysis THGemAnalysis.cc)
target_link_libraries(anlaysis Garfield::Garfield)

# ---Copy all files locally to the build directory-------------------------------
foreach(_file ELIST.lis MPLIST.lis NLIST.lis PRNSOL.lis ar_70_co2_30.gas)
# foreach(_file ELIST.lis MPLIST.lis NLIST.lis PRNSOL.lis ar_80_co2_20.gas)
# foreach(_file ELIST.lis MPLIST.lis NLIST.lis PRNSOL.lis ar_90_co2_10.gas)

  configure_file(${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file} COPYONLY)
endforeach()

# install(TARGET performance)

