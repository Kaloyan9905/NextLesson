#include <d3d11.h>
#include <string>
#include <tchar.h>
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "app.h"

std::string lessonResult;

// DirectX variables
ID3D11Device *g_pd3dDevice = nullptr;
ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
IDXGISwapChain *g_pSwapChain = nullptr;
ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
HWND g_hWnd = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Function to create a render target view
void CreateRenderTarget() {
    ID3D11Texture2D *pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

// Cleanup render target
void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

// Setup ImGui
void SetupImGui() {
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Set ImGui style
    ImGui::StyleColorsDark(); // You can choose different styles like StyleColorsClassic, etc.
}

// Cleanup ImGui
void CleanupImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// Initialize DirectX
bool InitD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 800;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext))) {
        return false;
    }

    CreateRenderTarget();
    return true;
}

// Cleanup DirectX
void CleanupD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

// Main rendering function
void Render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Create a simple ImGui window with styling
    ImGui::Begin("New Bulgarian University");

    if (ImGui::Button("Click")) {
        lessonResult = nextLesson(); // Call the function and store the result
    }

    // Display the result
    ImGui::Text(lessonResult.c_str());

    ImGui::End();

    // Rendering
    ImGui::Render();
    const float clear_color[] = {0.45f, 0.55f, 0.60f, 1.00f};
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present frame
    g_pSwapChain->Present(1, 0);
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Main function
int main() {
    // Register window class
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
        _T("ImGui Example"), NULL
    };
    RegisterClassEx(&wc);

    // Create application window
    g_hWnd = CreateWindow(wc.lpszClassName, _T("Dear ImGui Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL,
                          NULL, wc.hInstance, NULL);

    // Initialize DirectX
    if (!InitD3D(g_hWnd)) {
        CleanupD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Setup ImGui
    SetupImGui();

    // Show the window
    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);

    // Main loop
    MSG msg;
    bool done = false;
    while (!done) {
        // Process Windows messages
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        // Render ImGui
        if (!done)
            Render();
    }

    // Cleanup
    CleanupImGui();
    CleanupD3D();
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}
