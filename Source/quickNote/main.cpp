// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "quickNote.h"

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		newNoteDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_QUICKNOTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Init notification tray
	initNotifyIconData(g_hWnd);
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKNOTE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKNOTE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_QUICKNOTE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindowW(szWindowClass, L"QUICK NOTE", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, 0, 847, 570, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
    {
	case WM_TASKBARCREATED:
		if (!IsWindowVisible(g_hWnd))
		{
			minimizeWindow(hWnd);
		}
		return 0;
	case WM_CREATE:
	{
		icc.dwSize = sizeof(icc);
		icc.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&icc);
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		hFont = CreateFont(lf.lfHeight, lf.lfWidth,
			lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
			lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
			lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
			lf.lfPitchAndFamily, lf.lfFaceName);

		// Left panel
		hNoteLsv = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT,
			32, 108, 251, 368, hWnd, (HMENU)ID_NOTELISTVIEW, hInst, NULL);
		SendMessage(hNoteLsv, WM_SETFONT, WPARAM(hFont), TRUE);
		ListView_SetExtendedListViewStyle(hNoteLsv, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		initNoteLsvColumn(hNoteLsv);
		eNote = readNoteDB("notes.db");
		if (eNote.size() < 1)
		{
			MessageBox(hWnd, L"Dữ liệu chưa có hoặc đọc dữ liệu lỗi!", L"Chú ý", MB_ICONWARNING);
		}
		else
		{
			eNoteByTag = eNote;
			for (int i = 0; i < eNote.size(); i++)
			{
				addItmToNoteLsv(hNoteLsv, i, eNote[i]);
			}
		}
		// Choose the first note to display
		//ListView_SetItemState(hNoteLsv, 0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		//SetFocus(hNoteLsv);
		// Default tab page is Note tab so show Note tab windows
		//ShowWindow(hTagLsv, SW_SHOW);
		hTagLsv = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT,
			32, 108, 251, 368, hWnd, (HMENU)ID_TAGLISTVIEW, hInst, NULL);
		SendMessage(hTagLsv, WM_SETFONT, WPARAM(hFont), TRUE);
		ListView_SetExtendedListViewStyle(hTagLsv, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		initTagLsvColumn(hTagLsv);
		eTag = readTagDB("tags.db");
		if (eTag.size() > 0)
		{
			// Sort ascendedly alphabettally
			for (int i = 0; i < eTag.size() - 1; i++)
			{
				for (int j = i + 1; j < eTag.size(); j++)
				{
					if (eTag[i].getTagName() > eTag[j].getTagName())
					{
						tag aTag = eTag[i];
						eTag[i] = eTag[j];
						eTag[j] = aTag;
					}
				}
			}
			for (int i = 0; i < eTag.size(); i++)
			{
				addItmToTagLsv(hTagLsv, i, eTag[i]);
			}
		}
		// Default tab page is Note Tab so hide Tag Tab windows
		addItmToTagLsv(hTagLsv, 0, tag::tag(wstring(L"Tất cả"), vector<unsigned int>()));
		ShowWindow(hTagLsv, SW_HIDE);

		hTabCtrl = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_FIXEDWIDTH,
			31, 86, 255, 394, hWnd, (HMENU)ID_TABCONTROL, hInst, NULL);
		// Add items to tab control
		TCITEM tie;
		tie.mask = TCIF_TEXT | TCIF_IMAGE;
		tie.iImage = -1;
		// Note tab
		tie.pszText = wsToWc(L"Ghi chú");
		TabCtrl_InsertItem(hTabCtrl, 0, &tie);
		tie.pszText = wsToWc(L"Nhãn");
		TabCtrl_InsertItem(hTabCtrl, 1, &tie);

		hGrb = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
			15, 61, 285, 435, hWnd, NULL, hInst, NULL);
		SendMessage(hGrb, WM_SETFONT, WPARAM(hFont), TRUE);

		hBtn = CreateWindowEx(0, L"BUTTON", L"Ghi chú mới", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
			15, 17, 137, 36, hWnd, (HMENU)ID_NEWNOTEBUTTON, hInst, NULL);
		SendMessage(hBtn, WM_SETFONT, WPARAM(hFont), TRUE);

		hBtn = CreateWindowEx(0, L"BUTTON", L"Xem thống kê", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
			163, 17, 137, 36, hWnd, (HMENU)ID_VIEWSTATICBUTTON, hInst, NULL);
		SendMessage(hBtn, WM_SETFONT, WPARAM(hFont), TRUE);

		// Right panel
		hGrb = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
			314, 10, 500, 485, hWnd, NULL, hInst, NULL);
		SendMessage(hGrb, WM_SETFONT, WPARAM(hFont), TRUE);

		hLbl = CreateWindowEx(0, L"STATIC", L"Nội dung ghi chú", WS_CHILD | WS_VISIBLE,
			330, 33, 100, 20, hWnd, NULL, hInst, NULL);
		SendMessage(hLbl, WM_SETFONT, WPARAM(hFont), TRUE);

		hNoteContent = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			330, 53, 468, 393, hWnd, (HMENU)ID_CONTENTEDIT, hInst, NULL);
		SendMessage(hNoteContent, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(hNoteContent, EM_SETCUEBANNER, 0, (LPARAM)TEXT(" Nội dung ghi chú..."));

		hLbl = CreateWindowEx(0, L"STATIC", L"Nhãn", WS_CHILD | WS_VISIBLE,
			330, 463, 30, 20, hWnd, NULL, hInst, NULL);
		SendMessage(hLbl, WM_SETFONT, WPARAM(hFont), TRUE);

		hNoteTags = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
			367, 460, 432, 20, hWnd, (HMENU)ID_TAGSEDIT, hInst, NULL);
		SendMessage(hNoteTags, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(hNoteTags, EM_SETCUEBANNER, 0, (LPARAM)TEXT(" cách nhau bởi dấu phẩy (,)"));

		// Tray icon
		g_Menu = CreatePopupMenu();
		AppendMenu(g_Menu, MF_STRING, ID_LAUNCHAPPMENU, L"Hiện giao diện chính");
		AppendMenu(g_Menu, MF_STRING, ID_LAUNCHNEWNOTEMENU, L"Thêm ghi chú mới");
		AppendMenu(g_Menu, MF_STRING, ID_STATISTICDIALOG, L"Thống kê nhãn");
		AppendMenu(g_Menu, MF_SEPARATOR, 0, L"");
		AppendMenu(g_Menu, MF_STRING, ID_LAUNCHEXITMENU, L"Kết thúc chương trình");

		// Keyboard hook
		doInstallHook(hWnd);
	}
		break;
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
		{
			if (LOWORD(wParam) == ID_NOTELISTVIEW)
			{
				iItm = ListView_GetNextItem(hNoteLsv, -1, LVNI_FOCUSED); // must be -1
				if (-1 != iItm)
				{
					SetWindowText(hNoteContent, wsToWc(eNoteByTag[iItm].getContent()));
					wstring wsTags = L"";
					for (int i = 0; i < eNoteByTag[iItm].getTags().size() - 1; i++)
					{
						wsTags += eNoteByTag[iItm].getTags()[i];
						wsTags += L",";
					}
					wsTags += eNoteByTag[iItm].getTags()[eNoteByTag[iItm].getTags().size() - 1];
					SetWindowText(hNoteTags, wsToWc(wsTags));
				}
			}
		}
			return TRUE;
		case NM_DBLCLK:
			if (LOWORD(wParam) == ID_TAGLISTVIEW)
			{
				eNoteByTag = eNote;
				ListView_DeleteAllItems(hNoteLsv);
				iItm = ListView_GetNextItem(hTagLsv, -1, LVNI_FOCUSED); // musttt beee -1
				if (0 == iItm)
				{
					for (int i = 0; i < eNote.size(); i++)
					{
						addItmToNoteLsv(hNoteLsv, i, eNote[i]);
					}
				}
				else if(0 < iItm && eTag.size() > 0)
				{
					eNoteByTag.clear();
					// Take notes with ids coressponding to note ids in eTag[iItm - 1]
					// iItm - 1 because 0 is for "Tất cả"
					for (int i = 0; i < eTag[iItm - 1].getNoteIds().size(); i++)
					{
						for (int j = 0; j < eNote.size(); j++)
						{
							if (eNote[j].getId() == eTag[iItm - 1].getNoteIds()[i])
							{
								eNoteByTag.push_back(eNote[j]);
							}
						}
					}
					for (int i = 0; i < eNoteByTag.size(); i++)
					{
						addItmToNoteLsv(hNoteLsv, i, eNoteByTag[i]);
					}
				}
				// Jump to tab note
				TabCtrl_SetCurSel(hTabCtrl, 0);
				ShowWindow(hNoteLsv, SW_SHOW);
				ShowWindow(hTagLsv, SW_HIDE);
			}
			return TRUE;
		case TCN_SELCHANGE:
		{
			int iPage = TabCtrl_GetCurSel(hTabCtrl);
			if (-1 != iPage)
			{
				if (0 == iPage)
				{
					ShowWindow(hNoteLsv, SW_SHOW);
					ShowWindow(hTagLsv, SW_HIDE);
				}
				else if (1 == iPage)
				{
					ShowWindow(hNoteLsv, SW_HIDE);
					ShowWindow(hTagLsv, SW_SHOW);
				}
			}
		}
			break;
		default:
			return FALSE;
		}
	return TRUE;
	}
	case WM_SYSCOMMAND:
		switch (wParam & 0xfff0)
		{
		case SC_MINIMIZE:
		case SC_CLOSE:
			minimizeWindow(g_hWnd);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_COMMAND:
    {
		int wmId = LOWORD(wParam); 
		unsigned int szSearch;
		TCHAR *wcSearch = nullptr;
		wstring wsSearch = L"";
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_NEWNOTEBUTTON:
		{
			int res;
			res = DialogBoxW(hInst, MAKEINTRESOURCE(ID_NEWNOTEDIALOG), hWnd, (DLGPROC)newNoteDlgProc);
			if (-1 == res)
			{
				MessageBox(hWnd, L"Lỗi hiển thị hộp thoại", L"Lỗi", MB_ICONWARNING);
			}
		}
			break;
		case ID_VIEWSTATICBUTTON:
		{
			hStatistic = CreateDialog(hInst, MAKEINTRESOURCE(ID_STATISTICDIALOG), hWnd, (DLGPROC)statisticDlgProc);
			InvalidateRect(hStatistic, NULL, TRUE);
			if (!hStatistic)
			{
				MessageBox(hWnd, L"Lỗi hiển thị hộp thoại thống kê.", L"Lỗi", MB_ICONWARNING);
			}
			else
			{
				ShowWindow(hStatistic, SW_SHOW);
			}
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
    }
		break;
	case WM_TRAYICON:
	{
		switch (lParam)
		{
		case WM_LBUTTONUP:
			restoreWindow(hWnd);
			break;
		case WM_RBUTTONDOWN:
			POINT curPnt;
			GetCursorPos(&curPnt);
			SetForegroundWindow(hWnd);
			UINT menuClicked;
			menuClicked = TrackPopupMenu(g_Menu, TPM_RETURNCMD | TPM_NONOTIFY, curPnt.x, curPnt.y, 0, hWnd, NULL);
			switch (menuClicked)
			{
			case ID_LAUNCHAPPMENU:
				restoreWindow(hWnd);
				break;
			case ID_LAUNCHNEWNOTEMENU:
			{
				int res;
				res = DialogBoxW(hInst, MAKEINTRESOURCE(ID_NEWNOTEDIALOG), hWnd, (DLGPROC)newNoteDlgProc);
				if (-1 == res)
				{
					MessageBox(hWnd, L"Lỗi hiển thị hộp thoại", L"Lỗi", MB_ICONWARNING);
				}
				break;
			}
			case ID_LAUNCHEXITMENU:
			{
				Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
				ShowWindow(hWnd, SW_HIDE);
				doUninstallHook(hWnd);
				PostQuitMessage(0);
			}
			break;
			case ID_STATISTICDIALOG:
			{
				hStatistic = CreateDialog(hInst, MAKEINTRESOURCE(ID_STATISTICDIALOG), hWnd, (DLGPROC)statisticDlgProc);
				InvalidateRect(hStatistic, NULL, TRUE);
				if (!hStatistic)
				{
					MessageBox(hWnd, L"Lỗi hiển thị hộp thoại thống kê.", L"Lỗi", MB_ICONWARNING);
				}
				else
				{
					ShowWindow(hStatistic, SW_SHOW);
				}
			}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
    case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
    }
		break;
    case WM_DESTROY:
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
		doUninstallHook(hWnd);
		PostQuitMessage(0);
	}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Statisting
void Swap(tracker & t1, tracker & t2)
{
	tracker aTracker;
	aTracker.index = t1.index;
	aTracker.percent = t1.percent;

	t1.index = t2.index;
	t1.percent = t2.percent;

	t2.index = aTracker.index;
	t2.percent = aTracker.percent;
}

void SelectionSort(vector<tracker>& tracks)
{
	int maxIndex;

	for (int i = 0; i < tracks.size() - 1; i++)
	{
		maxIndex = i;
		for (int j = i + 1; j < tracks.size(); j++)
		{
			if (tracks[maxIndex].percent < tracks[j].percent)
			{
				maxIndex = j;
			}
		}
		Swap(tracks[i], tracks[maxIndex]);
	}
}

void drawChart(HWND hWnd, HDC hDC, vector<tag> eTag)
{
	if (eTag.size() > 0)
	{
		const COLORREF colorRect[6] = { RGB(255,0,0),RGB(255,127,0), RGB(255,255,0), RGB(0,255,0), RGB(0,0,255), RGB(148,0,211) };

		// Calculate total number of notes
		unsigned int sumNNotes = 0;
		for (int i = 0; i < eTag.size(); i++)
		{
			sumNNotes += eTag[i].getNoteIds().size();
		}

		// Calculate percent of each tag
		// use vector, the order of tags is remained:
		// percentPerTag[i] => etag[i]
		vector<tracker> tracks;
		for (int i = 0; i < eTag.size(); i++)
		{
			tracker aTracker;
			aTracker.percent = float(eTag[i].getNoteIds().size()) / float(sumNNotes);
			aTracker.index = i;
			tracks.push_back(aTracker);
		}

		// Sort to pick top 5 tags
		SelectionSort(tracks);

		// Will just draw top 5 tags, the rest combined as one
		if (eTag.size() > 5)
		{
			for (int i = 6; i < tracks.size(); i++)
			{
				tracks[5].percent += tracks[i].percent;
			}
		}

		HWND hLbl;
		int nValidTags = (eTag.size() > 5) ? 6 : eTag.size();
		// Start drawing
		HBRUSH *hBrush = new HBRUSH[nValidTags];
		// Square note
		RECT *sqr = new RECT[nValidTags];
		int iSquareEdge = 15;
		// Chart
		int iFullWidth = 430, *iWidth = new int[nValidTags];
		RECT *rect = new RECT[nValidTags];
		// Width for each chart
		for (int i = 0; i < nValidTags; i++)
		{
			iWidth[i] = (int)(float(iFullWidth)*tracks[i].percent);
		}

		if (nValidTags > 0)
		{
			// Square 00
			sqr[0].left = 20;
			sqr[0].top = 20;
			sqr[0].right = sqr[0].left + iSquareEdge;
			sqr[0].bottom = sqr[0].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", wsToWc(eTag[tracks[0].index].getTagName()),
				WS_CHILD | WS_VISIBLE, sqr[0].left + 22, sqr[0].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 00
			rect[0].left = 20;
			rect[0].top = 90;
			rect[0].right = (nValidTags < 2) ? iFullWidth : rect[0].left + iWidth[0];
			rect[0].bottom = 120;
		}
		if (nValidTags > 1)
		{
			// Square 01
			sqr[1].left = sqr[0].left;
			sqr[1].top = 50;
			sqr[1].right = sqr[1].left + iSquareEdge;
			sqr[1].bottom = sqr[1].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", wsToWc(eTag[tracks[1].index].getTagName()),
				WS_CHILD | WS_VISIBLE, sqr[0].left + 22, sqr[1].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 01
			rect[1].left = rect[0].right;
			rect[1].top = rect[0].top;
			rect[1].right = (nValidTags < 3) ? iFullWidth : rect[1].left + iWidth[1];
			rect[1].bottom = rect[0].bottom;
		}
		if (nValidTags > 2)
		{
			// Square 02
			sqr[2].left = 160;
			sqr[2].top = sqr[0].top;
			sqr[2].right = sqr[2].left + iSquareEdge;
			sqr[2].bottom = sqr[2].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", wsToWc(eTag[tracks[2].index].getTagName()),
				WS_CHILD | WS_VISIBLE, sqr[2].left + 22, sqr[0].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 02
			rect[2].left = rect[1].right;
			rect[2].top = rect[0].top;
			rect[2].right = (nValidTags < 4) ? iFullWidth : rect[2].left + iWidth[2];
			rect[2].bottom = rect[0].bottom;
		}
		if (nValidTags > 3)
		{
			// Square 03
			sqr[3].left = sqr[2].left;
			sqr[3].top = sqr[1].top;
			sqr[3].right = sqr[3].left + iSquareEdge;
			sqr[3].bottom = sqr[3].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", wsToWc(eTag[tracks[3].index].getTagName()),
				WS_CHILD | WS_VISIBLE, sqr[2].left + 22, sqr[1].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 03
			rect[3].left = rect[2].right;
			rect[3].top = rect[0].top;
			rect[3].right = (nValidTags < 5) ? iFullWidth : rect[3].left + iWidth[3];
			rect[3].bottom = rect[0].bottom;
		}
		if (nValidTags > 4)
		{
			// Square 04
			sqr[4].left = 310;
			sqr[4].top = sqr[0].top;
			sqr[4].right = sqr[4].left + iSquareEdge;
			sqr[4].bottom = sqr[4].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", wsToWc(eTag[tracks[4].index].getTagName()),
				WS_CHILD | WS_VISIBLE, sqr[4].left + 22, sqr[0].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 04
			rect[4].left = rect[3].right;
			rect[4].top = rect[0].top;
			rect[4].right = (nValidTags < 6) ? iFullWidth : rect[4].left + iWidth[4];
			rect[4].bottom = rect[0].bottom;
		}
		if (nValidTags > 5)
		{
			// Square 05
			sqr[5].left = sqr[4].left;
			sqr[5].top = sqr[1].top;
			sqr[5].right = sqr[5].left + iSquareEdge;
			sqr[5].bottom = sqr[5].top + iSquareEdge;
			hLbl = CreateWindowEx(0, L"STATIC", (nValidTags == 6) ? wsToWc(eTag[tracks[5].index].getTagName()) : L"Khác",
				WS_CHILD | WS_VISIBLE, sqr[4].left + 22, sqr[1].top + 2, 60, 20, hWnd, NULL, hInst, NULL);
			SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
			// Rect 05
			rect[5].left = rect[4].right;
			rect[5].top = rect[0].top;
			rect[5].right = (iWidth[5] == 0) ? rect[5].left : iFullWidth;
			rect[5].bottom = rect[0].bottom;
		}

		for (int i = 0; i < nValidTags; i++)
		{
			hBrush[i] = CreateSolidBrush(colorRect[i]);
			SelectObject(hDC, hBrush[i]);
			Rectangle(hDC, sqr[i].left, sqr[i].top, sqr[i].right, sqr[i].bottom);
			SelectObject(hDC, hBrush[i]);
			Rectangle(hDC, rect[i].left, rect[i].top, rect[i].right, rect[i].bottom);
			DeleteObject(hBrush[i]);
		}

		if (hBrush) delete hBrush;
		if (sqr) delete sqr;
		if (rect) delete rect;
	}
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL CALLBACK newNoteDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM	lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SAVEBUTTON:
		{
			// Extract text from edit control
			int szContent, szTag;
			szContent = GetWindowTextLength(GetDlgItem(hwnd, ID_NEWNOTECONTENT));
			if (szContent < 1) // empty
			{
				MessageBox(hwnd, L"Ghi chú rỗng.", L"Chú ý", MB_ICONINFORMATION);
			}
			else
			{
				szTag = GetWindowTextLength(GetDlgItem(hwnd, ID_NEWNOTETAGS));
				TCHAR *buffContent = new TCHAR[szContent + 1],
					*buffTag = new TCHAR[szTag + 1];
				GetWindowText(GetDlgItem(hwnd, ID_NEWNOTECONTENT), buffContent, szContent + 1);
				GetWindowText(GetDlgItem(hwnd, ID_NEWNOTETAGS), buffTag, szTag + 1);

				// Saving to databases
				note aNote(idGenerator(eNote),
							wstring(&buffContent[0]),
							wsToWSETag(wstring(&buffTag[0])));
				eNote.push_back(aNote);
				if (-1 == writeNoteDB("notes.db", aNote))
				{
					MessageBox(hwnd, L"Lỗi khi lưu ghi chú.", L"Lỗi", MB_ICONWARNING);
				}
				else
				{
					// Update Note listview
					eNote = readNoteDB("notes.db");
					ListView_DeleteAllItems(hNoteLsv);
					if (eNote.size() < 1)
					{
						MessageBox(hwnd, L"Dữ liệu chưa có hoặc đọc dữ liệu lỗi!", L"Chú ý", MB_ICONWARNING);
					}
					else
					{
						for (int i = 0; i < eNote.size(); i++)
						{
							addItmToNoteLsv(hNoteLsv, i, eNote[i]);
						}
					}
					// Update Tag listview
					if (aNote.getTags().size() > 0)
					{
						if (-1 == writeTagDB("tags.db", aNote, eTag))
							// Nothing was updated
						{
							MessageBox(hwnd, L"Lưu ghi chú thành công.\nLỗi khi lưu nhãn.", L"Lỗi", MB_ICONWARNING);
						}
						else
							// Update Tag listview and eTag
						{
							eTag = readTagDB("tags.db");
							// Sort ascendedly alphabettally
							for (int i = 0; i < eTag.size() - 1; i++)
							{
								for (int j = i + 1; j < eTag.size(); j++)
								{
									if (eTag[i].getTagName() > eTag[j].getTagName())
									{
										tag aTag = eTag[i];
										eTag[i] = eTag[j];
										eTag[j] = aTag;
									}
								}
							}
							ListView_DeleteAllItems(hTagLsv);
							for (int i = 0; i < eTag.size(); i++)
							{
								addItmToTagLsv(hTagLsv, i, eTag[i]);
							}
							addItmToTagLsv(hTagLsv, 0, tag::tag(wstring(L"Tất cả"), vector<unsigned int>()));
							eNoteByTag = eNote;
							EndDialog(hwnd, LOWORD(wParam));
							return (INT_PTR)TRUE;
						}
					}
				}
			}
		}
		break;
		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}

BOOL CALLBACK statisticDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM	lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			return (INT_PTR)TRUE;
		}
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		drawChart(hwnd, hdc, eTag);
		EndPaint(hwnd, &ps);
		break;
	}
	return (INT_PTR)FALSE;
}

