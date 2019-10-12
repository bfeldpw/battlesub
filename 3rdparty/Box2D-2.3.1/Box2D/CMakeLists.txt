cmake_minimum_required(VERSION 2.6)

project(Box2D)

if(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT ON)
else(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT OFF)
endif(UNIX)

option(BOX2D_INSTALL "Install Box2D libs, includes, and CMake scripts" ${BOX2D_INSTALL_BY_DEFAULT})
option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" OFF)
option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)

set(BOX2D_VERSION 2.3.0)
set(LIB_INSTALL_DIR lib${LIB_SUFFIX})

# The Box2D library.
add_subdirectory(Box2D)

