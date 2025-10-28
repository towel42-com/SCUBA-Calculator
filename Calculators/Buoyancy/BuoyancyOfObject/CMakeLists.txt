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

get_filename_component( _PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )

get_filename_component( _GROUP_NAME ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( _GROUP_NAME ${_GROUP_NAME} NAME )
project( ${_PROJECT_NAME} )


include( include.cmake )
include( ${CMAKE_SOURCE_DIR}/T42-CMakeUtils/QtProject.cmake )
find_package(Qt6 COMPONENTS Svg REQUIRED)
add_library(${PROJECT_NAME} SHARED
    ${_PROJECT_DEPENDENCIES} 
)

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Calculators )

target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE CALCULATORS_INTERNAL)

AddToCalculatorList( "${PROJECT_NAME}" "${_GROUP_NAME}" )