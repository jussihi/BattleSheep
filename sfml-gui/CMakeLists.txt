# Check that we aren't building in the source tree
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  MESSAGE(FATAL_ERROR "In-tree build is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

# settings
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(BattleSheep-GUI)


# Set compiler and linker flags
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-g")

SET(SOURCES
  GuiHex.hpp GuiHex.cpp
  GuiMap.hpp GuiMap.cpp
  Orientation.hpp
  ../engine/Hex.hpp  ../engine/Hex.cpp
  ../engine/Map.hpp  ../engine/Map.cpp
  ../engine/Point.hpp
  main.cpp)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DroidSans.ttf
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

INCLUDE_DIRECTORIES(../engine)
ADD_EXECUTABLE(BattleSheep-GUI ${SOURCES})
TARGET_LINK_LIBRARIES(BattleSheep-GUI sfml-graphics sfml-window sfml-system)
