#include <windows.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include<iostream>
using namespace std;

struct EdgeNode {
    double x;
    double invSlope;
    int ymax;
    EdgeNode* next;
    EdgeNode(double x_, double invSlope_, int ymax_)
        : x(x_), invSlope(invSlope_), ymax(ymax_), next(nullptr) {}
};

int Round(double val) {
    return int(val + 0.5);
}

void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, const COLORREF c) {
    int dx = x2 - x1, dy = y2 - y1;
    SetPixel(hdc, x1, y1, c);
    if (abs(dx) >= abs(dy)) {
        int x = x1;
        int xinc = dx > 0 ? 1 : -1;
        double y = y1, yinc = static_cast<double>(dy) / dx * xinc;
        while (x != x2) {
            x += xinc;
            y += yinc;
            SetPixel(hdc, x, Round(y), c);
        }
    } else {
        int y = y1;
        int yinc = dy > 0 ? 1 : -1;
        double x = x1, xinc = static_cast<double>(dx) / dy * yinc;
        while (y != y2) {
            x += xinc;
            y += yinc;
            SetPixel(hdc, Round(x), y, c);
        }
    }
}

void InsertEdge(EdgeNode*& head, EdgeNode* newNode) {
    if (!head || newNode->x < head->x) {
        newNode->next = head;
        head = newNode;
        return;
    }
    EdgeNode* current = head;
    while (current->next && current->next->x < newNode->x)
        current = current->next;
    newNode->next = current->next;
    current->next = newNode;
}

void ScanLineFill(HDC hdc, const std::vector<POINT>& vertices, COLORREF color) {
    if (vertices.size() < 3) return;

    int n = vertices.size();
    int maxY = vertices[0].y, minY = vertices[0].y;

    // Get min and max Y
    for (const auto& p : vertices) {
        if (p.y > maxY) maxY = p.y;
        if (p.y < minY) minY = p.y;
    }

    std::vector<EdgeNode*> edgeTable(maxY + 1, nullptr);

    // Build Edge Table
    for (int i = 0; i < n; ++i) {
        POINT p1 = vertices[i];
        POINT p2 = vertices[(i + 1) % n];
        if (p1.y == p2.y) continue; // Skip horizontal edges

        // Ensure p1 is the lower point
        if (p1.y > p2.y) std::swap(p1, p2);

        double invSlope = (double)(p2.x - p1.x) / (p2.y - p1.y);
        EdgeNode* edge = new EdgeNode(p1.x, invSlope, p2.y);
        InsertEdge(edgeTable[p1.y], edge);
    }

    std::vector<EdgeNode*> activeList;


    // Start from the lowest Y
    for (int y = minY; y <= maxY; ++y) {
        // 1. Add edges starting at this scan line
        EdgeNode* current = edgeTable[y];
        while (current) {
            activeList.push_back(new EdgeNode(current->x, current->invSlope, current->ymax));
            current = current->next;
        }

        // 2. Remove edges where ymax == y
        activeList.erase(
            std::remove_if(activeList.begin(), activeList.end(), [y](EdgeNode* e) {
                if (e->ymax == y) {
                    delete e;
                    return true;
                }
                return false;
            }),
            activeList.end()
        );

        // 3. Sort active list by x
        std::sort(activeList.begin(), activeList.end(), [](EdgeNode* a, EdgeNode* b) {
            return a->x < b->x;
        });

        // 4. Draw horizontal lines between pairs
        for (size_t i = 0; i + 1 < activeList.size(); i += 2) {
            int x1 = ceil(activeList[i]->x);
            int x2 = floor(activeList[i + 1]->x);
            DrawLineDDA(hdc, x1, y, x2, y, color);
        }

        // 5. Update x for active edges
        for (EdgeNode* edge : activeList) {
            edge->x += edge->invSlope;
        }
    }

    // Cleanup
    for (EdgeNode* e : activeList) delete e;
    for (EdgeNode*& list : edgeTable) {
        while (list) {
            EdgeNode* tmp = list;
            list = list->next;
            delete tmp;
        }
    }
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
        ScanLineFill(hdc,polygon, RGB(0, 200, 255));
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
    HWND hwnd = CreateWindow(reinterpret_cast<LPCSTR>("MyClass"), reinterpret_cast<LPCSTR>("polygon drawing"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, 0);
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