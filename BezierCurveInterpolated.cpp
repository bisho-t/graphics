#include <iostream>
#include <cmath>
#include <valarray>
#include <Windows.h>
using namespace std;

double Round(double num) {
    return int(num + 0.5);
}

// Euclidean distance between two points
double distance(POINT p1, POINT p2) {
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

// Get a point on the cubic Bezier curve at parameter t
POINT BezierPoint(double t, POINT p0, POINT p1, POINT p2, POINT p3) {
    double u = 1 - t;
    double tt = t * t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * t;

    POINT point;
    point.x = Round(uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x);
    point.y = Round(uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y);
    return point;
}

// Get an interpolated color on the Bézier curve at parameter t
COLORREF BezierColor(double t, COLORREF c0, COLORREF c1, COLORREF c2, COLORREF c3) {
    auto bezier = [t](int a, int b, int c, int d) {
        double u = 1 - t;
        return Round(
            pow(u, 3) * a +
            3 * pow(u, 2) * t * b +
            3 * u * pow(t, 2) * c +
            pow(t, 3) * d
        );
    };
    return RGB(
        bezier(GetRValue(c0), GetRValue(c1), GetRValue(c2), GetRValue(c3)),
        bezier(GetGValue(c0), GetGValue(c1), GetGValue(c2), GetGValue(c3)),
        bezier(GetBValue(c0), GetBValue(c1), GetBValue(c2), GetBValue(c3))
    );
}

// Draw the Bézier curve with interpolated colors
void InterpolatedBezierCurve(HDC hdc, POINT p0, POINT p1, POINT p2, POINT p3,
                             COLORREF c0, COLORREF c1, COLORREF c2, COLORREF c3, int segments = 1000) {
    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        POINT pt = BezierPoint(t, p0, p1, p2, p3);
        COLORREF color = BezierColor(t, c0, c1, c2, c3);
        SetPixel(hdc, pt.x, pt.y, color);
    }
}

POINT p1, p2, p3, p4;
bool firstClick = true, secondClick = false, thirdClick = false, fourthClick = false;

// Windows message handling
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hdc = GetDC(hwnd);

    switch (uMsg) {
    case WM_LBUTTONDOWN: {
        if (firstClick) {
            p1.x = LOWORD(lParam); p1.y = HIWORD(lParam);
            SetPixel(hdc, p1.x, p1.y, RGB(255, 0, 0));
            firstClick = false; secondClick = true;
        }
        else if (secondClick) {
            p2.x = LOWORD(lParam); p2.y = HIWORD(lParam);
            SetPixel(hdc, p2.x, p2.y, RGB(0, 255, 0));
            secondClick = false; thirdClick = true;
        }
        else if (thirdClick) {
            p3.x = LOWORD(lParam); p3.y = HIWORD(lParam);
            SetPixel(hdc, p3.x, p3.y, RGB(0, 0, 255));
            thirdClick = false; fourthClick = true;
        }
        else {
            p4.x = LOWORD(lParam); p4.y = HIWORD(lParam);
            SetPixel(hdc, p4.x, p4.y, RGB(255, 255, 0));
            fourthClick = false; firstClick = true;

            // Draw the interpolated colored Bézier curve
            InterpolatedBezierCurve(hdc, p1, p2, p3, p4,
                                    RGB(100, 0, 0),
                                    RGB(0, 100, 0),
                                    RGB(0, 0, 255),
                                    RGB(0, 200, 255));
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        ReleaseDC(hwnd, hdc);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    ReleaseDC(hwnd, hdc);
    return 0;
}

int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh) {
    WNDCLASS wc = {};
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    wc.lpszClassName = reinterpret_cast<LPCSTR>("MyClass");
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WindowProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("Interpolated Bezier Curve"),
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);

    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
