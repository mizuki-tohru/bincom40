#define BOOST_TEST_MODULE mytests
#define BOOST_SYSTEM_USE_UTF8
#include <boost/test/included/unit_test.hpp>
#include "stdafx.h"
#include "disp.h"
#include "parser.h"
#include "bincom40.h"
#include "bincom40_test.h"

/*--------------------------------------------------------------------------*/
/* 拡張Bitmapヘッダ構造体													*/
/*--------------------------------------------------------------------------*/
//struct EXBITMAPINFO{
//	BITMAPINFOHEADER	BmInfoHed;
//	int *				bmiColors;
//};
//-------------------------------------------------------------------------
//CreateDIBSectionクラス
//-------------------------------------------------------------------------
class CMemoryDC {
public:
	CMemoryDC() {
		pBit = 0;
		bmp1 = 0;
	}
	//----------------
	BYTE* pBit;
	HBITMAP bmp1;
	HBITMAP bmp2;
	HDC		hdm;
	int		Width;
	int		Height;
	struct EXBITMAPINFO BmInfo;
	BOOL Create(HDC, int, int);
	void Delete(HWND);
	void showbitmap(HDC hDC);
	void writebitmap(HANDLE);
};
//---------------------------------------------------------------------------
//　画像再描画
//　引数1	:デバイスコンテキストのハンドル
//　戻り値	:なし
//---------------------------------------------------------------------------
void CMemoryDC::showbitmap(HDC hDC)
{
	BitBlt(hDC, 0, 0, Width, Height, hdm, 0, 0, SRCCOPY);
}
//---------------------------------------------------------------------------
//　CreateDIBSectionオブジェクトの初期化
//　引数1	:デバイスコンテキストのハンドル
//　引数2	:生成画像横幅
//　引数3	:生成画像高さ
//　戻り値	:真=成功/偽=失敗
//---------------------------------------------------------------------------
BOOL CMemoryDC::Create(HDC hdc, int w, int h)
{
	Width = w;
	Height = h;
	ZeroMemory((PVOID)&BmInfo, sizeof(struct EXBITMAPINFO));
	//---DIBパラメータ設定---
	BmInfo.BmInfoHed.biSize = sizeof(BITMAPINFOHEADER);
	BmInfo.BmInfoHed.biWidth = Width;		// DIB横幅
	BmInfo.BmInfoHed.biHeight = Height;		// DIB高さ
	BmInfo.BmInfoHed.biPlanes = 1;			// プレーンの数(基本は1)
	BmInfo.BmInfoHed.biBitCount = 24;			// プレーンの色数
	BmInfo.BmInfoHed.biCompression = BI_RGB;
	BmInfo.BmInfoHed.biSizeImage = 0;
	BmInfo.BmInfoHed.biXPelsPerMeter = 0;
	BmInfo.BmInfoHed.biYPelsPerMeter = 0;
	BmInfo.BmInfoHed.biClrUsed = 0;
	BmInfo.BmInfoHed.biClrImportant = 0;
	//	BmInfo.bmiColors = NULL;
	BmInfo.bmiColors;
	hdm = CreateCompatibleDC(hdc);
	bmp1 = CreateDIBSection(hdm, (LPBITMAPINFO)&BmInfo, DIB_RGB_COLORS
		, (VOID**)&pBit, NULL, 0);
	if (bmp1 == NULL) return FALSE;
	if (pBit == NULL) return FALSE;
	bmp2 = (HBITMAP)SelectObject(hdm, bmp1);
	return TRUE;
}
//---------------------------------------------------------------------------
//　CreateDIBSectionオブジェクトの廃棄
//　引数1	:ウィンドウのハンドル
//　戻り値	:無し

