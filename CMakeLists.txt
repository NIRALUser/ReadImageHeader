cmake_minimum_required(VERSION 2.6)
project (ReadImageHeader)

# Set the source code for the Slicer3 BatchMake module
set(ReadImageHeader_SOURCE ReadImageHeader.cxx)

  find_package(ITK REQUIRED)
  include(${ITK_USE_FILE})

#Do not compile as a library. We need the image files to be saved on the hard drive.
add_executable(ReadImageHeader ${ReadImageHeader_SOURCE})

target_link_libraries (ReadImageHeader
 ${ITK_LIBRARIES}
)
