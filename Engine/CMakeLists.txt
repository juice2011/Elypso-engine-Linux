cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Engine files
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Externally added files
set(EXT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/include")
set(EXT_LIB_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/lib")
set(EXT_SRC_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/src")

# Manually added source file paths
set(SRC_CORE_DIR "${CMAKE_SOURCE_DIR}/src/core")
set(SRC_FILE_DIR "${CMAKE_SOURCE_DIR}/src/file")
set(SRC_GRAPHICS_DIR "${CMAKE_SOURCE_DIR}/src/graphics")
set(SRC_GRAPHICS_SHAPE_DIR "${CMAKE_SOURCE_DIR}/src/graphics/shapes")
set(SRC_PHYSICS_DIR "${CMAKE_SOURCE_DIR}/src/physics")
set(SRC_UTILS_DIR "${CMAKE_SOURCE_DIR}/src/utils")

# Add GLAD
add_library(glad STATIC ${EXT_SRC_DIR}/glad/glad.c)
target_include_directories(glad PUBLIC ${EXT_INCLUDE_DIR})

# Add GLFW
set(GLFW_LIBRARY_PATH "${EXT_LIB_DIR}/glfw/glfw3.lib")

# Add Assimp
set(ASSIMP_LIBRARY_PATH "${EXT_LIB_DIR}/assimp/assimp-vc143-mt.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_INCLUDE_DIR}/magic_enum)

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/main.cpp
	${SRC_CORE_DIR}/compile.cpp
	${SRC_CORE_DIR}/console.cpp
	${SRC_CORE_DIR}/core.cpp
	${SRC_CORE_DIR}/input.cpp
	${SRC_CORE_DIR}/timeManager.cpp
	${SRC_FILE_DIR}/configFile.cpp
	${SRC_FILE_DIR}/fileexplorer.cpp
	${SRC_FILE_DIR}/sceneFile.cpp
	${SRC_GRAPHICS_DIR}/gui/gui.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_assetlist.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_console.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_debugmenu.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_importAsset.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_inspector.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_node.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_nodeblock.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_nodecircle.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_nodeconnection.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/billboard.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/gameobject.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/model.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/modelchild.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/pointlight.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/selectedobjectaction.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/selectedobjectborder.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/spotlight.cpp
	${SRC_GRAPHICS_DIR}/grid.cpp
	${SRC_GRAPHICS_DIR}/render.cpp
	${SRC_GRAPHICS_DIR}/shader.cpp
	${SRC_GRAPHICS_DIR}/texture.cpp
	${SRC_PHYSICS_DIR}/collision.cpp
	${SRC_PHYSICS_DIR}/selectobject.cpp
	${SRC_UTILS_DIR}/browserUtils.cpp
	${SRC_UTILS_DIR}/fileUtils.cpp
	${SRC_UTILS_DIR}/stringUtils.cpp
)

# Add your external files
list(APPEND SOURCE_FILES
	${EXT_SRC_DIR}/glad/glad.c
	${EXT_SRC_DIR}/imgui/imgui.cpp
	${EXT_SRC_DIR}/imgui/imgui_demo.cpp
	${EXT_SRC_DIR}/imgui/imgui_draw.cpp
	${EXT_SRC_DIR}/imgui/imgui_impl_glfw.cpp
	${EXT_SRC_DIR}/imgui/imgui_impl_opengl3.cpp
	${EXT_SRC_DIR}/imgui/imgui_tables.cpp
	${EXT_SRC_DIR}/imgui/imgui_widgets.cpp
	${EXT_SRC_DIR}/stb_image/stb_image.cpp
)

# Add the executable
add_executable(Elypso_engine ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces the name to be Elypso engine
set_target_properties(Elypso_engine PROPERTIES OUTPUT_NAME "Elypso engine")

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine PRIVATE 
	${INCLUDE_DIR}/core
	${INCLUDE_DIR}/file
	${INCLUDE_DIR}/graphics
	${INCLUDE_DIR}/graphics/gui
	${INCLUDE_DIR}/graphics/shapes
	${INCLUDE_DIR}/physics
	${INCLUDE_DIR}/utils
	${EXT_INCLUDE_DIR}/assimp
	${EXT_INCLUDE_DIR}/glad
	${EXT_INCLUDE_DIR}/glfw
	${EXT_INCLUDE_DIR}/glm
	${EXT_INCLUDE_DIR}/glm/detail
	${EXT_INCLUDE_DIR}/glm/ext
	${EXT_INCLUDE_DIR}/glm/gtc
	${EXT_INCLUDE_DIR}/glm/gtx
	${EXT_INCLUDE_DIR}/glm/simd
	${EXT_INCLUDE_DIR}/magic_enum
	${EXT_INCLUDE_DIR}/imgui
	${EXT_INCLUDE_DIR}/stb_image
)

target_link_libraries(Elypso_engine PRIVATE external_libs glad ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_engine PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Elypso_engine PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#    set_target_properties(Elypso_engine PROPERTIES
#       LINK_FLAGS "/ENTRY:mainCRTStartup"
#    )
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Elypso_engine>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files 
	$<TARGET_FILE_DIR:Elypso_engine>/files
)

# Copy assimp dll to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Elypso_engine>/assimp-vc143-mt.dll
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/files/external\ dlls/assimp-vc143-mt.dll
    $<TARGET_FILE_DIR:Elypso_engine>/assimp-vc143-mt.dll
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
)

# Include CPack to enable packaging
include(CPack)