//---------------------------------------------------------------------------
void CMemoryDC::Delete(HWND hWnd)
{
	ReleaseDC(hWnd, hdm);
}
//---------------------------------------------------------------------------
//　画像をbitmapでファイルに書き出し
//　引数1	:ファイルのハンドル
//　戻り値	:なし
//---------------------------------------------------------------------------
void CMemoryDC::writebitmap(HANDLE fh)
{
	BITMAPFILEHEADER bmfHeader = { 0 };
	DWORD dwFSize;

	bmfHeader.bfType = *(LPWORD)"BM";
	bmfHeader.bfSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER)
		+ 256 * 4
		+ (Width * Height) * 3;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER)
		+ 256 * 4
		+ sizeof(BITMAPINFOHEADER);
	WriteFile(fh, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwFSize, NULL);
	WriteFile(fh, &BmInfo, sizeof(BITMAPINFOHEADER), &dwFSize, NULL);
	WriteFile(fh, pBit, (Width * Height) * 3, &dwFSize, NULL);
}
//---------------------------------------------------------------------------
//ハンドルを渡されたウインドゥとファイルを比較する
//戻り値:1 一致 0:不一致
int compWindow(HWND hwnd, LPCWSTR lpszFn, LPCWSTR lpszFm)
{
	DWORD dwSize, dwFSize, dwWidth, dwHeight, dwLength;
	HANDLE fh;
	HANDLE fm;
	LPBITMAPFILEHEADER lpHead;
	LPBITMAPINFOHEADER lpInfo;
	LPBYTE lpBuf, lpPixel;
	LPBYTE lpBuf2;
	RECT rec;
	HDC hdc, hdcMem;
	HBITMAP hBMP, hOld;
	int flg;
	unsigned long i;

	GetClientRect(hwnd, &rec); /* クライアント領域取得 */
	dwWidth = rec.right;
	dwHeight = rec.bottom;
	if ((dwWidth * 3) % 4 == 0) { /* バッファの１ラインの長さを計算 */
		dwLength = dwWidth * 3;
	}
	else {
		dwLength = dwWidth * 3 + (4 - (dwWidth * 3) % 4);
	}
	/* 書き込み用バッファのサイズ計算 */
	dwFSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER) + dwLength * dwHeight;
	/* バッファ確保とポインタ設定 */
	lpBuf = (LPBYTE)GlobalAlloc(GPTR, dwFSize);
	lpHead = (LPBITMAPFILEHEADER)lpBuf;
	lpInfo = (LPBITMAPINFOHEADER)(lpBuf + sizeof(BITMAPFILEHEADER));
	lpPixel = lpBuf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	/* 24ビットBMPファイルのヘッダ作成 */
	lpHead->bfType = 'M' * 256 + 'B';
	lpHead->bfSize = dwFSize;
	lpHead->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	lpInfo->biSize = sizeof(BITMAPINFOHEADER);
	lpInfo->biWidth = dwWidth;
	lpInfo->biHeight = dwHeight;
	lpInfo->biPlanes = 1;
	lpInfo->biBitCount = 24;

	/* ウインドウのデバイスコンテキスト取得 */
	hdc = GetDC(hwnd);
	/* ウインドウのデバイスコンテキスト互換のBITMAP作成 */
	hBMP = CreateCompatibleBitmap(hdc, dwWidth, dwHeight);

	/* BITMAPにウインドウのクライアント領域をコピー */
	hdcMem = CreateCompatibleDC(hdc);
	hOld = (HBITMAP)(SelectObject(hdcMem, hBMP));
	BitBlt(hdcMem, 0, 0, dwWidth, dwHeight, hdc, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	GetDIBits(hdc, hBMP, 0, dwHeight, lpPixel, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);

	ReleaseDC(hwnd, hdc);
	DeleteObject(hBMP);
	DeleteObject(hdcMem);

	fm = CreateFileW(lpszFm, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(fm, lpBuf, dwFSize, &dwSize, NULL);
	CloseHandle(fm);
	/* バッファをファイルに書き出す */
	fh = CreateFileW(lpszFn, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	lpBuf2 = (LPBYTE)malloc(GetFileSize(fh, NULL));
	ReadFile(fh, lpBuf2, dwFSize, &dwSize, NULL);
	flg = 1;
	for (i = 0; i < dwFSize; i++) {
		if (lpBuf[i] != lpBuf2[i])flg = 0;
	}
	//printf("%d %d %d\n", flg,i,dwSize);
	CloseHandle(fh);
	GlobalFree(lpBuf);
	free(lpBuf2);
	return flg;
}
//---------------------------------------------------------------------------
void saveBMP(HWND hwnd, LPCTSTR lpszFn)
{
	DWORD dwSize, dwFSize, dwWidth, dwHeight, dwLength;
	HANDLE fh;
	LPBITMAPFILEHEADER lpHead;
	LPBITMAPINFOHEADER lpInfo;
	LPBYTE lpBuf, lpPixel;
	RECT rec;
	HDC hdc, hdcMem;
	HBITMAP hBMP, hOld;

	GetClientRect(hwnd, &rec); /* クライアント領域取得 */
	dwWidth = rec.right;
	dwHeight = rec.bottom;
	if ((dwWidth * 3) % 4 == 0) { /* バッファの１ラインの長さを計算 */
		dwLength = dwWidth * 3;
	}
	else {
		dwLength = dwWidth * 3 + (4 - (dwWidth * 3) % 4);
	}
	/* 書き込み用バッファのサイズ計算 */
	dwFSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER) + dwLength * dwHeight;
	/* バッファ確保とポインタ設定 */
	lpBuf = (LPBYTE)GlobalAlloc(GPTR, dwFSize);
	lpHead = (LPBITMAPFILEHEADER)lpBuf;
	lpInfo = (LPBITMAPINFOHEADER)(lpBuf + sizeof(BITMAPFILEHEADER));
	lpPixel = lpBuf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	/* 24ビットBMPファイルのヘッダ作成 */
	lpHead->bfType = 'M' * 256 + 'B';
	lpHead->bfSize = dwFSize;
	lpHead->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	lpInfo->biSize = sizeof(BITMAPINFOHEADER);
	lpInfo->biWidth = dwWidth;
	lpInfo->biHeight = dwHeight;
	lpInfo->biPlanes = 1;
	lpInfo->biBitCount = 24;

	/* ウインドウのデバイスコンテキスト取得 */
	hdc = GetDC(hwnd);
	/* ウインドウのデバイスコンテキスト互換のBITMAP作成 */
	hBMP = CreateCompatibleBitmap(hdc, dwWidth, dwHeight);

	/* BITMAPにウインドウのクライアント領域をコピー */
	hdcMem = CreateCompatibleDC(hdc);
	hOld = (HBITMAP)(SelectObject(hdcMem, hBMP));
	BitBlt(hdcMem, 0, 0, dwWidth, dwHeight, hdc, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	GetDIBits(hdc, hBMP, 0, dwHeight, lpPixel, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);

	ReleaseDC(hwnd, hdc);
	DeleteObject(hBMP);
	DeleteObject(hdcMem);
	/* バッファをファイルに書き出す */
	fh = CreateFile(lpszFn, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(fh, lpBuf, dwFSize, &dwSize, NULL);
	CloseHandle(fh);
	GlobalFree(lpBuf);
}


using namespace boost;
using namespace boost::unit_test_framework;
HWND GetConsoleHwnd(void);
HWND InitTestWindow(int, int, char*);
LRESULT CALLBACK TestWndProc(HWND, UINT, WPARAM, LPARAM);

//void test_rchar(void);
//void test_cgetchar(void);
//void test_ltohex(void);
//void test_axtol(void);
//void test_scroll(void);
//void test_charset(void);
//void test_SendDataP(void);
//void test_SendDataPK(void);
//void test_HexDataP(void);
//void test_HexDataREP(void);
//void test_daytime_write(void);
//void test_mzParser(void);
//void test_mxParser(void);
void test_MyRegisterClass(void);
void test_InitInstance(void);
void test_WndProc(void);
void test_About(void);
void test_dlgProc(void);
void test_dlgProc2(void);
void test_ThreadFunc(void);

/*--------------------------------------------------------------------*/
HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // コンソールウィンドウのタイトル用のバッファサイズ
	HWND hwndFound;         // 呼び出し側へ返される値
	//	char pszNewWindowTitle[MY_BUFSIZE];
	WCHAR pszNewWindowTitle[MY_BUFSIZE];
	// 作成されるウィンドウのタイトルが入ります
//  char pszOldWindowTitle[MY_BUFSIZE]; // 元のウィンドウタイトルが入ります
	WCHAR pszOldWindowTitle[MY_BUFSIZE]; // 元のウィンドウタイトルが入ります

	// 現在のウィンドウタイトルを取得します
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
	// 独自に、ウィンドウの新規タイトルをフォーマットします
	wsprintf(pszNewWindowTitle, L"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());
	// 現在のウィンドウタイトルを変更します
	SetConsoleTitle(pszNewWindowTitle);
	// ウィンドウタイトルのアップデートを確実なものにさせます
	Sleep(40);
	// ウィンドウの新規タイトルを探しにいきます
	hwndFound = FindWindow(NULL, pszNewWindowTitle);
	// 元のウィンドウタイトルへ戻します
	SetConsoleTitle(pszOldWindowTitle);
	return(hwndFound);
}
/*--------------------------------------------------------------------*/
LRESULT CALLBACK TestWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC	hdc;

	switch (message) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
/*-----------------------------------------------------------------------*/
ATOM TestRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	//	LoadString(hInstance,"TEST", szWindowClass, 100);

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = TestWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	//	wcex.lpszClassName	= szWindowClass;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}
/*--------------------------------------------------------------------*/
HWND InitTestWindow(int Width, int Height, wchar_t* WindowName)
{
	RECT rect;
	HWND TWnd;
	HINSTANCE hInst = GetModuleHandle(NULL);

	rect.left = 0;
	rect.top = 0;
	rect.right = Width;
	rect.bottom = Height;
	LoadStringW(hInst, IDC_BINCOM40, szWindowClass, 100);

	//	MyRegisterClass(hInst);
	TestRegisterClass(hInst);
	TWnd = CreateWindowW(szWindowClass, LPCWSTR(WindowName),
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right - rect.left),
		(rect.bottom - rect.top),
		nullptr,
		nullptr,
		hInst,
		nullptr);
	ShowWindow(TWnd, SW_SHOW);
	UpdateWindow(TWnd);
	return TWnd;
}

BOOST_AUTO_TEST_CASE(myTestCase)
{
	hNewFont = CreateFont(12,    //フォント高さ
		12,                   //文字幅
		0,                    //テキストの角度
		0,                    //ベースラインとｘ軸との角度
		FW_REGULAR,            //フォントの重さ（太さ）
		FALSE,                //イタリック体
		FALSE,                //アンダーライン
		FALSE,                //打ち消し線
		SHIFTJIS_CHARSET,    //文字セット
		OUT_DEFAULT_PRECIS,    //出力精度
		CLIP_DEFAULT_PRECIS,//クリッピング精度
		PROOF_QUALITY,        //出力品質
		FIXED_PITCH | FF_MODERN,//ピッチとファミリー
		(LPCTSTR)L"ＭＳ ゴシック");

	hDispFont = CreateFont(15,    //フォント高さ
		0,                    //文字幅
		0,                    //テキストの角度
		0,                    //ベースラインとｘ軸との角度
		FW_REGULAR,            //フォントの重さ（太さ）
		FALSE,                //イタリック体
		FALSE,                //アンダーライン
		FALSE,                //打ち消し線
		SHIFTJIS_CHARSET,    //文字セット
		OUT_DEFAULT_PRECIS,    //出力精度
		CLIP_DEFAULT_PRECIS,//クリッピング精度
		PROOF_QUALITY,        //出力品質
		FIXED_PITCH | FF_MODERN,//ピッチとファミリー
		(LPCTSTR)L"Courier");

	hUFont = CreateFontW(15,    //フォント高さ
		0,                    //文字幅
		0,                    //テキストの角度
		0,                    //ベースラインとｘ軸との角度
		FW_REGULAR,            //フォントの重さ（太さ）
		FALSE,                //イタリック体
		FALSE,                //アンダーライン
		FALSE,                //打ち消し線
		SHIFTJIS_CHARSET,    //文字セット
		OUT_DEFAULT_PRECIS,    //出力精度
		CLIP_DEFAULT_PRECIS,//クリッピング精度
		PROOF_QUALITY,        //出力品質
		FIXED_PITCH | FF_MODERN,//ピッチとファミリー
		(LPCWSTR)L"ＭＳ ゴシック");

	WSIZE_WIDTH = 530;
	WSIZE_HEIGHT = 504;

	BOOST_TEST(1 == 1);
	BOOST_TEST(true);
//	BOOST_TEST(test_rchar);
//	BOOST_TEST(test_cgetchar);
//	BOOST_TEST(test_ltohex);
//	BOOST_TEST_CASE(test_axtol);
//	BOOST_TEST_CASE(test_scroll);
//	BOOST_TEST_CASE(test_charset);
//	BOOST_TEST_CASE(test_SendDataP);
//	BOOST_TEST_CASE(test_SendDataPK);
//	BOOST_TEST_CASE(test_HexDataP);
//	BOOST_TEST_CASE(test_daytime_write);
//	BOOST_TEST_CASE(test_mzParser);
//	BOOST_TEST_CASE(test_mxParser);
	BOOST_TEST_CASE(test_About);
	BOOST_TEST_CASE(test_dlgProc);
	BOOST_TEST_CASE(test_dlgProc2);
}


#if (TEST == 0)
//------------------------------------------------------------------
test_suite* init_unit_test_suite(int argc, char* argv[])
{
	// テストスイートを準備
	boost::unit_test_framework::test_suite* test = BOOST_TEST_SUITE("Hoge Test");
	//--------------------------------------------------
	test->add(BOOST_TEST_CASE(test_rchar));
	test->add(BOOST_TEST_CASE(test_cgetchar));
	test->add(BOOST_TEST_CASE(test_ltohex));
	test->add(BOOST_TEST_CASE(test_axtol));
	test->add(BOOST_TEST_CASE(test_scroll));
	test->add(BOOST_TEST_CASE(test_charset));
	test->add(BOOST_TEST_CASE(test_SendDataP));
	test->add(BOOST_TEST_CASE(test_SendDataPK));
	test->add(BOOST_TEST_CASE(test_HexDataP));
	test->add(BOOST_TEST_CASE(test_daytime_write));
	test->add(BOOST_TEST_CASE(test_mzParser));
	test->add(BOOST_TEST_CASE(test_mxParser));
	test->add(BOOST_TEST_CASE(test_CRC16));
	//	test->add( BOOST_TEST_CASE( test_MyRegisterClass ));
	//	test->add( BOOST_TEST_CASE( test_InitInstance ));
	//	test->add( BOOST_TEST_CASE( test_WndProc ));
	test->add(BOOST_TEST_CASE(test_About));
	test->add(BOOST_TEST_CASE(test_dlgProc));
	test->add(BOOST_TEST_CASE(test_dlgProc2));
	//	test->add( BOOST_TEST_CASE( test_ThreadFunc ));

	return test;
}
/*------------------------------------------------------------------------*/
#endif

/*---void rchar(FILE *)-----------------------------------------*/
//void test_rchar()
//BOOST_AUTO_TEST_CASE (test_rchar)
BOOST_AUTO_TEST_CASE(test_rchar)
{
	FILE* Fh;
	C_C.flg = 0;
	C_C.text[0] = 0;
	char dummy[20];

	/*ファイルハンドラを受け取り、一文字づつ読み取って分類し、ものに*/
	/*よっては連続して読み出して、なんらかの単位にして返す*/
	fopen_s(&Fh, "test_rchar_001.txt", "rt");
	BOOST_CHECK_MESSAGE(Fh != NULL, Fh);

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "abc")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "ABC")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "a123bCDE_FG")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "_12345")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "12345")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "6789")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '+', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '-', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "hoe")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '*', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "dame")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '/', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "damedame")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '%', C_C.flg);

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "foo")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '=', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Boo")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 11, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 12, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '{', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 13, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == '}', C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 14, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 15, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, C_C.flg);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 16, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "fuga")), C_C.text);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 19, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "Hoge || fuga")), C_C.Wtext);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 19, dummy);
//	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "ほげ")), C_C.text);
	BOOST_CHECK_MESSAGE(!(wcscmp((WCHAR *)C_C.Wtext, L"ほげ")), (WCHAR *)C_C.Wtext);
