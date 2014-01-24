
cmake_minimum_required(VERSION 2.8)

message(STATUS "\n----------------------------------------------\n----------src/Antioch/CMakeLists.txt----------\n----------------------------------------------")

IF( ANTIOCH )
   MESSAGE( STATUS "Include the Antioch interface for chemical kinetics resolution, cantera library needed for linking (-DANTIOCH=0 to desactivate)")

set(LIB_NAME impact_antioch)

#-------------------------------------------------------------------------------------------------
#------------------ include directories of Base and set source files-----------------------------
#-------------------------------------------------------------------------------------------------

message(STATUS "Molecule include directories: ${ATOMISM_INCLUDE_DIRS}")
message(STATUS "Molecule libraries: ${ATOMISM_LIBRARIES}")
message(STATUS "impact base include directories: ${BASE_INCLUDE_DIRS}")
message(STATUS "impact base libraries: ${BASE_LIBRARIES}")
#-----------------------
#-----------------------

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake_FindModules )

# -----  Antioch

find_package(Antioch REQUIRED )

#-----------------------
#-----------------------

FILE(GLOB HEADER_FILES  
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.h
     )

FILE(GLOB  SRC_FILES
                              ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
       )

FILE(GLOB  PYTHON_FILES
                              ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/*.py
       )
       
#-----------------------
#-----------------------

SET( INCLUDE_DIRS
                        ${CMAKE_CURRENT_SOURCE_DIR}/
                        ${BASE_INCLUDE_DIRS}
                        ${ATOMISM_INCLUDE_DIRS}
                        ${ANTIOCH_INCLUDE_DIRS}
        )
SET( DEPENDANCIES
			${BASE_LIBRARIES}
                       ${ATOMISM_LIBRARIES}
                        ${ANTIOCH_LIBRARIES}
    )

#-------------------------------------------------------------------------------------------------
#------------------ compile library base --------------------------------------------------------
#-------------------------------------------------------------------------------------------------

add_library ( ${LIB_NAME}   ${SRC_FILES} )
             
INCLUDE_DIRECTORIES( ${INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEPENDANCIES} )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -std=c++11 -O3 -fPIC )
   
#IF( NOT COVERAGE )
#   SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -fPIC )
#   SET_TARGET_PROPERTIES( ${LIB_NAME} PROPERTIES COMPILE_FLAGS -O3 )
#ENDIF(NOT COVERAGE )

#set_target_properties(${LIB_NAME} PROPERTIES  SUFFIX .so)

foreach(next_ITEM ${SRC_FILES})
   file(APPEND ${LAUNCHER_FILE} "#include<${next_ITEM}>\n" )
endforeach(next_ITEM ${SRC_FILES})

INSTALL(FILES  ${HEADER_FILES} DESTINATION ${INCLUDE_INSTALL_PREFIX}/ )
INSTALL(FILES  ${PYTHON_FILES} DESTINATION ${PYTHON_INSTALL_PREFIX}/widgets )
INSTALL(TARGETS ${LIB_NAME}    DESTINATION ${LIB_INSTALL_PREFIX} )

#-----------------------
#-----------------------

SET( ANTIOCH_INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/
            ${ANTIOCH_INCLUDE_DIRS}
    )

SET( ANTIOCH_LIBRARIES
                        ${LIB_NAME}
                        ${ANTIOCH_LIBRARIES}
    )
    

ELSEIF( NOT ANTIOCH )
   MESSAGE( STATUS "Does not include the Antioch interface for chemical kinetics resolution (-DANTIOCH=1 to activate)")
ENDIF( ANTIOCH )
