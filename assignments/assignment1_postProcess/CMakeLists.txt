#Transformations example

file(
 GLOB_RECURSE ASSIGNMENT1_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENT1_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment1_postProcess's asset folder to bin when it is built
add_custom_target(copyAssetsA1 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENT1_INC} DESTINATION include/assignment1_postProcess)
add_executable(assignment1_postProcess ${ASSIGNMENT1_SRC} ${ASSIGNMENT1_INC} ${ASSIGNMENT1_ASSETS})
target_link_libraries(assignment1_postProcess PUBLIC core IMGUI assimp)
target_include_directories(assignment1_postProcess PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment1_postProcess is built
add_dependencies(assignment1_postProcess copyAssetsA1)