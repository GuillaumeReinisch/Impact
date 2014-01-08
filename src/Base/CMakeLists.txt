cmake_minimum_required(VERSION 2.8)

set(LIB_NAME base)

#-------------------------------------------------------------------------------------------------
#------------------ include directories of Base and set source files-----------------------------
#-------------------------------------------------------------------------------------------------

message(STATUS "\n----------------------------------------------\n----------src/Base/CMakeLists.txt----------\n----------------------------------------------")

#-----------------------
#-----------------------

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake_FindModules )

# -----  Python

IF(PYTHON)
  find_package(PythonLibs REQUIRED)
  #SET( PYTHON_INCLUDE_DIRS /opt/local/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7/ )
  #SET( PYTHON_LIBRARIES /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib )
  include_directories(${PYTHON_INCLUDE_DIRS})
  find_package(Boost COMPONENTS system  program_options filesystem python REQUIRED )
ELSE(PYTHON)
find_package(Boost COMPONENTS system  program_options filesystem REQUIRED )
ENDIF(PYTHON)

# -----  Eigen3

SET( EIGEN3_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/eigen3/
       )
message(STATUS "Eigen3 include directory: ${EIGEN3_INCLUDE_DIR} ")

#-----------------------
#-----------------------

FILE(GLOB HEADER_FILES  
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.h
			            ${CMAKE_CURRENT_SOURCE_DIR}/muparser_v2_2_3/*.h
     )

FILE(GLOB  SRC_FILES
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
			            ${CMAKE_CURRENT_SOURCE_DIR}/muparser_v2_2_3/*.cpp
       )

FILE(GLOB  PYTHON_FILES
                        ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/*.py
       )
       
#-----------------------
#-----------------------

SET( INCLUDE_DIRS
                        ${CMAKE_CURRENT_SOURCE_DIR}/
                        ${CMAKE_CURRENT_SOURCE_DIR}/muparser_v2_2_3
                        ${EIGEN3_INCLUDE_DIR}
                        ${PYTHON_INCLUDE_DIRS}
                        ${Boost_INCLUDE_DIRS}
        )
SET( DEPENDANCIES
                        ${PYTHON_LIBRARIES}  
                        ${Boost_LIBRARIES} 
    )

#-------------------------------------------------------------------------------------------------
#------------------ compile library base --------------------------------------------------------
#-------------------------------------------------------------------------------------------------

add_library ( ${LIB_NAME}   ${SRC_FILES} )

INCLUDE_DIRECTORIES( ${INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEPENDANCIES} )

IF( NOT COVERAGE )
   SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -fPIC )
   SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -O3 )
ENDIF(NOT COVERAGE )


#set_target_properties(${LIB_NAME} PROPERTIES  SUFFIX .so)

foreach(next_ITEM ${SRC_FILES})
   file(APPEND ${LAUNCHER_FILE} "#include<${next_ITEM}>\n" )
endforeach(next_ITEM ${SRC_FILES})

INSTALL(FILES  ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/include )
INSTALL(FILES  ${PYTHON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/python/widgets/ )
INSTALL(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/lib )
INSTALL(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )
#-----------------------
#-----------------------

SET( IMPACT_BASE_INCLUDE_DIRS
                        ${INCLUDE_DIRS}/
    )

SET( IMPACT_BASE_LIBRARIES 
                        ${LIB_NAME}
                        ${DEPENDANCIES}
    )
