#include <cmath>
#include <complex>
#include <iostream>
#include <valarray>
#include <Windows.h>
double Round(double num)
{
    return int(num + 0.5);
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
void circleDirect(HDC hdc, const int xc, const int yc, const int R, COLORREF c)
{
    // second octant
    int x = 0, y = R;
    int R2 = R * R;
    draw8points(hdc, xc, yc, x, y, c);
    while (y > x)
    {
        x++;
        y = round(sqrt(R2 - x*x));
        draw8points(hdc, xc, yc, x, y, c);
    }

}
void circlePolar(HDC hdc, const int xc, const int yc, const int R, COLORREF c)
{
    int x = R, y = 0;
    double theta = 0, deltaTheta = 1.0/R;
    draw8points(hdc, xc, yc, x, y, c);
    while (x > y)
    {
        theta += deltaTheta;
        x = Round(  R * std::cos(theta));
        y = Round(R * std::sin(theta));
        draw8points(hdc, xc, yc, x, y, c);
    }
}
void circlePolarIterative(HDC hdc, const int xc, const int yc, const int R, COLORREF c)
{
    double x = R , y = 0;
    double dtheta = 1.0 / R;
    double cosDtheta = std::cos(dtheta);
    double sinDtheta = std::sin(dtheta);
    draw8points(hdc, xc, yc, x, y , c);
    while (x > y)
    {
        double temp = x * cosDtheta - y * sinDtheta;
        y = y * cosDtheta + x * sinDtheta;
        x = temp;
        draw8points(hdc , xc, yc, Round(x), Round(y), c);
    }
}
void circleMidpoint1(HDC hdc, const int xc, const int yc, const int R, COLORREF c)
{
    int x = 0 , y = R;
    int r2 = R * R;
    draw8points(hdc, xc, yc, x, y, c);
    while (x < y)
    {
        if (pow(x + 1, 2) + pow(y - 0.5, 2) - r2 >= 0)
            y--;
        x++;
        draw8points(hdc, xc, yc, x, y , c);
    }
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
    case WM_LBUTTONUP:
        hdc = GetDC(hwnd);
        point.x = LOWORD(lp);
        point.y = HIWORD(lp);
        R = static_cast<int>(sqrt(pow(center.x - point.x , 2) + pow(center.y - point.y, 2) ));
        CircleBresenham(hdc, center.x, center.y ,R, RGB(0,0,255));
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
    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("Circle Drawing"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
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