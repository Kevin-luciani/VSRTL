cmake_minimum_required(VERSION 3.9)

file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.h)

######################################################################
## Linking
######################################################################

INCLUDE_DIRECTORIES("../core/")

add_library(${COMPONENTS_LIB} ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(${COMPONENTS_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${COMPONENTS_LIB} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${COMPONENTS_LIB} ${VSRTL_CORE})
