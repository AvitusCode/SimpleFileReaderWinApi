#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "menuid.h"
#include "Structures.h"
#include "SupportFunctions.h"
#include "FileReader.h"

#define NO_SCROLL 25

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("WinWindow");
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclass;

	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = sizeof(MODEL*) + sizeof(VIEWER*);
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName  = MAKEINTRESOURCE(MyMenu);
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName,		 // window class name
		_T("EXTRA HOT TEXT EDITOR"),		 // window caption
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,	 // window style
		CW_USEDEFAULT,			 // initial x position
		CW_USEDEFAULT,			 // initial y position
		CW_USEDEFAULT,			 // initial x size
		CW_USEDEFAULT,			 // initial y size
		NULL,					// parent window handle
		NULL,					// window menu handle
		hInstance,				 // program instance handle
		szCmdLine);					 // creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	TEXTMETRIC tm;
	PAINTSTRUCT ps;
	SCROLLINFO si;

	MODEL *model = NULL;
	VIEWER *viewer = NULL;

	switch (iMsg)
	{
    // Перехватываем сообщение WM_COMMAND для обработки меню
    HANDLE_MSG(hwnd, WM_COMMAND, WndProc_OnCommand);

    // Первичная инициализация параметров окна
	case WM_CREATE:
	{
	    MODEL *md  = (MODEL*)malloc(sizeof(MODEL));
	    VIEWER *vw =  (VIEWER*)malloc(sizeof(VIEWER));
	    CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
	    FILE *file = NULL;

		initStructures(md, vw);

		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		vw->cxChar = tm.tmAveCharWidth;
		vw->cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * vw->cxChar / 2;
		vw->cyChar = tm.tmHeight + tm.tmExternalLeading;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)md);
	    SetWindowLongPtr(hwnd, sizeof(MODEL*), (LONG_PTR)vw);
	    ReleaseDC(hwnd, hdc);

		file = fopen((char*)cs->lpCreateParams, "r");
		
		if (!file)
        {
            printf("ERROR, file cannot be open!\n");
            exit(-4);
		}
		if (!MyFileReader(hwnd, file))
            SendMessage(hwnd, WM_DESTROY, 0, 0);
		
		fclose(file);
		SetWindowPos(hwnd, HWND_TOP, 200, 200, 900, 700, 0);
	}
	break;
	
	// Смена размера окна
	case WM_SIZE:
	    model = (MODEL*)GetWindowLongPtr(hwnd, 0);
	    viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));

		viewer->cyClient = HIWORD(lParam);
		viewer->cxClient = LOWORD(lParam);

		viewer->iVscrollMax = max(0, (*model).MAX_STRING_LINES + 2 - viewer->cyClient / viewer->cyChar);
		viewer->iVscrollPos = min(viewer->iVscrollPos, viewer->iVscrollMax);

		SetScrollRange(hwnd, SB_VERT, 0, viewer->iVscrollMax, FALSE);
		SetScrollPos(hwnd, SB_VERT, viewer->iVscrollPos, TRUE);

		// Если включен режим верстки, то горизонтальная прокрутка не нужна
		if (viewer->OUTPUT_FORMAT == LAYOUT_OFF)
        {
            viewer->iHscrollMax = max(0, (viewer->MAX_STRING_LENGTH - viewer->cxClient) / viewer->cxChar);
		    viewer->iHscrollPos = min(viewer->iHscrollPos, viewer->iHscrollMax);

		    SetScrollRange(hwnd, SB_HORZ, 0, viewer->iHscrollMax, FALSE);
		    SetScrollPos(hwnd, SB_HORZ, viewer->iHscrollPos, TRUE);
        }
		
		InvalidateRect(hwnd, NULL, TRUE);
		SendMessage(hwnd, WM_VSCROLL, NO_SCROLL, 0L);
		SendMessage(hwnd, WM_HSCROLL, NO_SCROLL, 0L);
		UpdateWindow(hwnd);

		return DefWindowProc(hwnd, iMsg, wParam, lParam);
		
	case WM_VSCROLL:
	    viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			viewer->iVscrollInc = -viewer->iVscrollPos;
			break;
		case SB_BOTTOM:
			viewer->iVscrollInc = viewer->iVscrollMax - viewer->iVscrollPos;
			break;
		case SB_LINEUP:
			viewer->iVscrollInc = -1;
			break;
		case SB_LINEDOWN:
			viewer->iVscrollInc = 1;
			break;
		case SB_PAGEUP:
			viewer->iVscrollInc = min(-1, -viewer->cyClient / viewer->cyChar);
			break;
		case SB_PAGEDOWN:
			viewer->iVscrollInc = max(1, viewer->cyClient / viewer->cyChar);
			break;
		case SB_THUMBTRACK:
		    ZeroMemory(&si, sizeof(si));
		    si.cbSize = sizeof(si);
		    si.fMask = SIF_TRACKPOS;
		    if (!GetScrollInfo(hwnd, SB_VERT, &si))
                break;

			viewer->iVscrollInc = si.nTrackPos - viewer->iVscrollPos;
			break;
		default:
			viewer->iVscrollInc = 0;
			break;
		}

		viewer->iVscrollInc = max(-viewer->iVscrollPos, min(viewer->iVscrollInc, viewer->iVscrollMax - viewer->iVscrollPos));

		if (viewer->iVscrollInc != 0)
		{
			viewer->iVscrollPos += viewer->iVscrollInc;
			ScrollWindow(hwnd, 0, -viewer->cyChar * viewer->iVscrollInc, NULL, NULL);
			SetScrollPos(hwnd, SB_VERT, viewer->iVscrollPos, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
		
    case WM_HSCROLL:
	    viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));
	    if (viewer->OUTPUT_FORMAT == LAYOUT_ON)
            return DefWindowProc(hwnd, iMsg, wParam, lParam);
	    
        switch(LOWORD(wParam))
        {
        case SB_LINEUP:
            viewer->iHscrollInc = -1;
            break;
        case SB_LINEDOWN:
            viewer->iHscrollInc = 1;
            break;
        case SB_PAGEUP:
            viewer->iHscrollInc = -8;
            break;
        case SB_PAGEDOWN:
           viewer->iHscrollInc = 8;
            break;
        case SB_THUMBTRACK:
            ZeroMemory(&si, sizeof(si));
		    si.cbSize = sizeof(si);
		    si.fMask = SIF_TRACKPOS;
		    if (!GetScrollInfo(hwnd, SB_HORZ, &si))
                break;

			viewer->iHscrollInc = si.nTrackPos - viewer->iHscrollPos;
            break;
        default:
            viewer->iHscrollInc = 0;
            break;
        }

        viewer->iHscrollInc = max(-viewer->iHscrollPos, min(viewer->iHscrollInc, viewer->iHscrollMax - viewer->iHscrollPos));

        if (viewer->iHscrollInc != 0)
        {
            viewer->iHscrollPos += viewer->iHscrollInc;
            ScrollWindow(hwnd, -viewer->cxChar * viewer->iHscrollInc, 0, NULL, NULL);
            SetScrollPos(hwnd, SB_HORZ, viewer->iHscrollPos, TRUE);
        }
        
        return DefWindowProc(hwnd, iMsg, wParam, lParam);
        
    case WM_KEYDOWN:
		switch(wParam)
		{
        case VK_DOWN:
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
            break;
        case VK_UP:
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0L);
            break;
        case VK_LEFT:
            SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0L);
            break;
        case VK_RIGHT:
            SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0L);
            break;
        case VK_F1:
            SendMessage(hwnd, WM_COMMAND, IDM_VER1, 0L);
            break;
        case VK_F2:
            SendMessage(hwnd, WM_COMMAND, IDM_VER2, 0L);
            break;
        case VK_ESCAPE:
            SendMessage(hwnd, WM_COMMAND, IDM_EXIT, 0L);
		}
		
        return DefWindowProc(hwnd, iMsg, wParam, lParam);
        
    case WM_MOUSEWHEEL:
            if (GET_WHEEL_DELTA_WPARAM(wParam) >= 0)
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0L);
            else
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
            
        return DefWindowProc(hwnd, iMsg, wParam, lParam);
	case WM_PAINT:
	    model = (MODEL*)GetWindowLongPtr(hwnd, 0);
	    viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));

		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);

		// Начальная и конечная область отрисовки нерабочей области
		int iPaint = max(0, viewer->iVscrollPos + ps.rcPaint.top / viewer->cyChar - 1);
		int iPaintEnd = min(model->MAX_STRING_LINES, viewer->iVscrollPos + ps.rcPaint.bottom / viewer->cyChar);
		int y, x, i;
		x = y = i = 0;

		// Режим верстки отключен
		if (viewer->OUTPUT_FORMAT == LAYOUT_OFF)
		{
		    for (i = iPaint; i < iPaintEnd; i++)
		    {
		        x = viewer->cxChar * (1 - viewer->iHscrollPos);
			    y = viewer->cyChar * (1 - viewer->iVscrollPos + i);

			    if (model->paragraphPos[i + 1] - model->paragraphPos[i] == 0)
                    continue;
			    
			    TextOut(hdc, x, y, model->textBuffer + model->paragraphPos[i], model->paragraphPos[i + 1] - model->paragraphPos[i] - 1);
			    SetTextAlign(hdc, TA_LEFT | TA_TOP);
		    }
		}
		// Режим верстки включен (реализован посимвольный перевод)
		else if (viewer->OUTPUT_FORMAT == LAYOUT_ON)
        {
            /** \brief
             *
             * \param remains - остаток строки, которая не поместилась в рабочую область
             * \param length - длина строки или остаток предыдущей строки
             * \param current_size - длина по горизонтали рабочей области
             * \param offset_coeff - коэффициент смещения. Нужен, чтобы корректно выводить остаток предыдущей строки, а также просчитывать новое максимальное число строк
             *
             */
            size_t remains = 0, length = 0, current_size = 0, offset_coeff = 0, k = 0;
            model->MAX_STRING_LINES = model->MAX_STRINGS_LAYOUT;


            for (k = iPaint, i = iPaint; i < iPaintEnd; i++)
            {
                x = viewer->cxChar;
			    y = viewer->cyChar * (1 - viewer->iVscrollPos + i);
                length = (remains > 0) ? remains : abs(model->paragraphPos[k + 1] - model->paragraphPos[k] - 1);
                
                if (length > viewer->MAX_STRING_LENGTH)
                    length = viewer->MAX_STRING_LENGTH;

                current_size = (viewer->cxClient) / viewer->cxChar;

                if (length > current_size)
                {
                    remains = length - current_size;

                    if (model->paragraphPos[k] + offset_coeff * current_size + current_size >= model->size)
                        break;

                    TextOut(hdc, x, y, model->textBuffer + model->paragraphPos[k] + offset_coeff * current_size, current_size);
                    offset_coeff++;
                    iPaintEnd++;
                    continue;
                }
                else if (length <= current_size)
                {
                    if (model->paragraphPos[k] + offset_coeff * current_size + length >= model->size && model->MAX_STRING_LINES != 1)
                    {
                        TextOut(hdc, x, y, model->textBuffer + model->paragraphPos[k] + offset_coeff * current_size, remains);
                        continue;
                    }

                    TextOut(hdc, x, y, model->textBuffer + model->paragraphPos[k] + offset_coeff * current_size, length);
                    k++;
                    model->MAX_STRING_LINES += offset_coeff;
                    offset_coeff = remains = 0;
                    continue;
                }
            }
        }
		
		EndPaint(hwnd, &ps);
		return DefWindowProc(hwnd, iMsg, wParam, lParam);

	case WM_DESTROY:
	    model = (MODEL*)GetWindowLongPtr(hwnd, 0);
	    viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));

		if (model->paragraphPos)
			free(model->paragraphPos);
		if (model->textBuffer)
			free(model->textBuffer);
        free(model);
        free(viewer);
        PostQuitMessage(0);
        
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
