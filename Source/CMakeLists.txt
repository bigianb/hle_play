
cmake_minimum_required(VERSION 3.9)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../Play/deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(HlePlayUI)

add_definitions(-DPLAY_VERSION="${PROJECT_Version}")

set(USE_GSH_VULKAN OFF)

if(NOT TARGET PlayCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source
		${CMAKE_CURRENT_BINARY_DIR}/Source
	)
endif()
list(APPEND PROJECT_LIBS PlayCore)

if(NOT TARGET gsh_opengl)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source/gs/GSH_OpenGL
		${CMAKE_CURRENT_BINARY_DIR}/gs/GSH_OpenGL
	)
endif()
list(INSERT PROJECT_LIBS 0 gsh_opengl)


if(TARGET_PLATFORM_UNIX)
	find_package(LIBEVDEV REQUIRED)
	list(APPEND PROJECT_LIBS ${LIBEVDEV_LIBRARY})
	list(APPEND DEFINITIONS_LIST HAS_LIBEVDEV=1)

	list(APPEND PROJECT_LIBS "-static-libgcc")
	list(APPEND PROJECT_LIBS "-static-libstdc++")

	if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
		# most unix system seem to implicitly link to libinotify, but not FreeBSD
		list(APPEND PROJECT_LIBS "libinotify.so")
	endif()
endif()

if(TARGET_PLATFORM_MACOS OR TARGET_PLATFORM_UNIX)
	if(NOT TARGET sh_openal)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../Play/tools/PsfPlayer/Source/SH_OpenAL
			${CMAKE_CURRENT_BINARY_DIR}/SH_OpenAL
		)
	endif()
	list(APPEND PROJECT_LIBS sh_openal)
endif()

if(NOT TARGET ui_shared)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source/ui_shared
		${CMAKE_CURRENT_BINARY_DIR}/ui_shared
	)
	endif()
list(APPEND PROJECT_LIBS ui_shared)

set(QT_SOURCES
	../Play/Source/ui_qt/bootablelistdialog.cpp
	../Play/Source/ui_qt/bootablelistdialog.h
	../Play/Source/ui_qt/BootableModel.cpp
	../Play/Source/ui_qt/BootableModel.h
	../Play/Source/ui_qt/ControllerConfig/controllerconfigdialog.cpp
	../Play/Source/ui_qt/ControllerConfig/controllerconfigdialog.h
	../Play/Source/ui_qt/ControllerConfig/inputbindingmodel.cpp
	../Play/Source/ui_qt/ControllerConfig/inputbindingmodel.h
	../Play/Source/ui_qt/ControllerConfig/inputeventselectiondialog.cpp
	../Play/Source/ui_qt/ControllerConfig/inputeventselectiondialog.h
	../Play/Source/ui_qt/CoverUtils.cpp
	../Play/Source/ui_qt/CoverUtils.h
	./ui_qt/main.cpp
	./ui_qt/mainwindow.cpp
	./ui_qt/hle_mainwindow.h
	../Play/Source/ui_qt/ContinuationChecker.cpp
	../Play/Source/ui_qt/ContinuationChecker.h
	../Play/Source/ui_qt/ElidedLabel.cpp
	../Play/Source/ui_qt/ElidedLabel.h
	../Play/Source/ui_qt/GSH_OpenGLQt.cpp
	../Play/Source/ui_qt/GSH_OpenGLQt.h
	../Play/Source/ui_qt/InputProviderQtKey.cpp
	../Play/Source/ui_qt/InputProviderQtKey.h
	../Play/Source/ui_qt/settingsdialog.cpp
	../Play/Source/ui_qt/settingsdialog.h
	../Play/Source/ui_qt/openglwindow.cpp
	../Play/Source/ui_qt/openglwindow.h
	../Play/Source/ui_qt/outputwindow.cpp
	../Play/Source/ui_qt/outputwindow.h
	../Play/Source/ui_qt/memorycardmanagerdialog.cpp
	../Play/Source/ui_qt/memorycardmanagerdialog.h
	../Play/Source/ui_qt/PreferenceDefs.h
	../Play/Source/ui_qt/QStringUtils.cpp
	../Play/Source/ui_qt/QStringUtils.h
	../Play/Source/ui_qt/QtUtils.cpp
	../Play/Source/ui_qt/QtUtils.h
	../Play/Source/ui_qt/vfsmanagerdialog.cpp
	../Play/Source/ui_qt/vfsmanagerdialog.h
	../Play/Source/ui_qt/vfsmodel.cpp
	../Play/Source/ui_qt/vfsmodel.h
	../Play/Source/ui_qt/vfsdiscselectordialog.cpp
	../Play/Source/ui_qt/vfsdiscselectordialog.h
	../Play/Source/ui_qt/VfsDevice.cpp
	../Play/Source/ui_qt/VfsDevice.h

)
if(ENABLE_AMAZON_S3)
	set(QT_SOURCES
		${QT_SOURCES}
		../Play/Source/ui_qt/S3FileBrowser.cpp
		../Play/Source/ui_qt/S3FileBrowser.h
	)
	set(AMAZON_S3_QT_MOC
		../Play/Source/ui_qt/S3FileBrowser.h
	)
