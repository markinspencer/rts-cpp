#include <windows.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    APPLICATION app;

    if (FAILED(app.Init(hInstance, 800, 600, true)))
    {
        printf("failed init");
        return 0;
    }

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    int startTime = timeGetTime();

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            int t = timeGetTime();
            float deltaTime = (t - startTime) * 0.001f;

            app.Update(deltaTime);
            app.Render();

            startTime = t;
        }
    }

    app.Cleanup();
    return msg.wParam;
}
