# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (3dspacegame)
add_definitions(-DCMAKE_GENERATOR_PLATFORM=x64)


find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
    include/
	.
)

# Compile external dependencies 
add_subdirectory (external)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

if(APPLE)
# 3dspacegame
add_executable(3dspacegame
        src/main.cpp
        src/gfx_animation.cpp
        src/gfx_skinning.cpp
        src/gfx_camera.cpp
        src/gfx_shader.cpp
        src/gfx_texture.cpp
        src/gfx_sevbo.cpp
        src/gfx_window.cpp
        src/game_brick_type.cpp
        src/game_celestial.cpp
        src/game_construct.cpp
        src/nv_dds.cpp
        macos/timer.cpp
		macos/timer.h
        include/engine/gfx_animation.h
        include/engine/gfx_skinning.h
        include/engine/gfx_window.h
        include/engine/gfx_primitives.h
        include/engine/gfx_camera.h
        include/engine/gfx_sevbo.h
        include/engine/gfx_shader.h
        include/engine/gfx_texture.h
        include/engine/game_brick_type.h
        include/engine/game_construct.h
        include/engine/game_celestrial.h
        include/engine/nv_dds.h
)
target_link_libraries(3dspacegame
        ${ALL_LIBS}
)

elseif(WIN32)

#nut2mdl
ADD_EXECUTABLE(nut2mdl
    tools/src/animreader.cpp
    tools/src/animreader.h
    tools/src/log.h
    tools/src/main.cpp
    tools/src/mdl.cpp
    tools/src/mdl.hpp
    tools/src/mdlwriter.cpp
    tools/src/mdlwriter.hpp
    src/mmh3.cpp
    include/engine/mmh3.h
    tools/src/osfuncs.h
    tools/src/pugiconfig.hpp
    tools/src/pugixml.cpp
    tools/src/pugixml.hpp
    include/engine/data_file.h
    include/engine/data_archive.h
    src/data_file.cpp
    src/data_archive.cpp
    src/game_brick_type.cpp
    src/game_celestial.cpp
    src/game_construct.cpp
        
    src/gfx_object.cpp
    src/gfx_texture.cpp
	src/gfx_sevbo.cpp
    src/gfx_animation.cpp
    src/gfx_skinning.cpp
    include/3dspacegame/game_brick_type.h
    include/3dspacegame/game_celestial.h
    include/3dspacegame/game_construct.h
    include/engine/gfx_animation.h
    include/engine/gfx_skinning.h
    
    include/engine/gfx_object.h
    include/engine/gfx_texture.h
    include/engine/gfx_sevbo.h
    include/engine/gfx_log.h
    src/nv_dds.cpp
    include/engine/nv_dds.h
)
target_link_libraries(nut2mdl
	${ALL_LIBS}
)

# archiver
add_executable(archiver
	src/archiver.cpp
    src/data_file.cpp
    src/data_archive.cpp
    src/mmh3.cpp
    src/gfx_reader.cpp
    src/gfx_object.cpp
    src/gfx_primitives.cpp
    src/gfx_camera.cpp
	src/gfx_shader.cpp
	src/gfx_texture.cpp
	src/gfx_sevbo.cpp
    src/gfx_window.cpp
	src/nv_dds.cpp
	win32/timer.cpp
    win32/timer.h
    include/engine/data_file.h
    include/engine/data_archive.h
    include/engine/mmh3.h
    include/engine/gfx_reader.h
    include/engine/gfx_object.h
    include/engine/gfx_window.h
    include/engine/gfx_camera.h
    include/engine/gfx_primitives.h
    include/engine/gfx_sevbo.h
    include/engine/gfx_shader.h
    include/engine/gfx_texture.h
    include/engine/gfx_log.h
    include/engine/nv_dds.h
    shaders/pos_col.fs
    shaders/pos_col.vs
    shaders/pos_norm_uv.fs
    shaders/pos_norm_uv.vs
    shaders/pos_uv.fs
    shaders/pos_uv.vs
)
target_link_libraries(archiver
	${ALL_LIBS}
)

# 3dspacegame
add_executable(3dspacegame
	src/main.cpp
    src/data_file.cpp
    src/data_archive.cpp
    src/mmh3.cpp
    src/gfx_animation.cpp
    src/gfx_skinning.cpp
    src/gfx_reader.cpp
    src/gfx_object.cpp
    src/gfx_primitives.cpp
    src/gfx_camera.cpp
	src/gfx_shader.cpp
	src/gfx_texture.cpp
	src/gfx_sevbo.cpp
    src/gfx_window.cpp
    
    src/game_brick_type.cpp
    src/game_celestial.cpp
    src/game_construct.cpp
    src/game_brick_def.cpp
    
	src/nv_dds.cpp
	win32/timer.cpp
    win32/timer.h
    include/engine/data_file.h
    include/engine/data_archive.h
    include/engine/mmh3.h
    include/engine/gfx_animation.h
    include/engine/gfx_skinning.h
    include/engine/gfx_reader.h
    include/engine/gfx_object.h
    include/engine/gfx_window.h
    include/engine/gfx_camera.h
    include/engine/gfx_primitives.h
    include/engine/gfx_sevbo.h
    include/engine/gfx_shader.h
    include/engine/gfx_texture.h
    include/3dspacegame/game_celestial.h
    include/3dspacegame/game_brick_type.h
    include/3dspacegame/game_construct.h
    include/3dspacegame/game_brick_def.h
    include/engine/nv_dds.h
    include/engine/gfx_log.h
    shaders/pos_col.fs
    shaders/pos_col.vs
    shaders/pos_norm_uv.fs
    shaders/pos_norm_uv.vs
    shaders/pos_uv.fs
    shaders/pos_uv.vs
    shaders/pos_norm_uv_bone.fs
    shaders/pos_norm_uv_bone.vs
)
target_link_libraries(3dspacegame
	${ALL_LIBS}
)
endif()


if(APPLE)
SOURCE_GROUP(macos REGULAR_EXPRESSION ".*/macos/.*" )
elseif(WIN32)
SOURCE_GROUP(win32 REGULAR_EXPRESSION ".*/win32/.*" )
endif()

SOURCE_GROUP(engine REGULAR_EXPRESSION ".*/engine/.*" )
SOURCE_GROUP(game_headers REGULAR_EXPRESSION ".*/include/3dspacegame/.*" )
SOURCE_GROUP(engine_headers REGULAR_EXPRESSION ".*/include/engine/.*" )
#SOURCE_GROUP(imgui REGULAR_EXPRESSION ".*/external/imgui/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*(vs|fs)$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