endif()
if(LIBEVDEV_FOUND)
	set(QT_SOURCES
		${QT_SOURCES}
		../Play/Source/ui_qt/unix/GamePadDeviceListener.cpp
		../Play/Source/ui_qt/unix/GamePadDeviceListener.h
		../Play/Source/ui_qt/unix/GamePadInputEventListener.cpp
		../Play/Source/ui_qt/unix/GamePadInputEventListener.h
		../Play/Source/ui_qt/unix/GamePadUtils.cpp
		../Play/Source/ui_qt/unix/GamePadUtils.h
		../Play/Source/ui_qt/unix/InputProviderEvDev.cpp
		../Play/Source/ui_qt/unix/InputProviderEvDev.h
	)
endif()
if(TARGET_PLATFORM_WIN32)
	if(NOT TARGET Framework_Win32)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../Play/deps/Framework/build_cmake/FrameworkWin32
			${CMAKE_CURRENT_BINARY_DIR}/Framework_Win32
		)
	endif()
	list(APPEND PROJECT_LIBS Framework_Win32)
	
	set(QT_SOURCES
		${QT_SOURCES}
		../Play/tools/PsfPlayer/Source/win32_ui/SH_WaveOut.cpp
	)
	list(APPEND PROJECT_LIBS winmm)
	
	if(TARGET_PLATFORM_WIN32_X86)
		set(MANIFEST ../Play/Source/ui_qt/win32/Play-x86.manifest)
	else()
		set(MANIFEST ../Play/Source/ui_qt/win32/Play-x64.manifest)
	endif()

	set(QT_SOURCES
		${QT_SOURCES}
		${MANIFEST}
	)
	
	list(APPEND PROJECT_LIBS dinput8)
	list(APPEND PROJECT_LIBS xinput)
endif()
	
if(DEBUGGER_INCLUDED)
	if(NOT TARGET PlayDebugSupport)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source/ui_qt/DebugSupport
			${CMAKE_CURRENT_BINARY_DIR}/PlayDebugSupport
		)
	endif()
	list(APPEND PROJECT_LIBS PlayDebugSupport)
endif()
	
set(QT_MOC_HEADERS
	../Play/Source/ui_qt/bootablelistdialog.h
	../Play/Source/ui_qt/BootableModel.h
	../Play/Source/ui_qt/ControllerConfig/controllerconfigdialog.h
	../Play/Source/ui_qt/ControllerConfig/inputbindingmodel.h
	../Play/Source/ui_qt/ControllerConfig/inputeventselectiondialog.h
	../Play/Source/ui_qt/mainwindow.h
	../Play/Source/ui_qt/ContinuationChecker.h
	../Play/Source/ui_qt/ElidedLabel.h
	../Play/Source/ui_qt/settingsdialog.h
	../Play/Source/ui_qt/openglwindow.h
	../Play/Source/ui_qt/outputwindow.h
	../Play/Source/ui_qt/memorycardmanagerdialog.h
	../Play/Source/ui_qt/vfsmanagerdialog.h
	../Play/Source/ui_qt/vfsmodel.h
	../Play/Source/ui_qt/vfsdiscselectordialog.h
	${AMAZON_S3_QT_MOC}
	${GSH_VULKAN_QT_MOC}
)

set(QT_UIS
	../Play/Source/ui_qt/Qt_ui/bootablelistdialog.ui
	../Play/Source/ui_qt/Qt_ui/controllerconfigdialog.ui
	../Play/Source/ui_qt/Qt_ui/debugmenu.ui
	../Play/Source/ui_qt/Qt_ui/inputeventselectiondialog.ui
	../Play/Source/ui_qt/Qt_ui/mainwindow.ui
	../Play/Source/ui_qt/Qt_ui/memorycardmanager.ui
	../Play/Source/ui_qt/Qt_ui/s3filebrowser.ui
	../Play/Source/ui_qt/Qt_ui/settingsdialog.ui
	../Play/Source/ui_qt/Qt_ui/vfsmanagerdialog.ui
	../Play/Source/ui_qt/Qt_ui/vfsdiscselectordialog.ui
)

