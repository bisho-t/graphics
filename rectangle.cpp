// // task2: rectangle drawing with different algorithms
// // # user input : take two clicks to define the diagonal corners of a rectangle
// // # implementation: draw the four edges using : DDA algorithm(top and bottom edges), midpoint algorithm(left and right edges)
// // output display the rectangle in different color for each edge.

#include <windows.h>
#include <iostream>
using namespace std;

int Round(double x)
{
    return static_cast<int>(x + 0.5);
}

COLORREF colors[] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0)};

// DDA Algorithm for drawing a line
void DDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    float xIncrement = dx / static_cast<float> (steps);
    float yIncrement = dy / static_cast<float> (steps);
    auto x = static_cast<float> (x1), y = static_cast<float>(y1);

    for (int i = 0 ; i <= steps; i++)
    {
        SetPixel(hdc, Round(x) , Round(y), color);
        x += xIncrement;
        y+= yIncrement;
    }
}

// Midpoint Line Algorithm for drawing a line
void MidpointLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int xIncrement = (x2 > x1)? 1: -1;
    int yIncrement = (y2 > y1)? 1: -1;
    const bool swapXY = (dy > dx);
    if (swapXY)
        swap(dx, dy);
    int d = 2 * dy - dx;
    int d1 = 2 * (dy - dx);
    int d2 = 2 * dy;
    int  x = x1 , y = y1;
    for (int i = 0 ;i < dx; i++)
    {
        SetPixel(hdc, x, y , color);
        if (d > 0)
        {
            if (swapXY) x += xIncrement;
            else        y += yIncrement;
            d +=d1;
        }
        else
            d += d2;
        if (swapXY)     y += yIncrement;
        else            x += xIncrement;
    }
}

POINT p1, p2;
bool firstClick = true;

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hdc = GetDC(hwnd);
    switch (uMsg) {
        case WM_LBUTTONDOWN: { // Mouse click event
            if (firstClick) {
                p1.x = LOWORD(lParam);
                p1.y = HIWORD(lParam);
                firstClick = false;
            } else {
                p2.x = LOWORD(lParam);
                p2.y = HIWORD(lParam);
                firstClick = true;

                // Determine rectangle corners
                int xLeft = min(p1.x, p2.x);
                int xRight = max(p1.x, p2.x);
                int yTop = min(p1.y, p2.y);
                int yBottom = max(p1.y, p2.y);

                // Draw edges using different algorithms
                 DDA(hdc, xLeft,yTop , xRight, yTop, colors[0]);      // Top edge (DDA) - Red
                 DDA(hdc,xLeft,yBottom , xRight, yBottom, colors[1]);// Bottom edge (DDA) - Green
                 MidpointLine(hdc, xLeft, yTop, xLeft, yBottom, colors[2]);  // Left edge (Midpoint) - Blue
                 MidpointLine(hdc, xRight, yBottom, xRight, yTop, colors[3]); // Right edge (Midpoint) - Yellow
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    ReleaseDC(hwnd, hdc);
    return 0;
}

// Windows Main Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RectangleDrawer";
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "RectangleDrawer", "Draw Rectangle using DDA & Midpoint",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1500, 800, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
