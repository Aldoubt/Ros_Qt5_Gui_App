include(FetchContent)

message(STATUS "get topology_msgs ...")

set(topology_msgs_DOWNLOAD_URL
    "https://github.com/chengyangkj/topology_msgs/archive/refs/heads/main.zip"
    CACHE STRING "")

FetchContent_Declare(
  topology_msgs
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL "${topology_msgs_DOWNLOAD_URL}")

FetchContent_GetProperties(topology_msgs)
if(NOT topology_msgs_POPULATED)
  FetchContent_MakeAvailable(topology_msgs)
  
  set(TOPOLOGY_MSGS_TARGETS
    topology_msgs__rosidl_generator_cpp
    topology_msgs__rosidl_typesupport_cpp
    topology_msgs__rosidl_typesupport_fastrtps_cpp
    topology_msgs__rosidl_typesupport_introspection_cpp
  )
  
  foreach(TARGET_NAME ${TOPOLOGY_MSGS_TARGETS})
    if(TARGET ${TARGET_NAME})
      get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
      # rosidl_generator_cpp is an INTERFACE_LIBRARY. It is linkable but has
      # no file to install; passing it to install(TARGETS) breaks Humble's
      # ament symlink-install generator expression handling.
      if(NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
        set_target_properties(${TARGET_NAME} PROPERTIES
          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )

        install(TARGETS ${TARGET_NAME}
          RUNTIME DESTINATION bin/lib
          LIBRARY DESTINATION bin/lib
          ARCHIVE DESTINATION bin/lib
        )
      endif()
    endif()
  endforeach()
endif()

# import targets:
# topology_msgs::topology_msgs
