#include <android_native_app_glue.h>

#include "FlappyEngine.h"

void android_main(struct android_app* app) {
    FlappyEngine::GetInstance()->Init(app);
    FlappyEngine::GetInstance()->Run();
}
