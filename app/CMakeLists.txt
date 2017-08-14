cmake_minimum_required(VERSION 3.4.1)

add_library( FlappyPelican SHARED
             src/main/cpp/GLState.cpp
             src/main/cpp/TextRenderer.cpp
             src/main/cpp/Ui.cpp
             src/main/cpp/Utilities.cpp
             src/main/cpp/EventManager.cpp
             src/main/cpp/Events.cpp
             src/main/cpp/Actor.cpp
             src/main/cpp/ActorFactory.cpp
             src/main/cpp/ActorComponents.cpp
             src/main/cpp/SceneGame.cpp
             src/main/cpp/Android.cpp
             src/main/cpp/TimeManager.cpp
             src/main/cpp/Log.cpp
             src/main/cpp/TouchDetector.cpp
             src/main/cpp/FlappyEngine.cpp
             src/main/cpp/ResourceManager.cpp
             src/main/cpp/Texture.cpp
             src/main/cpp/Shader.cpp
             src/main/cpp/SpriteRenderer.cpp
             src/main/cpp/Main.cpp )

add_subdirectory(libs/freetype-2.8)
include_directories(libs/freetype-2.8/include)

add_library(app-glue STATIC
            libs/native_app_glue/android_native_app_glue.c)

add_library(libSOIL2 STATIC
            libs/SOIL2/etc1_utils.c
            libs/SOIL2/image_DXT.c
            libs/SOIL2/image_helper.c
            libs/SOIL2/SOIL2.c)

add_library(tinyxml2 STATIC
            libs/tinyxml2/tinyxml2.cpp)

find_library(android
             app-glue
             tinyxml2
             libSOIL2
             EGL
             GLESv3
             freetype
             log)

target_include_directories(FlappyPelican PRIVATE
                           libs/native_app_glue
                           libs/tinyxml2
                           libs/freetype-2.8/include
                           libs/SOIL2
                           libs/glm)

target_link_libraries(FlappyPelican
                      android
                      app-glue
                      tinyxml2
                      libSOIL2
                      EGL
                      GLESv3
                      freetype
                      log)