//	printf("ほげ");
//	printf(" , ");
//	wprintf(C_C.Wtext);
//	BOOST_CHECK_MESSAGE(!(wcscmp((WCHAR*)C_C.Wtext, L"ほげ")), (WCHAR *)"ほげ");
	C_C.flg = 0;
	C_C.text[0] = 0;

	rchar(Fh);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, C_C.flg);
	C_C.flg = 0;
	C_C.text[0] = 0;

	fclose(Fh);
}
//BOOST_AUTO_TEST_SUITE_END()
/*---int cgetchar(FILE *)---------------------------------------*/
//void test_cgetchar()
BOOST_AUTO_TEST_CASE(test_cgetchar)
{
	FILE* Fh;
	C_C.flg = 0;
	C_C.text[0] = 0;
	char dummy[20];

	/*ファイルハンドラを受け取り、一文字づつ読み取って分類し、ものに*/
	/*よっては連続して読み出して、なんらかの単位にして返す*/
	fopen_s(&Fh, "test_cgetchar_001.txt", "rt");
	BOOST_CHECK_MESSAGE(Fh != NULL, Fh);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "1234")), C_C.text);
	_ltoa_s(C_C.data, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.data == 1234, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0x1234")), C_C.text);
	_ltoa_s(C_C.data, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.data == 0x1234, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0Xabcdef")), C_C.text);
	_ltoa_s(C_C.data, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.data == 0xabcdef, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0b01011111")), C_C.text);
	_ltoa_s(C_C.data, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.data == 0x5f, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 2, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0B00000001")), C_C.text);
	_ltoa_s(C_C.data, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.data == 1, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0x00XX")), C_C.text);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0XWWW1")), C_C.text);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "0b22222222")), C_C.text);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 1, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp(C_C.text, "1312x5877")), C_C.text);

	cgetchar(Fh);
	_ltoa_s(C_C.flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(C_C.flg == 0x0a, dummy);

	fclose(Fh);
}
/*---int ltohex(unsigned long ,unsigned char *,unsigned char)---*/
//void test_ltohex()
BOOST_AUTO_TEST_CASE(test_ltohex)
{
	/*あらゆるunsigned long型数値を受け取る*/
	/*バッファに16進文字列にして返す*/
	/*返す桁数は指定されたサイズで*/

	/*戻る値の最大値は0xffffffff、つまり8桁。8桁以上の値を受け取ったら*/
	/*無視するか？*/

	long in;
	int ret;
	char dummy[20];
	unsigned char buf[256];
	/*全数検査が理想だが、それは無理だ*/

	in = 0;

	ret = ltohex((unsigned long)in, buf, 0);/*出力バイト長指定が0*/
	_ltoa_s(in, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 0, in);

	ret = ltohex((unsigned long)in, buf, 8);/*出力バイト長指定が8*/
	_ltoa_s(ret, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 8, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp((char*)buf, "00000000")), buf);

	ret = ltohex((unsigned long)in, buf, 16);/*出力バイト長指定が16*/
	_ltoa_s(ret, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 16, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp((char*)buf, "0000000000000000")), buf);

	in = 0x100;

	ret = ltohex((unsigned long)in, buf, 8);/*出力バイト長指定が8*/
	_ltoa_s(ret, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 8, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp((char*)buf, "00000100")), buf);

	in = 0xDEADBEEF;

	ret = ltohex((unsigned long)in, buf, 8);/*出力バイト長指定が8*/
	_ltoa_s(ret, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 8, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp((char*)buf, "DEADBEEF")), buf);

	in = -100;

	ret = ltohex((unsigned long)in, buf, 8);/*出力バイト長指定が8*/
	_ltoa_s(in, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(ret == 8, dummy);
	BOOST_CHECK_MESSAGE(!(strcmp((char*)buf, "FFFFFF9C")), buf);

}
/*------------------------------------------------------------------------*/
/*---int axtol(char * ,unsigned char * ,unsigned int)---*/
//void test_axtol()
BOOST_AUTO_TEST_CASE(test_axtol)
{
	/*あらゆるバイナリデータ型数値を受け取る*/
	/*できれば数値文字を受け取りたい*/
	/*バッファにバイナリにして返す*/
	/*受け取りバッファは4バイト*/

	unsigned char retbuf[256];
	//	char inbuf[255];
	int ret;
	ret = axtol("0x100", retbuf, 0);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0x0033", retbuf, 6);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[0] == 0x00, retbuf[0]);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0x33, retbuf[1]);

	ret = axtol("hoge", retbuf, 4);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0000hoge", retbuf, 4);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0x00, retbuf[0]);

	ret = axtol("0000hoge", retbuf, 8);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0x2021hoge", retbuf, 6);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[2] == 0x20, retbuf[0]);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0x21, retbuf[1]);

	ret = axtol("0X2021hoge", retbuf, 10);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0b100", retbuf, 0);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0b00110011", retbuf, 10);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0x33, retbuf[0]);

	ret = axtol("0b0555", retbuf, 6);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("0b0011001110101010", retbuf, 18);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[2] == 0x33, retbuf[0]);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0xAA, retbuf[1]);

	ret = axtol("123", retbuf, 0);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("123", retbuf, 3);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[2] == 0x00, retbuf[0]);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0x7B, retbuf[1]);

	ret = axtol("1234", retbuf, 4);/*1を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 1, ret);
	BOOST_CHECK_MESSAGE(retbuf[2] == 0x04, retbuf[0]);
	BOOST_CHECK_MESSAGE(retbuf[3] == 0xD2, retbuf[1]);

	ret = axtol("1X34", retbuf, 4);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("1a34", retbuf, 4);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

	ret = axtol("1234", retbuf, 10);/*0を返すべき*/
	BOOST_CHECK_MESSAGE(ret == 0, ret);

}
/*------------------------------------------------------------------------*/
/*---void scroll(unsigned int)---*/
//void test_scroll()
BOOST_AUTO_TEST_CASE(test_scroll)
{
	int i, j, flg;
	//	char frm[24][65];		//画面表示キャラクタバッファ
	//	char frmm[LINGBUF_SIZE];		//画面表示キャラクタバッファ
	long RtnPos[RTN_SIZE + 2];
	unsigned long RtnN;
	unsigned long FstN;
	unsigned long stpoint; //表示先頭位置
	unsigned long edpoint; //表示末尾位置

	//	for(i=0;i<LINGBUF_SIZE;i++)frmm[i&LINGBUF_MAX] = 0x20;
	//    for(i=0;i<RTN_SIZE;i++){
		//	frm_U[i&0xff] = (Display_flg&0x01);//ASCII
	//		RtnPos[i] = -1;//改行位置
	//	}
	//	RtnN = 0;
	//	FstN = 0;
	//	stpoint = 0; //表示先頭位置
	//	edpoint = 0; //表示末尾位置

	//	for(i=0;i<24;i++){
	//		for(j=0;j<65;j++){
	//			frm[i][j] = 'a' + i;
	//		}
	//	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = ('a' + (i & 0xff)) & 0xff;

	scroll(0);
	for (i = 0; i < 24; i++) {
		flg = 0;
		for (j = 0; j < 65; j++) {
			//		if(frm[i][j] != (char)('a' + 0 + i)){
			if (frmm[(i * 24) + j] != (char)('a' + 0 + i)) {
				flg = 1;
				//	BOOST_CHECK_MESSAGE(flg,frm[i][j]);/*ちゃんと動いていない*/
				BOOST_CHECK_MESSAGE(flg, frmm[(i * 24) + j]);/*ちゃんと動いていない*/
				BOOST_CHECK_MESSAGE(flg, (char)('a' + i));/*ちゃんと動いていない*/
				flg = 0;
			}
		}
	}

	scroll(1);
	for (i = 1; i < 24; i++) {
		flg = 0;
		for (j = 0; j < 65; j++) {
			//		if(frm[i][j] != (char)('a' + 1 + i))flg = 1;
			if (frmm[(i * 24) + j] != (char)('a' + 1 + i))flg = 1;
		}
		//	BOOST_CHECK_MESSAGE(flg,frm[i]);
		BOOST_CHECK_MESSAGE(flg, frmm[i]);
	}

	scroll(10);
	for (i = 10; i < 24; i++) {
		flg = 0;
		for (j = 0; j < 65; j++) {
			//		if(frm[i][j] != (char)('a' + 10 + i))flg = 1;
			if (frmm[(i * 24) + j] != (char)('a' + 10 + i))flg = 1;
		}
		//	BOOST_CHECK_MESSAGE(flg,frm[i]);
		BOOST_CHECK_MESSAGE(flg, frmm[i]);
	}

	scroll(23);
	for (i = 23; i < 24; i++) {
		flg = 0;
		for (j = 0; j < 65; j++) {
			//		if(frm[i][j] != (char)('a' + 23 + i))flg = 1;
			if (frmm[(i * 24) + j] != (char)('a' + 23 + i))flg = 1;
		}
		//	BOOST_CHECK_MESSAGE(flg,frm[i]);
		BOOST_CHECK_MESSAGE(flg, frmm[i]);
	}

	scroll(24);/*画面サイズを超える入力*/
	for (i = 24; i < 24; i++) {
		flg = 0;
		for (j = 0; j < 65; j++) {
			//		if(frm[i][j] != (char)('a' + 24 + i))flg = 1;
			if (frmm[(i * 24) + j] != (char)('a' + 24 + i))flg = 1;
		}
		//	BOOST_CHECK_MESSAGE(flg,frm[i]);
		BOOST_CHECK_MESSAGE(flg, frmm[i]);
	}
}
/*-------------------unsigned char charset(unsigned char)-------*/
//void test_charset()
BOOST_AUTO_TEST_CASE(test_charset)
{
	/*charset()は、以下に定義するアスキーキャラクタのみを出力しなければ*/
	/*ならない*/
	char asc[] = { ' ','!','"','#','$','%','&',0x27,'(',')','*','+',	//12
				',','-','.','/',':',';','<','=','>','?','@',		//11
				'0','1','2','3','4','5','6','7','8','9',			//10
				'A','B','C','D','E','F','G','H','I','J','K','L','M',	//13
				'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',	//13
				'[',0x5c,']','^','_','`','{','|','}','~',			//10
				'a','b','c','d','e','f','g','h','i','j','k','l','m',	//13
				'n','o','p','q','r','s','t','u','v','w','x','y','z' };	//13
	int i, flg;
	unsigned int j;
	for (i = 0; i < 255; i++) {
		flg = 0;
		for (j = 0; j < strlen(asc); j++) {
			if ((unsigned char)(asc[j]) == charset(i))flg = 1;
		}
		BOOST_CHECK_MESSAGE(flg, i);
	}
}
/*-------------------void SendDataP(HWND,unsigned char *,DWORD)-----------*/
//void test_SendDataP()
BOOST_AUTO_TEST_CASE(test_SendDataP)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[65];
	//	char getbuf[100];
	sRtnN = 0;
	sFstN = 0;
	sstpoint = 0; //表示先頭位置
	sedpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		sfrm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		sRtnPos[i] = -1;//改行位置
	}
	sRtnPos[0] = 0;
	sfrm_U[0] = 1;
	for (j = 0; j < SENDBUF_SIZE; j++)sfrmm[j & SENDBUF_MAX] = 0x20;

	for (i = 0; i < 6; i++) {
		//		for(j=0;j<65;j++)sfrm[i][j] = 0x20;
		for (j = 0; j < 65; j++)sfrmm[(i * 6) + j] = 0x20;
	}
	for (j = 0; j < 65; j++)buf[j] = 0x20;

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_003");
	Sleep(100);

	BOOST_TEST_MESSAGE("Fuga2");

	buf[0] = 0x01;
	buf[1] = 0x02;
	buf[2] = 0x03;
	buf[3] = 0x04;
	buf[4] = 0x05;

	SendDataP(fugaWnd, buf, 5);
	SetActiveWindow(fugaWnd);
	Sleep(100);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_003.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_003_org.bmp"), LPCWSTR(L"TEST_003_make.bmp")) == 1);

	//printf("%d,%d,%d,%d,%d\n", sfrmm[4], sfrmm[5], sRtnPos[0], sRtnPos[1],sRtnN);
	//printf("%d,%d,%d\n", sFstN, sfrm_U[0], sfrm_U[1]);
	Sleep(1000);
	BOOST_TEST_MESSAGE("Fuga2A");

	//	len = GetWindowText(fugaWnd,LPTSTR(getbuf),100);
	//	BOOST_CHECK_MESSAGE(len == 8,len);
	//	BOOST_CHECK_MESSAGE(getbuf == "TEST_003",getbuf);

	DestroyWindow(fugaWnd);
}
/*-------------------void SendDataPK(HWND,unsigned char)-----------*/
//void test_SendDataPK()
BOOST_AUTO_TEST_CASE(test_SendDataPK)
{
	int i, j;
	unsigned char ch;
	HWND fugaWnd;
	key.spnflg = 0;
	key.spline = 0;
	key.spchar = 0;
	sRtnN = 0;
	sFstN = 0;
	sstpoint = 0; //表示先頭位置
	sedpoint = 0; //表示末尾位置
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		sfrm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		frm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		RtnPos[i] = -1;//改行位置
		sRtnPos[i] = -1;//改行位置
	}
	sRtnPos[0] = 0;
	RtnPos[0] = 0;
	sfrm_U[0] = 1;
	for (j = 0; j < SENDBUF_SIZE; j++)sfrmm[j & SENDBUF_MAX] = 0x20;
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;

	for (i = 0; i < 6; i++) {
		//	for(j=0;j<65;j++)sfrm[i][j] = 0x20;
		for (j = 0; j < 65; j++)sfrmm[(i * 6) + j] = 0x20;
	}

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_002");
	Sleep(100);

	sfrm_U[sRtnN & 0xff] = 0;

	ch = 0x30;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	Sleep(100);
	ch = 0x31;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	Sleep(100);
	ch = 0x32;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	Sleep(100);
	ch = 0x20;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	Sleep(100);
	ch = 0x33;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	key.spnflg = 0;
	Sleep(100);
	ch = 0x0d;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);

	sfrm_U[sRtnN & 0xff] = 0;

	BOOST_TEST_MESSAGE("Fuga1");

	Sleep(100);
	ch = 0x40;
	SendDataPK(fugaWnd, ch);
	HexDataREP(fugaWnd);
	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_002.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_002_org.bmp"), LPCWSTR(L"TEST_002_make.bmp")) == 1);

	Sleep(1000);
	BOOST_TEST_MESSAGE("Fuga1A");

	DestroyWindow(fugaWnd);
}
/*-------------------void HexDataP(HWND,unsigned char * ,DWORD)-----------*/
//void test_HexDataP()
BOOST_AUTO_TEST_CASE(test_HexDataP)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFG";
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		RtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	frm_U[0] = 1;
	RtnPos[0] = 0;
	
	BOOST_TEST_MESSAGE("Fuga3");
	printf("Fuga3\n");

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_004");
//	Sleep(1000);

	HexDataP(fugaWnd, buf, 7);
