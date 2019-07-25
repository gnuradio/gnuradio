#####################
# GNU Radio C++ Flow Graph CMakeLists.txt
#
# Title: ${title}
% if flow_graph.get_option('author'):
# Author: ${flow_graph.get_option('author')}
% endif
% if flow_graph.get_option('description'):
# Description: ${flow_graph.get_option('description')}
% endif
# GNU Radio version: ${version}
#####################

<%
class_name = flow_graph.get_option('id')
%>\

cmake_minimum_required(VERSION 3.8)

% if generate_options == 'qt_gui':
find_package(Qt5Widgets REQUIRED)
% endif

include_directories(
    ${'$'}{GNURADIO_ALL_INCLUDE_DIRS}
    ${'$'}{Boost_INCLUDE_DIRS}
    % if generate_options == 'qt_gui':
    ${'$'}{Qt5Widgets_INCLUDES}
    % endif
    $ENV{HOME}/.grc_gnuradio
)

% if generate_options == 'qt_gui':
add_definitions(${'$'}{Qt5Widgets_DEFINITIONS})

set(CMAKE_AUTOMOC TRUE)
% endif

% if cmake_tuples:
% for key, val in cmake_tuples:
set(${key} ${val})
% endfor
% endif

% if flow_graph.get_option('gen_linking') == 'static':
set(BUILD_SHARED_LIBS false)
set(CMAKE_EXE_LINKER_FLAGS " -static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
% endif

set(GR_LIBRARIES
    boost_system
    % if parameters:
    boost_program_options
    % endif
    gnuradio-blocks
    gnuradio-runtime
    gnuradio-pmt
    log4cpp
    % for link in links:
    % if link:
    ${link}
    % endif
    % endfor
)

add_executable(${class_name} ${class_name}.cpp)
target_link_libraries(${class_name} ${'$'}{GR_LIBRARIES})
