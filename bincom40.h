#pragma once

#include "resource.h"
#include "stdafx.h"



#define SC_TXT 1
#define SC_NUM 2
#define COM_P_NONE  0
#define COM_P_ODD   1
#define COM_P_EVEN  2
#define COM_P_SPACE 3
#define COM_P_MARK  4
#define COM_S_ONE     0
#define COM_S_ONEHALF 1
#define COM_S_TWO     2


#define MAKER_YOKOKAWA	1
#define MAKER_KIKUSUI	2
#define MAKER_HP		3
#define MAKER_KEYSIGHT	4
#define MAKER_KEISOKU	5
#define MAKER_FTDI		6
#define MAKER_OWON		7
#define MAKER_MATSUSADA	8
#define MAKER_TAKASAGO	9
#define MAKER_ALIENTEK  10
#define MAKER_DUMMY		0
#define MAKER_MAX 8

/*--------------------------------------------------------------------------*/
using namespace std;
/*--------------------------------------------------------------------------*/
/*  関数宣言	                                                            */
/*--------------------------------------------------------------------------*/
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL   CALLBACK dlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL   CALLBACK dlgProc2(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam);
//DWORD  WINAPI ThreadFunc(LPVOID);

int getUSBVID(void);
unsigned short CRC_CCITT_MSBfirst_ALL(unsigned short, unsigned char []);
unsigned short CRC_CCITT_LSBfirst_ALL1(unsigned short, unsigned char []);
int ltohex(unsigned long ,unsigned char *,unsigned char);
long axtol(char *,unsigned char *,unsigned int);
long axtolW(WCHAR *,unsigned char *,unsigned int);
void scroll(unsigned int);
void send_scroll(unsigned int);
int UTF8Len(const unsigned char *,int, unsigned long *);
unsigned char charset(unsigned char);
void DisplayData(HWND,unsigned char *,DWORD);
//void DisplayData2(HWND,unsigned char *,DWORD);
void SendDataP(HWND,unsigned char *,DWORD);
//void SendDataP2(HWND,unsigned char *,DWORD);
void SendDataPK(HWND,unsigned char);
//void SendDataPK2(HWND,unsigned char);
void HexDataP(HWND,unsigned char *,DWORD);
//void HexDataP2(HWND,unsigned char *,DWORD);
void HexDataREP(HWND);
//void HexDataREP3(HWND);
void DisplayParam(HWND);
//void DisplayParam3(HWND);
void daytime_write(HANDLE);
int mzParser(FILE *);
int mxParser(FILE *);

/*--------------------------------------------------------------------------*/
/*  構造体宣言                                                              */
/*--------------------------------------------------------------------------*/

/*-----シ－ンラベル及び変数ラベル用クラス------------------*/
struct mlabel{
	char * name; /*ラベル文字列*/
	long addr;
	int id;		 /**/
	long num;		 /**/
	double fnum;		 /**/
	int type;    /**/
};
class nlabel{
	public:
	vector<mlabel> LV;
	/*文字列と同じラベルの変数が配列の何処にあるかを返す*/
	int serch(char *);
	/*シ－ンラベル構造体を挿入する*/
//	void insert(char *,long,double,int);
	void insert(char *,int,int);
	/*変数の個数を返す*/
	int size(void){
		return (int)(LV.size());
	}
};

/*----パーザデコード時キャラクタ状態構造体----------------------------------*/
/* テキストパーザ関数rchar(),cgetchar(),mzParser()用						*/
/*--------------------------------------------------------------------------*/
struct char_type{
	char text[256];
	WCHAR Wtext[256];
	char flg;
	char level;
	long data;
};

/*----送信データ保持構造体----*/
struct cmd_pak{
	char * data;
	char * label;
	WCHAR * name;
	long addr;
	int len;
	int label_len;
	int name_len;
	int interval;
	int flg;
};

/*---受信データバッファ構造体--*/
struct rcv_data{
	unsigned char	buf[1024];
	int				len;
};

struct key_data{
	int spnflg;
	int spline;
	int spchar;
	int lastparam;
	int baklen;
};

struct send_timer{
	int flg;
	int num;
	int local;
	int interval;
};

//struct serial_param{
//	char comm_string[32];
//	long BaudRate;
//	int modem_flg;
//	int Init;
//	unsigned char ByteSize;
//	unsigned char Parity;
//	unsigned char fParity;
//	unsigned char StopBits;
//	unsigned char newline;
//};

//struct dispframe{
//	char str_Name[12][32];
//	char str_Val[12][32];
//};

#if 0
/*----USB シリアルポートパラメータ列挙構造体---------------------*/
struct usb_serial_param{
	char port[256] = {0};
	int port_num;
	int vid;
	int pid;
	int delimiter = 0;
//	char ser[18] = {0};
	TCHAR ser[18] = {0};
};
#endif
extern struct char_type C_C;
extern struct rcv_data SND;
extern struct key_data key;
extern DCB	  dcb;
extern HWND   hogeWnd;
//extern HANDLE hCom;			//ファイル(シリアルポート)のハンドル
extern CRITICAL_SECTION cscom;
//extern TCHAR szTitle[];		// タイトル バーのテキスト
extern WCHAR szTitle[];		// タイトル バーのテキスト
//extern TCHAR szWindowClass[];	// メイン ウィンドウ クラス名
extern WCHAR szWindowClass[];	// メイン ウィンドウ クラス名
extern char frm[30][66];		//画面表示キャラクタバッファ
extern char sfrm[6][66];		//画面表示キャラクタバッファ
//extern unsigned char GetBufin[6][];
extern char comm_string[32];
extern long BaudRate_read;
extern int modem_flg;
//extern struct serial_param COMMPARAM;
extern FILE * F;
extern vector<cmd_pak> CV;
//extern OVERLAPPED	COMOverlapped;
//extern HANDLE		hEventObject;
extern HFONT		hDispFont;		//フォントハンドル