//	printf("Fuga3A\n");
	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_004.bmp"));
//	printf("Fuga3B\n");
//	Sleep(1000);
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_004_org.bmp"), LPCWSTR(L"TEST_004_make.bmp")) == 1);
	printf("Fuga3C\n");

	Sleep(1000);
	DestroyWindow(fugaWnd);
}

/*-------------------void HexDataP2(HWND,unsigned char * ,DWORD)-----------*/
//void test_HexDataP2()
BOOST_AUTO_TEST_CASE(test_HexDataP2)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFGHIJKLMNOPQRSTUVWXY";
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = 0x01;
		RtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	frm_U[0] = 1;
	RtnPos[0] = 0;

	BOOST_TEST_MESSAGE("Fuga4");

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_004A");
	for (i = 0; i < 10; i++) {
		HexDataP(fugaWnd, buf, 25);
		printf("FstN = %d RtnN = %d RtnPos = %d RtnPos = %d %d %d %d %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3], TESTDBG[4], TESTDBG[5], TESTDBG[6], TESTDBG[7]);
		Sleep(1000);
	}
	//printf("ws = %d vs = %d hs = %d wh = %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3]);

	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_004.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_004A_org.bmp"), LPCWSTR(L"TEST_004A_make.bmp")) == 1);

	Sleep(1000);
	DestroyWindow(fugaWnd);
}
/*-------------------void HexDataP3(HWND,unsigned char * ,DWORD)-----------*/
//void test_HexDataP3()
BOOST_AUTO_TEST_CASE(test_HexDataP3)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFGHIJKLMNOPQRSTUVWXY";
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = 0x01;
		RtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	frm_U[0] = 1;
	RtnPos[0] = 0;
	TESTDBG[8] = 0;

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_004B");
	for (i = 0; i < 60; i++) {
		HexDataP(fugaWnd, buf, 25);
		if(i>30)printf("FstN = %d RtnN = %d RtnPos = %d RtnPos = %d %d %d %d %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3], TESTDBG[4], TESTDBG[5], TESTDBG[6], TESTDBG[7]);
		TESTDBG[8]++;
		Sleep(100);
	}
	//printf("ws = %d vs = %d hs = %d wh = %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3]);

	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_004.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_004B_org.bmp"), LPCWSTR(L"TEST_004B_make.bmp")) == 1);

	Sleep(1000);
	DestroyWindow(fugaWnd);
}
/*-------------------void HexDataP4(HWND,unsigned char * ,DWORD)-----------*/
//void test_HexDataP4()
BOOST_AUTO_TEST_CASE(test_HexDataP4)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxy";
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = 0x01;
		RtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	frm_U[0] = 1;
	RtnPos[0] = 0;
	TESTDBG[8] = 0;

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_004C");
	for (i = 0; i < 100; i++) {
		HexDataP(fugaWnd, buf, 50);
		if (i > 80)printf("FstN = %d RtnN = %d RtnPos = %d RtnPos = %d %d %d %d %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3], TESTDBG[4], TESTDBG[5], TESTDBG[6], TESTDBG[7]);
		TESTDBG[8]++;
		Sleep(100);
	}
	//printf("ws = %d vs = %d hs = %d wh = %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3]);

	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_004.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_004C_org.bmp"), LPCWSTR(L"TEST_004C_make.bmp")) == 1);

	Sleep(1000);
	DestroyWindow(fugaWnd);
}
/*-------------------void HexDataP5(HWND,unsigned char * ,DWORD)-----------*/
//void test_HexDataP5()
BOOST_AUTO_TEST_CASE(test_HexDataP5)
{
	int i, j;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxyABCDEFGHIJKLMNOPQRSTUVWXYabcdefghijklmnopqrstuvwxy";
	RtnN = 0;
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = 0x01;
		RtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	frm_U[0] = 1;
	RtnPos[0] = 0;
	TESTDBG[8] = 0;

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_004D");
	for (i = 0; i < 10; i++) {
		HexDataP(fugaWnd, buf, 300);
	//	if (i > 80)printf("FstN = %d RtnN = %d RtnPos = %d RtnPos = %d %d %d %d %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3], TESTDBG[4], TESTDBG[5], TESTDBG[6], TESTDBG[7]);
		printf("FstN = %d RtnN = %d RtnPos = %d RtnPos = %d %d %d %d %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3], TESTDBG[4], TESTDBG[5], TESTDBG[6], TESTDBG[7]);
		TESTDBG[8]++;
		Sleep(1000);
	}
	//printf("ws = %d vs = %d hs = %d wh = %d\n", TESTDBG[0], TESTDBG[1], TESTDBG[2], TESTDBG[3]);

	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_004.bmp"));
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_004D_org.bmp"), LPCWSTR(L"TEST_004D_make.bmp")) == 1);

	Sleep(1000);
	DestroyWindow(fugaWnd);
}
/*-------------------void HexDataP(HWND)----------------------------------*/
//void test_HexDataREP()
BOOST_AUTO_TEST_CASE(test_HexDataREP)
{
	int i, j;
	unsigned char ch;
	HWND fugaWnd;
	unsigned char buf[] = "ABCDEFG";
	RtnN = 0;	
	FstN = 0;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	for (i = 0; i < RTN_SIZE; i++) {
		frm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		sfrm_U[i & 0xff] = (Display_flg & 0x01);//ASCII
		RtnPos[i] = -1;//改行位置
		sRtnPos[i] = -1;//改行位置
	}
	for (i = 0; i < LINGBUF_SIZE; i++)frmm[i & LINGBUF_MAX] = 0x20;
	sRtnPos[0] = 0;
	frm_U[0] = 1;
	RtnPos[0] = 0;
	sfrm_U[0] = 1;
	for (i = 0; i < 24; i++) {
		//	for(j=0;j<65;j++)frm[i][j] = 0x20;
		for (j = 0; j < 65; j++)frmm[(i * 24) + j] = 0x20;
	}

	fugaWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_005");
	sfrm_U[sRtnN & 0xff] = 0;
	HexDataP(fugaWnd, buf, 7);

	ch = 0x30;
	SendDataPK(fugaWnd, ch);


	Sleep(100);
	ch = 0x31;
	SendDataPK(fugaWnd, ch);


	Sleep(100);
	ch = 0x32;
	SendDataPK(fugaWnd, ch);


	Sleep(100);
	ch = 0x20;
	SendDataPK(fugaWnd, ch);


	Sleep(100);
	ch = 0x33;
	SendDataPK(fugaWnd, ch);


	key.spnflg = 0;
	Sleep(100);
	ch = 0x0d;
	SendDataPK(fugaWnd, ch);


	sfrm_U[sRtnN & 0xff] = 0;

	Sleep(100);
	ch = 0x40;
	SendDataPK(fugaWnd, ch);
	Sleep(100);

	HexDataREP(fugaWnd);
	SetActiveWindow(fugaWnd);
	BOOST_CHECK(compWindow(fugaWnd, LPCWSTR(L"TEST_005_org.bmp"), LPCWSTR(L"TEST_005_make.bmp")) == 1);
	Sleep(1000);

	DestroyWindow(fugaWnd);
}
/*-------------------void daytime_write(HANDLE)---------------------------*/
//void test_daytime_write()
BOOST_AUTO_TEST_CASE(test_daytime_write)
{
	HANDLE hP;
	HANDLE hSF;
	//	TCHAR InBuf[1024];
	char InBuf[1024];
	//	TCHAR OutBuf[1024];
	char OutBuf[1024];
	DWORD len;

	hP = CreateNamedPipe(L"\\\\.\\pipe\\P1", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE, 1, 1024, 1024, 1000, NULL);
	hSF = CreateFile(L"\\\\.\\pipe\\P1",
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	daytime_write(hSF);	//データ終端に日付書き込み

	ReadFile(hP, InBuf, 1024, &len, NULL);
	BOOST_CHECK_MESSAGE(len, InBuf);
	OutBuf[0] = InBuf[0];
	OutBuf[1] = InBuf[1];
	OutBuf[2] = InBuf[2];
	OutBuf[3] = InBuf[3];
	OutBuf[4] = InBuf[4];
	OutBuf[5] = 0;
	//	OutBuf[5] = InBuf[5];
	//	OutBuf[6] = InBuf[6];
	//	OutBuf[7] = InBuf[7];
	//	OutBuf[8] = InBuf[8];
	//	OutBuf[9] = InBuf[9];
	//	OutBuf[10] = 0;
		/*テスト文字列は試験日を*/
	//	BOOST_CHECK_MESSAGE(!(strcmp(OutBuf,"2008/09/12")),OutBuf);
	BOOST_CHECK_MESSAGE(!(strcmp(OutBuf, "2026/")), OutBuf);

	CloseHandle(hSF);
	CloseHandle(hP);
}
/*-------------------int mzParser(void)-----------------------------------*/
//void test_mzParser()
BOOST_AUTO_TEST_CASE(test_mzParser)
{
	char dummy[20];
	FILE* Fh;

	fopen_s(&Fh, "test_config01.txt", "rt");
	if (Fh != NULL) {
		while (mzParser(Fh));
		fclose(Fh);
	}
	BOOST_CHECK_MESSAGE(!(strcmp(comm_string, "\\\\.\\COM1")), C_C.text);
	_ltoa_s(COMMPARAM[0].BaudRate, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].BaudRate == 19200, dummy);
	_ltoa_s(COMMPARAM[0].Parity, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].Parity == 0, dummy);/*no*/
	_ltoa_s(COMMPARAM[0].StopBits, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].StopBits == 0, dummy);/*one*/
	_ltoa_s(COMMPARAM[0].modem_flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].modem_flg == 1, dummy);

	fopen_s(&Fh, "test_config02.txt", "rt");
	if (Fh != NULL) {
		while (mzParser(Fh));
		fclose(Fh);
	}
	BOOST_CHECK_MESSAGE(!(strcmp(comm_string, "\\\\.\\COM25")), C_C.text);
	_ltoa_s(COMMPARAM[0].BaudRate, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].BaudRate == 38400, dummy);
	_ltoa_s(COMMPARAM[0].Parity, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].Parity == 2, dummy);/*even*/
	_ltoa_s(COMMPARAM[0].StopBits, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].StopBits == 1, dummy);/*onehalf*/
	_ltoa_s(COMMPARAM[0].modem_flg, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(COMMPARAM[0].modem_flg == 0, dummy);
}
/*-------------------int mxParser(void)-----------------------------------*/
//void test_mxParser()
BOOST_AUTO_TEST_CASE(test_mxParser)

