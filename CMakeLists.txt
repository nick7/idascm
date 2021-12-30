cmake_minimum_required (VERSION 3.14)

project ("idascm")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# set (IDASDK_ROOT "H:/_nick/src/3rd-party/idasdk68")
# set (IDASDK_ROOT "H:/_nick/src/3rd-party/idasdk73")
# set (IDASDK_ROOT "H:/_nick/src/3rd-party/idasdk75")
# set (IDASDK_ROOT "H:/_nick/src/3rd-party/idasdk76")

add_subdirectory (core)
# add_subdirectory (loader)
add_subdirectory (engine)
add_subdirectory (processor)

add_subdirectory (tests)
