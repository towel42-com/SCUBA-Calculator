# The MIT License (MIT)
#
# Copyright (c) 2025 Scott Aron Bloom
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.31)

project(Root-SCUBA-Calculator VERSION 1.0)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/T42-CMakeUtils/Modules ${CMAKE_MODULE_PATH})
SET(SAB_ENABLE_TESTING ON)

set( TOWEL42_NO_DEPLOY_SUBMODULES ON )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)


find_package(Qt6 COMPONENTS Core Widgets Test Gui SvgWidgets WebEngineWidgets REQUIRED)
find_package(Deploy REQUIRED)
find_package(AddUnitTest REQUIRED)
find_package(Qt6SrcMoc REQUIRED)
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

include( VersionInfo.cmake )

file( REAL_PATH ~/bin/SCUBA-Calculator CMAKE_INSTALL_PREFIX EXPAND_TILDE)

function(AddToCalculatorList calculator)
    get_property(_CURR GLOBAL PROPERTY TOWEL42_CALCULATOR_LIST)
    list(APPEND _CURR "${calculator}")
    set_property(GLOBAL PROPERTY TOWEL42_CALCULATOR_LIST "${_CURR}")
    
    INSTALL( TARGETS ${calculator} RUNTIME DESTINATION Calculators )
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${calculator}.pdb DESTINATION Calculators CONFIGURATIONS Debug RelWithDebInfo )

    set( ROOT_TARGET "SCUBA-Calculator" )
    set( CALC_DIR "Calculators" )
    SET( DEST_DIR "$<TARGET_FILE_DIR:${ROOT_TARGET}>/${CALC_DIR}" )
    SET( DLL_NAME "$<TARGET_FILE_NAME:${calculator}>" )
    SET( DLL_SRC_PATH "$<TARGET_FILE:${calculator}>" )
    SET( DLL_DEST_PATH "${DEST_DIR}/${DLL_NAME}" )
    
    SET( PDB_NAME "$<TARGET_FILE_NAME:${calculator}>" )
    SET( PDB_SRC_PATH "$<TARGET_PDB_FILE:${calculator}>" )
    SET( PDB_DEST_PATH "${DEST_DIR}/$<TARGET_FILE_BASE_NAME:${calculator}>.pdb" )
    
    add_custom_command(TARGET ${calculator} POST_BUILD
#        COMMAND "${CMAKE_COMMAND}" -E echo "Making Directory '${DEST_DIR}' for '${ROOT_TARGET}'"
        COMMAND "${CMAKE_COMMAND}" -E make_directory \"${DEST_DIR}\"
        COMMAND "${CMAKE_COMMAND}" -E echo "Linking Target Library Library '${DLL_NAME}' for '${ROOT_TARGET}'"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${DLL_SRC_PATH}" \"${DLL_DEST_PATH}\"
        COMMAND "${CMAKE_COMMAND}" -E echo "Linking Target PDB File '${PDB_SRC_PATH}' for '${ROOT_TARGET}'"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PDB_SRC_PATH}" \"${PDB_DEST_PATH}\"
    )
    
    set_source_files_properties( 
        ${CMAKE_CURRENT_SOURCE_DIR}/formulas.json 
        PROPERTIES
            QT_RESOURCE_ALIAS ./${PROJECT_NAME}.json
    )
    qt6_add_resources(${PROJECT_NAME} "calculator"
        PREFIX
            "/formulas"
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/formulas.json
    )

endfunction()

add_subdirectory( T42-MathJaxQt6  )
add_subdirectory( SABUtils )
add_subdirectory( MainWindow )

SET_PROPERTY(GLOBAL PROPERTY TOWEL42_CALCULATOR_LIST "")

add_subdirectory( Calculators )
add_subdirectory( main )

SET( CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION} )
SET( CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION} )
SET( CPACK_PACKAGE_VERSION_PATCH ${VERSION_FILE_PATCH_VERSION} )
SET( CPACK_PACKAGE_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${VERSION_FILE_PATCH_VERSION})

SET( CPACK_PACKAGE_NAME ${APP_NAME} )
SET( CPACK_PACKAGE_VENDOR "${VENDOR} ${EMAIL}" )
SET( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE )
SET( CPACK_PACKAGE_HOMEPAGE_URL ${HOMEPAGE} )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${VENDOR}/${APP_NAME}" )
SET( CPACK_PACKAGE_DESCRIPTION "Application to find duplicate files" )

SET(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SCUBA-Calculator.lnk' '$INSTDIR\\\\SCUBA-Calculator.exe'"
)

SET(CPACK_NSIS_MODIFY_PATH "ON")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut \\\"$DESKTOP\\\\${APP_NAME}.lnk\\\" \\\"$INSTDIR\\\\SCUBA-Calculator.exe\\\"
")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \\\"$DESKTOP\\\\${APP_NAME}.lnk\\\"
")

SET( CPACK_GENERATOR NSIS ZIP )
SET( CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/MainWindow/resources/application.ico )
SET( CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/MainWindow/resources/application.ico )
include( CPack )
