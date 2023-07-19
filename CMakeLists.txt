# CMakeList.txt : CMake project for DeleteNodeModules, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("DeleteNodeModules")

# Set source and header variables
set(SOURCES src/DeleteNodeModules.c)
# Add source to this project's executable.
add_executable (DeleteNodeModules ${SOURCES} ${HEADERS})


target_link_libraries(DeleteNodeModules
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET DeleteNodeModules PROPERTY C_STANDARD 99)
endif()
