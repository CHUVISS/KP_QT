# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appsnake_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appsnake_autogen.dir/ParseCache.txt"
  "appsnake_autogen"
  )
endif()
