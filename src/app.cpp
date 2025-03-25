#include <windows.h>
#include <d3dx9.h>
#include "debug.h"
#include "app.h"

APPLICATION::APPLICATION()
{
    m_pDevice = NULL;
    m_mainWindow = 0;
    m_pFont = NULL;
}

HRESULT
APPLICATION::Init(HINSTANCE hInstance, int width, int height, bool windowed)
{
    debug.Print("Application initialization started");

    WNDCLASS wc;
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("D3DWND");

    RegisterClass(&wc);
    m_mainWindow = CreateWindow(TEXT("D3DWND"), TEXT("Framework"), WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, hInstance, 0);
    SetCursor(NULL);
    ShowWindow(m_mainWindow, SW_SHOW);
    UpdateWindow(m_mainWindow);

    IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d9 == NULL)
    {
        debug.Print("Failed to create d3d9 interface");
        return E_FAIL;
    }

    // Check that the Device supports what we need from it
    D3DCAPS9 caps;
    d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    // Hardware Vertex Processing or not?
    int vp = 0;
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // Check vertex & pixelshader versions
    if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0) || caps.PixelShaderVersion < D3DPS_VERSION(2, 0))
    {
        debug.Print("Warning - Your graphic card does not support vertex and pixelshaders version 2.0");
    }

    D3DPRESENT_PARAMETERS d3dpp;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = m_mainWindow;
    d3dpp.Windowed = windowed;
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainWindow,
                                  vp, &d3dpp, &m_pDevice)))
    {
        debug.Print("Failed to create d3d9 device");
        return E_FAIL;
    }

    d3d9->Release();

    // Load Application Specific resources here...
    D3DXCreateFont(m_pDevice, 48, 0, FW_BOLD, 1, false,
                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                   DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_pFont);

    return S_OK;
}

HRESULT APPLICATION::Update(float deltaTime)
{
    if (KEYDOWN(VK_ESCAPE))
        Quit();

    return S_OK;
}

HRESULT APPLICATION::Render()
{
    m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);
    // Begin the scene
    if (SUCCEEDED(m_pDevice->BeginScene()))
    {
        // Render scene here...
        RECT r = {0, 0, 800, 600};

        m_pFont->DrawText(NULL,
                          TEXT("Hello World!"),
                          -1,
                          &r,
                          DT_CENTER | DT_NOCLIP | DT_VCENTER,
                          0xffffffff);

        m_pDevice->EndScene();
        m_pDevice->Present(0, 0, 0, 0);
    }

    return S_OK;
}

HRESULT APPLICATION::Cleanup()
{
    try
    {
        // Release all resources here...
        m_pFont->Release();
        m_pDevice->Release();

        debug.Print("Application terminated");
    }
    catch (...)
    {
    }
    return S_OK;
    return S_OK;
}

HRESULT APPLICATION::Quit()
{
    ::DestroyWindow(m_mainWindow);
    ::PostQuitMessage(0);
    return S_OK;
}
