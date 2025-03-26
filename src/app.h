#include <windows.h>
#include <d3dx9.h>
#include "debug.h"
#include "heightMap.h"

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

class APPLICATION
{
public:
    APPLICATION();
    HRESULT Init(HINSTANCE hInstance, int width, int height, bool windowed);
    HRESULT Update(float deltaTime);
    HRESULT Render();
    HRESULT Cleanup();
    HRESULT Quit();

private:
    IDirect3DDevice9 *m_pDevice;
    HEIGHTMAP *m_pHeightMap;

    float m_angle, m_angle_b;
    HWND m_mainWindow;
    ID3DXFont *m_pFont;
};
