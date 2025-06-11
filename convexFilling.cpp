#include <algorithm>
#include <cmath>
#include <iostream>
#include <windows.h>
#include<vector>
#include<list>
using namespace std;
typedef struct
{
    int xleft, xright;
} EDGETABLE[1080];


int Round (double val)
{
    return int(val + 0.5);
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
void tableInitialization(EDGETABLE table)
{
    for (int i = 0 ; i < 1080 ; i++)
    {
        table[i].xleft = INT_MAX;
        table[i].xright = INT_MIN;
    }
}
void edge2table(POINT v1, POINT v2, EDGETABLE table)
{

    if (v1.y == v2.y)
        return;
    if (v1.y > v2.y)
        swap(v1, v2);
    int y = v1.y;
    double x = v1.x;
    const double slopeInverse = static_cast<double>(v2.x - v1. x) / (v2.y - v1. y);
    while (y < v2.y)
    {
        if (x < table[y].xleft)
            table[y].xleft = static_cast<int>(ceil(x));
        if (x > table[y].xright)
            table[y].xright = static_cast<int>(floor(x));
        y++;
        x += slopeInverse;

    }
}
void polygon2table(const vector<POINT>& vertices , EDGETABLE table)
{
    POINT v1 = vertices[vertices.size() - 1];
    for (auto vertix : vertices)
    {
        // POINT v2 = vertix;
        edge2table(v1, vertix, table);
        v1 = vertix;
    }

}
void tableDraw(HDC hdc, EDGETABLE table, COLORREF c)
{
    for (int i = 0 ; i < 1080 ; i++)
    {
        if (table[i].xleft < table[i].xright)
            DrawLineDDA(hdc, table[i].xleft, i, table[i].xright, i, c);
        else if (table[i].xleft == table[i].xright)
            SetPixel(hdc, table[i].xleft, i , c);
    }
}
void convexFill(HDC hdc, const vector<POINT>& polygon, COLORREF c)
{
    EDGETABLE table;
    tableInitialization(table);
    polygon2table(polygon, table);
    tableDraw(hdc, table, c);
}

LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    static vector<POINT> polygon ;
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        hdc = GetDC(hwnd);
        polygon.push_back(POINT(LOWORD(lp), HIWORD(lp)));
        SetPixel(hdc, LOWORD(lp), HIWORD(lp), RGB(250, 0, 0));
        cout << LOWORD(lp) << " " << HIWORD(lp) << endl;
        ReleaseDC(hwnd, hdc);
        break;
    case WM_RBUTTONDOWN:
        hdc = GetDC(hwnd);
        convexFill(hdc, polygon, RGB(0, 0, 255));
        polygon.clear();
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
    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("convex filling"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
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