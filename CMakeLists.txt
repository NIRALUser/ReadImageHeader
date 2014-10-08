cmake_minimum_required(VERSION 2.8)

project (ReadImageHeader)

if(NOT COMMAND SETIFEMPTY)
  macro(SETIFEMPTY)
    set(KEY ${ARGV0})
    set(VALUE ${ARGV1})
    if(NOT ${KEY})
      set(${ARGV})
    endif()
  endmacro()
endif()

SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)
SETIFEMPTY(INSTALL_LIBRARY_DESTINATION bin)
SETIFEMPTY(INSTALL_ARCHIVE_DESTINATION lib/static)

find_package(SlicerExecutionModel REQUIRED GenerateCLP)
include( ${SlicerExecutionModel_USE_FILE} )

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

option( Build_Static "Static libraries and executables only" ON )
if( Build_Static )
  set( STATIC "EXECUTABLE_ONLY" )
endif()

SEMMacroBuildCLI(
    NAME ReadImageHeader
    ${STATIC}
    TARGET_LIBRARIES ${ITK_LIBRARIES}
    INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DESTINATION}
    INSTALL_LIBRARY_DESTINATION ${INSTALL_LIBRARY_DESTINATION}
    INSTALL_ARCHIVE_DESTINATION ${INSTALL_ARCHIVE_DESTINATION}
)