{
	int i, size;
	char dummy[256];
	FILE* Fh;

	fopen_s(&Fh, "test_command01.txt", "rt");
	if (Fh != NULL) {
		while (mxParser(Fh));
		fclose(Fh);
	}
	size = CV.size();
	_ltoa_s(size, dummy, 20, 10);
	BOOST_CHECK_MESSAGE(size == 13, dummy);
	BOOST_CHECK_MESSAGE(!(wcscmp(CV[0].name,L"ほげ")), CV[0].name);
	for (i = 0; i < size; i++) {
		delete CV[i].data;
		delete CV[i].label;
		delete CV[i].name;
	}
	CV.clear();/*全削除*/

	fopen_s(&Fh, "test_command02.txt", "rt");
	if (Fh != NULL) {
		while (mxParser(Fh));
		fclose(Fh);
	}
	size = CV.size();

	//	BOOST_CHECK_MESSAGE(!(strcmp(comm_string,"\\\\.\\COM25")),C_C.text);
	//	_ltoa_s(BaudRate_read, dummy,20, 10);
	//	BOOST_CHECK_MESSAGE(BaudRate_read == 38400 ,dummy);
	//	_ltoa_s(modem_flg, dummy,20, 10);
	//	BOOST_CHECK_MESSAGE(modem_flg == 0,dummy);

	for (i = 0; i < size; i++) {
		delete CV[i].data;
		delete CV[i].label;
		delete CV[i].name;
	}
	CV.clear();/*全削除*/
}
//void test_CRC16()
BOOST_AUTO_TEST_CASE(test_CRC16)
{
	int i,j;
	int len;
	size_t len2,len3;
	lua_State* L = luaL_newstate();
	char dummy[20];
	char Buf[8];
	Buf[0] = 0x01;
	Buf[1] = 0x02;
	Buf[2] = 0x03;
	Buf[3] = 0x04;
	Buf[4] = 0x05;
	Buf[5] = 0x06;
	Buf[6] = 0x00;
	Buf[7] = 0x00;
	len = 6;

	luaL_openlibs(L);
	lua_pushcfunction(L, CRC16);
	lua_setglobal(L, "CRC16");
	lua_pcall(L, 0, 0, 0);

  	lua_settop(L, 0);
	lua_pop(L, lua_gettop(L));
	lua_pushlstring(L, Buf, len);
    lua_pushinteger(L, len);
	CRC16(L);
	const unsigned char * inBuf = (const unsigned char *)lua_tolstring(L, 1, &len2);
	const unsigned char * inBuf2 = (const unsigned char*)lua_tolstring(L, 2, &len3);
	//len = (int)lua_tonumber(L, 2);
	BOOST_CHECK_MESSAGE(inBuf[0] == '1', inBuf[0]);
	BOOST_CHECK_MESSAGE(inBuf[1] == '8', inBuf[1]);
	BOOST_CHECK_MESSAGE(inBuf2[0] == 0x01, inBuf2[0]);
	BOOST_CHECK_MESSAGE(inBuf2[1] == 0x02, inBuf2[1]);
	BOOST_CHECK_MESSAGE(inBuf2[2] == 0x03, inBuf2[2]);
	BOOST_CHECK_MESSAGE(inBuf2[3] == 0x04, inBuf2[3]);
  	lua_settop(L, 0);
	lua_close(L);
	L = NULL;
}
//void test_SerialRecv()
BOOST_AUTO_TEST_CASE(test_SerialRecv)
{
	int i,j;
	int len;
	size_t len2;
	static lua_State* globalL = NULL;
	lua_State* L = luaL_newstate();
	char dummy[20];
	//char Buf[3];
	COMBUF[0].Buf2[0] = 0x01;
	COMBUF[0].Buf2[1] = 0x02;
	COMBUF[0].Buf2[2] = 0x00;
	COMBUF[0].len2 = 2;
	BOOST_TEST_MESSAGE("Hoge1");

	luaL_openlibs(L);
	BOOST_TEST_MESSAGE("Hoge1A");
	lua_pushcfunction(L, SerialRecv);
	BOOST_TEST_MESSAGE("Hoge1B");
	lua_setglobal(L, "SerialRecv");
	BOOST_TEST_MESSAGE("Hoge1C");
	lua_pcall(L, 0, 0, 0);
	BOOST_TEST_MESSAGE("Hoge2");
  	lua_settop(L, 0);
	BOOST_TEST_MESSAGE("Hoge2A");
	lua_pop(L, lua_gettop(L));
	BOOST_TEST_MESSAGE("Hoge2B");
	lua_pushinteger(L, 0);
	lua_pushlstring(L, (const char *)COMBUF[0].Buf2, COMBUF[0].len2);
//  lua_pushinteger(L, len);
	BOOST_TEST_MESSAGE("Hoge3");
	SerialRecv(L);

	len = (int)lua_tonumber(L, 1);
	const unsigned char * inBuf = (const unsigned char *)lua_tolstring(L, 2, &len2);

	BOOST_CHECK_MESSAGE(inBuf[0] == 0x01, inBuf[0]);
	BOOST_CHECK_MESSAGE(inBuf[1] == 0x02, inBuf[1]);
	BOOST_CHECK_MESSAGE(len == 2, len);
  	lua_settop(L, 0);
	lua_close(L);
	L = NULL;
}
/*-------------------ATOM MyRegisterClass(HINSTANCE)----------------------*/
void test_MyRegisterClass()
{
	ATOM flg;
	HINSTANCE hInst = GetModuleHandle(NULL);
	LoadString(hInst, IDC_BINCOM40, szWindowClass, 100);
	flg = MyRegisterClass(hInst);
	BOOST_CHECK_MESSAGE(flg, flg);
}
/*-------------------HWND InitInstance(HINSTANCE, int)--------------------*/
void test_InitInstance()
{
	//	int nCmdShow;
	//	HWND fugaWnd;
	WNDCLASSEX wcex;

	//	nCmdShow = SW_SHOW;
	HINSTANCE hInsta = GetModuleHandle(NULL);
	LoadString(hInsta, IDC_BINCOM40, szWindowClass, 100);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInsta;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = NULL;
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);

	//	fugaWnd = InitInstance(hInsta,nCmdShow); /*戻りハンドルがおかしい*/
	//	Sleep(100);
	//	BOOST_CHECK_MESSAGE(fugaWnd,fugaWnd);
	//	BOOST_CHECK_MESSAGE(0,fugaWnd);
	//	SetActiveWindow(fugaWnd);
	//	saveBMP(fugaWnd,LPCTSTR("TEST_006.bmp"));
	//	BOOST_CHECK(compWindow(fugaWnd,LPCTSTR("TEST_006_org.bmp"))==1);
	//	Sleep(500);
	//	DestroyWindow (fugaWnd);
}
/*---LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)-----------------*/
void test_WndProc()
{
	HWND fugaWnd;

	hogeWnd = InitTestWindow(530, 504, L"TEST_007");
	Sleep(100);
	SetActiveWindow(hogeWnd);

	SendMessage(hogeWnd, WM_CHAR, 30, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_CHAR, 32, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_CHAR, 126, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_CHAR, 127, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_TIMER, 0, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_TIMER, 1, 0);
	Sleep(100);
	SendMessage(hogeWnd, WM_COMMAND, IDM_EDIT, 0);
	fugaWnd = FindWindow(L"#32770", L"BaudRate");
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, ID_OK, 0);/*メインスレッドで処理*/
	Sleep(100);
	SendMessage(hogeWnd, WM_COMMAND, IDM_CMD, 0);
	fugaWnd = FindWindow(L"#32770", L"Command");
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, IDCANCEL, 0);

	Sleep(1000);
	DestroyWindow(hogeWnd);
}
/*---INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM)-------------------*/
void test_About()
{
	HWND fugaWnd;
	HWND hoeWnd;
	hoeWnd = GetConsoleHwnd();

	HINSTANCE hInst = GetModuleHandle(NULL);

	CreateDialog(hInst, L"IDD_ABOUTBOX", hoeWnd, (DLGPROC)About);
	Sleep(500);
	fugaWnd = FindWindow(L"#32770", NULL);
	//	fugaWnd = FindWindow("#32770", "バージョン情報 bincom40");
	//	fugaWnd = FindWindow(NULL, "バージョン情報 bincom40");
	BOOST_CHECK_MESSAGE(fugaWnd, fugaWnd);
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, IDOK, 0);

}
/*---DWORD WINAPI ThreadFunc(LPVOID)--------------------------------------*/
//スレッドは単体テストの対象外にするしかない。終了条件が考慮されていないから。
//同様の理由でRTOSのタスクも駄目だろう。
void test_ThreadFunc()
{
	HANDLE hP;
	DWORD dwID;
	DWORD dwParam;
	HANDLE hThread;
	DWORD len_return;

	hogeWnd = InitTestWindow(WSIZE_WIDTH, WSIZE_HEIGHT, L"TEST_006");
	Sleep(100);
	SetActiveWindow(hogeWnd);

	/*イベントは手動リセット*/
	hEventObject[0] = CreateEvent(NULL, TRUE, TRUE, LPCWSTR(""));
	COMOverlapped[0].hEvent = hEventObject;
	COMOverlapped[0].Offset = 0;
	COMOverlapped[0].OffsetHigh = 0;

	//	TCHAR InBuf[1024];
	//	TCHAR OutBuf[1024];
	//	DWORD len;

	hP = CreateNamedPipe(L"\\\\.\\pipe\\P1", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE, 1, 1024, 1024, 1000, NULL);
	hCom[0] = CreateFile(L"\\\\.\\pipe\\P1",
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_FLAG_OVERLAPPED, NULL);
	WriteFile(hCom, "Hoge", 4, &len_return, NULL);

	Sleep(1000);

	hThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)hogeWnd, 0, &dwID);
	//スレッドから制御が戻らないので、この試験は行わない。

	//	Sleep(1000);

	BOOST_CHECK_MESSAGE(GetExitCodeThread(hThread, &dwParam), dwParam);
	ExitThread(dwParam);
	CloseHandle(hThread);
	CloseHandle(hCom);
	CloseHandle(hP);
	DestroyWindow(hogeWnd);
}
/*-------------------BOOL CALLBACK dlgProc(HWND,UINT,WPARAM,LPARAM)-------*/
void test_dlgProc()
{
	//	DCB			dcb;			//シリアルポートのDCB構造体
	HWND fugaWnd;
	HWND hoeWnd;
	hoeWnd = GetConsoleHwnd();

	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = CBR_9600;	//9600bps
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;       // enable parity checking
	dcb.fOutxCtsFlow = 0;      // CTS output flow control
	dcb.fOutxDsrFlow = 0;      // DSR output flow control
	dcb.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
	dcb.fDtrControl = 0;	   //DTR_CONTROL_DISABLE DTR flow control type 
	dcb.fDsrSensitivity = 0;   // DSR sensitivity 
	dcb.fTXContinueOnXoff = 1; // XOFF continues Tx 
	dcb.fOutX = 0;            // XON/XOFF out flow control 
	dcb.fInX = 0;             // XON/XOFF in flow control 
	dcb.fErrorChar = 0;       // enable error replacement 
	dcb.fNull = 0;            // enable null stripping 
	dcb.fAbortOnError = 0;     // abort reads/writes on error 
	dcb.ByteSize = 8;			//
	dcb.Parity = NOPARITY;		//…見ての通り
	dcb.StopBits = ONESTOPBIT;	//

	HINSTANCE hInst = GetModuleHandle(NULL);
	//	DialogBox(hInst,"DIALOG1",hoeWnd,(DLGPROC)dlgProc);
	CreateDialog(hInst, L"DIALOG1", hoeWnd, (DLGPROC)dlgProc);
	Sleep(500);
	//	fugaWnd = FindWindowEx(hoeWnd, 0, "#32770", NULL);
	//	fugaWnd = FindWindow("#32770", NULL);
	fugaWnd = FindWindow(L"#32770", L"BaudRate");
	SetDlgItemText(fugaWnd, ID_TA, L"19200");
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, ID_OK, 0);/*メインスレッドで処理*/
	/*	BOOST_CHECK_MESSAGE(0,fugaWnd);*/

		/*ダイアログが現れるので*/
		/*1:テキストボックス内が9600と表示されているのを確認する*/
		/*2:テクストボックス内を19200に変更する*/
	BOOST_CHECK_MESSAGE(dcb.BaudRate == 19200, dcb.BaudRate);

}
/*-------------------BOOL CALLBACK dlgProc2(HWND,UINT,WPARAM,LPARAM)-------*/
void test_dlgProc2()
{
	int i, j;
	HWND fugaWnd;
	HWND hoeWnd;
	RECT	rect;
	hoeWnd = GetConsoleHwnd();

	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = CBR_9600;	//9600bps
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;       // enable parity checking
	dcb.fOutxCtsFlow = 0;      // CTS output flow control
	dcb.fOutxDsrFlow = 0;      // DSR output flow control
	dcb.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
	dcb.fDtrControl = 0;	   //DTR_CONTROL_DISABLE DTR flow control type 
	dcb.fDsrSensitivity = 0;   // DSR sensitivity 
	dcb.fTXContinueOnXoff = 1; // XOFF continues Tx 
	dcb.fOutX = 0;            // XON/XOFF out flow control 
	dcb.fInX = 0;             // XON/XOFF in flow control 
	dcb.fErrorChar = 0;       // enable error replacement 
	dcb.fNull = 0;            // enable null stripping 
	dcb.fAbortOnError = 0;     // abort reads/writes on error 
	dcb.ByteSize = 8;			//
	dcb.Parity = NOPARITY;		//…見ての通り
	dcb.StopBits = ONESTOPBIT;	//

	rect.left = 0;
	rect.top = 0;
	rect.right = 530;
	rect.bottom = 504;
	for (i = 0; i < 6; i++) {
		//	for(j=0;j<65;j++)sfrm[i][j] = 0x20;
		for (j = 0; j < 65; j++)sfrmm[(i * 6) + j] = 0x20;
	}

	hCom[0] = CreateFile(L"\\\\.\\COM1",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);							//Comm初期化
	SetCommState(hCom, &dcb);
	InitializeCriticalSection(&cscom);

	HINSTANCE hInst = GetModuleHandle(NULL);

	MyRegisterClass(hInst);
	hogeWnd = CreateWindow(L"STATIC", L"TEST_001",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right - rect.left),
		(rect.bottom - rect.top),
		NULL,
		NULL,
		hInst,
		NULL);	//幅530,高さ420ドット
	BOOST_CHECK_MESSAGE(hogeWnd != 0, hogeWnd);
	ShowWindow(hogeWnd, SW_SHOW);
	UpdateWindow(hogeWnd);

	//	DialogBox(hInst,"DIALOG2",hoeWnd,(DLGPROC)dlgProc2);
	CreateDialog(hInst, L"DIALOG2", hoeWnd, (DLGPROC)dlgProc2);
	Sleep(500);
	//	fugaWnd = FindWindow( NULL, "Command");
	fugaWnd = FindWindow(L"#32770", L"Command");
	//	fugaWnd = FindWindow("#32770", NULL);
	SetDlgItemText(fugaWnd, ID_TB, L"0x20");
	SetDlgItemText(fugaWnd, ID_TC, L"0x55");
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, ID_WRITE, 0);

	//	BOOST_CHECK_MESSAGE(SND.buf[0] == 0x00,SND.buf[0]);
	//	BOOST_CHECK_MESSAGE(SND.buf[1] == 0x20,SND.buf[1]);
	//	BOOST_CHECK_MESSAGE(SND.buf[2] == 0x55,SND.buf[2]);
	//	BOOST_CHECK_MESSAGE(SND.buf[3] == 0x00,SND.buf[3]);

	CreateDialog(hInst, L"DIALOG2", hoeWnd, (DLGPROC)dlgProc2);
	Sleep(500);
	fugaWnd = FindWindow(L"#32770", L"Command");
	SetDlgItemText(fugaWnd, ID_TB, L"0x30");
	SetDlgItemText(fugaWnd, ID_TC, L"0xAA");
	Sleep(500);
	SendMessage(fugaWnd, WM_COMMAND, ID_READ, 0);

	//	BOOST_CHECK_MESSAGE(SND.buf[0] == 0x01,SND.buf[0]);
	//	BOOST_CHECK_MESSAGE(SND.buf[1] == 0x30,SND.buf[1]);
	//	BOOST_CHECK_MESSAGE(SND.buf[2] == 0xAA,SND.buf[2]);
	//	BOOST_CHECK_MESSAGE(SND.buf[3] == 0x00,SND.buf[3]);

	DeleteCriticalSection(&cscom);
	CloseHandle(hCom);
	DestroyWindow(hogeWnd);
}

