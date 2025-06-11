#include <cmath>
#include <iostream>
#include <valarray>
#include <Windows.h>
using namespace std;
double Round(double num)
{
    return int(num + 0.5);
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

void draw8points(HDC hdc, const int xc, const int yc, const int a, const int b, COLORREF c)
{
    SetPixel(hdc, xc + a, yc + b , c);
    SetPixel(hdc, xc - a, yc + b , c);
    SetPixel(hdc, xc + a, yc - b , c);
    SetPixel(hdc, xc - a, yc - b , c);
    SetPixel(hdc, xc + b, yc + a , c);
    SetPixel(hdc, xc - b, yc + a , c);
    SetPixel(hdc, xc - b, yc - a , c);
    SetPixel(hdc, xc + b, yc - a , c);
}

void CircleBresenham(HDC hdc, const int xc, const int yc, const int R,COLORREF color)
{
    int x = 0,y = R;
    int d = 1-R;
    int d1 = 3, d2 = 5 - 2 * R;
    draw8points(hdc, xc, yc,x,y, color);
    while(x < y)
    {
        if(d < 0)
        {
            d += d1;
            d2 += 2;
        }
        else
        {

            d += d2;
            d2 += 4;
            y--;
        }
        d1 += 2;
        x++;
        draw8points(hdc, xc, yc,x,y, color);
    }
}
POINT center, point;
LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static int R;
    static double pointOnCircle;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        hdc = GetDC(hwnd);
        center.x = LOWORD(lp);
        center.y = HIWORD(lp);
        SetPixel(hdc, center.x, center.y, RGB(255,0,0));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_RBUTTONDOWN:
        hdc = GetDC(hwnd);
        point.x = LOWORD(lp);
        point.y = HIWORD(lp);
        R = static_cast<int>(sqrt(pow(center.x - point.x , 2) + pow(center.y - point.y, 2) ));
        pointOnCircle = static_cast<double>(R) / sqrt(2);
        // the increasing and decreasing lines have angles 45 and 135  and tan (45) and tan(135) -> y / x = 1 , -1
        // which means x = y

        CircleBresenham(hdc, center.x, center.y ,R, RGB(0,0,255));
        // Horizontal line
        MidpointLine(hdc, center.x - R, center.y, center.x + R, center.y, RGB(0,0,255));

        // // vertical line
        MidpointLine(hdc, center.x, center.y - R, center.x , center.y + R, RGB(0,0,255));

        // // increasing line
        MidpointLine(hdc, Round(center.x - pointOnCircle), Round(center.y - pointOnCircle), Round(center.x + pointOnCircle), Round(center.y + pointOnCircle), RGB(0,0,255));

        // // decreasing line
        MidpointLine(hdc, Round(center.x + pointOnCircle), Round(center.y - pointOnCircle), Round(center.x - pointOnCircle), Round(center.y + pointOnCircle), RGB(0,0,255));


        SetPixel(hdc , point.x, point.y, RGB(255,0,0));
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
int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh)        // HINSTANCE => handle of current and previous instance                                                                          // LPSTR => long pointer to string
{
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);    // coloring of the background
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    wc.lpszClassName = reinterpret_cast<LPCSTR>("MyClass");
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("Circle with sectors"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}