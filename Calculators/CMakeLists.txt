# The MIT License (MIT)
#
# Copyright (c) 2020 Scott Aron Bloom
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


function(AddFilesToCalculatorQRC calculator globString baseName )
   file( GLOB 
        _FILES  
        ${globString}
    )
    
    if ( NOT _FILES OR ( "${_FILES}" STREQUAL "" ) )
        return()
    endif()

    foreach( file ${_FILES} )
        string(REGEX MATCH "${baseName}([^./]*)\.(.*)" MATCHIT "${file}")
        
        if ( NOT "${CMAKE_MATCH_1}" STREQUAL "" )
            SET( _FN_SUFFIX ${CMAKE_MATCH_1} )
        endif()
        
        SET( _ALIAS ./${PROJECT_NAME}${_FN_SUFFIX}.${CMAKE_MATCH_2} )
        #MESSAGE( STATUS "File: ${file} Alias: ${_ALIAS}" )
        set_source_files_properties( 
            ${file}
            PROPERTIES
                QT_RESOURCE_ALIAS ${_ALIAS}
        )
    endforeach()

    qt6_add_resources(${PROJECT_NAME} "${baseName}"
        PREFIX
            "/${baseName}"
        FILES
            ${_FILES}
        OUTPUT_TARGETS 
            QRC_OTHER_TARGETS    
    )
    
    target_sources( ${calculator} PUBLIC ${_FILES} )
endfunction()

function(AddToCalculatorList calculator groupName)
    set( GROUP_NAME ${groupName} )
    set( ROOT_TARGET "SCUBA-Calculator" )
    set( CALC_DIR "Calculators" )
    SET( DEST_DIR "$<TARGET_FILE_DIR:${ROOT_TARGET}>/${CALC_DIR}" )

    if ( TARGET ${calculator} )
        get_property(_CURR GLOBAL PROPERTY TOWEL42_CALCULATOR_LIST)
        list(APPEND _CURR "${calculator}")
        set_property(GLOBAL PROPERTY TOWEL42_CALCULATOR_LIST "${_CURR}")

        INSTALL( TARGETS ${calculator} RUNTIME DESTINATION Calculators )
        INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${calculator}.pdb DESTINATION Calculators CONFIGURATIONS Debug RelWithDebInfo )

        set_target_properties( ${calculator} PROPERTIES FOLDER Libs/Calculators/${GROUP_NAME} )
        set_target_properties( ${calculator} PROPERTIES DEBUG_POSTFIX d )

        SET( DLL_NAME "$<TARGET_FILE_NAME:${calculator}>" )
        SET( DLL_SRC_PATH "$<TARGET_FILE:${calculator}>" )
        SET( DLL_DEST_PATH "${DEST_DIR}/${DLL_NAME}" )
    
        SET( PDB_NAME "$<TARGET_FILE_NAME:${calculator}>" )
        SET( PDB_SRC_PATH "$<TARGET_PDB_FILE:${calculator}>" )
        SET( PDB_DEST_PATH "${DEST_DIR}/$<TARGET_FILE_BASE_NAME:${calculator}>.pdb" )
    
        configure_file(
            "${CMAKE_SOURCE_DIR}/Calculators/CalculatorDef.h.in"
            "${CMAKE_CURRENT_BINARY_DIR}/CalculatorDef.h"
            @ONLY
        )

        add_custom_command(TARGET ${calculator} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory \"${DEST_DIR}\"
            COMMAND "${CMAKE_COMMAND}" -E echo "Linking Target Library Library '${DLL_NAME}' for '${ROOT_TARGET}'"
            COMMAND "${CMAKE_COMMAND}" -E create_symlink "${DLL_SRC_PATH}" \"${DLL_DEST_PATH}\"
            COMMAND "${CMAKE_COMMAND}" -E echo "Linking Target PDB File '${PDB_SRC_PATH}' for '${ROOT_TARGET}'"
            COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PDB_SRC_PATH}" \"${PDB_DEST_PATH}\"
        )
    
        AddFilesToCalculatorQRC( ${calculator} ${CMAKE_CURRENT_SOURCE_DIR}/formulas/*formulas*.json formulas)
        AddFilesToCalculatorQRC( ${calculator} ${CMAKE_CURRENT_SOURCE_DIR}/calculator.json calculator)
    endif()
    
    if ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/calculator.json )
        set( _JSON_CALC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/calculator.json )

        INSTALL( FILES ${_JSON_CALC_FILE} DESTINATION ${CALC_DIR} RENAME ${calculator}.json )
        
        get_property(_CURR GLOBAL PROPERTY TOWEL42_JSON_CALCULATOR_LIST)
        list(APPEND _CURR ${_JSON_CALC_FILE} )
        list(APPEND _CURR ${calculator} )
        set_property(GLOBAL PROPERTY TOWEL42_JSON_CALCULATOR_LIST "${_CURR}")

        if ( TARGET ${calculator} )
            add_custom_command(TARGET ${calculator} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E make_directory \"${DEST_DIR}\"
                COMMAND "${CMAKE_COMMAND}" -E echo "Linking Target JSON Calculator '${_JSON_CALC_FILE}' for '${calculator}'"
                COMMAND "${CMAKE_COMMAND}" -E create_symlink "${_JSON_CALC_FILE}" \"${DEST_DIR}/${calculator}.json\"
            )
        endif()
    endif()
endfunction()

add_subdirectory( Core )

add_subdirectory( Buoyancy )
add_subdirectory( Miscellaneous )
add_subdirectory( PartialPressure )
add_subdirectory( PressureDepth )
add_subdirectory( PressureVolume )
add_subdirectory( PressureTemperatureVolume )
add_subdirectory( UnitConversion )
