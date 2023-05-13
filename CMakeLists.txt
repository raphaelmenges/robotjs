cmake_minimum_required(VERSION 3.10)
project(robotjs)

# Compiler setup.
set(CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "" FORCE) # Tell CMake the build type.
add_definitions(-DNDEBUG) # Disable asserts in release mode.
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ version selection") # Or 11, 14, 17, 20.
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Optional, ensure standard is supported.
set(CMAKE_CXX_EXTENSIONS OFF) # Optional, keep compiler extensions off.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wparentheses")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wbad-function-cast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT") # Link statically to MSVC runtime library.
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd") # Link statically to MSVC runtime library.
endif()

# Source code.
include_directories(${CMAKE_JS_INC})
set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/robotjs.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/screen.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/screengrab.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MMBitmap.c
)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

# Libraries
if(APPLE)
  find_library(CORE_FOUNDATION_LIBRARY CoreFoundation REQUIRED)
  find_library(CARBON_LIBRARY Carbon REQUIRED)
  find_library(APPLICATION_SERVICES_LIBRARY ApplicationServices REQUIRED)
  find_library(OPENGL_LIBRARY OpenGL REQUIRED)
  set(PLATFORM_LIBS
    ${CORE_FOUNDATION_LIBRARY}
    ${CARBON_LIBRARY}
    ${APPLICATION_SERVICES_LIBRARY}
    ${OPENGL_LIBRARY}
  )
endif()
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})

# Include node-addon-api wrappers.
execute_process(COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})