cmake_minimum_required ( VERSION 2.8.5 )
project ( dfhack )
set ( PLUGIN_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
set ( DFHACK_INCLUDES
${PLUGIN_ROOT}
)
# Use all cpp files in directory as source code
file ( GLOB DFHACK_SRC "${PLUGIN_ROOT}/*.cpp" )
add_library ( dfhack STATIC ${DFHACK_SRC} )
target_include_directories( dfhack PUBLIC ${DFHACK_INCLUDES})
target_link_libraries (dfhack moai-core moai-util moai-sim zlcore)
