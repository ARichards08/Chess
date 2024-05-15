# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Chess_2P_Visualisation_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Chess_2P_Visualisation_autogen.dir\\ParseCache.txt"
  "Chess_2P_Visualisation_autogen"
  )
endif()
