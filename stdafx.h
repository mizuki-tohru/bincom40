// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
#define TLMDATA_NUM 256
#define COMMBUFSIZE 131072
#define BUF_SIZE	2048*4
#define MAX_CLIENTS 5
//#define LINGBUF_SIZE 37778
#define LINGBUF_SIZE 37779
#define SENDBUF_SIZE 4096
#define RTN_SIZE 100
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <commdlg.h>
#include <objbase.h>
#include <synchapi.h>
#include <string>
#include <map>
#include <mutex>
#include <strsafe.h>
#include <wbemcli.h>
#include <comutil.h>
#include <setupapi.h>
#include <ntddser.h>
#include <initguid.h>
#include <concrtrm.h>
//#include <iostream>
#include <devguid.h>  // GUID_DEVINTERFACE_COMPORT
#include <cfgmgr32.h>
#include <crtdbg.h>
#include <Ws2def.h>
#include <winsock.h>
//#include <tchar.h>
//#include <setupapi.h>
#include <wingdi.h>
#include <cstdio>
#include <cwchar> // mbstowcs_s

#include "winusb.h" //WinDDK\BuildNumber\inc\ddk
#include "winusbio.h" //WinDDK\BuildNumber\inc\ddk
#include "usb.h" //WinDDK\BuildNumber\inc\api
#include "usb100.h" //WinDDK\BuildNumber\inc\api
#include "usb200.h" //WinDDK\BuildNumber\inc\api
//#include "visa.h"
#include "libusb.h"
#include "wininet.h"
#include "ftd2xx.h"
//#include "LibFT4222.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "Setupapi.lib")
//#pragma comment(lib, "visa64.lib")
#pragma comment(lib, "libusb-1.0.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

enum maker_t {
    MAKER_DUMMY = 0,
    MAKER_YOKOKAWA,
    MAKER_KIKUSUI,
    MAKER_HP,
    MAKER_KEYSIGHT,
    MAKER_KEISOKU,
    MAKER_FTDI,
    MAKER_OWON,
    MAKER_MATSUSADA,
    MAKER_TAKASAGO,
    MAKER_ALIENTEK
};
/*--------------------------------------------------------------------------*/
using namespace std;

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
/*テレメトリフォーマット構造体*/
struct tlm_part{
	int major;
	long minor;
	int mask;
	int shift;
};
/*----USB シリアルポートパラメータ列挙構造体---------------------*/
struct usb_serial_param{
	char port[256] = {0};
	int port_num;
	int vid;
	int pid;
	int delimiter = 0;
	TCHAR ser[18] = {0};
};
/*---計測器コマンド構造体---*/
struct mcmd{
	char * cmd;
	int type;
	int param;
};
/*---計測器コマンド構造体---*/
struct measurement_cmd{
	char * name = {0};
	vector<mcmd> CMD;
};

/*----計測器設定構造体----*/
struct measurement_device{
//	char * name = {0};
	std::string name;
//	char * setting_file_name ={0};
	std::string setting_file_name;
//	int maker = 0;
    maker_t maker = MAKER_DUMMY;
	int interface_type = 1;
	int delimiter = 0;
	int init = 0;
//	char * serial_port = {0};
	std::string serial_port;
	int serial_port_num = 0;
	int serial_baud = 0;
//	char * VISA_Addr = {0};
	std::string VISA_Addr;
//	int lan_addr[4] = {0};
//  long lan_addr[4] = {0,0,0,0};
    int lan_addr[4] = {0,0,0,0};
	int lan_port = 0;
	int gpib_port = 0;
	int usb_vid = 0;
	int usb_pid = 0;
	int ser_ch = 0;
//	char * usb_ser={0};
//	TCHAR * usb_ser = {0};
	std::wstring usb_ser;
	vector<measurement_cmd> MCMD;
};

struct keysight_ivi_device{
	char name[20] = {0};
	int ident;/*識別コード*/
	int delimiter = 0;
//	ViSession session;
};

