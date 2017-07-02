#include <android_native_app_glue.h>

#include "Android.h"
#include "FlappyEngine.h"

void android_main(struct android_app* androidApp) {

    Android::GetInstance().Init(androidApp);
    FlappyEngine::GetInstance().Run();
}
