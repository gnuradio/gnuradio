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
# GNU Radio version: ${config.version}
#####################

<%
class_name = flow_graph.get_option('id')
version_list = config.version_parts
short_version = '.'.join(version_list[0:2])
%>\

cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)

project(${class_name})

find_package(Gnuradio "${short_version}" COMPONENTS
    % for component in config.enabled_components.split(";"):
    % if component.startswith("gr-"):
    % if not component in ['gr-utils', 'gr-ctrlport']:
    ${component.replace("gr-", "")}
    % endif
    % endif
    % endfor
)

% if generate_options == 'qt_gui':
find_package(Qt5Widgets REQUIRED)
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

% for package in packages:
% if package:
find_package(${package})
% endif
% endfor
add_executable(${class_name} ${class_name}.cpp)
target_link_libraries(${class_name}
    gnuradio::gnuradio-blocks
    % if generate_options == 'qt_gui':
    gnuradio::gnuradio-qtgui
    % endif
    % for link in links:
    % if link:
    ${link}
    % endif
    % endfor

)

