#include <windows.h>
#include "app.h"
#include "debug.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    APPLICATION app;

    if (FAILED(app.Init(hInstance, 1280, 720, true)))
    {
        debug.Print("Application Init failed");
        return 0;
    }

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    int startTime = timeGetTime();

    debug.Print("Gameloop started");
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
    debug.Print("Gameloop stopped");

    app.Cleanup();
    return msg.wParam;
}
