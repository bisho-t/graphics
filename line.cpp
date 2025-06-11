#include <cmath>
#include <complex>
#include <valarray>
#include <Windows.h>
void swap(int &val1, int &val2) noexcept
{
    int temp = val1;
    val1 = val2;
    val2 = temp;
}
int Round(double x)
{
    return static_cast<int>(x + 0.5);
}
void InterpolatedColoredLine(HDC hdc, int x1, int y1, int x2, int y2, const COLORREF c1, const COLORREF c2)
{
    // getting components of the first color
    int r1 = GetRValue(c1),  g1 = GetGValue(c1),  b1 = GetBValue(c1);
    // getting components of the second color
    int r2 = GetRValue(c2) , g2 = GetGValue(c2) , b2 = GetBValue(c2);
    int alpha1 = x2 - x1, alpha2 = y2 - y1;
    int alpha3 = r2 - r1;
    int alpha4 = g2 - g1;
    int alpha5 = b2 - b1;
    int steps = std::max(abs(alpha1), abs(alpha2));
    double xIncrement = static_cast<double> (alpha1) / steps;
    double yIncrement = static_cast<double> (alpha2) / steps;
    double redIncrement = static_cast<double> (alpha3) / steps;
    double greenIncrement = static_cast<double> (alpha4) / steps;
    double blueIncrement = static_cast<double> (alpha5) / steps;
    double x = x1, y = y1, red = r1, green = g1 , blue = b1;
    SetPixel(hdc, x1, y1 , RGB(red, green, blue));
    for (int step = 1 ; step <= steps; step++)
    {
        x += xIncrement;
        y += yIncrement;
        red += redIncrement;
        green += greenIncrement;
        blue += blueIncrement;
        SetPixel(hdc, Round(x), Round(y), RGB(Round(red), Round(green), Round(blue)));
    }
}
void naiveDrawLine(HDC hdc, int x1, int y1, int x2, int y2, const COLORREF c)
{
    const int dx = x2 - x1;
    const int dy = y2 - y1;
    double slope = static_cast<double>(dy) / dx;
    if (x1 > x2)
    {
        swap(x2, x1);
        swap(y2, y1);
    }
    for (int x = x1 ; x <= x2; x++)
    {
        double y = y1 + slope *(x - x1);
        SetPixel(hdc , x , Round(y) , c);
    }
}
void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, const COLORREF c)
{
    int dx = x2 - x1, dy = y2 - y1;
    SetPixel(hdc, x1, y1, c);
    if (abs(dx) >= abs(dy))     // The line is closer to the x-axis
    {
        int x = x1;
        int xinc = dx > 0 ? 1 : -1;
        double y = y1 , yinc = static_cast<double>(dy) / dx * xinc;
        SetPixel(hdc, x, Round(y) , c);
        while (x != x2)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, x, Round(y) , c);
        }
    }
    else        // The line is closer to the y-axis
    {
        int y = y1, yinc = dy > 0 ? 1 : -1;
        double x = x1 , xinc = static_cast<double>(dx) / dy * yinc;
        while (y != y2)
        {
            x += xinc;
            y += yinc;
            SetPixel(hdc, Round(x) , y, c);
        }
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
LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static int x1, y1, x2, y2;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        hdc = GetDC(hwnd);
        x1 = LOWORD(lp);
        y1 = HIWORD(lp);
        ReleaseDC(hwnd, hdc);
        break;
    case WM_LBUTTONUP:
        hdc = GetDC(hwnd);
        x2 = LOWORD(lp);
        y2 = HIWORD(lp);
        InterpolatedColoredLine(hdc, x1, y1, x2, y2,  RGB(0, 0, 0 ), RGB(255, 255, 255));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd); break;
    case WM_DESTROY:
        PostQuitMessage(0); break;
    default:return DefWindowProc(hwnd, m, wp, lp);
    }
    return 0;
}
int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh)        // HINSTANCE => handle of current and previous instance
                                                                            // LPSTR => long pointer to string
{
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));    // coloring of the background
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.hIcon = LoadIcon(nullptr, IDI_INFORMATION);
    wc.lpszClassName = reinterpret_cast<LPCSTR>("MyClass");
    wc.lpszMenuName = nullptr;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("line"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hi, nullptr);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}