cmake_minimum_required(VERSION 2.8)

PROJECT(ATOMISM)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


#-------------------------------------------------------------------------------------------------
#------------------ Set directory to collect libraries -------------------------------------------
#-------------------------------------------------------------------------------------------------

STRING(REGEX REPLACE "/" "_" COMPILER_PATH ${CMAKE_CXX_COMPILER})

SET( LIBRARY_OUTPUT_PATH
          ${CMAKE_CURRENT_SOURCE_DIR}/build/libs${COMPILER_PATH}
    )

MESSAGE( STATUS "The libraries are collected in ${LIBRARY_OUTPUT_PATH}")

MESSAGE( STATUS "The installation is realized in ${CMAKE_INSTALL_PREFIX}. Use 'cmake -DCMAKE_INSTALL_PREFIX=$install_dir CMakeList.txt' to change the installation directory to $install_dir")

#-------------------------------------------------------------------------------------------------
#------------------ Options for compilation                      ---------------------------------
#-------------------------------------------------------------------------------------------------

CONFIGURE_FILE(config.h.cmake config.h)

SET( COVERAGE 0 CACHE BOOL "if true activate code coverage option for tests")
SET( PYTHON   1 CACHE BOOL "if true python binding. It needs the python library.")
SET( ATOMISM  1 CACHE BOOL "if true compile the Atomism interface.")
SET( MATH     1 CACHE BOOL "if true compile the math interface.")
SET( ANTIOCH  0 CACHE BOOL "if true include antioch.")
SET( QUESO    0 CACHE BOOL "if true include the QUESO library.")
SET( VTK      0 CACHE BOOL "if true enabled 3D using VTK.")

IF( PYTHON )
   MESSAGE( STATUS "Python binding activated (-DPYTHON=0 to desactivate)")
ELSEIF( NOT PYTHON )
   MESSAGE( STATUS "Python binding desactivated (-DPYTHON=0 to activate)")
ENDIF( PYTHON )

IF( VTK )
   MESSAGE( STATUS "Enable 3D using VTK library (-VTK=0 to desactivate)")
ELSEIF( NOT VTK )
   MESSAGE( STATUS "Disable 3D using VTK library (-VTK=1 to desactivate)")
ENDIF( VTK )

IF( COVERAGE )
   MESSAGE( STATUS "Enable code coverage for tests (-DCOVERAGE=0 to desactivate)")
  INCLUDE(CTest)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
   SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
   SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
ELSEIF( NOT COVERAGE )
   MESSAGE( STATUS "Disable code coverage for tests (-DCOVERAGE=1 to activate)")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -fPIC")
ENDIF( COVERAGE )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET( LAUNCHER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/launcher.cpp)
file(WRITE ${LAUNCHER_FILE} "" )

#-------------------------------------------------------------------------------------------------
#------------------ include sub directories for compilation
#-------------------------------------------------------------------------------------------------

#-- Base  

ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/Base 
                )

GET_DIRECTORY_PROPERTY (IMPACT_BASE_INCLUDE_DIRS DIRECTORY 	  ./src/Base 	DEFINITION 	IMPACT_BASE_INCLUDE_DIRS
                        )

GET_DIRECTORY_PROPERTY (IMPACT_BASE_LIBRARIES DIRECTORY 	      ./src/Base 	DEFINITION 	IMPACT_BASE_LIBRARIES 
                        )

#-- Math

ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/Math )

GET_DIRECTORY_PROPERTY (MATH_INCLUDE_DIRS DIRECTORY        ./src/Math  	DEFINITION 	MATH_INCLUDE_DIRS
                        )

GET_DIRECTORY_PROPERTY (MATH_LIBRARIES DIRECTORY 	      ./src/Math 	DEFINITION 	MATH_LIBRARIES
                        )

#-- Atomism

ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/Atomism )

GET_DIRECTORY_PROPERTY (ATOMISM_INCLUDE_DIRS DIRECTORY        ./src/Atomism  	DEFINITION 	ATOMISM_INCLUDE_DIRS
                        )

GET_DIRECTORY_PROPERTY (ATOMISM_LIBRARIES DIRECTORY 	      ./src/Atomism 	DEFINITION 	ATOMISM_LIBRARIES
                        )

#-- Antioch

ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/Antioch
                 )

GET_DIRECTORY_PROPERTY (ANTIOCH_INCLUDE_DIRS DIRECTORY 	  ./src/Antioch		DEFINITION     ANTIOCH_INCLUDE_DIRS
                        )

GET_DIRECTORY_PROPERTY (ANTIOCH_LIBRARIES    DIRECTORY    ./src/Antioch    DEFINITION     ANTIOCH_LIBRARIES
                       )
             
#-- QUESO

ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/Queso
                 )

GET_DIRECTORY_PROPERTY (QUESO_INCLUDE_DIRS DIRECTORY 	  ./src/Queso		DEFINITION     QUESO_INCLUDE_DIRS
                        )

GET_DIRECTORY_PROPERTY (QUESO_LIBRARIES    DIRECTORY    ./src/Queso     DEFINITION     QUESO_LIBRARIES
                       )


# Graphical Interface

IF( PYTHON )
    ADD_SUBDIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR}/src/GUI_MainWindow
                     )
ENDIF( PYTHON )


SET( IMPACT_INCLUDE_DIRS
     ${IMPACT_BASE_INCLUDE_DIRS}
     ${MATH_INCLUDE_DIRS}
     ${ATOMISM_INCLUDE_DIRS}
     ${CANTERA_INCLUDE_DIRS}
     ${POLYMER_INCLUDE_DIRS}
     ${NASA_INCLUDE_DIRS}
     ${QUESO_INCLUDE_DIRS}
     ${ANTIOCH_INCLUDE_DIRS}
    )

SET( IMPACT_LIBRARIES
     ${IMPACT_BASE_LIBRARIES}
     ${MATH_LIBRARIES}
     ${ATOMISM_LIBRARIES}
     ${CANTERA_LIBRARIES}   
     ${POLYMER_LIBRARIES}
     ${NASA_LIBRARIES}
     ${QUESO_LIBRARIES}
     ${ANTIOCH_LIBRARIES}
      )

FILE(GLOB DOC_FILES  
     ${CMAKE_CURRENT_SOURCE_DIR}/doc/html/*.*
     )
     
#-------------------------------------------------------------------------------------------------
#------------------ compile library       --------------------------------------------------------
#-------------------------------------------------------------------------------------------------

include_directories( ${IMPACT_INCLUDE_DIRS} )

message(STATUS "\n----------------------------------------------\n----------./CMakeLists.txt----------\n----------------------------------------------")

message(STATUS "Libraries: ${IMPACT_LIBRARIES}")
message(STATUS "Includes:  ${IMPACT_INCLUDE_DIRS}")

add_library ( impact   SHARED src/launcher.cpp )
             
SET_TARGET_PROPERTIES( impact PROPERTIES COMPILE_FLAGS -std=c++11 -O3 -fPIC )

set_target_properties( impact PROPERTIES  SUFFIX .so)
TARGET_LINK_LIBRARIES( impact ${IMPACT_LIBRARIES} )

INSTALL(TARGETS impact DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/lib )
INSTALL(TARGETS impact DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/mainpage.dox  DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile      DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )

INSTALL(FILES  ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/doc/html )
