cmake_minimum_required (VERSION 3.24 FATAL_ERROR)

set(PROJECT Dark3DEngine)
project(${PROJECT})

set(RENDER_SOURCE 
    src/Render/ShaderProgram.cpp
    src/Render/VertexBuffer.cpp
    src/Render/VertexArray.cpp
	src/Render/Texture2D.cpp
	src/Render/Camera.cpp
	src/Render/Model.cpp
	src/Render/Mesh.cpp
	src/Render/PostProcessing.cpp
	src/Render/SkyboxRender.cpp
)

set(RENDER_HEADERS 
    src/Render/ShaderProgram.h
    src/Render/VertexBuffer.h
    src/Render/VertexArray.h
	src/Render/Texture2D.h
	src/Render/Camera.h
	src/Render/Model.h
	src/Render/Mesh.h
	src/Render/PostProcessing.h
	src/Render/SkyboxRender.h
	src/Render/stb_image.h
)

set(MANAGER_SOURCE 
    src/Manager/ResourceManager.cpp
	src/Manager/TextureLoader.cpp
	src/Manager/ConfigManager.cpp
)

set(MANAGER_HEADERS 
    src/Manager/ResourceManager.h
	src/Manager/TextureLoader.h
	src/Manager/ConfigManager.h
)



add_executable(${PROJECT} 
	src/main.cpp
	${RENDER_SOURCE}
	${RENDER_HEADERS}
	${MANAGER_SOURCE}
	${MANAGER_HEADERS}
)

source_group("Source Files\\Render" FILES ${RENDER_SOURCE})
source_group("Header Files\\Render" FILES ${RENDER_HEADERS})
source_group("Source Files\\Manager" FILES ${MANAGER_SOURCE})
source_group("Header Files\\Manager" FILES ${MANAGER_HEADERS})

target_compile_features(${PROJECT} PUBLIC cxx_std_17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT} glad)

include_directories(external/glm)

include_directories(external/rapidjson/include)

add_subdirectory(external/freetype)
target_link_libraries(${PROJECT} freetype)

add_subdirectory(external/openal-soft)
include_directories(external/openal-soft/include)
target_link_libraries(${PROJECT} OpenAL)

add_subdirectory(external/libsndfile)
include_directories(external/libsndfile/include)
target_link_libraries(${PROJECT} sndfile)

add_subdirectory(external/assimp)
include_directories(external/assimp/include)
target_link_libraries(${PROJECT} assimp)

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${PROJECT} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT}>/res)