set(QT_RESOURCES
	../Play/Source/ui_qt/Qt_ui/resources.qrc
)

set(QT_COMPONENTS Widgets Core)
if(TARGET_PLATFORM_UNIX)
	list(APPEND QT_COMPONENTS X11Extras)
endif()
find_package(Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})

list(APPEND PROJECT_LIBS Qt5::Widgets Qt5::Core)
if(TARGET_PLATFORM_UNIX)
	list(APPEND PROJECT_LIBS Qt5::X11Extras)
endif()

add_definitions(${QT_DEFINITIONS})

QT5_ADD_RESOURCES(QT_RES_SOURCES ${QT_RESOURCES})
QT5_WRAP_UI(QT_UI_HEADERS ${QT_UIS})
QT5_WRAP_CPP(QT_MOC_SRCS ${QT_MOC_HEADERS})

if(TARGET_PLATFORM_MACOS)
	set(QT_SOURCES
		${QT_SOURCES}
		../Play/Source/ui_qt/macos/InputProviderMacOsHid.cpp
		../Play/Source/ui_qt/macos/InputProviderMacOsHid.h
	)
	set(OSX_RES
		${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source/ui_qt/macos/AppIcon.icns
		${CMAKE_CURRENT_SOURCE_DIR}/../../patches.xml
	)
	if(DEBUGGER_INCLUDED)
		list(APPEND OSX_RES ${CMAKE_CURRENT_SOURCE_DIR}/../Play/ee_functions.xml)
	endif()
	add_executable(Play MACOSX_BUNDLE ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS} ${OSX_RES})
	# Set a custom plist file for the app bundle
	# NOTE: for these values to be used Info.plist has to be edited
	# NOTE: from cmake 3.7.0 you can use %b for month name abbreviations
	string(TIMESTAMP DATE "%d-%m-%Y")
	set_target_properties(
		Play
		PROPERTIES
			MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}"
			MACOSX_BUNDLE_GUI_IDENTIFIER "com.virtualapplications.Play"
			MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${PROJECT_Version}"
			MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
			MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_Version}"
			MACOSX_BUNDLE_BUNDLE_VERSION ${DATE}
			MACOSX_BUNDLE_COPYRIGHT "© Virtual Applications, 2019"
			MACOSX_BUNDLE_ICON_FILE "AppIcon.icns"
			MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../Play/Source/ui_qt/macos/Info.plist.in"
		RESOURCE "${OSX_RES}"
	)
	list(APPEND PROJECT_LIBS "-framework IOKit -framework CoreFoundation")
elseif(TARGET_PLATFORM_WIN32)
	set(QT_SOURCES
		${QT_SOURCES}
		../Play/Source/ui_qt/win32/InputProviderDirectInput.cpp
		../Play/Source/ui_qt/win32/InputProviderDirectInput.h
		../Play/Source/ui_qt/win32/InputProviderXInput.cpp
		../Play/Source/ui_qt/win32/InputProviderXInput.h
	)
	add_executable(Play WIN32 ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS})
elseif(TARGET_PLATFORM_UNIX)
	add_executable(Play ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS})

	install(TARGETS Play DESTINATION bin RENAME Play PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../Play/icons/icon_base.png DESTINATION share/icons/hicolor/1024x1024/apps RENAME Play.png)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../Play/icons/icon.svg DESTINATION share/icons/hicolor/scalable/apps RENAME Play.svg)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../Play/installer_unix/Play.desktop DESTINATION share/applications)
else()
	add_executable(Play ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS})
endif()

target_link_libraries(Play ${PROJECT_LIBS})
target_compile_definitions(Play PRIVATE ${DEFINITIONS_LIST})

target_include_directories(Play PRIVATE
	../Play/Source/ui_qt
	../Play
	${LIBEVDEV_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

if(TARGET_PLATFORM_WIN32)
	find_program(WINDEPLOYQT_EXE windeployqt HINTS "${QT_BINARY_DIRECTORY}")
	add_custom_command(TARGET Play POST_BUILD
		COMMAND "${WINDEPLOYQT_EXE}" $<IF:$<CONFIG:Debug>,--debug,--release>
									 --no-translations
									 --no-compiler-runtime
									 "$<TARGET_FILE:Play>"
	)
endif()
