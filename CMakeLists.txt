cmake_minimum_required(VERSION 3.14)
if(NOT DEFINED PRISM_VERSION)
    file(READ "${CMAKE_CURRENT_LIST_DIR}/vcpkg.json" _contents)
    string(JSON PRISM_VERSION GET "${_contents}" version)
endif()

project(prism_qt_core VERSION ${PRISM_VERSION} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets Quick Qml QuickControls2 REQUIRED)
find_package(prism COMPONENTS qt::modular container)


include_directories(${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS})
include_directories(${Qt${QT_VERSION_MAJOR}Core_PRIVATE_INCLUDE_DIRS})

option(VCPKG_DISABLE_TEST "disable run test when vcpkg install " OFF)

add_library(${PROJECT_NAME} SHARED
    include/prism/qt/core/hpp/prismQt.hpp
    include/prism/qt/core/hpp/prismQtJson.hpp
    include/prism/qt/core/hpp/prismModelListProxy.hpp
    include/prism/qt/core/hpp/prismModelProxy.hpp
    include/prism/qt/core/hpp/prismTreeModelProxy.hpp
    include/prism/qt/core/hpp/prismTreeNodeProxy.hpp

    include/prism/qt/core/helper/imagePaintedItem.h
    include/prism/qt/core/helper/imagepainteditemqsg.h
    include/prism/qt/core/helper/sortfilterproxymodel.h
    include/prism/qt/core/helper/qeventloopguard.h
    include/prism/qt/core/helper/condition_varaiable_guard.h

    include/prism/qt/core/prismQt_core_global.h
    include/prism/qt/core/prismQt_events.h


    src/helper/qeventloopguard.cpp
    src/helper/condition_varaiable_guard.cpp
    src/helper/bind.h src/helper/bind.cpp
    src/helper/imagePaintedItem.cpp
    src/helper/imagepainteditemqsg.cpp
    src/helper/sortfilterproxymodel.cpp

    src/prismQt_core.h
    src/prismQt_core.cpp
    src/prismQt_core.qrc
)
add_library(prism::qt::core ALIAS ${PROJECT_NAME})


target_compile_definitions(${PROJECT_NAME} PRIVATE PRISM_QT_CORE_LIBRARY)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)



target_link_libraries(${PROJECT_NAME} PRIVATE
    prism::prism
    prism::qt::modular
    prism::container
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::QuickControls2)


#
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        #NAMESPACE prism::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# vcpkg
if(EXISTS ${VCPKG_ROOT})
    execute_process( COMMAND git rev-parse --abbrev-ref HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE GIT_BRANCH_RESULT )
    execute_process( COMMAND git rev-parse HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REPLACE "_" "-" PORT_NAME "${PROJECT_NAME}")
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg.json.in ${VCPKG_ROOT}/ports/${PORT_NAME}/vcpkg.json @ONLY NEWLINE_STYLE LF)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg.json.in ${PROJECT_SOURCE_DIR}/vcpkg.json @ONLY NEWLINE_STYLE LF)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/vcpkg_cus_info.json.in ${VCPKG_ROOT}/ports/${PORT_NAME}/vcpkg_cus_info.json @ONLY NEWLINE_STYLE LF)
    file(COPY ${PROJECT_SOURCE_DIR}/cmake/portfile.cmake DESTINATION ${VCPKG_ROOT}/ports/${PORT_NAME})
endif()

#

if(CMAKE_PROJECT_NAME  STREQUAL "${PROJECT_NAME}"  AND NOT VCPKG_DISABLE_TEST)
    find_package(Catch2 REQUIRED)
    include(CTest)
    include(Catch)

    #test prismQt json
    set(TST_PRISMQTJSON_SOURCE
        tests/test_prismQtJson.cpp
        tests/models/test_model.h)
    add_executable(tst_${PROJECT_NAME}  ${TST_PRISMQTJSON_SOURCE} )
    target_include_directories(tst_${PROJECT_NAME} PRIVATE  include)
    target_include_directories(tst_${PROJECT_NAME} PRIVATE ${prism_INCLUDE_DIRS})

    target_link_libraries(tst_${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}
        Catch2::Catch2
        prism::prism
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::QuickControls2)


    #catch_discover_tests(tst_${PROJECT_NAME})


    add_custom_target(run_tst_${PROJECT_NAME}
        COMMAND ${CMAKE_CTEST_COMMAND}  --verbose
        DEPENDS tst_${PROJECT_NAME}
    )
endif()


#execute_process(COMMAND ${CMAKE_COMMAND} -E echo "
#----------
#----------" )