// Tray icon
void minimizeWindow(HWND hwnd)
{
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);
	ShowWindow(hwnd, SW_HIDE);
}

void restoreWindow(HWND hwnd)
{
	ShowWindow(hwnd, SW_SHOW);
}

void initNotifyIconData(HWND hWnd)
{
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));
	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = hWnd;
	notifyIconData.uID = ID_TRAYICON;
	notifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	notifyIconData.uCallbackMessage = WM_TRAYICON;
	notifyIconData.hIcon = LoadIcon(GetModuleHandle(NULL) , MAKEINTRESOURCE(IDI_QUICKNOTE));
	wcscpy(notifyIconData.szTip, TEXT("quickNote: Chuột phải để thấy tuỳ chọn."));
}

int doInstallHook(HWND hWnd)
{
	typedef VOID(*PROC)(HWND); // "(" ")" round *PROC is important
	PROC procAddr = NULL;

	hInstLib = LoadLibrary(L"keyboardHook.dll");
	if (hInstLib)
	{
		procAddr = (PROC)GetProcAddress(hInstLib, "?installKeyboardHook@@YGHPAUHWND__@@@Z");
		if (procAddr)
		{
			procAddr(hWnd);
			return 0;
		}
		return -2;
	}
	return -1;
}

int doUninstallHook(HWND hWnd)
{
	typedef VOID(*PROC)(HWND);
	PROC procAddr = NULL;

	hInstLib = LoadLibrary(L"keyboardHook.dll");
	if (hInstLib)
	{
		procAddr = (PROC)GetProcAddress(hInstLib, "?uninstallKeyboardHook@@YGHPAUHWND__@@@Z");
		FreeLibrary(hInstLib);
		if (procAddr)
		{
			procAddr(hWnd);
			return 0;
		}
		return -2;
	}
	return -1;
}