struct usbhid_dev{
	libusb_device_handle * handle;
	int MDEV_num;
	uint8_t EPOUT;
	uint8_t EPIN;
};

struct ftdi_dev{
	FT_HANDLE ftHandle;       // USB handler
	int cnt;
};

struct tlm_struct{
	char * name;
	int type;
	int size;
	int view;
	unsigned long long data;
	struct tlm_part PART[8];
};
/*レジスタ構造体*/
struct reg_sub {
	int start;
	int isize;
	unsigned int* DATA;
};

struct reg_part{
	int isize;
	struct reg_sub SUB[32];
};

struct reg_struct{
	char * name;
	int isize;
	struct reg_part PART[12];
};

/*レジスタ構造体*/

struct reg_main{
//	char * name;
    std::string name;
	long start = 0;
	int isize = 0;
//	long * DATA;
    std::vector<long> DATA;
//	vector<reg_data> REG;
};

struct reg_data {
	char * name;
	int isize;
	long address;
};

struct ComReadBuf{
	unsigned char Buf[COMMBUFSIZE] = {0};
	unsigned char Buf2[COMMBUFSIZE] = {0};
	long len;
	long len2;
	int flg;
};

class Socket{
public:
//	int sc;	   			 // socket descriptor.
	SOCKET sc;	   		 // socket descriptor.
//	int GN_connect_counter; /*新GN拡張*/
//	int GN_connect_counter_start; /*新GN拡張*/
//	int GN_sendinterval_counter;
	unsigned char sendbuf[50] = {0};
	int Initflg;
	int MDEV_num;
	sockaddr_in addr ={AF_INET,htons((u_short)0),(UCHAR)(htonl(INADDR_ANY)),0};	 // address of the socket.
	Socket(void):sc(0){
	//	sendbuf[0] = 0x02;
	//	sendbuf[1] = 0xf5;
	//	sendbuf[2] = 0x00;
	//	sendbuf[3] = 0x04;
	//	sendbuf[4] = 0x00;
	//	sendbuf[5] = 0x00;
	//	sendbuf[6] = 0x00;
	//	sendbuf[7] = 0x0C;
	//	sendbuf[11] = 0x00;
	};
	Socket(const u_short port){
	//	sendbuf[0] = 0x02;
	//	sendbuf[1] = 0xf5;
	//	sendbuf[2] = 0x00;
	//	sendbuf[3] = 0x04;
	//	sendbuf[4] = 0x00;
	//	sendbuf[5] = 0x00;
	//	sendbuf[6] = 0x00;
	//	sendbuf[7] = 0x0C;
	//	sendbuf[11] = 0x00;
	    sc = socket(AF_INET, SOCK_STREAM, 0);
	//	GN_connect_counter = 0; /*新GN拡張*/
	//	GN_connect_counter_start = 0; /*新GN拡張*/
	//	GN_sendinterval_counter = 0;
		Initflg = 0;
		MDEV_num = -1;
		if(sc == INVALID_SOCKET){
        	exit(1);
		}
		addr.sin_family		 = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port		 = htons((u_short)port);
		memset(addr.sin_zero,(int)0,sizeof(addr.sin_zero));
		if(bind(sc,(sockaddr*)&addr,sizeof(sockaddr_in)) == (int)SOCKET_ERROR){
			perror("bind");
			exit(1);
		}
		listen(sc,MAX_CLIENTS+1);
	};
	~Socket(){
		Close();
	};
	void init(const u_short port){
	//	sendbuf[0] = 0x02;
	//	sendbuf[1] = 0xf5;
	//	sendbuf[2] = 0x00;
	//	sendbuf[3] = 0x04;
	//	sendbuf[4] = 0x00;
	//	sendbuf[5] = 0x00;
	//	sendbuf[6] = 0x00;
	//	sendbuf[7] = 0x0C;
	//	sendbuf[11] = 0x00;
	    sc = socket(AF_INET, SOCK_STREAM, 0);
	//	GN_connect_counter = 0; /*新GN拡張*/
	//	GN_connect_counter_start = 0; /*新GN拡張*/
	//	GN_sendinterval_counter = 0;
		Initflg = 0;
		MDEV_num = -1;
		if(sc == INVALID_SOCKET){
        	exit(1);
		}
		addr.sin_family		 = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port		 = htons((u_short)port);
		memset(addr.sin_zero,(int)0,sizeof(addr.sin_zero));
		if(bind(sc,(sockaddr*)&addr,sizeof(sockaddr_in)) == (int)SOCKET_ERROR){
			perror("bind");
			exit(1);
		}
		listen(sc,MAX_CLIENTS+1);
	}
//	void init_client(const u_short port,char * IP_Address){
	void init_client(const u_short port,int IP_Address[]){
	//	sendbuf[0] = 0x02;
	//	sendbuf[1] = 0xf5;
	//	sendbuf[2] = 0x00;
	//	sendbuf[3] = 0x04;
	//	sendbuf[4] = 0x00;
	//	sendbuf[5] = 0x00;
	//	sendbuf[6] = 0x00;
	//	sendbuf[7] = 0x0C;
	//	sendbuf[11] = 0x00;
		Initflg = 1;
		MDEV_num = -1;
	    sc = socket(AF_INET, SOCK_STREAM, 0);
		if(sc == INVALID_SOCKET){
        	exit(1);
		}
		addr.sin_family		 = AF_INET;
	//	inet_pton(AF_INET,IP_Address, &addr.sin_addr.s_addr);
	//	InetPton(AF_INET,IP_Address, &addr.sin_addr.s_addr);
	//	InetPtonW(AF_INET,IP_Address, &addr.sin_addr.s_addr);
		addr.sin_addr.s_addr = (IP_Address[0] << 24)|(IP_Address[1] << 16)
							  |(IP_Address[2] << 8) |IP_Address[3];
		addr.sin_port		 = htons((u_short)port);
	}
	void Accept( const Socket& base ){
#if defined(_XOPEN_SOURCE_EXTENDED)
		size_t len = sizeof(sockaddr_in);
#else
		int len = sizeof(sockaddr_in);
#endif
		sc = accept( base.sc, (sockaddr*)&addr, &len );
	};
	int Recv(char buf[BUF_SIZE]){
    	int len = recv(sc, buf, BUF_SIZE, 0);
		buf[len] = '\0';
		return len;
	};
	void Connect(void){
		if(connect(sc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			Close();
		}
	}
	void Send(char* buf){
		if(send(sc,buf,(int)(strlen(buf)),0) < 0){
			closesocket(sc);
			sc = 0;
		}
	};
	bool SendB(unsigned char* buf ,int len){
		if(send(sc,(char *)buf,len,0) < 0){
 		    closesocket(sc);
			sc = 0;
			return false;
		}
		return true;
	};
	void Close(void){
		closesocket(sc);
		sc = 0;
	};
	int IsActive(void){
		return sc!=0;
	};
	unsigned long * RemoteAddr(void){
		return &addr.sin_addr.s_addr;
	};
	u_short RemotePort(void){
		return ntohs(addr.sin_port);
	};
};
/*-----シ－ンラベル及び変数ラベル用クラス------------------*/
struct plabel{
	char * name; /*ラベル文字列*/
	long addr;
	int id;		 /**/
	long num;		 /**/
	double fnum;		 /**/
	int type;    /**/
};
class ulabel{
	public:
	vector<plabel> LU;
	/*文字列と同じラベルの変数が配列の何処にあるかを返す*/
	int serch(char *);
	/*シ－ンラベル構造体を挿入する*/
//	void insert(char *,long,double,int);
	void insert(char *,long,long);
	/*変数の個数を返す*/
	int size(void){
		return (int)(LU.size());
	}
};

// RAII wrapper for libusb device handles + UHDEV container
class UsbhidDev {
public:
    libusb_device_handle* handle = nullptr;
    unsigned char EPIN = 0;
    unsigned char EPOUT = 0;
    int MDEV_num = -1;

