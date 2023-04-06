find_package(xmempool REQUIRED)

add_executable(steroids)

st_target_set_common(steroids)

target_include_directories(steroids PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/core>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/core>
    ${XMEMPOOL_INCLUDE_DIRS}
)

target_sources(steroids PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/core.c"
    "${CMAKE_SOURCE_DIR}/src/core/genid.c"
    "${CMAKE_SOURCE_DIR}/src/core/modules_manager.c"
    "${CMAKE_SOURCE_DIR}/src/utils.c"
)

target_link_libraries(steroids PRIVATE
    ${ST_INTERNAL_MODULES_LIBS}
    ${XMEMPOOL_LIBRARIES}
)

if (NOT ST_HAVE_QUEUE_H)
    find_package(libbsd REQUIRED)
    target_link_libraries(steroids PRIVATE ${LIBBSD_LIBRARY})
endif()

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
