#include "SupportFunctions.h"
#include <memory.h>
#include <commdlg.h>
#include "menuid.h"
#include "FileReader.h"

void FileOpen(HWND hwnd)
{
  char szFile[256] = "";
  char szDirName[256] = "";
  char szFileTitle[256] = "";
  char szFilter[256] =
    "Text Files\0*.txt\0"
    "Any Files\0*.*\0";

  MODEL  *model = (MODEL*)GetWindowLongPtr(hwnd, 0);
  VIEWER *viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));

  OPENFILENAME ofn;
  FILE        *file;

  memset(&ofn, 0, sizeof(OPENFILENAME));
  GetCurrentDirectory(sizeof(szDirName), szDirName);
  szFile[0] = '\0';

  // Подготавливаем структуру для выбора входного файла
  ofn.lStructSize     = sizeof(OPENFILENAME);
  ofn.hwndOwner       = hwnd;
  ofn.lpstrFilter     = szFilter;
  ofn.lpstrInitialDir = szDirName;
  ofn.nFilterIndex    = 1;
  ofn.lpstrFile       = szFile;
  ofn.nMaxFile        = sizeof(szFile);
  ofn.lpstrFileTitle  = szFileTitle;
  ofn.nMaxFileTitle   = sizeof(szFileTitle);
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  // Выводим на экран диалоговую панель, предназначенную
  // для выбора входного файла
  if(GetOpenFileName(&ofn) == TRUE)
  {
    // Если файл выбран, открываем его для чтения
    if (*ofn.lpstrFile)
    {
      file = fopen(ofn.lpstrFile, "r");

      if (file)
      {
       if (!MyFileReader(hwnd, file))
       {
           SendMessage(hwnd, WM_DESTROY, 0, 0);
       }
       fclose(file);
       InvalidateRect(hwnd, NULL, TRUE);
       SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
       SendMessage(hwnd, WM_SIZE, 0, 0);
       SetWindowPos(hwnd, HWND_TOP, 200, 200, 900, 700, 0);
      }
      else
         MessageBox(hwnd, "Cannot open the file!", "Title", MB_OK | MB_ICONINFORMATION);
    }
  }
}

void WndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    HWND hmenu;
    VIEWER *viewer = NULL;
    MODEL *model = NULL;

    switch(id)
    {
        case IDM_OPEN:
            hmenu = GetMenu(hwnd);
            EnableMenuItem(hmenu, IDM_VER1, MF_GRAYED);
            EnableMenuItem(hmenu, IDM_VER2, MF_ENABLED);
            viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));
            viewer->OUTPUT_FORMAT = LAYOUT_OFF;
            FileOpen(hwnd);
            
            break;
        case IDM_VER1:
            hmenu = GetMenu(hwnd);
            EnableMenuItem(hmenu, IDM_VER1, MF_GRAYED);
            EnableMenuItem(hmenu, IDM_VER2, MF_ENABLED);
            model = (MODEL*)GetWindowLongPtr(hwnd, 0);
            viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));
            viewer->OUTPUT_FORMAT = LAYOUT_OFF;
            model->MAX_STRING_LINES = model->MAX_STRINGS_LAYOUT;

            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);

            break;
        case IDM_VER2:
            hmenu = GetMenu(hwnd);
            EnableMenuItem(hmenu, IDM_VER1, MF_ENABLED);
            EnableMenuItem(hmenu, IDM_VER2, MF_GRAYED);
            viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));
            viewer->OUTPUT_FORMAT = LAYOUT_ON;
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);

            break;
        case IDM_HELP:
            MessageBox(hwnd,
        "This is the text viewer. You can just read text in two options and scroll text\n"
        "To read text with layout press F1, without layout F2\n"
        "Good luck!",
        "Help text", MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_ABOUT:
            MessageBox(hwnd,
        "Text Editor SIMPLETEXT, v.1.1\n"
        "(C) Dmitry Jarmak, 2021\n"
        "Email: rinogik97@gmail.com",
        "Title", MB_OK | MB_ICONINFORMATION);
            break;

        case IDM_EXIT:
            SendMessage(hwnd, WM_DESTROY, 0, 0L);
            break;
    }

    return FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
}
