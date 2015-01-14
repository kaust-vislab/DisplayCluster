
# Copyright (c) 2013-2014, EPFL/Blue Brain Project
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>

include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${PROJECT_BINARY_DIR}) ### for config.h ###

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include config.h")

set(DISPLAYCLUSTER_MOC_HEADERS
  BackgroundWidget.h
  ContentWindowGraphicsItem.h
  DisplayGroupGraphicsView.h
  DisplayGroupListWidget.h
  MasterApplication.h
  MasterWindow.h
  WallApplication.h
  WebbrowserWidget.h
)

set(DISPLAYCLUSTER_SOURCES
  main.cpp
  BackgroundWidget.cpp
  CommandLineParameters.cpp
  ContentWindowGraphicsItem.cpp
  ContentWindowListWidgetItem.cpp
  DisplayGroupGraphicsScene.cpp
  DisplayGroupGraphicsView.cpp
  DisplayGroupListWidget.cpp
  MasterApplication.cpp
  MasterWindow.cpp
  WallApplication.cpp
  WebbrowserWidget.cpp
)

find_package(ALSA)
include_directories(${ALSA_INCLUDE_DIRS})

set(DISPLAYCLUSTER_LINK_LIBRARIES dccore dcwebservice 
  ${Boost_PROGRAM_OPTIONS_LIBRARY} 
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${ALSA_LIBRARIES} 
  ${SuperCollider_LIBRARIES}
)

qt4_wrap_cpp(DISPLAYCLUSTER_MOC_OUTFILES ${DISPLAYCLUSTER_MOC_HEADERS})

add_executable(displaycluster ${DISPLAYCLUSTER_MOC_OUTFILES} ${DISPLAYCLUSTER_SOURCES})
target_link_libraries(displaycluster ${DISPLAYCLUSTER_LINK_LIBRARIES})

install(TARGETS displaycluster RUNTIME DESTINATION bin COMPONENT core)
install(PROGRAMS startdisplaycluster DESTINATION bin COMPONENT core)
