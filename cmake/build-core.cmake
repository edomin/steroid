find_package(list REQUIRED)

add_executable(steroids)

st_target_set_common(steroids)

target_include_directories(steroids PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/core>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/core>
    ${LIST_INCLUDE_DIR}
)

target_sources(steroids PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/core.c"
    "${CMAKE_SOURCE_DIR}/src/core/modules_manager.c"
    "${CMAKE_SOURCE_DIR}/src/stdlib/strlcpy.c"
)

target_link_libraries(steroids PRIVATE
    ${ST_INTERNAL_MODULES_LIBS}
    ${LIST_LIBRARY}
)

#if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    #target_sources(marathoner PRIVATE "src/core/engine.rc")
#endif()

#install(
    #TARGETS       marathoner
    #RUNTIME       DESTINATION "${CMAKE_SOURCE_DIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
    ##LIBRARY       DESTINATION lib
    ##ARCHIVE       DESTINATION lib
    ##PUBLIC_HEADER DESTINATION include
#)
