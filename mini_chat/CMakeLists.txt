cmake_minimum_required(VERSION 3.5)

project(mini_chat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mini_chat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
        logindialog.h logindialog.cpp logindialog.ui
        registerdialog.h registerdialog.cpp registerdialog.ui
        global.h global.cpp
        singleton.h
        httpmgr.h httpmgr.cpp
        config.ini
        timerbtn.h timerbtn.cpp
        clickedlabel.h clickedlabel.cpp
        resetdialog.h resetdialog.cpp resetdialog.ui
        tcpmgr.h tcpmgr.cpp
        usermgr.h usermgr.cpp
        chatdialog.h chatdialog.cpp chatdialog.ui
        clickedbtn.h clickedbtn.cpp
        customizeedit.h customizeedit.cpp
        chatuserlist.h chatuserlist.cpp
        ChatUserWid.h ChatUserWid.cpp ChatUserWid.ui
        listitembase.h listitembase.cpp
        loadingdialog.h loadingdialog.cpp loadingdialog.ui
        chatpage.h chatpage.cpp chatpage.ui
        chatview.h chatview.cpp
        chatitembase.h chatitembase.cpp
        bubbleframe.h bubbleframe.cpp
        textbubble.h textbubble.cpp
        picturebubble.h picturebubble.cpp
        messagetextedit.h messagetextedit.cpp
        statewidget.h statewidget.cpp
        searchlist.h searchlist.cpp
        userdata.h userdata.cpp
        adduseritem.h adduseritem.cpp adduseritem.ui
        findsuccessdlg.h findsuccessdlg.cpp findsuccessdlg.ui
        applyfrienddlg.h applyfrienddlg.cpp applyfrienddlg.ui
        clickedoneclabel.h clickedoneclabel.cpp
        friendlabel.h friendlabel.cpp friendlabel.ui
        contactuserlist.h contactuserlist.cpp


        contactuseritem.h contactuseritem.cpp contactuseritem.ui
        grouptipitem.h grouptipitem.cpp grouptipitem.ui





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET mini_chat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(mini_chat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(mini_chat
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(mini_chat PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(mini_chat PRIVATE Qt${QT_VERSION_MAJOR}::Network)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.mini_chat)
endif()
set_target_properties(mini_chat PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS mini_chat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mini_chat)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TARGET_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/config.ini")
string(REPLACE "/" "\\\\" TARGET_CONFIG "${TARGET_CONFIG}")

set(OUTPUT_DIR "${CMAKE_BINARY_DIR}")

add_custom_command(TARGET mini_chat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${TARGET_CONFIG}"
    "${OUTPUT_DIR}/config.ini"
)


# 定义源文件夹路径
set(MY_SOURCE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/static")

# 定义目标路径
set(TARGET_OUTPUT_DIR "${CMAKE_BINARY_DIR}/static")


# 添加自定义命令
add_custom_command(TARGET mini_chat POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MY_SOURCE_FOLDER}" "${TARGET_OUTPUT_DIR}"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
