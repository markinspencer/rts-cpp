

#pragma once
#include "app.h"
#include <d3dx9.h>
#include "debug.h"

APPLICATION::APPLICATION()
{
    m_pDevice = NULL;
    m_pHeightMap = NULL;
    m_mainWindow = 0;
    m_angle = 0.0f;
    m_angle_b = 0.5f;
}

HRESULT
APPLICATION::Init(HINSTANCE hInstance, int width, int height, bool windowed)
{
    debug.Print("Starting application");

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
    D3DXCreateFont(m_pDevice,
                   16,
                   0,
                   FW_THIN,
                   1,
                   false,
                   DEFAULT_CHARSET,
                   OUT_TT_PRECIS,   // Try OUT_TT_PRECIS instead
                   DEFAULT_QUALITY, // Try this instead of DEFAULT_QUALITY
                   DEFAULT_PITCH | FF_DONTCARE,
                   TEXT("Source Sans Pro"), // Source Code Pro for monospacedry Consolas or other clear fonts
                   &m_pFont);

    debug.Print("Application successfully initialized");
    return S_OK;
}

HRESULT APPLICATION::Update(float deltaTime)
{
    // Create Heightmap
    // Create Heightmap
    if (m_pHeightMap == NULL)
    {
        // Create flat heightmap
        m_pHeightMap = new HEIGHTMAP(m_pDevice, INTPOINT(50, 50));

        if (FAILED(m_pHeightMap->CreateParticles()))
        {
            debug.Print("Failed to create particles");
            Quit();
        }
    }
    else
    {
        // Control camera
        D3DXMATRIX matWorld, matView, matProj;
        D3DXVECTOR2 centre = m_pHeightMap->GetCentre();
        D3DXVECTOR3 Eye = D3DXVECTOR3(centre.x + cos(m_angle) * cos(m_angle_b) * centre.x * 1.5f,
                                      sin(m_angle_b) * m_pHeightMap->m_maxHeight * 5.0f,
                                      -centre.y + sin(m_angle) * cos(m_angle_b) * centre.y * 1.5f);

        D3DXVECTOR3 Lookat = D3DXVECTOR3(centre.x, 0.0f, -centre.y);

        D3DXMatrixIdentity(&matWorld);
        D3DXMatrixLookAtLH(&matView, &Eye, &Lookat, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
        float fov = 45.0f * (D3DX_PI / 180.0f);
        D3DXMatrixPerspectiveFovLH(&matProj, fov, 1.3333f, 1.0f, 1000.0f);

        m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
        m_pDevice->SetTransform(D3DTS_VIEW, &matView);
        m_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

        // Move selection rectangle
        if (KEYDOWN('W') && m_pHeightMap->m_selRect.left > 0)
            m_pHeightMap->MoveRect(LEFT);
        if (KEYDOWN('S') && m_pHeightMap->m_selRect.right < m_pHeightMap->m_size.x - 1)
            m_pHeightMap->MoveRect(RIGHT);
        if (KEYDOWN('D') && m_pHeightMap->m_selRect.top > 0)
            m_pHeightMap->MoveRect(UP);
        if (KEYDOWN('A') && m_pHeightMap->m_selRect.bottom < m_pHeightMap->m_size.y - 1)
            m_pHeightMap->MoveRect(DOWN);

        // Raise/Lower heightmap
        if (KEYDOWN('E'))
            m_pHeightMap->RaiseTerrain(m_pHeightMap->m_selRect, deltaTime * 3.0f);
        if (KEYDOWN('R'))
            m_pHeightMap->RaiseTerrain(m_pHeightMap->m_selRect, -deltaTime * 3.0f);

        // Smooth Heightmap
        if (KEYDOWN(VK_SPACE))
            m_pHeightMap->SmoothTerrain();
    }

    if (KEYDOWN(VK_ESCAPE))
        Quit();

    // Rotate camera (more on cameras in Chapter 5)
    if (KEYDOWN(VK_UP) && m_angle_b < D3DX_PI * 0.4f)
        m_angle_b += deltaTime * 0.5f;
    if (KEYDOWN(VK_DOWN) && m_angle_b > 0.1f)
        m_angle_b -= deltaTime * 0.5f;
    if (KEYDOWN(VK_LEFT))
        m_angle -= deltaTime * 0.5f;
    if (KEYDOWN(VK_RIGHT))
        m_angle += deltaTime * 0.5f;

    return S_OK;
}

HRESULT APPLICATION::Render()
{
    // Clear the viewport
    m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

    // Begin the scene
    if (SUCCEEDED(m_pDevice->BeginScene()))
    {
        if (m_pHeightMap != NULL)
            m_pHeightMap->Render();

        RECT r[] = {{10, 10, 0, 0}, {10, 30, 0, 0}, {10, 50, 0, 0}, {10, 70, 0, 0}};
        m_pFont->DrawText(NULL, "Arrows: Move Camera", -1, &r[0], DT_LEFT | DT_TOP | DT_NOCLIP, 0xffffffff);
        m_pFont->DrawText(NULL, "W/A/S/D: Move Square", -1, &r[1], DT_LEFT | DT_TOP | DT_NOCLIP, 0xffffffff);
        m_pFont->DrawText(NULL, "E/R: Raise/Lower Square", -1, &r[2], DT_LEFT | DT_TOP | DT_NOCLIP, 0xffffffff);
        m_pFont->DrawText(NULL, "Space: Smooth Terrain", -1, &r[3], DT_LEFT | DT_TOP | DT_NOCLIP, 0xffffffff);

        // End the scene.
        m_pDevice->EndScene();
        m_pDevice->Present(0, 0, 0, 0);
    }

    return S_OK;
}

HRESULT APPLICATION::Cleanup()
{
    try
    {
        if (m_pHeightMap != NULL)
        {
            delete m_pHeightMap;
            m_pHeightMap = NULL;
        }

        m_pFont->Release();
        m_pDevice->Release();

        debug.Print("Application terminated");
    }
    catch (...)
    {
    }

    return S_OK;
}

HRESULT APPLICATION::Quit()
{
    ::DestroyWindow(m_mainWindow);
    ::PostQuitMessage(0);
    return S_OK;
}
