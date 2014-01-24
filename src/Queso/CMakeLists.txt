cmake_minimum_required(VERSION 2.8)

message(STATUS "\n----------------------------------------------\n----------src/Queso/CMakeLists.txt----------\n----------------------------------------------")

IF( QUESO )
   MESSAGE( STATUS "Include the QUESO interface for uncertainties estimation and inverse problems resolution, queso library needed for linking (-DQUESO=0 to desactivate)")

#-------------------------------------------------------------------------------------------------
#------------------ include directories of Base and set source files-----------------------------
#-------------------------------------------------------------------------------------------------
set(LIB_NAME impact_queso)

message(STATUS "impact base include directories: ${BASE_INCLUDE_DIRS}")
message(STATUS "impact base libraries: ${BASE_LIBRARIES}")

message(STATUS "math include directories: ${MATH_INCLUDE_DIRS}")
message(STATUS "math libraries: ${MATH_LIBRARIES}")

#-----------------------
#-----------------------

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake_FindModules )

# -----  QUESO

find_package(Queso REQUIRED )

find_package( MPI REQUIRED )
message(STATUS "MPI include paths: ${MPI_INCLUDE_PATH}")
message(STATUS "MPI libraries: ${MPI_LIBRARIES}")

find_package( GSL REQUIRED )
message(STATUS "GSL include paths: ${GSL_INCLUDE_DIRS}")
message(STATUS "GSL libraries: ${GSL_LIBRARIES}")

find_package( HDF5 REQUIRED )
message(STATUS "HDF5 include paths: ${HDF5_INCLUDE_DIRS}")
message(STATUS "HDF5 libraries: ${HDF5_LIBRARIES}")


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
                        ${PECOSQUESO_INCLUDE_DIRS}
                        ${MATH_INCLUDE_DIRS}
                        ${MPI_INCLUDE_PATH}
                        ${GSL_INCLUDE_DIRS}
                        ${HDF5_INCLUDE_DIRS}
        )
SET( DEPENDANCIES
                        ${BASE_LIBRARIES}
                        ${PECOSQUESO_LIBRARIES}
                        ${MATH_LIBRARIES}
                        ${GSL_LIBRARIES}
                        ${MPI_LIBRARIES}
                        ${HDF5_LIBRARIES}
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

INSTALL(FILES   ${HEADER_FILES} DESTINATION ${INCLUDE_INSTALL_PREFIX} )
INSTALL(FILES   ${PYTHON_FILES} DESTINATION ${PYTHON_INSTALL_PREFIX}/widgets )
INSTALL(TARGETS ${LIB_NAME}     DESTINATION ${LIB_INSTALL_PREFIX} )

#-----------------------
#-----------------------

SET( QUESO_INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/
                        ${INCLUDE_DIRS}
    )

SET( QUESO_LIBRARIES
                        ${LIB_NAME}
                        ${DEPENDANCIES}
    )
    

ELSEIF( NOT QUESO )
  MESSAGE( STATUS "Does not include the QUESO interface for uncertainties estimation and inverse problems resolution (-DQUESO=1 to activate)")
ENDIF( QUESO )