    UsbhidDev() noexcept = default;
    explicit UsbhidDev(libusb_device_handle* h, unsigned char epin = 0, unsigned char epout = 0, int m = -1) noexcept
        : handle(h), EPIN(epin), EPOUT(epout), MDEV_num(m) {}

    ~UsbhidDev() noexcept {
        if (handle) {
            // best-effort: release interface then close
            libusb_release_interface(handle, 0);
            libusb_close(handle);
            handle = nullptr;
        }
    }

    // move-only semantics
    UsbhidDev(UsbhidDev&& o) noexcept
        : handle(o.handle), EPIN(o.EPIN), EPOUT(o.EPOUT), MDEV_num(o.MDEV_num) { o.handle = nullptr; }
    UsbhidDev& operator=(UsbhidDev&& o) noexcept {
        if (this != &o) {
            if (handle) { libusb_release_interface(handle, 0); libusb_close(handle); }
            handle = o.handle; EPIN = o.EPIN; EPOUT = o.EPOUT; MDEV_num = o.MDEV_num;
            o.handle = nullptr;
        }
        return *this;
    }
    UsbhidDev(const UsbhidDev&) = delete;
    UsbhidDev& operator=(const UsbhidDev&) = delete;
};

extern int BUSON_flg;
//extern char frm[30][66];		//画面表示キャラクタバッファ
//extern char frmm[100*200];		//画面表示キャラクタバッファ
extern char frmm[LINGBUF_SIZE];		//画面表示キャラクタバッファ
extern char sfrmm[SENDBUF_SIZE];		//画面表示キャラクタバッファ
//extern unsigned long wtpoint;	//表示終端文字位置
//extern unsigned long wwtpoint;	//表示終端文字位置
extern unsigned long stpoint; //表示先頭位置
extern unsigned long edpoint; //表示末尾位置
extern unsigned long lnpoint;
extern unsigned long sstpoint; //表示先頭位置
extern unsigned long sedpoint; //表示末尾位置
extern unsigned char GetBufin[6][COMMBUFSIZE*2];
extern HANDLE hCom[5];			//ファイル(シリアルポート)のハンドル
extern vector<measurement_device> MDEV;/*計測器定義*/
//extern vector<usb_serial_param> USBSER;
extern vector<keysight_ivi_device> VISESS;
extern vector<tlm_struct> TLM;
//extern vector<usbhid_dev> UHDEV;
extern std::mutex UHDEV_mtx;
extern std::vector<UsbhidDev> UHDEV;
extern vector<Socket> NET;
extern unsigned long long TLMDATA[TLMDATA_NUM];
extern HANDLE hThread[10];  /*静的変数にする */
extern int keywait_flg;
extern int timewait_flg;
extern int Display_flg; //0:text 1:hex
extern int Interval;
extern ulabel LM;	/*変数ラベル格納クラス*/
extern unsigned int WSIZE_WIDTH;
extern unsigned int	WSIZE_HEIGHT;

extern OVERLAPPED	COMOverlapped[5];
extern HANDLE		hEventObject[5];
extern OVERLAPPED	COMOverlappedW[5];
extern HANDLE		hEventObjectW[5];

extern int frm_U[100];			//画面表示ASCII-UTF-8-HEXフラグ
extern int sfrm_U[RTN_SIZE+2];			//画面表示ASCII-UTF-8-HEXフラグ
extern int retcnt;
extern long RtnPos[RTN_SIZE+2];
extern long sRtnPos[RTN_SIZE+2];
extern unsigned int RtnN;
extern unsigned int FstN;
extern unsigned int sRtnN;
extern unsigned int sFstN;

unsigned short CRC_CCITT_LSBfirst_ALL1(unsigned short, unsigned char []);

DWORD  WINAPI ThreadFunc(LPVOID);
DWORD  WINAPI ThreadFunc2(LPVOID);
DWORD  WINAPI ThreadFunc3(LPVOID);
DWORD  WINAPI ThreadFunc4(LPVOID);
DWORD  WINAPI ThreadFunc5(LPVOID);
DWORD  WINAPI ThreadFunc6(LPVOID);
