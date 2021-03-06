#pragma once

#include "stdafx.h"
#include "resource.h"
#include <windowsX.h>
#include <winuser.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include <shellapi.h>
#include "convertor.h"
#include "listview.h"
#include "database.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

#define MAX_LOADSTRING 100

using namespace std;

// Global Variables:
static HINSTANCE hInst;                                // current instance
static WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
static WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

BOOL CALLBACK		newNoteDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		statisticDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static INITCOMMONCONTROLSEX icc;
static LOGFONT lf;
static HFONT hFont;

static HWND g_hWnd,
hGrb, hBtn, hTabCtrl,
hLbl,
hNoteContent, hNoteTags, hScrBar,
hStatistic;

static vector<note> eNote; // full list of notes
static vector<tag> eTag;
static vector<note> eNoteByTag;

static int iItm = -1; // which item is chosen in listview

// Statisting
struct tracker {
	float percent;
	int index;
};

void Swap(tracker & t1, tracker & t2);
void SelectionSort(vector<tracker> & tracks);
void drawChart(HWND hWnd, HDC hDC, vector<tag> eTag);

// Tray icon
NOTIFYICONDATA notifyIconData;
HMENU g_Menu;
void minimizeWindow(HWND hwnd);
void restoreWindow(HWND hwnd);
void initNotifyIconData(HWND hWnd);

// Keyboard hooking shortcut creating new note
HINSTANCE hInstLib;
int doInstallHook(HWND hWnd);
int doUninstallHook(HWND hWnd);