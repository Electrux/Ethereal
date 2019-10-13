cmake_minimum_required(VERSION 2.8)

project(Ethereal CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(CheckCXXCompilerFlag)

find_library(gmp REQUIRED)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)

add_definitions(
	-DVERSION_MAIN=${VERSION_MAJOR}
	-DVERSION_SUB=${VERSION_MINOR}
	-DVERSION_PATCH=${VERSION_PATCH}
)

if(WIN32)
	error("Project not supported on Windows yet")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES ".*BSD.*")
	add_definitions(-D_WITH_GETLINE)
endif()

if(DEFINED ENV{DEBUG})
	add_definitions(-DDEBUG_MODE)
	message("-- Running in debug mode")
else()
	if(NOT DEFINED ENV{SKIP_LTO})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

check_cxx_compiler_flag(-march=native COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

if(DEFINED ENV{PREFIX_DIR} AND NOT "$ENV{PREFIX_DIR}" STREQUAL "")
	set(CMAKE_INSTALL_PREFIX "$ENV{PREFIX_DIR}")
else()
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
endif()
add_definitions(-DBUILD_PREFIX_DIR=${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/ethereal")
message("-- Using PREFIX = ${CMAKE_INSTALL_PREFIX}")

# Finally! The sources!!!!!

# Includes
file(COPY "${CMAKE_SOURCE_DIR}/include" DESTINATION "${CMAKE_INSTALL_PREFIX}/")

# Ethereal Core
file(GLOB_RECURSE SRCS RELATIVE "${PROJECT_SOURCE_DIR}" "src/*.cpp")
list(FILTER SRCS EXCLUDE REGEX ".*Main.cpp$")

add_library(et-lib SHARED ${SRCS})
target_link_libraries(et-lib ${CMAKE_DL_LIBS})
target_link_libraries(et-lib gmp)
set_target_properties(et-lib
    PROPERTIES
    OUTPUT_NAME et
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/ethereal"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)
install(TARGETS et-lib
	LIBRARY
	  DESTINATION lib/ethereal
	  COMPONENT Libraries
	PUBLIC_HEADER
	  DESTINATION include/ethereal
)

add_executable(et-bin "${PROJECT_SOURCE_DIR}/src/FE/Main.cpp")
target_link_libraries(et-bin et-lib)
set_target_properties(et-bin
	PROPERTIES
	OUTPUT_NAME et
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	INSTALL_RPATH_USE_LINK_PATH TRUE
)
install(TARGETS et-bin
	RUNTIME
	  DESTINATION bin
	  COMPONENT Binaries
)

# Libraries
file(GLOB_RECURSE mods RELATIVE "${PROJECT_SOURCE_DIR}" "modules/*.cpp")
foreach(m ${mods})
	get_filename_component(mod ${m} NAME_WE)
	get_filename_component(moddir ${m} DIRECTORY)
	string(SUBSTRING ${moddir} 8 -1 moddir)
	add_library(${mod} SHARED "${m}")
	target_link_libraries(${mod} et-lib)
	set_target_properties(${mod}
	    PROPERTIES
	    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/ethereal/${moddir}"
	    INSTALL_RPATH_USE_LINK_PATH TRUE
	)
	install(TARGETS ${mod}
		LIBRARY
		  DESTINATION lib/ethereal/${moddir}
		  COMPONENT Libraries
	)
endforeach()
