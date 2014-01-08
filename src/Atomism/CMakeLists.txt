cmake_minimum_required(VERSION 2.8)

message(STATUS "\n----------------------------------------------\n----------src/Atomism/CMakeLists.txt----------\n----------------------------------------------")


IF( ATOMISM )

MESSAGE( STATUS "Include the Atomism interface for molecular computations from potential energy surfaces (-DATOMISM=0 to desactivate). The 'math' package needs to be turned on too.")


set(LIB_NAME atomism)

#-------------------------------------------------------------------------------------------------
#------------------ include directories of Base and set source files-----------------------------
#-------------------------------------------------------------------------------------------------

message(STATUS "Base include directories: ${IMPACT_BASE_INCLUDE_DIRS}")
message(STATUS "Base libraries: ${IMPACT_BASE_LIBRARIES}")
message(STATUS "math include directories: ${MATH_INCLUDE_DIRS}")
message(STATUS "math libraries: ${MATH_LIBRARIES}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake_FindModules )

IF( VTK )
 FIND_PACKAGE(VTK 6.0 REQUIRED COMPONENTS vtkRenderingOpenGL vtkRenderingFreeType  vtkRenderingFreeTypeOpenGL vtkInteractionStyle vtkDomainsChemistry NO_MODULE)
 #FIND_PACKAGE(VTK 6.0 REQUIRED )
 INCLUDE(${VTK_USE_FILE})
message(STATUS "VTK include directories: ${VTK_INCLUDE_DIRS}")
message(STATUS "VTK libraries: ${VTK_LIBRARIES}")
ENDIF( VTK )

FIND_PACKAGE(GSL REQUIRED)

#-----------------------
#-----------------------

FILE(GLOB HEADER_FILES  
                        ${CMAKE_CURRENT_SOURCE_DIR}/AnalyticMechanics/*.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Molecule/*.h
    )

FILE(GLOB  SRC_FILES
                        ${CMAKE_CURRENT_SOURCE_DIR}/AnalyticMechanics/*.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Molecule/*.cpp

       )

FILE(GLOB  PYTHON_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/AnalyticMechanics/Widgets/*.py
            ${CMAKE_CURRENT_SOURCE_DIR}/Molecule/Widgets/*.py
       )
#-----------------------
#-----------------------

SET( INCLUDE_DIRS
                        ${CMAKE_CURRENT_SOURCE_DIR}/AnalyticMechanics
                        ${CMAKE_CURRENT_SOURCE_DIR}/Molecule
            ${IMPACT_BASE_INCLUDE_DIRS}
            ${MATH_INCLUDE_DIRS}
            ${GSL_INCLUDE_DIRS}
            ${VTK_INCLUDE_DIRS}
    )
SET( DEPENDANCIES
                       ${IMPACT_BASE_LIBRARIES}
                       ${MATH_LIBRARIES}
                       ${GSL_LIBRARIES}
                       ${VTK_LIBRARIES}
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
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Molecule/elements_ODB.xml  ${PYTHON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )
INSTALL(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/lib )
INSTALL(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/Impact/bin )

#-----------------------
#-----------------------

SET( ATOMISM_INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/AnalyticMechanics
            ${CMAKE_CURRENT_SOURCE_DIR}/Molecule
            ${GSL_INCLUDE_DIRS}
            ${VTK_INCLUDE_DIRS}
    )

SET( ATOMISM_LIBRARIES
            ${LIB_NAME}
            ${GSL_LIBRARIES}
            ${VTK_LIBRARIES}
    )
ELSEIF( NOT ATOMISM )
   MESSAGE( STATUS "Does not include the Cantera interface for molecular computations from potential energy surfaces (-DATOMISM=1 to activate)")
ENDIF( ATOMISM )