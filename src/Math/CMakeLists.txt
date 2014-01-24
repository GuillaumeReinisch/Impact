
cmake_minimum_required(VERSION 2.8)
message(STATUS "\n----------------------------------------------\n----------src/Math/CMakeLists.txt----------\n----------------------------------------------")

IF( MATH )

MESSAGE( STATUS "Include the MATH interface (-DMATH=0 to desactivate)")


set(LIB_NAME impact_math)

#-------------------------------------------------------------------------------------------------
#------------------ include directories of Base and set source files-----------------------------
#-------------------------------------------------------------------------------------------------


message(STATUS "impact base include directories: ${BASE_INCLUDE_DIRS}")
message(STATUS "impact base libraries: ${BASE_LIBRARIES}")
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
        )
SET( DEPENDANCIES
            ${BASE_LIBRARIES}
    )

#-------------------------------------------------------------------------------------------------
#------------------ compile library base --------------------------------------------------------
#-------------------------------------------------------------------------------------------------

add_library ( ${LIB_NAME}  ${SRC_FILES} )

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

SET( MATH_INCLUDE_DIRS
                        ${CMAKE_CURRENT_SOURCE_DIR}/
    )

SET( MATH_LIBRARIES ${LIB_NAME}
    )
    
ELSEIF( NOT MATH )
   MESSAGE( STATUS "Does not include the MATH interface (-DMATH=1 to activate)")
ENDIF( MATH )
