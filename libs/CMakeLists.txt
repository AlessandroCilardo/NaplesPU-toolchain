cmake_minimum_required ( VERSION 3.1 )
add_subdirectory ( isr )
add_subdirectory ( libc )
add_subdirectory ( libcompiler )

add_custom_target ( startup ALL COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/startup )
set_property ( DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES   "${CMAKE_CURRENT_SOURCE_DIR}/startup/build" )

file ( GLOB STARTUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/startup/build/*.o )
install ( FILES ${STARTUP_FILES} DESTINATION /usr/local/llvm-npu/libs-npu )
