// bincomm40.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "bincom40.h"
#include "parser.h"
#include "disp.h"
#include "lua\lua.hpp"
#include "lua\lprefix.h"
#include <boost/lexical_cast.hpp>
//#include <winioctl.h>

//#pragma comment(lib,"LibLua.lib")

#define MAX_LOADSTRING 100

#if !defined(LUA_PROGNAME)
#define LUA_PROGNAME		"lua"
#endif

#if !defined(LUA_INIT_VAR)
#define LUA_INIT_VAR		"LUA_INIT"
#endif

#define LUA_INITVARVERSION	LUA_INIT_VAR LUA_VERSUFFIX
#define COMMBUFSIZE 131072
//#define LINGBUF_SIZE 37778
#define LINGBUF_SIZE 37779
#define SENDBUF_SIZE 4096
#define RTN_SIZE 100
//#define GUID_DEVINTERFACE_COMPORT GUID_CLASS_COMPORT

#define POLY 0x8408
#define MODPOLY 0xA001
#define D_WIN32_WINNT 0x0400
#define COMMBUFSIZE 131072
#define READENDNAME "ReadEnd"
#define OPENMODE_ONCE 0
#define OPENMODE_CHECK 1
/*--------------------------------------------------------------------------*/
using namespace std;
/*--------------------------------------------------------------------------*/
using boost::lexical_cast;

// グローバル変数:
HINSTANCE	hInst;							// 現在のインターフェイス
WCHAR		szTitle[MAX_LOADSTRING];		// タイトル バーのテキスト
WCHAR		szWindowClass[MAX_LOADSTRING];	// メイン ウィンドウ クラス名
HMENU		hMenu;
HMENU		hSubMenu0;
HMENU		hSubMenu1;
HMENU		hSubMenu2;
//HANDLE		hEventObject;
//HANDLE		hEventObjectW;
//OVERLAPPED	COMOverlapped = {0};
//OVERLAPPED	COMOverlappedW = {0};

HANDLE		hEventObject[5];
HANDLE		hEventObjectW[5];
//OVERLAPPED	COMOverlapped[5] = {0};
//OVERLAPPED	COMOverlappedW[5] = {0};
OVERLAPPED	COMOverlapped[5];
OVERLAPPED	COMOverlappedW[5];

OPENFILENAMEW lpofn;
DCB			dcb;			//シリアルポートのDCB構造体
COMMTIMEOUTS cmto;			//Comm Timeout略語化
//HANDLE	hCom;			//ファイル(シリアルポート)のハンドル
HANDLE		hCom[5];			//ファイル(シリアルポート)のハンドル
HANDLE hThread[10];  /*静的変数にする */
DWORD		dweError;		
LOGFONT		lplf;			//論理フォント構造体
HFONT		hNewFont;		//フォントハンドル
HFONT		hDispFont;		//フォントハンドル
HFONT		hUFont;		//フォントハンドル
FILE * F;					/*設定ファイル*/
HANDLE		hLOGF;
HANDLE		hSAVE1F;		/*セーブファイル1*/
HANDLE hSF;
HANDLE hSSF;
HWND   hogeWnd;
CRITICAL_SECTION cscom;
DWORD      dwCommEvent;
DWORD      dwStoredFlags;

struct char_type C_C;
struct key_data key;
struct rcv_data SND;
struct send_timer sendtimer;
//struct serial_param COMMPARAM;
struct serial_param COMMPARAM[5];
char TMPDMY[100];
//unsigned char frm[31][66];		//画面表示キャラクタバッファ
//char frm[30][66];		//画面表示キャラクタバッファ
//char sfrm[6][66];		//画面表示キャラクタバッファ
//char frmm[100*200];		//画面表示キャラクタバッファ
//char sfrmm[20*200];		//画面表示キャラクタバッファ
//unsigned long wtpoint;	//表示終端文字位置
//unsigned long xtpoint;	//表示終端文字位置
//unsigned long wwtpoint;	//表示終端文字位置
//unsigned long wstpoint;	//表示終端文字位置
int frm_U[100];			//画面表示ASCII-UTF-8-HEXフラグ
int sfrm_U[RTN_SIZE+2];			//画面表示ASCII-UTF-8-HEXフラグ
int retcnt;
long RtnPos[RTN_SIZE+2];
long sRtnPos[RTN_SIZE+2];
unsigned int RtnN;
unsigned int FstN;
char frmm[LINGBUF_SIZE];		//画面表示キャラクタバッファ
unsigned int sRtnN;
unsigned int sFstN;
char sfrmm[SENDBUF_SIZE];		//画面表示キャラクタバッファ
unsigned long stpoint; //表示先頭位置
unsigned long edpoint; //表示末尾位置
unsigned long lnpoint;
unsigned long sstpoint; //表示先頭位置
unsigned long sedpoint; //表示末尾位置

struct dispframe CDSP;

char  DataFolder[128];
char  LOGFolder[128];
char  LOG_file[128];
char  SAVE1_file[128];
char  CMD_file[64];
char  MEASURE_DEVICE_file[64];
char  SCRIPT_file[64];
char  TLM_file[64];
char  REG_file[64];
WCHAR  PROGRAM_TITLE[64];
unsigned long long TLMDATA[TLMDATA_NUM];

int cmd_count;
unsigned char C_dummy[256];
char label_dummy[256];
WCHAR Wlabel_dummy[256];
int C_count;
int P_count;
int portset_flg;
int label_count;
int label_flg;
int interval_flg;
int interval_val;
int Display_flg; //0:text 1:hex
int port_flg;
int measure_onflg;
int part_counter;
int sub_counter;
int RtnCnt;
int RtnThd;
int LogSave_flg;
int LogSave_Count;
int DataSave_Count;
int Interval;
int Lua_flg;
int FIXParam_Cnt;
int check_interval;
int devicemaker_num[MAKER_MAX];
int keywait_flg;
int timewait_flg;
int NowTask;
int Debug_flg;
int first_flg;

int BUSON_flg;
//int WSIZE_WIDTH = 558;
//int	WSIZE_HEIGHT= 530;
unsigned int WSIZE_WIDTH = 586;
unsigned int WSIZE_HEIGHT= 586;

int OpenPort_mode;
nlabel LL;	/*変数ラベル格納クラス*/
vector<cmd_pak> CV;
vector<tlm_struct> TLM; /*テレメトリ定義：tlm_decorderを使うとデコードできる*/
ulabel LM;	/*変数ラベル格納クラス*/

BOOL fSuccess;		//シリアルポート設定エラーフラグ
vector<measurement_device> MDEV;/*計測器定義*/
vector<usb_serial_param> USBSER;
//vector<keysight_ivi_device> VISESS;
vector<usbhid_dev> UHDEV;
vector<Socket> NET;
//vector<ftdi_dev> BITBANG;

long BaudRate_read;
int Save_flg;
int send_save_flg;
int modem_flg;
int cmd_format_flg;
unsigned int fkazu;
char Filename[20];
WCHAR WFilename[20];
char PortText[32] ={0};
int spline;
int spline2;
unsigned char key_outbuf[128];
int key_outbuflen;
//unsigned char	GetBufin[COMMBUFSIZE*2];
unsigned char	GetBufin[6][COMMBUFSIZE*2];
char comm_string[32];
char comm_string2[32];
CHAR szAppName[]="BinCom";

//extern vector<reg_struct> REG;
extern vector<reg_main> REG;

static lua_State *globalL = NULL;
//static lua_State *LUA = luaL_newstate();
static const char *progname = LUA_PROGNAME;
//ViSession resourceManager;

lua_State *L = luaL_newstate();

// これはUSBデバイスクラスのGUIDです
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
			0xA5DCBF10L,0x6530,0x11D2,0x90,0x1F,0x00,0xC0,0x4F,0xB9,0x51,0xED);


//struct ComReadBuf{
//	unsigned char Buf[COMMBUFSIZE];
//	unsigned char Buf2[COMMBUFSIZE];
//	long len;
//	long len2;
//	int flg;
//}COMBUF;

//struct ComReadBuf COMBUF;
struct ComReadBuf COMBUF[6];

std::map<int,char[32]> PortList;

/*--------------------------------------------------------------------------*/
/* USB VID PID シリアル番号を取得する。取得情報はusb_serial_param型の       */
/* stl::vectorのグローバルオブジェクトUSBSERに格納される。事前に宣言する事  */
/*--------------------------------------------------------------------------*/
int getUSBVID(void)
{
    HDEVINFO device_info;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detail_data = NULL;
    ULONG required_length = 0;
	int idx = 0;
	int i,n,m,k;
	char * end;
	usb_serial_param V1;
	TCHAR *BUFF2 = new TCHAR[128];

    // デバイス情報セットを取得
    device_info = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, 
    								  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (device_info == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to get device information set." << std::endl;
        return 1;
    }                                      
    SP_DEVINFO_DATA deviceInfoData = { 0 };
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD j = 0; SetupDiEnumDeviceInfo(device_info, j, &deviceInfoData); j++) {
        char hardwareIDBuffer[256];
        DWORD requiredSize;
        // ハードウェアIDを取得
        if (SetupDiGetDeviceRegistryPropertyA(device_info, &deviceInfoData, 
        	SPDRP_HARDWAREID, NULL,(PBYTE)hardwareIDBuffer, sizeof(hardwareIDBuffer), 
        	&requiredSize)) {
            // VIDとPIDを解析
            std::string hardwareID(hardwareIDBuffer);
            size_t vidPos = hardwareID.find("VID_");
            size_t pidPos = hardwareID.find("PID_");
            if (vidPos != std::string::npos && pidPos != std::string::npos) {
                std::string vid = hardwareID.substr(vidPos + 4, 4);
                std::string pid = hardwareID.substr(pidPos + 4, 4);
				V1.vid = strtol(vid.c_str(),&end,16);
				V1.pid = strtol(pid.c_str(),&end,16);
            }
		}
        // COMポート名を取得
        if (SetupDiGetDeviceRegistryPropertyA(device_info, &deviceInfoData, 
        	SPDRP_FRIENDLYNAME, NULL, (PBYTE)V1.port,sizeof(V1.port), &requiredSize)) {
        }
		/*シリアル番号を取り出す。一緒にVID,PIDも取れるがここではシリアルだけ*/
		if(CM_Get_Device_IDW(deviceInfoData.DevInst, BUFF2,128, 0)== CR_SUCCESS){
		//	MessageBoxW(NULL,BUFF2, L"Hoge", MB_ICONHAND);	
		// "FTDIBUS\VID_0403+PID_6015+DK0G0GKMA\0000"
			n = 0;
			m = 0;
			k = 0;
			for(i=0;i<wcslen(BUFF2);i++){
			//	if((BUFF2[i] == '+')||(BUFF2[i] == '\\')){
				if(BUFF2[i] == '\\'){
					n++;
					m++;
				}else if(BUFF2[i] == '+'){
					n++;
				}else if(BUFF2[i] == '&'){
					m++;
				}else{
				//	if((n == 3)&&(k<8)){//WCHARで8文字(charで倍)
					if(((n == 3)||(m == 4))&&(k<8)){//WCHARで8文字(charで倍)
						V1.ser[k] = BUFF2[i];
						k++;
					}
				}
			}
			V1.ser[k] = 0;
		//	MessageBoxW(NULL,V1.ser, L"Hoge", MB_ICONHAND);
		}
		char ATmp[4];
	//	char * result = strpbrk(V1.port, "COM");
	//	ATmp[0] = result[3];
	//	ATmp[1] = result[4];

		char * result = strpbrk(V1.port, "C");
		char * result2 = strpbrk(result, "O");
		char * result3 = strpbrk(result2, "M");
		
		ATmp[0] = result3[1];
		ATmp[1] = result3[2];

		ATmp[2] = 0;
		if (ATmp[1] == ')')ATmp[1] = 0;
		V1.port_num = atoi(ATmp);
		USBSER.push_back(V1);
        idx++;
        ::ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);        
    }
    // 情報セットを解放
    SetupDiDestroyDeviceInfoList(device_info);
    delete[] BUFF2;
    BUFF2 = NULL;
    return 0;
}

unsigned short CRC_CCITT_MSBfirst_ALL(unsigned short len, unsigned char bufp[]){
	unsigned char i,j;
	unsigned char ch = 0;
	unsigned short crc = 0xffff;
//	unsigned short crc = 0x0000;

	for(j = 0;j < len;j++){
		ch = bufp[j];
		crc ^= ((unsigned short)ch << 8);
		for (i = 0; i < 8; i++){
			if(crc & 0x8000){
				crc = crc << 1;
				crc ^= POLY;
			}else{
				crc = crc << 1;
			}
		}
	}
	return crc;
}
unsigned short CRC_CCITT_LSBfirst_ALL1(unsigned short len, unsigned char bufp[]){
	unsigned char i,j;
	unsigned char ch = 0;
	unsigned short crc = 0xffff;

	for(j = 0;j < len;j++){
		ch = bufp[j];
		crc ^= (unsigned short)ch;
		for (i = 0; i < 8; i++){
			if(crc & 0x0001){
				crc = crc >> 1;
				crc ^= POLY;
			}else{
				crc = crc >> 1;
			}
		}
	}
	return crc;
}
/*--------------------------------------------------------------------------*/
int nlabel::serch(char * nname){
	unsigned int i;
	int j;
	j = -1;	/*渡された文字列と同じラベル文字列が無ければ-1を返す*/
	for(i=0;i<LV.size();i++){
		if(!(strcmp(LV[i].name,nname))){
			j = i;
			break;
		}
	}
	return j;
}
void nlabel::insert(char * nname,int nvar,int type){
	mlabel M;
//	unsigned long long i;
	rsize_t i;

//	i = (unsigned long long)(strlen(nname));
	i = (rsize_t)(strlen(nname));
	M.name = new char[i+1];
	strcpy_s(M.name,i+1,nname);
	M.id = nvar;
	M.type = type;
	LV.push_back(M);
}
/*--------------------------------------------------------------------------*/
int ulabel::serch(char * nname){
	unsigned int i;
	int j;
	j = -1;	/*渡された文字列と同じラベル文字列が無ければ-1を返す*/
	for(i=0;i<LU.size();i++){
		if(!(strcmp(LU[i].name,nname))){
			j = i;
			break;
		}
	}
	return j;
}
void ulabel::insert(char * nname,long addr, long var){
	plabel M;
//	unsigned long long i;
	rsize_t i;

//	i = (unsigned long long)(strlen(nname));
	i = (rsize_t)(strlen(nname));
	M.name = new char[i+1];
	strcpy_s(M.name,i+1,nname);
	M.addr = addr;
	M.num = var;
	LU.push_back(M);
}
//------------------------------------------------------------------------
//	long値実数から16進ASCIIへ。ltoa()を使わないのは、出力を大文字にしたい
//だけの理由から
//------------------------------------------------------------------------
int ltohex(unsigned long l,unsigned char * buf,unsigned char i)
{
	int j,k;
	unsigned long x;
	for(j = 0;j < i;j++) {
		x = l;
		for (k = 0;k < (i - j - 1);k++) x = (x >> 4);
		buf[j] = "0123456789ABCDEF"[(x & 0x0f)];
	}
	buf[j] = 0;
	return j;
}
//----------------------------------------------------------
//　16進/2進/10進文字列をバイナリに変換
// 戻り値はエラー(=0)、lenは受け入れバイト数(普通は入力文字数)
//----------------------------------------------------------
long axtol(char * inbuf,unsigned char * outbuf,unsigned int len)
{
	char asc[]={'0','1','2','3','4','5','6','7','8','9',
				'A','B','C','D','E','F',
				'a','b','c','d','e','f','Q'};
	unsigned long val,i,j,k,m;
	val = 0;
	if(len == 0)return 0;/*エラー*/
	if((inbuf[0] == '0')&&((inbuf[1] == 'x')||(inbuf[1] == 'X'))){
	/*16進とみなす*/
		if(len > 10)len = 10;/*16進8桁以上は無効 先頭側が有効*/
		for(i=0;i<(len-2);i++){
			for(j=0;j<23;j++){
				if(asc[j] == inbuf[2+i])break;
			}
			if(j>21){
				return 0;	/*エラー*/
			}else if(j>15){
				j-=6;
			}
			val |= j <<(((len-3)-i)<<2);
		}
	}else if((inbuf[0] == '0')&&((inbuf[1] == 'b')||(inbuf[1] == 'B'))){
	/*2進とみなす*/
		if(len > 34)len = 34;/*2進32桁以上は無効 先頭側が有効*/
		for(i=0;i<(len-2);i++){
			if(inbuf[2+i] == '1'){
				val |= 1<<((len-3)-i);
			}else if(inbuf[2+i] != '0'){
				return 0;
			}
		}
	}else{
	/*10進とみなす*/
		if(len > 10)len = 10;/*10進10桁以上は無効 先頭側が有効*/
		if(inbuf[0] == '-'){
			j = 1;
		}else{
			j = 0;
		}
		for(i=0;i<(len-j);i++){
			if(inbuf[i+j] >= '0' && inbuf[i+j] <= '9'){
				k = 1;
				for(m=0;m<(((len-j)-1)-i);m++)k*=10;
				val += (inbuf[i+j] - '0')*k;
			}else{
				return 0;
			}
		}
		if(val > 0xffffffff)val = 0xffffffff;
		if(j)val = (0xffffffff - val)+1;/*2の補数で*/
	}
	for(i=0;i<4;i++)outbuf[i] = (val >> ((3-i)*8))&0xff;
	return 1;
}
//----------------------------------------------------------
//　16進/2進/10進Unicode文字列をバイナリに変換
// 戻り値はエラー(=0)、lenは受け入れバイト数(普通は入力文字数)
//----------------------------------------------------------
long axtolW(WCHAR * inbuf,unsigned char * outbuf,unsigned int len)
{
	char asc[]={'0','1','2','3','4','5','6','7','8','9',
				'A','B','C','D','E','F',
				'a','b','c','d','e','f','Q'};
	unsigned long val,i,j,k,m;
	val = 0;
	if(len == 0)return 0;/*エラー*/
	if(((inbuf[0]&0x00ff) == '0')
	&&(((inbuf[1]&0x00ff) == 'x')
	 ||((inbuf[1]&0x00ff) == 'X'))){
	/*16進とみなす*/
		if(len > 10)len = 10;/*16進8桁以上は無効 先頭側が有効*/
		for(i=0;i<(len-2);i++){
			for(j=0;j<23;j++){
				if(asc[j] == (inbuf[2+i]&0x00ff))break;
			}
			if(j>21){
				return 0;	/*エラー*/
			}else if(j>15){
				j-=6;
			}
			val |= j <<(((len-3)-i)<<2);
		}
	}else if(((inbuf[0]&0x00ff) == '0')
	      &&(((inbuf[1]&0x00ff) == 'b')
	       ||((inbuf[1]&0x00ff) == 'B'))){
	/*2進とみなす*/
		if(len > 34)len = 34;/*2進32桁以上は無効 先頭側が有効*/
		for(i=0;i<(len-2);i++){
			if(inbuf[2+i] == '1'){
				val |= 1<<((len-3)-i);
			}else if((inbuf[2+i]&0x00ff) != '0'){
				return 0;
			}
		}
	}else{
	/*10進とみなす*/
		if(len > 10)len = 10;/*10進10桁以上は無効 先頭側が有効*/
		if((inbuf[0]&0x00ff) == '-'){
			j = 1;
		}else{
			j = 0;
		}
		for(i=0;i<(len-j);i++){
			if((inbuf[i+j]&0x00ff) >= '0' && (inbuf[i+j]&0x00ff) <= '9'){
				k = 1;
				for(m=0;m<(((len-j)-1)-i);m++)k*=10;
				val += ((inbuf[i+j]&0x00ff) - '0')*k;
			}else{
				return 0;
			}
		}
		if(val > 0xffffffff)val = 0xffffffff;
		if(j)val = (0xffffffff - val)+1;/*2の補数で*/
	}
	for(i=0;i<4;i++)outbuf[i] = (val >> ((3-i)*8))&0xff;
	return 1;
}
//------------------------------------------------------------------------
//　キャラクタのスクロール
//------------------------------------------------------------------------
void scroll(unsigned int kai)
{
    unsigned int j;
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	int ws = ((WSIZE_WIDTH-12)/8)-1;

    for(j=0;j<kai;j++){
		if(frm_U[RtnN] == 1){//Hex
			stpoint+= ws;//表示先頭の行終端へ
		//	if(stpoint>=(LINGBUF_SIZE-1))stpoint=(stpoint-(LINGBUF_SIZE-1));
			if(stpoint>(LINGBUF_SIZE-1))stpoint=(stpoint-(LINGBUF_SIZE-1));
			if((int)(FstN+1) >= ((hs*4)+(3-wh))){
		//	if((int)(FstN+1) > ((hs*4)+(3-wh))){
				FstN = 0;
			}else{
				if((FstN+1) == RtnN){
					if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
				//	if((int)(RtnN+1) > ((hs*4)+(3-wh))){
						RtnN = 0;
					}else{
						RtnN++;
					}
					RtnPos[RtnN] = edpoint;
				}
				FstN++;
			}
			RtnPos[FstN] = stpoint;
		}else{
	    	while((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n')){//表示先頭の行終端を探す
				stpoint++;
				if(stpoint>(LINGBUF_SIZE-1))stpoint=0;
			}
			//ここでstpointは0か\nを指している
			while(frmm[stpoint] == 0){
				stpoint++;
				if(stpoint>(LINGBUF_SIZE-1))stpoint=0;
			}
			if((int)(FstN+1) >= ((hs*4)+(3-wh))){
				FstN = 0;
			}else{
				if((FstN+1) == RtnN){
					if((int)RtnN >= ((hs*4)+(3-wh))){
						RtnN = 0;
					}else{
						FstN++;
					}
					RtnPos[RtnN] = edpoint;
				}
				FstN++;
			}
			RtnPos[FstN] = stpoint;
		}
	}
}
//------------------------------------------------------------------------
//　キャラクタのスクロール
//------------------------------------------------------------------------
void send_scroll(unsigned int kai)
{
    unsigned int j;
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	int ws = ((WSIZE_WIDTH-12)/8)-1;

    for(j=0;j<kai;j++){
		if(sfrm_U[sRtnN] == 1){//Hex
			sstpoint+= ws;//表示先頭の行終端へ
			if(sstpoint>=(SENDBUF_SIZE-1))sstpoint=(sstpoint-(SENDBUF_SIZE-1));
			if((int)(sFstN+1) > hs){
				sFstN = 0;
			}else{
				if((sFstN+1) >= sRtnN){
					if((int)(sRtnN+1) > hs){
						sRtnN = 0;
					}else{
						sRtnN++;
					}
					sRtnPos[sRtnN] = sedpoint;
				}
				sFstN++;
			}
			sRtnPos[sFstN] = sstpoint;
		}else{
	    	while((sfrmm[sstpoint]!=0)&&(sfrmm[sstpoint]!='\n')){//表示先頭の行終端を探す
				sstpoint++;
				if(sstpoint>(SENDBUF_SIZE-1))sstpoint=0;
			}
			//ここでstpointは0か\nを指している
			while(sfrmm[sstpoint] == 0){
				sstpoint++;
				if(sstpoint>(SENDBUF_SIZE-1))sstpoint=0;
			}
			if((int)(sFstN+1) > RTN_SIZE){
				sFstN = 0;
			}else{
				if((sFstN+1) >= sRtnN){
					if((int)sRtnN > RTN_SIZE){
						sRtnN = 0;
					}else{
						sRtnN++;
					}
					sRtnPos[sRtnN] = sedpoint;
				}
				sFstN++;
			}
			
			if((int)(sRtnN+1) > RTN_SIZE){
				sRtnN = 0;
			}else{
				if((sRtnN+1) == sFstN){
					if((int)(sFstN+1) > RTN_SIZE){
						sFstN = 0;
					}else{
						sFstN++;
					}
					sRtnPos[sFstN] = sstpoint;
				}
		//		sRtnN++;
			}
		//	sRtnPos[sRtnN] = sedpoint;
			sRtnPos[sFstN] = sstpoint;
		}
	}
}
/*-------------------------------------------------------------------------------------*/
/*int UTF8Len(const char * Buf,int * nlen)                                             */
/* Unsigned char バイト列で渡されたUTF-8文字列の文字数を返す                           */
/* またUTF-8文字列長が引数2のwidec文字目に達した時のバイト数を引数の3番目で返す        */
/*これはUTF-8文字列を画面一行の表示で収まるバイト文字列長を返すことを意図している      */
/*-------------------------------------------------------------------------------------*/
int UTF8Len(const unsigned char * Buf,int widec, unsigned long * nlen)
{
	int i,j;
	i = 0;
	WCHAR WBUF[256] = {0};
	
	while((Buf[i] != 0)&&(i<128)){
		if(Buf[i] > 0x1f && Buf[i] < 0x7f){	/*もし1バイト文字なら*/
			j = (int)(wcslen(WBUF));
			if(j < 255){
	    	    WBUF[j] = Buf[i];
				WBUF[j+1] = 0;
			}
			j = (int)(wcslen(WBUF));
			i++;
			if(j == widec){
				(*nlen) = i;
			}else{
				(*nlen) = j;
			}
		}else{
			if(((Buf[i] & 0xff) >= 0xC2)&&((Buf[i] & 0xff) <= 0xDF)){
				j = (int)wcslen(WBUF);
				if(j < 255){
				    WBUF[(j&0xff)] = ((Buf[i] & 0x1f)<< 6)|(Buf[i+1] & 0x3f);
					WBUF[(j+1)&0xff] = 0;
				}
				j = (int)wcslen(WBUF);
				i+=2;
				if(j == widec){
					*nlen = i;
				}else{
					(*nlen) = j;
				}
			}else if(((Buf[i] & 0xff) >= 0xE0)&&((Buf[i] & 0xff) <= 0xEF)){
				j = (int)wcslen(WBUF);
				if(j<255){
	       			WBUF[(j&0xff)] = ((Buf[i] & 0x0f) << 12)
  							|((Buf[i+1] & 0x3f) << 6)|(Buf[i+2] & 0x3f);
					WBUF[((j+1)&0xff)] = 0;
				}
				j = (int)wcslen(WBUF);
				i+=3;
				if(j == widec){
					*nlen = i;
				}else{
					(*nlen) = j;
				}
			}else if(((Buf[i] & 0xff) >= 0xF0)&&((Buf[i] & 0xff) <= 0xF4)){
				j = (int)wcslen(WBUF);
				if(j<255){
					WBUF[(j&0xff)] = ((Buf[i]   & 0x07) << 18)
									|((Buf[i+1] & 0x3f) << 12)
									|((Buf[i+2] & 0x3f) << 6)
									| (Buf[i+3] & 0x3f);
					WBUF[(j+1)&0xff] = 0;
				}
				j = (int)wcslen(WBUF);
				i+=4;
				if(j == widec){
					*nlen = i;
				}else{
					(*nlen) = j;
				}
			}else{
				j = (int)wcslen(WBUF);
				if(j == widec){
					*nlen = i;
				}else{
					(*nlen) = j;
				}
				if(j<253){
					if(i < 250){
						i++;
					}else{
						break;
					}
				}else{
					break;
				}
			}
		}
		if(j>254)break;
	}
	return (int)wcslen(WBUF);
}
//------------------------------------------------------------------------
//	特殊アスキーコード及び未定義分を"*"に置き換える
//------------------------------------------------------------------------
unsigned char charset(unsigned char in)
{									//ASCIIコードを受け取り
    if (in<32 || in>126) in = 42;	//フツーの文字以外は皆"*"に
    return in;
}
//------------------------------------------------------------------------
//  16進文字列表記をキャラクターに直す
//	特殊アスキーコード及び未定義分を"*"に置き換える
//------------------------------------------------------------------------
unsigned char Incharset(unsigned char * in)
{									//ASCIIコードを受け取り
	unsigned char ch = 0;
	if(in[0] > '9'){
		ch = ((10 + (in[0] -'A')) << 4)&0xf0;
	}else{
		ch = ((      in[0] - '0') << 4)&0xf0;
	}
	if(in[1] > '9'){
		ch |= (10 + (in[1] -'A'))&0x0f;
	}else{
		ch |=       (in[1] - '0')&0x0f;
	}
    if (ch<32 || ch>126) ch = 42;	//フツーの文字以外は皆"*"に
    return ch;
}
//------------------------------------------------------------------------
//汎用表示
//------------------------------------------------------------------------
void DisplayData(HWND hWnd, unsigned char* Bufin, DWORD len)
{
	unsigned int i,k;
	unsigned long n;
	int m;
	int ws = ((WSIZE_WIDTH-12)/8)-1;
	int vs = ((WSIZE_WIDTH-12)/8);
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);

	for (i = 0; i < len; i++) {
		/*改行条件*/
		if ((Bufin[i] == '\n')||(Bufin[i] == '\r')) {
			if(edpoint!=0){
				if(frmm[edpoint-1] != '\n'){//CRLF除去
					frmm[edpoint] = '\n';
					if((edpoint+1) == stpoint){
						stpoint++;
						if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
						//stpointを先行させ直す
					}
					if((edpoint+1) > LINGBUF_SIZE){
						edpoint = 0;
						if(stpoint == 0){
							stpoint++;
							if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
						}
					}
					if(stpoint >= LINGBUF_SIZE){
						stpoint = 0;
					}
					retcnt++;
				}//既に改行文字がある場合文字は無視される
			}else{
				frmm[edpoint] = '\n';
				if((edpoint+1) == stpoint){
					stpoint++;
					if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
				}
				if((edpoint+1) > LINGBUF_SIZE){
					edpoint = 0;
					if(stpoint == 0){
						stpoint++;
						if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
					}
				}
				if(stpoint >= LINGBUF_SIZE){
					stpoint = 0;
				}
				retcnt++;
			}
			if(retcnt >= ((hs*4)+(3-wh)-1)) {
				scroll(1);			//収まるようにスクロール
				retcnt--;
			}
			m = UTF8Len((const unsigned char *)&frmm[lnpoint],ws,&n);
			if(m == strlen(&frmm[lnpoint])){//ASCII
			//	frm_U[k] = 0;
				frm_U[k] = (Display_flg & 0x01);
			}else{
				frm_U[k] = 2;
			}
			if(m>ws){//一行より大きいなら改行する
				for(k=edpoint;k>(lnpoint+n);k--){
					frmm[k+1] = frmm[k];
				}
				frmm[lnpoint+n+1] = 0;
				edpoint++;
				retcnt++;

				if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
					RtnN = 0;
				}else{
					if((RtnN+1) == FstN){
						if((int)(FstN+1) >= ((hs*4)+(3-wh))){
							FstN = 0;
						}else{
							FstN++;
						}
						RtnPos[FstN] = stpoint;
					}
					RtnN++;
				}

				RtnPos[RtnN] = lnpoint+n+2;/*書き直し*/
				lnpoint = lnpoint+n+2;
			}
			RtnN++;
			if(FstN == RtnN)FstN++;
			if((int)RtnN >= ((hs*4)+(3-wh)))RtnN = 0;
			if((int)FstN >= ((hs*4)+(3-wh)))FstN = 0;
			edpoint++;
			RtnPos[RtnN] = edpoint;
			
			lnpoint = edpoint;
			HexDataREP(hWnd);
		}else if ((i + 1) == len) {
			frmm[edpoint] = Bufin[i];
			if((edpoint+1) == stpoint){
				stpoint++;
				if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
			}
			if((edpoint+1) > LINGBUF_SIZE){
				edpoint = 0;
				if(stpoint == 0)stpoint++;
			}
			if(stpoint > LINGBUF_SIZE){
				stpoint = 0;
			}
			edpoint++;
			frmm[edpoint] = 0;//行末
			if(edpoint == stpoint){
				stpoint++;
				if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
			}
			if((edpoint+1) > LINGBUF_SIZE){
				edpoint = 0;
				if(stpoint == 0)stpoint++;
			}
			if(stpoint > LINGBUF_SIZE){
				stpoint = 0;
			}
			m = UTF8Len((const unsigned char *)&frmm[lnpoint],ws,&n);
			if(m == strlen(&frmm[lnpoint])){//ASCII
			//	frm_U[k] = 0;
				frm_U[k] = (Display_flg & 0x01);
			}else{
				frm_U[k] = 2;
			}
			if(m>ws){//一行より大きいなら改行する
				for(k=edpoint;k>(lnpoint+n);k--){
					frmm[k+1] = frmm[k];
				}
				frmm[lnpoint+n+1] = 0;
				edpoint++;
				retcnt++;
				
				if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
					RtnN = 0;
				}else{
					if((RtnN+1) == FstN){
						if((int)(FstN+1) >= ((hs*4)+(3-wh))){
							FstN = 0;
						}else{
							FstN++;
						}
						RtnPos[FstN] = stpoint;
					}
					RtnN++;
				}
				RtnPos[RtnN] = lnpoint+n+2;/*書き直し*/
				lnpoint = lnpoint+n+2;
			}
			
			if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
				RtnN = 0;
			}else{
				if((RtnN+1) == FstN){
					if((int)(FstN+1) >= ((hs*4)+(3-wh))){
						FstN = 0;
					}else{
						FstN++;
					}
					RtnPos[FstN] = stpoint;
				}
				RtnN++;
			}
			edpoint++;
			RtnPos[RtnN] = edpoint;//新しい終端
			retcnt++;

			lnpoint = edpoint;
			HexDataREP(hWnd);
		}else {
			frmm[edpoint&0x7fff] = Bufin[i];
			edpoint++;
			frmm[edpoint&0x7fff] = 0;
			if(edpoint+1 < LINGBUF_SIZE)frmm[edpoint+1] = 0;
			if(edpoint+2 < LINGBUF_SIZE)frmm[edpoint+2] = 0;
			if(edpoint+3 < LINGBUF_SIZE)frmm[edpoint+3] = 0;
			if(edpoint == stpoint){
				stpoint++;
				if((frmm[stpoint]!=0)&&(frmm[stpoint]!='\n'))stpoint++;
			}
			if(edpoint > LINGBUF_SIZE){
				edpoint = 0;
				if(stpoint == 0)stpoint++;
			}
			if(stpoint > LINGBUF_SIZE){
				stpoint = 0;
			}
			
			m = UTF8Len((const unsigned char *)&frmm[lnpoint],ws,&n);
			if((long)n >= ws){
			//	m = UTF8Len((const unsigned char *)&frmm[lnpoint],ws,&n);
				for(k=edpoint;k>(lnpoint+n);k--){
					frmm[(k+1)&0x7fff] = frmm[k&0x7fff];
				}
				frmm[(lnpoint+n+1)&0x7fff] = 0;
				RtnPos[RtnN] = lnpoint+n+1;/*書き直し*/
				edpoint++;
				retcnt++;
				
				if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
					RtnN = 0;
				}else{
					if((RtnN+1) == FstN){
						if((int)(FstN+1) >= ((hs*4)+(3-wh))){
							FstN = 0;
						}else{
							FstN++;
						}
						RtnPos[FstN] = stpoint;
					}
					RtnN++;
				}
				
				lnpoint = lnpoint+n+2;
			}
			if(retcnt >= ((hs*4)+(3-wh)-1)) {
				scroll(1);			//収まるようにスクロール
				retcnt--;
			}
		}
	}
	DisplayParam(hWnd);
}

//------------------------------------------------------------------------
//送出データ表示
//------------------------------------------------------------------------
void SendDataP(HWND hWnd,unsigned char * Bufin,DWORD len)
{
	HDC		   hdc;	//ハンドル
	HFONT hFont;
	unsigned int i,j,k,m,n,o,p;
	size_t ret;
	unsigned int ws,ds,hs;
	ws = ((WSIZE_WIDTH-12)/8);
	ds = (ws/3)-1;
	hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	WCHAR * Disp = new WCHAR[ws+1];
	unsigned char buf[4096];		//バイナリ>キャラクタ変換用バッファ

	if(len > 0){
		for(i=0;i<len;i++){
			sfrmm[sedpoint&0xfff] = Bufin[i];
			if((sedpoint+1) == sstpoint){
				if((sstpoint+1) > SENDBUF_SIZE){
					sstpoint = 0;
				}else{
					sstpoint++;
				}
				while(sfrmm[sstpoint]!=0){
					sstpoint++;
				}
				while(sfrmm[sstpoint]==0){
					sstpoint++;
				}
				if(((sRtnN - sFstN)+1) > hs){
					send_scroll(1);
				}
				sFstN++;
				sRtnPos[sFstN] = sstpoint;
			}
			//行末位置更新
			if((sedpoint+1) > SENDBUF_SIZE){
				sedpoint = 0;
			}else{
				sedpoint++;
			}
		}
		sfrmm[sedpoint&0xfff] = 0;
		if((sedpoint+1) == sstpoint){
			if((sstpoint+1) > SENDBUF_SIZE){
				sstpoint = 0;
			}else{
				sstpoint++;
			}
			while(sfrmm[sstpoint]!=0){
				sstpoint++;
			}
			while(sfrmm[sstpoint]==0){
				sstpoint++;
			}
			if(((sRtnN - sFstN)+1) > hs){
				send_scroll(1);
			}
			sFstN++;
			sRtnPos[sFstN] = sstpoint;
		}
		if((sedpoint+1) > SENDBUF_SIZE){
			sedpoint = 0;
		}else{
			sedpoint++;
		}
		sfrm_U[sRtnN] = 1;
	}else{
		o = (sRtnPos[sRtnN] - sRtnPos[(sRtnN-1)]);
//	for(i=0;i<len;i++){//前回入力を表示する
		for(i=0;i<o;i++){//前回入力を表示する
	//	sfrmm[sedpoint&0xfff] = Bufin[i];
			sfrmm[sedpoint&0xfff] = sfrmm[(sRtnPos[(sRtnN-1)]+i)&0xfff];
			if((sedpoint+1) == sstpoint){
				if((sstpoint+1) > SENDBUF_SIZE){
					sstpoint = 0;
				}else{
					sstpoint++;
				}
				while(sfrmm[sstpoint]!=0){
					sstpoint++;
				}
				while(sfrmm[sstpoint]==0){
					sstpoint++;
				}
				if(((sRtnN - sFstN)+1) > hs){
					send_scroll(1);
				}
				sFstN++;
				sRtnPos[sFstN] = sstpoint;
			}
			//行末位置更新
			if((sedpoint+1) > SENDBUF_SIZE){
				sedpoint = 0;
			}else{
				sedpoint++;
			}
		}
	}
	if((sRtnN + 1)>= RTN_SIZE){
		sRtnN = 0;
	}else{
		sRtnN++;
	}
	sRtnPos[sRtnN] = sedpoint;

	if(sRtnN == sFstN){//一行先頭更新
	//	if((sFstN+1) > hs){//先頭カウンタ移動
		if(((sRtnN - sFstN)+1) > hs){
			send_scroll(1);
		}
		sFstN++;
		while(sfrmm[sstpoint]!=0){
			sstpoint++;//行末へ移動
		}
		while(sfrmm[sstpoint]==0){
			sstpoint++;//次の行頭へ移動
		}
		sRtnPos[sFstN] = sstpoint;
	}
	
	if(((sRtnN - sFstN)+1) > hs){
		send_scroll(1);
	}
	
	hdc = GetDC(hWnd);
    hFont = (HFONT)SelectObject(hdc, hUFont);
	
//  for(k=0;k<=hs;k++){			//6行
    for(k=0;k<hs;k++){			//6行
		j = sFstN + k;
		if((j+1)>hs){
			n = (j+1)-hs;
		}else{
			n = j+1;
		}
		if(j > hs)j -= hs;
	//	if((sRtnPos[j]!=-1)&&(sfrmm[sRtnPos[j]] != 0)){
	//	if(sRtnPos[j]!=-1){
		if(sRtnPos[n]!=-1){
			Disp[0] = '>';
			Disp[1] = 0;
	    //	size_t offset = sRtnPos[j];
    	//	const char* source = (const char*)&sfrmm[offset];

		//	if((sRtnPos[n]-2)<0){
		//		
		//	}else{
		//		
		//	}
			if(sfrm_U[j] == 1){
				p = (sRtnPos[n]-1) - sRtnPos[j];
				if(p > (ws/3))p = (ws/3);
				m = 0;
				buf[0] = 0;
			//	for(i=sRtnPos[j];i<(sRtnPos[n]-1);i++){
				for(i=sRtnPos[j];i<(sRtnPos[j]+p);i++){
    				ltohex((unsigned char)(sfrmm[i]),&buf[(m*3)],2);
        			buf[(m*3)+2] = 0x20;		//2文字ごとに空白文字を入れて見やすく
					buf[(m*3)+3] = 0;
					m++;
				}
    			const char* source = (const char*)buf;
     			if (source != nullptr) {
        			mbstowcs_s(&ret,Disp+1,ws,source,ws-1);
    			}
			}else{
	    		size_t offset = sRtnPos[j];
    			const char* source = (const char*)&sfrmm[offset];
    			if (source != nullptr) {
        			mbstowcs_s(&ret,Disp+1,ws,source,ws-1);
    			}
   			}
			
			i = (int)(wcslen(Disp));
			for(n=i;n<ws;n++)Disp[n] = L' ';
			Disp[ws] = 0;
			
		//	TextOutW(hdc,4,((hs*4)+(3-wh)+1)*15+4,Disp,ws-1);
			TextOutW(hdc,4,((hs*4)*15)+((wh+1)*30)+4+(15*k),Disp,ws-1);
		}
    }

//  SelectObject(hdc,hNewFont);
    SelectObject(hdc,hUFont);
    DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
	delete[] Disp;
	Disp = NULL;
}
//------------------------------------------------------------------------
//送出データ表示
//------------------------------------------------------------------------
void SendDataPK(HWND hWnd,unsigned char ch)
{
//	int j,k;
//	unsigned int i;
	int ws = ((WSIZE_WIDTH-12)/8);
	unsigned int hs = ((WSIZE_HEIGHT-80)/75)-1;

//	if(((ch > 31) && (ch < 127))||(ch == 0x0a)){
	if((ch > 31) && (ch < 127)){
		sfrmm[sedpoint&0xfff] = ch;
		key.spchar++;
		if((sedpoint+1) == sstpoint){
			if((sstpoint+1) > SENDBUF_SIZE){
				sstpoint = 0;
			}else{
				sstpoint++;
			}
			while(sfrmm[sstpoint]!=0){
				sstpoint++;
			}
			while(sfrmm[sstpoint]==0){
				sstpoint++;
			}
			if(((sRtnN - sFstN)+1) > hs){
				send_scroll(1);
			}
			sFstN++;
			sRtnPos[sFstN] = sstpoint;
		}
		if((sedpoint+1) > SENDBUF_SIZE){
			sedpoint = 0;
		}else{
			sedpoint++;
		}
	//	sRtnPos[sRtnN] = sedpoint;
		sfrmm[sedpoint&0xfff] = 0;
	}else if(ch == 0x0d){
		sfrmm[sedpoint&0xfff] = 0;
		key.spchar++;
		if((sedpoint+1) == sstpoint){
			if((sstpoint+1) > SENDBUF_SIZE){
				sstpoint = 0;
			}else{
				sstpoint++;
			}
			while(sfrmm[sstpoint]!=0){
				sstpoint++;
			}
			while(sfrmm[sstpoint]==0){
				sstpoint++;
			}
			if(((sRtnN - sFstN)+1) > hs){
				send_scroll(1);
			//	sFstN = 0;
			}else{
				sFstN++;
			}
			sRtnPos[sFstN] = sstpoint;
		}
		if(((sRtnN - sFstN)+1)>hs){//0-hs(0-5)
			send_scroll(1);
		}
		sRtnN++;

	//	if((sedpoint+1) > SENDBUF_SIZE){
	//		sedpoint = 0;
	//	}else{
			sedpoint++;
	//	}
		sRtnPos[sRtnN] = sedpoint;//新行頭
	}
	key.spnflg = 1;
}
//------------------------------------------------------------------------
//	テレメトリ全体表示
//------------------------------------------------------------------------
void HexDataP(HWND hWnd,unsigned char * Bufin,DWORD len)
{
	HDC		   hdc;	//ハンドル
	HFONT hFont;
	size_t ret;

	unsigned int i,j,kai;
	unsigned int k,n;
	unsigned long wtpoint2;
	unsigned int ws = ((WSIZE_WIDTH-12)/8)-1;
	unsigned int vs = (((((WSIZE_WIDTH-12)/8)-2)/4)*3);//48
	unsigned int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
//	WCHAR * Disp = new WCHAR[((WSIZE_WIDTH-12)/8)];
	if(ws<4)ws=4;
	WCHAR * Disp = new WCHAR[ws+1];

//	unsigned char buf[4096] = {0};	//バイナリ>キャラクタ変換用バッファ
	unsigned char buf[COMMBUFSIZE * 3] = {0};	//バイナリ>キャラクタ変換用バッファ
	unsigned char line[COMMBUFSIZE * 3] = {0};	//バイナリ>キャラクタ変換用バッファ
    for(i=0;i<len;i++){			//バイナリ>キャラクタ変換
    	ltohex((unsigned char)(Bufin[i]),&buf[i*3],2);
        buf[(i*3)+2] = 0x20;		//2文字ごとに空白文字を入れて見やすく
    }
	wtpoint2 = (edpoint + (len*3));	//前回の文字終端+今回の追加分
	if(wtpoint2 > (((hs*4)+(3-wh))*ws)){
//	if(wtpoint2 > (((ws-2)/4)*72)){		//1画面に収まる分を超えると
		kai = int((wtpoint2 - (((hs*4)+(3-wh))*ws))/ws);
	//	kai = int((wtpoint2 - (((ws-2)/4)*72))/vs);
		scroll(kai + 1);		//収まるようにスクロール
	}
    i = 0;
    do {							//追加文字を表示バッファに加える
		j = (edpoint + i);			//左からの文字位置
   	    if(j > (LINGBUF_SIZE-1))j = (j-(LINGBUF_SIZE-1));
   	    frmm[j] = buf[i];
        i++;
    } while (i < (len * 3));
   	j++;
   	if(j > (LINGBUF_SIZE-1))j = (j-(LINGBUF_SIZE-1));
    frmm[j] = 0;
    j++;
   	if(j > (LINGBUF_SIZE-1)){
		j = (j-(LINGBUF_SIZE-1));
		j = (j/3)*3;
	}
    edpoint = j;//文字終端更新
	RtnN++;
	if(FstN == RtnN)FstN++;
	if(RtnN >= ((hs*4)+(3-wh)))RtnN = 0;
	if(FstN >= ((hs*4)+(3-wh)))FstN = 0;
	RtnPos[RtnN] = edpoint;
	
	hdc = GetDC(hWnd);
    hFont = (HFONT)SelectObject(hdc, hUFont);

    for(k=0;k<((hs*4)+(3-wh));k++){		//24行
		j = FstN + k;
		if(j>= ((hs*4)+(3-wh)))j-=((hs*4)+(3-wh));
		if(frm_U[j] == 0){//ASCII
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)&&(RtnPos[j] < LINGBUF_SIZE)){
				const char* source = (const char*)&frmm[RtnPos[j]];
			   	if (source != nullptr) {
        			mbstowcs_s(&ret,Disp,ws,source,ws-1);
    			}
				n = (int)(wcslen(Disp));
				for(i=n;i<ws;i++)Disp[i] = L' ';
			}else{
				for(i=0;i<ws;i++)Disp[i] = L' ';
			}
			Disp[ws] = 0;
			TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
		}else if(frm_U[j] == 1){//HEX
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)){
				for(n=0;n<vs;n++){
					if(frmm[RtnPos[j]+n] != 0){
						line[n] = frmm[RtnPos[j]+n];
					}else{
						line[n] = 0x20;
					}
	 				if(n%3 == 0){
					//	line[vs+(n/3)] = charset(frmm[RtnPos[j]+vs+(n/3)]);
					//	line[vs+(n/3)] = charset(Bufin[n/3]);
						line[vs+1+(n/3)] = Incharset((unsigned char *)(frmm+RtnPos[j]+n));
					} //アスキー表示も右端におまけする
				}
				for(i=n+1;i<vs+1;i++)line[i] = 0x20;
			//	line[n+1] = 0x20;
			//	line[n+1+(vs/3)] = 0;
				line[vs] = 0x20;
				line[vs+2+(n/3)] = 0;
				const char* source = (const char*)&line;
			   	if (source != nullptr) {
					mbstowcs_s(&ret,Disp,ws,source,ws-1);
				}
				n = (int)(wcslen(Disp));
				for(i=n;i<ws;i++)Disp[i] = L' ';
			}else{
				for(i=0;i<ws;i++)Disp[i] = L' ';
			}
			Disp[ws] = 0;
			TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
		}else{//UTF-8(Lua)
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)){
				i = 0;
				Disp[0] = 0;
				while((frmm[RtnPos[j]+i] != 0)&&(frmm[RtnPos[j]+i] != '\n')){
					if((frmm[RtnPos[j]+i]>0x1f)&&(frmm[RtnPos[j]+i]<0x7f)){
						/*もし1バイト文字なら*/
						n = (int)(wcslen(Disp));
						if(n<ws){
	        		    	Disp[n] = frmm[RtnPos[j]+i];
							Disp[n+1] = 0;
						}
						if(i<129)i++;
					}else{
						if(((frmm[RtnPos[j]+i]&0xff)>=0xC2)
						 &&((frmm[RtnPos[j]+i]&0xff)<=0xDF)){
							n = (int)wcslen(Disp);
							if(n<ws){
							    Disp[n] = ((frmm[RtnPos[j]+i] & 0x1f)<< 6)
							    		  |(frmm[RtnPos[j]+i+1] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<128)i+=2;
						}else if(((frmm[RtnPos[j]+i] & 0xff) >= 0xE0)
							   &&((frmm[RtnPos[j]+i] & 0xff) <= 0xEF)){
							n = (int)wcslen(Disp);
							if(n<ws){
       							Disp[n] = ((frmm[RtnPos[j]+i] & 0x0f) << 12)
  										 |((frmm[RtnPos[j]+i+1] & 0x3f) << 6)
  										 | (frmm[RtnPos[j]+i+2] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<127)i+=3;
						}else if(((frmm[RtnPos[j]+i] & 0xff) >= 0xF0)
							   &&((frmm[RtnPos[j]+i] & 0xff) <= 0xF4)){
							n = (int)wcslen(Disp);
							if(n<ws){
		    					Disp[n] = ((frmm[RtnPos[j]+i] & 0x07) << 18)
		    						 	 |((frmm[RtnPos[j]+i+1] & 0x3f) << 12)
									 	 |((frmm[RtnPos[j]+i+2] & 0x3f) << 6)
									 	 | (frmm[RtnPos[j]+i+3] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<126)i+=4;
						}else{
							n = (int)wcslen(Disp);
							if(n<(ws-1)){
								if(i<126){
									i++;
								}else{
									break;
								}
							}else{
								break;
							}
						}
					}
					if(j>(ws-1))break;
					if(i>126)break;
				}
				i = (int)(wcslen(Disp));
				if(i>(ws-1))i=(ws-1);
				for(j=i;j<(ws-1);j++)Disp[j] = L' ';
				Disp[ws] = 0;
				TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
			}
		}
   	}
// 	SelectObject(hdc,hNewFont);
   	SelectObject(hdc,hUFont);
	DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
	delete[] Disp;
	Disp = NULL;
}
//---テレメトリ全体再描画---------------------------------------------------
void HexDataREP(HWND hWnd)
{
	HDC		   hdc;	//ハンドル
	size_t ret;
	HFONT hFont;
	int ws = ((WSIZE_WIDTH-12)/8)-1;
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	int vs = (((((WSIZE_WIDTH-12)/8)-2)/4)*3);//48
	unsigned int hs = ((WSIZE_HEIGHT-80)/75);
	if(ws < 4)ws = 4;
//	WCHAR * Disp = new WCHAR[((WSIZE_WIDTH-12)/8)+1];
	WCHAR * Disp = new WCHAR[ws+1];
	unsigned char line[COMMBUFSIZE * 3] = {0};	//バイナリ>キャラクタ変換用バッファ

	int i,j,n,m,p;
//	unsigned int j;
	unsigned int k;
	unsigned long tp = stpoint;
	unsigned long vp = stpoint;

	hdc = GetDC(hWnd);
    hFont = (HFONT)SelectObject(hdc, hUFont);

    for(k=0;k<((hs*4)+(3-wh));k++){		//24行
		j = FstN + k;
		if(j>= (((int)hs*4)+(3-wh)))j-=((hs*4)+(3-wh));
		if(frm_U[j] == 0){//ASCII
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)&&(RtnPos[j] < LINGBUF_SIZE)){
				const char* source = (const char*)&frmm[RtnPos[j]];
			   	if (source != nullptr) {
        			mbstowcs_s(&ret,Disp,ws,source,ws-1);
    			}
				n = (int)(wcslen(Disp));
				for(i=n;i<ws;i++)Disp[i] = L' ';
			}else{
				for(i=0;i<ws;i++)Disp[i] = L' ';
			}
			Disp[ws] = 0;
			TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
		}else if(frm_U[j] == 1){//HEX
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)){
				for(n=0;n<vs;n++){
					if(frmm[RtnPos[j]+n] != 0){
						line[n] = frmm[RtnPos[j]+n];
					}else{
						line[n] = 0x20;
					}
	 				if(n%3 == 0){
				//	//	line[vs+1+(n/3)] = charset(frmm[RtnPos[j]+(n/3)]);
				//		line[vs+(n/3)] = charset(Bufin[RtnPos[j]+(n/3)]);
						line[vs+1+(n/3)] = Incharset((unsigned char *)(frmm+RtnPos[j]+n));
					} //アスキー表示も右端におまけする
				}
				line[n+1] = 0x20;
				line[vs] = 0x20;
				line[n+1+(vs/3)] = 0;
				const char* source = (const char*)&line;
			   	if (source != nullptr) {
					mbstowcs_s(&ret,Disp,ws,source,ws-1);
				}
				n = (int)(wcslen(Disp));
				for(i=n;i<ws;i++)Disp[i] = L' ';
			}else{
				for(i=0;i<ws;i++)Disp[i] = L' ';
			}
			Disp[ws] = 0;
			TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
		}else{//UTF-8
			if((RtnPos[j]!=-1)&&(frmm[RtnPos[j]] != 0)){
				i = 0;
				Disp[0] = 0;
				while((frmm[RtnPos[j]+i] != 0)&&(frmm[RtnPos[j]+i] != '\n')){
					if((frmm[RtnPos[j]+i]>0x1f)&&(frmm[RtnPos[j]+i]<0x7f)){
						/*もし1バイト文字なら*/
						n = (int)(wcslen(Disp));
						if(n<ws){
	        		    	Disp[n] = frmm[RtnPos[j]+i];
							Disp[n+1] = 0;
						}
						if(i<129)i++;
					}else{
						if(((frmm[RtnPos[j]+i]&0xff)>=0xC2)
						 &&((frmm[RtnPos[j]+i]&0xff)<=0xDF)){
							n = (int)wcslen(Disp);
							if(n<ws){
							    Disp[n] = ((frmm[RtnPos[j]+i] & 0x1f)<< 6)
							    		  |(frmm[RtnPos[j]+i+1] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<128)i+=2;
						}else if(((frmm[RtnPos[j]+i] & 0xff) >= 0xE0)
							   &&((frmm[RtnPos[j]+i] & 0xff) <= 0xEF)){
							n = (int)wcslen(Disp);
							if(n<ws){
       							Disp[n] = ((frmm[RtnPos[j]+i] & 0x0f) << 12)
  										 |((frmm[RtnPos[j]+i+1] & 0x3f) << 6)
  										 | (frmm[RtnPos[j]+i+2] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<127)i+=3;
						}else if(((frmm[RtnPos[j]+i] & 0xff) >= 0xF0)
							   &&((frmm[RtnPos[j]+i] & 0xff) <= 0xF4)){
							n = (int)wcslen(Disp);
							if(n<ws){
		    					Disp[n] = ((frmm[RtnPos[j]+i] & 0x07) << 18)
		    						 	 |((frmm[RtnPos[j]+i+1] & 0x3f) << 12)
									 	 |((frmm[RtnPos[j]+i+2] & 0x3f) << 6)
									 	 | (frmm[RtnPos[j]+i+3] & 0x3f);
								Disp[n+1] = 0;
							}
							if(i<126)i+=4;
						}else{
							n = (int)wcslen(Disp);
							if(n<(ws-1)){
								if(i<126){
									i++;
								}else{
									break;
								}
							}else{
								break;
							}
						}
					}
					if(j>(ws-1))break;
					if(i>126)break;
				}
				i = (int)(wcslen(Disp));
				if(i>(ws-1))i=(ws-1);
				for(j=i;j<(ws-1);j++)Disp[j] = L' ';
				Disp[ws] = 0;
				TextOutW(hdc,4,(k*15)+4+(((unsigned int)wh+1)*30),Disp,ws);
			}
		}
	}
//  for(k=0;k<=hs;k++){			//6行
    for(k=0;k<hs;k++){			//6行
		j = sFstN + k;
		if(j > RTN_SIZE)j -= RTN_SIZE;
	//	if((sRtnPos[j]!=-1)&&(sfrmm[sRtnPos[j]] != 0)){
		if(sRtnPos[j]!=-1){
			Disp[0] = '>';
			Disp[1] = 0;
			if(sfrm_U[j] == 1){//hex
				if((j+1)>RTN_SIZE){
					n = (j+1)-RTN_SIZE;
				}else{
					n = j+1;
				}
				p = (sRtnPos[n]-1) - sRtnPos[j];
				if(p > (ws/3))p = (ws/3);
				m = 0;
				line[0] = 0;
			//	for(i=sRtnPos[j];i<(sRtnPos[n]-1);i++){
				for(i=sRtnPos[j];i<sRtnPos[j]+p;i++){
    				ltohex((unsigned char)(sfrmm[i]),&line[(m*3)],2);
        			line[(m*3)+2] = 0x20;		//2文字ごとに空白文字を入れて見やすく
					line[(m*3)+3] = 0;
					m++;
				}
				const char* source = (const char*)line;
    			if (source != nullptr) {
        			mbstowcs_s(&ret,Disp+1,ws,source,ws-1);
    			}
			}else{
	    		size_t offset = sRtnPos[j];
    			const char* source = (const char*)&sfrmm[offset];
    			if (source != nullptr) {
        			mbstowcs_s(&ret,Disp+1,ws,source,ws-1);
    			}
    		}
			i = (int)(wcslen(Disp));
			for(n=i;n<(ws-1);n++)Disp[n] = L' ';
			Disp[ws] = 0;
		//	TextOutW(hdc,4,(hs*60)+4+(k*15),Disp,ws-1);
			TextOutW(hdc,4,((hs*4)*15)+((wh+1)*30)+4+(15*k),Disp,ws-1);
		}
    }
//	SelectObject(hdc,hNewFont);
   	SelectObject(hdc,hUFont);
    DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
	delete[] Disp;
	Disp = NULL;
}
//---固定パラメータ描画---------------------------------------------------
void DisplayParam(HWND hWnd)
{
	HDC		   hdc;	//ハンドル
	hdc = GetDC(hWnd);
//	WCHAR Disp[66];
	unsigned int ws;
	if(WSIZE_WIDTH<276){
		WSIZE_WIDTH = 276;
		ws = 32;
	}else{
		ws = ((WSIZE_WIDTH-12)/8)-1;
	}
	size_t DispSize = ws;
	WCHAR * Disp = new WCHAR[DispSize];
	HFONT hFont;
	size_t ret;
//	int ws = ((WSIZE_WIDTH-12)/8)-1;
	int wh = (WSIZE_WIDTH-2)/135;
	int ch;

//  hFont = (HFONT)SelectObject(hdc, hDispFont);
    hFont = (HFONT)SelectObject(hdc, hUFont);
	
	for(ch = 0;ch < FIXParam_Cnt;ch++){
	//	mbstowcs_s(&ret,Disp,ws/wh,CDSP.str_Name[ch],ws/wh);
		mbstowcs_s(&ret,Disp,32,CDSP.str_Name[ch],32);
		TextOutW(hdc,(int(ch%wh))*135+2,(int(ch/wh))*30   ,Disp,(int)wcslen(Disp));
	//	mbstowcs_s(&ret,Disp,ws/wh,CDSP.str_Val[ch],ws/wh);
		mbstowcs_s(&ret,Disp,32,CDSP.str_Val[ch],32);
		TextOutW(hdc,(int(ch%wh))*135+2,(int(ch/wh))*30+15,Disp,(int)wcslen(Disp));
	}
//  SelectObject(hdc,hNewFont);
    SelectObject(hdc,hUFont);
    DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
	delete[] Disp;
	Disp = NULL;
}
//------------------------------------------------------------------------
void daytime_write(HANDLE F)
{
    struct tm tm1;
	__time64_t t; 
	DWORD len_return;

    char date[60];
	_time64(&t);
	localtime_s(&tm1,&t);

	_itoa_s(tm1.tm_year+1900,date,30,10);
	if(tm1.tm_mon+1 > 9){
		_itoa_s(tm1.tm_mon+1,date+5,30,10);
	}else{
		date[5] = '0';
		_itoa_s(tm1.tm_mon+1,date+6,30,10);
	}
	if(tm1.tm_mday > 9){
		_itoa_s(tm1.tm_mday,date+8,30,10);
	}else{
		date[8] = '0';
		_itoa_s(tm1.tm_mday,date+9,30,10);
	}
	if(tm1.tm_hour > 9){
		_itoa_s(tm1.tm_hour,date+11,30,10);
	}else{
		date[11] = '0';
		_itoa_s(tm1.tm_hour,date+12,30,10);
	}
	if(tm1.tm_min > 9){
		_itoa_s(tm1.tm_min,date+14,30,10);
	}else{
		date[14] = '0';
		_itoa_s(tm1.tm_min,date+15,30,10);
	}
	if(tm1.tm_sec > 9){
		_itoa_s(tm1.tm_sec,date+17,30,10);
	}else{
		date[17] = '0';
		_itoa_s(tm1.tm_sec,date+18,30,10);
	}
    date[4] = '/';
    date[7] = '/';
    date[10] = ' ';
    date[13] = ':';
    date[16] = ':';
    date[19] = '\n';
    date[20] = 0;
	WriteFile(F,date,(DWORD)(strlen(date)),&len_return,NULL);
}
//------------------------------------------------------------------------
void daytime_display(void)
{
    struct tm tm1;
	__time64_t t; 
    char date[60];

	time(&t);
	_time64(&t);
	localtime_s(&tm1,&t);

	_itoa_s(tm1.tm_year+1900,date,30,10);
	if(tm1.tm_mon+1 > 9){
		_itoa_s(tm1.tm_mon+1,date+5,30,10);
	}else{
		date[5] = '0';
		_itoa_s(tm1.tm_mon+1,date+6,30,10);
	}
	if(tm1.tm_mday > 9){
		_itoa_s(tm1.tm_mday,date+8,30,10);
	}else{
		date[8] = '0';
		_itoa_s(tm1.tm_mday,date+9,30,10);
	}
	if(tm1.tm_hour > 9){
		_itoa_s(tm1.tm_hour,date+11,30,10);
	}else{
		date[11] = '0';
		_itoa_s(tm1.tm_hour,date+12,30,10);
	}
	if(tm1.tm_min > 9){
		_itoa_s(tm1.tm_min,date+14,30,10);
	}else{
		date[14] = '0';
		_itoa_s(tm1.tm_min,date+15,30,10);
	}
	if(tm1.tm_sec > 9){
		_itoa_s(tm1.tm_sec,date+17,30,10);
	}else{
		date[17] = '0';
		_itoa_s(tm1.tm_sec,date+18,30,10);
	}
    date[4] = '/';
    date[7] = '/';
    date[10] = ' ';
    date[13] = ':';
    date[16] = ':';
    date[19] = '\n';
    date[20] = 0;
	DisplayData(hogeWnd,(unsigned char *)date,(DWORD)strlen(date));
}
//------------------------------------------------------------------------
char * daytime_chardata(char * date)
{
    struct tm tm1;
	__time64_t t; 
	_time64(&t);
	localtime_s(&tm1,&t);

	_itoa_s(tm1.tm_year+1900,date,30,10);
	if(tm1.tm_mon+1 > 9){
		_itoa_s(tm1.tm_mon+1,date+4,30,10);
	}else{
		date[4] = '0';
		_itoa_s(tm1.tm_mon+1,date+5,30,10);
	}
	if(tm1.tm_mday > 9){
		_itoa_s(tm1.tm_mday,date+6,30,10);
	}else{
		date[6] = '0';
		_itoa_s(tm1.tm_mday,date+7,30,10);
	}
	if(tm1.tm_hour > 9){
		_itoa_s(tm1.tm_hour,date+8,30,10);
	}else{
		date[8] = '0';
		_itoa_s(tm1.tm_hour,date+9,30,10);
	}
	if(tm1.tm_min > 9){
		_itoa_s(tm1.tm_min,date+10,30,10);
	}else{
		date[10] = '0';
		_itoa_s(tm1.tm_min,date+11,30,10);
	}
	if(tm1.tm_sec > 9){
		_itoa_s(tm1.tm_sec,date+12,30,10);
	}else{
		date[12] = '0';
		_itoa_s(tm1.tm_sec,date+13,30,10);
	}
    date[14] = 0;
	return date;
}
/*-------------------------------------------*/
/* 設定ファイル読み込み　　　　　            */
/*-------------------------------------------*/
int mzParser(FILE * F)
{
	int i;
	size_t ret;
	char * end;
		
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;

	rchar(F);
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"DATAFOLDER"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while ((C_C.flg != 1) && (C_C.flg != 0x0a))cgetchar(F);
			if (C_C.flg == SC_TXT) {
				strcpy_s(DataFolder, sizeof(C_C.text), C_C.text);
			}
		}else if(!(strcmp(C_C.text,"LOGFOLDER"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while ((C_C.flg != 1) && (C_C.flg != 0x0a))cgetchar(F);
			if (C_C.flg == SC_TXT) {
				strcpy_s(LOGFolder, sizeof(C_C.text), C_C.text);
			}
		}else if(!(strcmp(C_C.text,"PROGRAM_TITLE"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 1){
				wcscpy_s(PROGRAM_TITLE,wcslen(C_C.Wtext)+1,C_C.Wtext);
			}else if(C_C.flg == 19){
				wcscpy_s(PROGRAM_TITLE,wcslen(C_C.Wtext)+1,C_C.Wtext);
			}
		}else if(!(strcmp(C_C.text,"Interval"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 0x0a))rchar(F);
			if(C_C.flg == 2){
				Interval = atol(C_C.text);
			}
		}else if(!(strcmp(C_C.text,"port"))){
		/*サーバアドレス読み出し*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while(((C_C.flg != 2)&&(C_C.flg != 1))&&(C_C.flg != 0x0a))rchar(F);
			if(C_C.flg == 2){
				portset_flg = 1;
				strcpy_s(comm_string,"\\\\.\\COM");
				strcat_s(comm_string,sizeof(comm_string),C_C.text);
				COMMPARAM[0].USB = 0;
			}else if(C_C.flg == 1){
				if(!(strcmp(C_C.text,"USB"))){/*なし*/
					COMMPARAM[0].USB = 1;
				}else if(!(strcmp(C_C.text,"usb"))){/*なし*/
					COMMPARAM[0].USB = 1;
				}else{
					strcpy_s(comm_string,"\\\\.\\COM1");
					COMMPARAM[0].USB = 0;
				}
			}else{
				if(!(strcmp(C_C.text,"none"))){
					portset_flg = 0;
				}else if(!(strcmp(C_C.text,"NONE"))){
					portset_flg = 0;
				}else if(!(strcmp(C_C.text,"None"))){
					portset_flg = 0;
				}else{
					portset_flg = 1;
					if((C_C.text[0] >= 'A')&&(C_C.text[0] <= 'J')){
						portset_flg = C_C.text[0];
					}
				}
			}
		}else if(!(strcmp(C_C.text,"USB_VID"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			cgetchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 2){
				COMMPARAM[0].VID = strtol(C_C.text,&end,16);
			}else if(C_C.flg == 19){
				COMMPARAM[0].VID = C_C.data;
			}else{
				COMMPARAM[0].VID = 0;
			}
		}else if(!(strcmp(C_C.text,"USB_PID"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			cgetchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 2){
				COMMPARAM[0].PID = strtol(C_C.text,&end,16);
			}else if(C_C.flg == 19){
				COMMPARAM[0].PID = C_C.data;
			}else{
				COMMPARAM[0].PID = 0;
			}
		}else if(!(strcmp(C_C.text,"USB_SERIAL"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
		//	rchar(F);				/*次の数列を読み出し*/
			cgetchar(F);
		//	while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))rchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
		//	strcpy_s(COMMPARAM.SERIAL,strlen(C_C.text)+1,C_C.text);
			COMMPARAM[0].SERIAL[0] = 0;
			mbstowcs_s(&ret,COMMPARAM[0].SERIAL,18,C_C.text,18);
		}else if(!(strcmp(C_C.text,"baud"))){
		/*サーバアドレス読み出し*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 0x0a))rchar(F);
			if(C_C.flg == 2){
				COMMPARAM[0].BaudRate = atol(C_C.text);
			}
		}else if(!(strcmp(C_C.text,"parity"))){
		/*パリティ設定*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			if(!(strcmp(C_C.text,"no"))){/*なし*/
				COMMPARAM[0].Parity = COM_P_NONE;
				COMMPARAM[0].fParity = 0;
			}else if(!(strcmp(C_C.text,"odd"))){/*奇数*/
				COMMPARAM[0].Parity = COM_P_ODD;
				COMMPARAM[0].fParity = 1;
			}else if(!(strcmp(C_C.text,"even"))){/*偶数*/
				COMMPARAM[0].Parity = COM_P_EVEN;
				COMMPARAM[0].fParity = 1;
			}else if(!(strcmp(C_C.text,"space"))){
				COMMPARAM[0].Parity = COM_P_SPACE;
				COMMPARAM[0].fParity = 0;
			}else if(!(strcmp(C_C.text,"mark"))){
				COMMPARAM[0].Parity = COM_P_MARK;
				COMMPARAM[0].fParity = 0;
			}
		}else if(!(strcmp(C_C.text,"stopbits"))){
		/*ストップビット設定*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			if(!(strcmp(C_C.text,"one"))){/*1*/
				COMMPARAM[0].StopBits = COM_S_ONE;
			}else if(!(strcmp(C_C.text,"onehalf"))){/*1.5*/
				COMMPARAM[0].StopBits = COM_S_ONEHALF;
			}else if(!(strcmp(C_C.text,"two"))){/*2*/
				COMMPARAM[0].StopBits = COM_S_TWO;
			}
		}else if(!(strcmp(C_C.text,"modem"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			if(!(strcmp(C_C.text,"on"))){
				COMMPARAM[0].modem_flg = 1;
			}else if(!(strcmp(C_C.text,"ON"))){
				COMMPARAM[0].modem_flg = 1;
			}else if(!(strcmp(C_C.text,"off"))){
				COMMPARAM[0].modem_flg = 0;
			}else if(!(strcmp(C_C.text,"OFF"))){
				COMMPARAM[0].modem_flg = 0;
			}
		}else if(!(strcmp(C_C.text,"newline"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			if(!(strcmp(C_C.text,"none"))){	/*改行を付けずに送信する*/
				COMMPARAM[0].newline = 0;
			}else if(!(strcmp(C_C.text,"CRLF"))){
				COMMPARAM[0].newline = 1;
			}else if(!(strcmp(C_C.text,"LF"))){
				COMMPARAM[0].newline = 2;
			}else if(!(strcmp(C_C.text,"CR"))){
				COMMPARAM[0].newline = 3;
			}else{
				COMMPARAM[0].newline = 0;
			}
		}else if(!(strcmp(C_C.text,"cmd_format"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			if(!(strcmp(C_C.text,"short"))){
				cmd_format_flg = 0; /*short*/
			}else if(!(strcmp(C_C.text,"Short"))){
				cmd_format_flg = 0; /*short*/
			}else if(!(strcmp(C_C.text,"long"))){
				cmd_format_flg = 1; /*long*/
			}else if(!(strcmp(C_C.text,"Long"))){
				cmd_format_flg = 1; /*long*/
			}
		}else if(!(strcmp(C_C.text,"Openport_mode"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);
			if(!(strcmp(C_C.text,"startonce"))){
				OpenPort_mode = OPENMODE_ONCE;
			}else if(!(strcmp(C_C.text,"check"))){
				OpenPort_mode = OPENMODE_CHECK;
			}else{
				OpenPort_mode = OPENMODE_ONCE;
			}
		}else if(!(strcmp(C_C.text,"Display"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);
			if(!(strcmp(C_C.text,"Text"))){
				Display_flg = 0;
			}else if(!(strcmp(C_C.text,"TEXT"))){
				Display_flg = 0;
			}else if(!(strcmp(C_C.text,"text"))){
				Display_flg = 0;
			}else if(!(strcmp(C_C.text,"hex"))){
				Display_flg = 1;
			}else if(!(strcmp(C_C.text,"Hex"))){
				Display_flg = 1;
			}else if(!(strcmp(C_C.text,"HEX"))){
				Display_flg = 1;
			}else if(!(strcmp(C_C.text,"LUA"))){
				Display_flg = 2;
			}else if(!(strcmp(C_C.text,"Lua"))){
				Display_flg = 2;
			}else if(!(strcmp(C_C.text,"lua"))){
				Display_flg = 2;
			}else{
				Display_flg = 0;
			}
		}else if(!(strcmp(C_C.text,"FIXParam_Display"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 0x0a))rchar(F);
			if(C_C.flg == 2){
				FIXParam_Cnt = atol(C_C.text);
			}
		//	if(FIXParam_Cnt > 3)FIXParam_Cnt = 0;
			if(FIXParam_Cnt > 36)FIXParam_Cnt = 36;
		}else if(!(strcmp(C_C.text,"MEASURE_DEVICE"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 1){
				strcpy_s(MEASURE_DEVICE_file,strlen(C_C.text)+1,C_C.text);
			}else if(C_C.flg == 19){
				strcpy_s(MEASURE_DEVICE_file,strlen(C_C.text)+1,C_C.text);
			}
		}else if(!(strcmp(C_C.text,"SCRIPT"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 1){
				strcpy_s(SCRIPT_file,strlen(C_C.text)+1,C_C.text);
			}else if(C_C.flg == 19){
				strcpy_s(SCRIPT_file,strlen(C_C.text)+1,C_C.text);
			}
		}else if(!(strcmp(C_C.text,"CMD"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 1){
				strcpy_s(CMD_file,strlen(C_C.text)+1,C_C.text);
			}else if(C_C.flg == 19){
				strcpy_s(CMD_file,strlen(C_C.text)+1,C_C.text);
			}
		}else if(!(strcmp(C_C.text,"TLM"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == 1){
				strcpy_s(TLM_file,strlen(C_C.text)+1,C_C.text);
			}else if(C_C.flg == 19){
				strcpy_s(TLM_file,strlen(C_C.text)+1,C_C.text);
			}
		}else if (!(strcmp(C_C.text, "REG"))) {
			cgetchar(F);/*次の文字列を読み出し*/
			while ((C_C.flg != 1) && (C_C.flg != 19) && (C_C.flg != 0x0a))cgetchar(F);
			if (C_C.flg == 1) {
				strcpy_s(REG_file, strlen(C_C.text) + 1, C_C.text);
			}
			else if (C_C.flg == 19) {
				strcpy_s(REG_file, strlen(C_C.text) + 1, C_C.text);
			}
		}
	}
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}

/*-------------------------------------------*/
/* コマンドファイル読み込み　　　            */
/*-------------------------------------------*/
int mxParser(FILE * F)
{
	int i,j;
	WCHAR WC[2] = {0};
	unsigned char c;
	unsigned char d[2] = {0};
	
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;

	cgetchar(F);
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"INTERVAL"))){
			interval_flg = 1;
		}else if(!(strcmp(C_C.text,"SEND"))){
			if(C_count){
				struct cmd_pak C;
				C.data = new char[((long long int)(C_count))+1];
				C.len = C_count;
				for(i=0;i<C_count;i++)C.data[i] = C_dummy[i];
				C.data[C_count] = 0;
				C.flg = 0;
				if(interval_flg){
					C.interval = interval_val;
				}else{
					C.interval = -1;
				}
				interval_flg = 0;
				if(label_count){
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =label_dummy[i];
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}else{
					for(i=0;i<C_count;i++){
						//とりあえず全部16進にして表示
						c = C_dummy[i];
						Wlabel_dummy[i*5 + 0] = '0';
						Wlabel_dummy[i*5 + 1] = 'x';
						d[0] = ((c >> 4) & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 2] = WC[0];
						d[0] = ( c       & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 3] = WC[0];
						Wlabel_dummy[i*5 + 4] = ' ';
						Wlabel_dummy[i*5 + 5] = 0;
					}
					label_count = C_count*5;
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =0x00;
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}
				CV.push_back(C);/*要素挿入*/
			}
			C_count = 0;
			label_count = 0;
		}else if(!(strcmp(C_C.text,"send"))){
			if(C_count){
				struct cmd_pak C;
				C.data = new char[((long long int)(C_count))+1];
				C.len = C_count;
				for(i=0;i<C_count;i++)C.data[i] = C_dummy[i];
				C.data[C_count] = 0;
				C.flg = 0;
				if(interval_flg){
					C.interval = interval_val;
				}else{
					C.interval = -1;
				}
				interval_flg = 0;
				if(label_count){
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =label_dummy[i];
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}else{
					for(i=0;i<C_count;i++){
						//とりあえず全部16進にして表示
						c = C_dummy[i];
						Wlabel_dummy[i*5 + 0] = '0';
						Wlabel_dummy[i*5 + 1] = 'x';
						d[0] = ((c >> 4) & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 2] = WC[0];
						d[0] = ( c       & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 3] = WC[0];
						Wlabel_dummy[i*5 + 4] = ' ';
						Wlabel_dummy[i*5 + 5] = 0;
					}
					label_count = C_count*5;
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =0x00;
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}
				CV.push_back(C);/*要素挿入*/
			}
			C_count = 0;
			label_count = 0;
		}else if(!(strcmp(C_C.text,"REG"))){
			if(C_count){
				struct cmd_pak C;
			//	C.data = new char[((long long int)(C_count))+1];
			//	C.len = C_count;
			//	for(i=0;i<C_count;i++)C.data[i] = C_dummy[i];
			//	C.data[C_count] = 0;
				C.flg = 1;

				if(label_count){
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =label_dummy[i];
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}else{
					for(i=0;i<C_count;i++){
						//とりあえず全部16進にして表示
						c = C_dummy[i];
						Wlabel_dummy[i*5 + 0] = '0';
						Wlabel_dummy[i*5 + 1] = 'x';
						d[0] = ((c >> 4) & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 2] = WC[0];
						d[0] = ( c       & 0x0f);
						if(d[0] < 10){
							WC[0] = ('0'+d[0])&0x00ff;
						}else{
							WC[0] = ('A'+(d[0]-10))&0x00ff;
						}
						Wlabel_dummy[i*5 + 3] = WC[0];
						Wlabel_dummy[i*5 + 4] = ' ';
						Wlabel_dummy[i*5 + 5] = 0;
					}
					label_count = C_count*5;
					C.label = new char[((long long int)(label_count))+1];
					C.label_len = label_count;
					for(i=0;i<label_count;i++)C.label[i] =0x00;
					C.label[label_count] = 0;
					C.name_len = (int)wcslen(Wlabel_dummy);
					C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
					wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
				}
				CV.push_back(C);/*要素挿入*/
			}
		}else if(!(strcmp(C_C.text,"reg"))){
			
		}else if(!(strcmp(C_C.text,"LABEL"))){
			label_flg = 1;
		}else if(!(strcmp(C_C.text,"PORT"))){
			port_flg = 1;
		}else if(!(strcmp(C_C.text,"COM"))){
			if(port_flg == 1){
				port_flg = 2;
			}
		}else if(!(strcmp(C_C.text,"IP"))){
			if(port_flg == 1){
				port_flg = 3;
			}
		}else if(!(strcmp(C_C.text,"CRC"))){
			/*2バイトのCRCを付ける*/
			unsigned short CRC16;
			C_dummy[C_count]   = 0x00;
			C_dummy[C_count+1] = 0x00;
		//	CRC16 = CRC_CCITT_LSBfirst_ALL1(C_count, C_dummy);
			CRC16 = CRC_CCITT_LSBfirst_ALL1(C_count+2, C_dummy);
			C_dummy[C_count]   = (char)( CRC16      & 0xff);
			C_dummy[C_count+1] = (char)((CRC16 >> 8)& 0xff);
			C_count += 2;
		}else{
			if(axtol(C_C.text,C_dummy+C_count+3,1)){
//				char buf[10];
//				ltohex((unsigned long)C_dummy[C_count],buf,2);
//				buf[2] = 0;
//			   	MessageBoxW(NULL,buf,L"Error",MB_ICONHAND);
				C_count++;
			}else{
				/*コマンド終了*/
				if(C_count){
					struct cmd_pak C;
					C.data = new char[((long long int)(C_count))+1];
					C.len = C_count;
					for(i=0;i<C_count;i++)C.data[i] = C_dummy[i];
					C.data[C_count] = 0;
					C.flg = 0;
					if(label_count){
						C.label = new char[((long long int)(label_count))+1];
						C.label_len = label_count;
						for(i=0;i<label_count;i++)C.label[i] =label_dummy[i];
						C.label[label_count] = 0;
						C.name_len = (int)wcslen(Wlabel_dummy);
						C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
						wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
					}else{
						for(i=0;i<C_count;i++){
							//とりあえず全部16進にして表示
							c = C_dummy[i];
							Wlabel_dummy[i*5 + 0] = '0';
							Wlabel_dummy[i*5 + 1] = 'x';
							d[0] = ((c >> 4) & 0x0f);
							if(d[0] < 10){
								WC[0] = ('0'+d[0])&0x00ff;
							}else{
								WC[0] = ('A'+(d[0]-10))&0x00ff;
							}
							Wlabel_dummy[i*5 + 2] = WC[0];
							d[0] = ( c       & 0x0f);
							if(d[0] < 10){
								WC[0] = ('0'+d[0])&0x00ff;
							}else{
								WC[0] = ('A'+(d[0]-10))&0x00ff;
							}
							Wlabel_dummy[i*5 + 3] = WC[0];
							Wlabel_dummy[i*5 + 4] = ' ';
							Wlabel_dummy[i*5 + 5] = 0;
						}
						label_count = C_count*5;
						C.label = new char[((long long int)(label_count))+1];
						C.label_len = label_count;
						for(i=0;i<label_count;i++)C.label[i] =0x00;
						C.label[label_count] = 0;
						C.name_len = (int)wcslen(Wlabel_dummy);
						C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
						wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
					}

					CV.push_back(C);/*要素挿入*/
				}
				C_count = 0;
				label_count = 0;
			}
		}
	}else if(C_C.flg == 19){
		/*文字列*/
		if(label_flg){
			label_count = (int)(strlen(C_C.text));
			for(j=0;j<label_count;j++)label_dummy[j] = C_C.text[j];
			label_count = (int)(wcslen(C_C.Wtext));
			for(j=0;j<label_count;j++)Wlabel_dummy[j] = C_C.Wtext[j];
			Wlabel_dummy[label_count] = 0;
			label_flg = 0;
		}else{
			C_count = (int)(strlen(C_C.text));
			for(j=0;j<C_count;j++)C_dummy[j] = C_C.text[j];
		}
	}else if(C_C.flg == SC_NUM){/*数字だったら*/
		int i;
		if(interval_flg){
			interval_val = C_C.data;
		}else{
			if(C_C.data & 0xff000000){
				for(i=3;i>=0;i--){
					C_dummy[C_count+i] = (char)((C_C.data >> 8*(3-i))&0xff);
				}
				C_count += 4;
			}else if(C_C.data & 0x00ff0000){
				for(i=2;i>=0;i--){
					C_dummy[C_count+i] = (char)((C_C.data >> 8*(2-i))&0xff);
				}
				C_count += 3;
			}else if(C_C.data & 0x0000ff00){
				for(i=1;i>=0;i--){
					C_dummy[C_count+i] = (char)((C_C.data >> 8*(1-i))&0xff);
				}
				C_count += 2;
			}else{
				C_dummy[C_count] = (char)(C_C.data & 0xff);
				C_count += 1;
			}
		}
	}else if(C_C.flg == 0x0a){/*改行*/
		if(C_count){
			struct cmd_pak C;
			C.data = new char[((long long int)(C_count))+1];
			C.len = C_count;
			for(i=0;i<C_count;i++)C.data[i] = C_dummy[i];
			C.data[C_count] = 0;
			C.flg = 0;
			if(interval_flg){
				C.interval = interval_val;
			}else{
				C.interval = -1;
			}
			if(label_count){
				C.label = new char[((long long int)(label_count))+1];
				C.label_len = label_count;
				for(i=0;i<label_count;i++)C.label[i] =label_dummy[i];
				C.label[label_count] = 0;
				C.name_len = (int)wcslen(Wlabel_dummy);
				C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
				wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
			}else{
				for(i=0;i<C_count;i++){
					//とりあえず全部16進にして表示
					c = C_dummy[i];
					Wlabel_dummy[i*5 + 0] = '0';
					Wlabel_dummy[i*5 + 1] = 'x';
					d[0] = ((c >> 4) & 0x0f);
					if(d[0] < 10){
						WC[0] = ('0'+d[0])&0x00ff;
					}else{
						WC[0] = ('A'+(d[0]-10))&0x00ff;
					}
					Wlabel_dummy[i*5 + 2] = WC[0];
					d[0] = ( c       & 0x0f);
					if(d[0] < 10){
						WC[0] = ('0'+d[0])&0x00ff;
					}else{
						WC[0] = ('A'+(d[0]-10))&0x00ff;
					}
					Wlabel_dummy[i*5 + 3] = WC[0];
					Wlabel_dummy[i*5 + 4] = ' ';
					Wlabel_dummy[i*5 + 5] = 0;
				}
				label_count = C_count*5;
				C.label = new char[((long long int)(label_count))+1];
				C.label_len = label_count;
				for(i=0;i<label_count;i++)C.label[i] = 0x00;
				C.label[label_count] = 0;
				C.name_len = (int)wcslen(Wlabel_dummy);
				C.name = new WCHAR[static_cast<rsize_t>(C.name_len)+1];
				wcscpy_s(C.name,static_cast<rsize_t>(C.name_len)+1,Wlabel_dummy);
			}
			CV.push_back(C);/*要素挿入*/
		}
		C_count = 0;
		label_count = 0;
		label_flg = 0;
		interval_flg = 0;
	}
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}

/*-------------------------------------------*/
/* テレメトリ設定ファイル読み込み　	         */
/*-------------------------------------------*/
int TlmParser(FILE * F)
{
	int i,j;
	char dummy[256];
	
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;

	cgetchar(F);
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"TLM"))){
			/*コマンド追加*/
			part_counter = 0;
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=19)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == 1){
				struct tlm_struct * T1 = new struct tlm_struct;
				T1->name = new char[strlen(C_C.text)+1];
				strcpy_s(T1->name,strlen(C_C.text)+1,C_C.text);
				TLM.push_back(*T1);
			//	delete T1;
			}else if(C_C.flg == 19){
				struct tlm_struct * T1 = new struct tlm_struct;
				T1->name = new char[strlen(C_C.text)+1];
				strcpy_s(T1->name,strlen(C_C.text)+1,C_C.text);
				TLM.push_back(*T1);
			//	delete T1;
			}
		}
		if(!(strcmp(C_C.text,"TYPE"))){
			cgetchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == SC_TXT){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					TLM[TLM.size()-1].type = LL.LV[index].id;
				}else{
					/*エラー*/
					TLM[TLM.size()-1].type = 0;
				}
			}else{
				/*エラー*/
				TLM[TLM.size()-1].type = 0;
			}
		}
		if(!(strcmp(C_C.text,"PART"))){
			part_counter++;
			TLM[TLM.size()-1].size = part_counter;
		}
		if(!(strcmp(C_C.text,"MAJOR"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				TLM[TLM.size()-1].PART[part_counter-1].major = C_C.data;
			}else if(C_C.flg == SC_TXT){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					TLM[TLM.size()-1].PART[part_counter-1].major
					 = LL.LV[index].id;
				}else{
					/*エラー*/
					TLM[TLM.size()-1].PART[part_counter-1].major = 0;
				}
			}else{
				/*エラー*/
				TLM[TLM.size()-1].PART[part_counter-1].major = 0;
			}
		}
		if(!(strcmp(C_C.text,"MINOR"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				TLM[TLM.size()-1].PART[part_counter-1].minor = C_C.data;
			}else if(C_C.flg == 1){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					TLM[TLM.size()-1].PART[part_counter-1].minor
					 = LL.LV[index].id;
				}else{
					/*エラー*/
					TLM[TLM.size()-1].PART[part_counter-1].minor = 0;
				}
			}else{
				/*エラー*/
				TLM[TLM.size()-1].PART[part_counter-1].minor = 0;
			}
		}
		if(!(strcmp(C_C.text,"MASK"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				TLM[TLM.size()-1].PART[part_counter-1].mask = C_C.data;
			}else if(C_C.flg == SC_TXT){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					TLM[TLM.size()-1].PART[part_counter-1].mask
					 = LL.LV[index].id;
				}else{
					/*エラー*/
					TLM[TLM.size()-1].PART[part_counter-1].mask = 0;
				}
			}else{
				/*エラー*/
				TLM[TLM.size()-1].PART[part_counter-1].mask = 0;
			}
		}
		if(!(strcmp(C_C.text,"SHIFT"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				TLM[TLM.size()-1].PART[part_counter-1].shift = C_C.data;
			}else if(C_C.flg == 1){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					TLM[TLM.size()-1].PART[part_counter-1].shift
					 = LL.LV[index].id;
				}else{
					/*エラー*/
					TLM[TLM.size()-1].PART[part_counter-1].shift = 0;
				}
			}else{
				/*エラー*/
				TLM[TLM.size()-1].PART[part_counter-1].shift = 0;
			}
		}
		if(!(strcmp(C_C.text,"DEF"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == 1){
				strcpy_s(dummy,256,C_C.text);
				cgetchar(F);/*次の文字列を読み出し*/
				while((C_C.flg != 2)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == SC_NUM){
					j = C_C.data;
					/*変数ラベル管理配列に入れる*/
					LL.insert(dummy,j,0);
				}
			}
		}
	}
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}

/*-------------------------------------------*/
/* レジスタ設定ファイル読み込み　	         */
/*-------------------------------------------*/
int RegParser(FILE * F)
{
	int i;
	
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;

	cgetchar(F);
#if 0
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"REGION_MAIN"))){
			/*コマンド追加*/
			part_counter = 0;
			sub_counter = 0;
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=19)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == 1){
				struct reg_struct * R1 = new struct reg_struct;
				R1->name = new char[strlen(C_C.text)+1];
				strcpy_s(R1->name,strlen(C_C.text)+1,C_C.text);
				REG.push_back(*R1);
			//	delete R1;
			}else if(C_C.flg == 19){
				struct reg_struct * R1 = new struct reg_struct;
				R1->name = new char[strlen(C_C.text)+1];
				strcpy_s(R1->name,strlen(C_C.text)+1,C_C.text);
				REG.push_back(*R1);
			//	delete R1;
			}
		}
		if(!(strcmp(C_C.text,"PART"))){
			sub_counter = 0;
			part_counter++;
			REG[REG.size()-1].isize = part_counter;
		}
		if(!(strcmp(C_C.text,"SUB"))){
			sub_counter++;
			REG[REG.size()-1].PART[part_counter-1].isize = sub_counter;
		}
		if(!(strcmp(C_C.text,"START"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].start = C_C.data;
			}else if(C_C.flg == SC_TXT){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].start
					 = LL.LV[index].id;
				}else{
					/*エラー*/
					REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter].start = 0;
				}
			}else{
				/*エラー*/
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter].start = 0;
			}
		}
		if(!(strcmp(C_C.text,"END"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize 
					= C_C.data - REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].start +1;
				if(REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize < 1){
					/*エラー*/
					REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize = 0;
				}
			}else if(C_C.flg == 1){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize
					 = LL.LV[index].id - REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].start + 1;
					if(REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize < 1){
						/*エラー*/
						REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize = 0;
					}
				}else{
					/*エラー*/
					REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize = 0;
				}
			}else{
				/*エラー*/
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize = 0;
			}
			if(REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize > 0){
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].DATA 
					= new unsigned int[REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize];
			}
			for(i=0;i<REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].isize;i++){
				REG[REG.size()-1].PART[part_counter-1].SUB[sub_counter-1].DATA[i] = 0;
			}
		}
	}
#else
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"REGION"))){
			/*領域追加*/
			part_counter = 0;
			sub_counter = 0;
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=19)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == 1){
				struct reg_main * R1 = new struct reg_main;
				R1->name = new char[strlen(C_C.text)+1];
				strcpy_s(R1->name,strlen(C_C.text)+1,C_C.text);
				REG.push_back(*R1);
			//	delete R1;
			}else if(C_C.flg == 19){
				struct reg_main * R1 = new struct reg_main;
				R1->name = new char[strlen(C_C.text)+1];
				strcpy_s(R1->name,strlen(C_C.text)+1,C_C.text);
				REG.push_back(*R1);
			//	delete R1;
			}
		}
		if(!(strcmp(C_C.text,"START"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				REG[REG.size()-1].start = C_C.data;
			}else if(C_C.flg == SC_TXT){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					REG[REG.size()-1].start = LL.LV[index].id;
				}else{
					/*エラー*/
					REG[REG.size()-1].start = 0;
				}
			}else{
				/*エラー*/
				REG[REG.size()-1].start = 0;
			}
		}
		if(!(strcmp(C_C.text,"END"))){
			cgetchar(F);
			while((C_C.flg !=1)&&(C_C.flg !=2)&&(C_C.flg !=0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				REG[REG.size()-1].isize = C_C.data - REG[REG.size()-1].start +1;
				if(REG[REG.size()-1].isize < 1){
					/*エラー*/
					REG[REG.size()-1].isize = 0;
				}
			}else if(C_C.flg == 1){
				int index = LL.serch(C_C.text);
				if(index >= 0){
					REG[REG.size()-1].isize 
					  = LL.LV[index].id - REG[REG.size()-1].start + 1;
					if(REG[REG.size()-1].isize < 1){
						/*エラー*/
						REG[REG.size()-1].isize = 0;
					}
				}else{
					/*エラー*/
					REG[REG.size()-1].isize = 0;
				}
			}else{
				/*エラー*/
				REG[REG.size()-1].isize = 0;
			}
			if(REG[REG.size()-1].isize > 0){
				REG[REG.size()-1].DATA = new long[REG[REG.size()-1].isize];
			}
			for(i=0;i<REG[REG.size()-1].isize;i++){
				REG[REG.size()-1].DATA[i] = 0;
			}
		}
		if(!(strcmp(C_C.text,"DEF"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			cgetchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 0x0a))cgetchar(F);
			char * Buf = new char [strlen(C_C.text)+1];
			strcpy_s(Buf,strlen(C_C.text)+1,C_C.text);
		//	cgetchar(F);/*次の数字を読み出す*/
			while((C_C.flg != 2)&&(C_C.flg != 0x0a))cgetchar(F);
		//	while((C_C.flg != 1)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg == SC_NUM){
				LM.insert(Buf,C_C.data,0);
			}else{
				LM.insert(Buf,0,0);
			}
		//	delete[] Buf;
		}
	}
#endif
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}

/**********************************************************************************/
/*-------------------------------------------*/
/* 計測器設定ファイル読み込み                */
/*-------------------------------------------*/
int measureParser(FILE * F)
{
	int i,j,k,n;
	char tmp[10];
	char * end;
	size_t ret;
	char comm_string3[32];
	
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;

	rchar(F);
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"DEVICE"))){/*計測器デバイス名 ASCII文字*/
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			struct measurement_device * M1 = new struct measurement_device;
			measure_onflg = 1;
			M1->name = new char[strlen(C_C.text)+1];
			if(C_C.flg == 1){
				strcpy_s(M1->name,strlen(C_C.text)+1,C_C.text);
			}else if(C_C.flg == 19){
				strcpy_s(M1->name,strlen(C_C.text)+1,C_C.text);
			}
			if(C_C.flg != 0x0a){
				cgetchar(F);/*次の文字列を読み出し*/
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg != 0x0a){
					if(!(strcmp(C_C.text,"On"))){
						MDEV.push_back(*M1);
						measure_onflg = 1;
					}else if(!(strcmp(C_C.text,"on"))){
						MDEV.push_back(*M1);
						measure_onflg = 1;
					}else if(!(strcmp(C_C.text,"ON"))){
						MDEV.push_back(*M1);
						measure_onflg = 1;
					}else if(!(strcmp(C_C.text,"Off"))){
						delete M1->name;
						M1->name = NULL;
						measure_onflg = 0;
					}else if(!(strcmp(C_C.text,"OFF"))){
						delete M1->name;
						M1->name = NULL;
						measure_onflg = 0;
					}else if(!(strcmp(C_C.text,"off"))){
						delete M1->name;
						M1->name = NULL;
						measure_onflg = 0;
					}else{
						delete M1->name;
						M1->name = NULL;
						measure_onflg = 0;
					}
				}else{
					delete M1->name;
					M1->name = NULL;
					measure_onflg = 0;
				}
			}else{
				MDEV.push_back(*M1);
				measure_onflg = 1;
			}
		//	delete M1;
		}else if(!(strcmp(C_C.text,"DEVICE_SETTING"))){/*別ファイルにて設定。ファイル名*/
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			i = int(MDEV.size())-1;
			if(measure_onflg){
				MDEV[i].setting_file_name = new char[strlen(C_C.text)+1];
				if(C_C.flg == 1){
					strcpy_s(MDEV[i].setting_file_name,strlen(C_C.text)+1,C_C.text);
				}else if(C_C.flg == 19){
					strcpy_s(MDEV[i].setting_file_name,strlen(C_C.text)+1,C_C.text);
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_MAKER"))){/*計測器メーカー*/
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(!(strcmp(C_C.text,"YOKOKAWA"))){
					MDEV[i].maker = MAKER_YOKOKAWA;			//1
				}else if(!(strcmp(C_C.text,"KIKUSUI"))){
					MDEV[i].maker = MAKER_KIKUSUI;			//2
				}else if(!(strcmp(C_C.text,"HP"))){
					MDEV[i].maker = MAKER_HP;				//3
				}else if(!(strcmp(C_C.text,"KEYSIGHT"))){
					MDEV[i].maker = MAKER_KEYSIGHT;			//4
				}else if(!(strcmp(C_C.text,"KEISOKU"))){
					MDEV[i].maker = MAKER_KEISOKU;			//5
				}else if(!(strcmp(C_C.text,"FTDI"))){
					MDEV[i].maker = MAKER_FTDI;				//6
				}else if(!(strcmp(C_C.text,"OWON"))){
					MDEV[i].maker = MAKER_OWON;				//7
				}else if(!(strcmp(C_C.text,"MATSUSADA"))){
					MDEV[i].maker = MAKER_MATSUSADA;		//8
				}else if(!(strcmp(C_C.text,"TAKASAGO"))){
					MDEV[i].maker = MAKER_TAKASAGO;			//9
				}else if(!(strcmp(C_C.text,"ALIENTEK"))){
					MDEV[i].maker = MAKER_ALIENTEK;			//10
				}else if(!(strcmp(C_C.text,"DUMMY"))){
					MDEV[i].maker = MAKER_DUMMY;			//0
				}else{
					MDEV[i].maker = MAKER_DUMMY;			//0
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_TYPE"))){//1:serial2:IVI3:USB HID4:LAN5:GPIB
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 2)&&(C_C.flg != 0x0a))rchar(F);
			if(C_C.flg == 2){
				if(measure_onflg){
					i = int(MDEV.size())-1;
					MDEV[i].interface_type = atoi(C_C.text);
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_DELIMITER"))){/*デリミタ*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(C_C.flg != 0x0a){
				if(measure_onflg){
					i = int(MDEV.size())-1;
					if(!(strcmp(C_C.text,"None"))){
						MDEV[i].delimiter = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].delimiter = 0;
					}else if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].delimiter = 0;
					}else if(!(strcmp(C_C.text,"CR"))){
						MDEV[i].delimiter = 1;
					}else if(!(strcmp(C_C.text,"cr"))){
						MDEV[i].delimiter = 1;
					}else if(!(strcmp(C_C.text,"LF"))){
						MDEV[i].delimiter = 2;
					}else if(!(strcmp(C_C.text,"lf"))){
						MDEV[i].delimiter = 2;
					}else if(!(strcmp(C_C.text,"CRLF"))){
						MDEV[i].delimiter = 3;
					}else if(!(strcmp(C_C.text,"crlf"))){
						MDEV[i].delimiter = 3;
					}else{
						MDEV[i].delimiter = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_COMPORT"))){/*シリアルポート*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))rchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){/*番号指定*/
					MDEV[i].serial_port_num = atoi(C_C.text);
					strcpy_s(comm_string3,"\\\\.\\COM");
					strcat_s(comm_string3,sizeof(comm_string3),C_C.text);
					MDEV[i].serial_port = new char[strlen(comm_string3)+1];
					strcpy_s(MDEV[i].serial_port,strlen(comm_string3)+1,comm_string3);
				}else if(C_C.flg == 1){/*AからJまでのアルファベット大文字指定*/
					/*PID,VID,シリアルと一致するシリアルポートを見つけて使用する*/
					/*ここで指定したアルファベットでLuaスクリプトから指定操作できる*/
					if((C_C.text[0] >= 'A')&&(C_C.text[0] <= 'J')){
						MDEV[i].serial_port_num = 100 + int(C_C.text[0]);
					//	MDEV[i].serial_port = new char[2];
					//	MDEV[i].serial_port[0] = C_C.text[0];
					//	MDEV[i].serial_port[1] = 0;
					}else if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].serial_port_num = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].serial_port_num = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].serial_port_num = 0;
					}else{
						MDEV[i].serial_port_num = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_COMBAUD"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))rchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){
					MDEV[i].serial_baud = atol(C_C.text);
				}else if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].serial_baud = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].serial_baud = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].serial_baud = 0;
					}else{
						MDEV[i].serial_baud = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_VISA_ADDR"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				MDEV[i].VISA_Addr = new char[strlen(C_C.text)+1];
				if(C_C.flg == 1){
					strcpy_s(MDEV[i].VISA_Addr,strlen(C_C.text)+1,C_C.text);
				}else if(C_C.flg == 19){
					strcpy_s(MDEV[i].VISA_Addr,strlen(C_C.text)+1,C_C.text);
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_LANADDR"))){/*IPV4*/
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				k = 0;
				n = 0;
				for(j=0;j<strlen(C_C.text)+1;j++){
					if((C_C.text[j] == '.')||(C_C.text[j] == 0)){
						tmp[n] = 0;
						if(k<4){
							MDEV[i].lan_addr[k] = atoi(tmp);
							k++;
							n = 0;
						}
					}else{
						tmp[n] = C_C.text[j];
						if(n<9)n++;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_LANPORT"))){/*ポート番号は10進*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))rchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){
					MDEV[i].lan_port = atol(C_C.text);
				}else if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].lan_port = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].lan_port = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].lan_port = 0;
					}else{
						MDEV[i].lan_port = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_GPIBPORT"))){
			C_C.flg = 0;
			C_C.text[0] = 0;
			rchar(F);				/*次の数列を読み出し*/
			while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))rchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){
					MDEV[i].gpib_port = atol(C_C.text);
				}else if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].gpib_port = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].gpib_port = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].gpib_port = 0;
					}else{
						MDEV[i].gpib_port = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_VID"))){/*0xが無くても16進*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			cgetchar(F);				/*次の数列を読み出し*/
		//	while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))cgetchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 2)
				 &&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){
				//	MDEV[i].usb_vid = atol(C_C.text);
					MDEV[i].usb_vid = strtol(C_C.text,&end,16);
					MDEV[i].usb_vid = C_C.data;
				}else if(C_C.flg == 19){
					MDEV[i].usb_vid = C_C.data;
				}else if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].usb_vid = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_vid = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_vid = 0;
					}else{
						MDEV[i].usb_vid = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_PID"))){/*0xが無くても16進*/
			C_C.flg = 0;
			C_C.text[0] = 0;
			cgetchar(F);		/*次の数列を読み出し*/
		//	while(((C_C.flg != 1)&&(C_C.flg != 2))&&(C_C.flg != 0x0a))rchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 2)
				 &&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 2){
				//	MDEV[i].usb_pid = atol(C_C.text);
				//	MDEV[i].usb_pid = strtol(C_C.text,&end,16);
					MDEV[i].usb_pid = C_C.data;
				}else if(C_C.flg == 19){
					MDEV[i].usb_pid = C_C.data;
				}else if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].usb_pid = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_pid = 0;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_pid = 0;
					}else{
						MDEV[i].usb_pid = 0;
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_SERNUM"))){
			cgetchar(F);
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(measure_onflg){
				i = int(MDEV.size())-1;
				if(C_C.flg == 1){
					if(!(strcmp(C_C.text,"NONE"))){
						MDEV[i].usb_ser = NULL;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_ser = NULL;
					}else if(!(strcmp(C_C.text,"none"))){
						MDEV[i].usb_ser = NULL;
					}else{
						j = (int)strlen(C_C.text) + 1;
						if(j > 18)j = 17;
						MDEV[i].usb_ser = new TCHAR[j];
						mbstowcs_s(&ret,MDEV[i].usb_ser,j,C_C.text,16);
					//	mbstowcs_s(&ret,MDEV[i].usb_ser,j-1,C_C.text,16);
					//	strcpy_s(MDEV[i].usb_ser,strlen(C_C.text)+1,C_C.text);
					}
				}else if(C_C.flg == 19){
					j = (int)strlen(C_C.text) + 1;
					if(j > 9){
					/*たいてい先頭が6&みたいな長い文字列。2文字目に&があるのが特徴*/
					/*恐らくASCIIで領域に格納されているが、互換のためにWide char 16bitで*/
					/*データとして格納。文字列長と2文字目で見分けていく*/
						if(j > 18)j = 17;
						MDEV[i].usb_ser = new TCHAR[j];
						mbstowcs_s(&ret,MDEV[i].usb_ser,j,C_C.text,18);
					}else{
					/*FTDIは規格を守っているようで、シリアルはWide Char 16bitで8文字*/
					/*先頭2文字はメーカ、6文字は0-9にAからZまで使って表現する36進数*/
					/*これで実質製造秒が記録されている。これを固有番号として利用*/
					/*したがってWide Char上位8ビットは0である。下位だけ取り出せば*/
					/*char互換になる*/
						MDEV[i].usb_ser = new TCHAR[10];
					//	strcpy_s(MDEV[i].usb_ser,strlen(C_C.text)+1,C_C.text);
						mbstowcs_s(&ret,MDEV[i].usb_ser,10,C_C.text,10);
					}
				}
			}
		}else if(!(strcmp(C_C.text,"DEVICE_END"))){
			measure_onflg = 0;
		}
	}
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}
/**********************************************************************************/
/*-------------------------------------------*/
/* 計測器コマンドファイル読み込み            */
/*-------------------------------------------*/
int measureCMDParser(FILE * F, int n)
{
	int i,ch;
	long last;
	
	for(i=0;i<256;i++){
		C_C.text[i] = 0;
	}
	C_C.flg = 0;
	C_C.level = 0;
	ch = n;
	last = 0;

	rchar(F);
	if(C_C.flg == 1){/*文字列だったら*/
		if(!(strcmp(C_C.text,"NAME"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			for(i=0;i<MDEV.size();i++){
				if(!(strcmp(MDEV[i].name,C_C.text))){
					ch = i;
				}
			}
		}else if(!(strcmp(C_C.text,"INIT"))){
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(!(strcmp(C_C.text,"START"))){
				struct measurement_cmd * MC = new struct measurement_cmd;
			//	struct measurement_cmd MC;
				MC->name = new char[5];
				strcpy_s(MC->name,5,"INIT");
				MDEV[ch].MCMD.push_back(*MC);
			//	last = MDEV[ch].MCMD.size();
			}else if(!(strcmp(C_C.text,"END"))){
				//
			}else if(C_C.flg == 19){
				struct mcmd * MX = new struct mcmd;
			//	struct mcmd MX;
				MX->cmd = new char[strlen(C_C.text)+1];
				strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
				MX->type = 0;
				MX->param = 0;
				last = (long)MDEV[ch].MCMD.size();
				if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
			}else if(C_C.flg == 1){
				struct mcmd * MX = new struct mcmd;
			//	struct mcmd MX;
				if(!(strcmp(C_C.text,"SLEEP"))){
					MX->type = 1;
					cgetchar(F);/*次の文字列を読み出し*/
					while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
					if(C_C.flg == 2){
						MX->param = atoi(C_C.text);
					}else{
						MX->param = 0;
					}
				}else{
					MX->type = 1;
					MX->param = 0;
				}
				last = (long)MDEV[ch].MCMD.size();
				if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
			}
		}else if(!(strcmp(C_C.text,"CMD"))){/*別ファイルにて設定。ファイル名*/
			cgetchar(F);/*次の文字列を読み出し*/
			while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
			if(!(strcmp(C_C.text,"NAME"))){
				cgetchar(F);/*次の文字列を読み出し*/
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 19){
					struct measurement_cmd * MC = new struct measurement_cmd;
				//	struct measurement_cmd MC;
					MC->name = new char[strlen(C_C.text)+1];
					strcpy_s(MC->name,strlen(C_C.text)+1,C_C.text);
					MDEV[ch].MCMD.push_back(*MC);
				}
			}else if(!(strcmp(C_C.text,"SLEEP"))){
				while((C_C.flg != 1)&&(C_C.flg != 2)
					&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 2){
					struct mcmd * MX = new struct mcmd;
				//	struct mcmd MX;
					MX->cmd = {0};
					MX->type = 1;
					MX->param = atoi(C_C.text);
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if(!(strcmp(C_C.text,"MWAIT"))){
			//	while((C_C.flg != 1)&&(C_C.flg != 2)
			//		&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				while((C_C.flg != 2)
					&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 2){
					struct mcmd * MX = new struct mcmd;
				//	struct mcmd MX;
					MX->cmd = {0};
					MX->type = 3;
					MX->param = atoi(C_C.text);
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if(!(strcmp(C_C.text,"FUNC"))){
				while((C_C.flg != 1)&&(C_C.flg != 2)
					&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 1){
					struct mcmd * MX = new struct mcmd;
					MX->cmd = new char[strlen(C_C.text)+1];
					strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
					MX->type = 2;
					MX->param = -1;
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if(!(strcmp(C_C.text,"END"))){
			//	struct mcmd * MX = new struct mcmd;
			//	MX->cmd = {0};
			//	MX->type = 100;
			//	MX->param = 0;
			//	last = MDEV[ch].MCMD.size();
			//	if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
			}else if(!(strcmp(C_C.text,"P1"))){
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 19){
					struct mcmd * MX = new struct mcmd;
				//	struct mcmd MX;
					MX->cmd = new char[strlen(C_C.text)+1];
					strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
					MX->type = 0;
					MX->param = -1;
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if(!(strcmp(C_C.text,"P2"))){
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 19){
					struct mcmd * MX = new struct mcmd;
				//	struct mcmd MX;
					MX->cmd = new char[strlen(C_C.text)+1];
					strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
					MX->type = 0;
					MX->param = -2;
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if(!(strcmp(C_C.text,"P3"))){
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 19){
					struct mcmd * MX = new struct mcmd;
				//	struct mcmd MX;
					MX->cmd = new char[strlen(C_C.text)+1];
					strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
					MX->type = 0;
					MX->param = -3;
					last = (long)MDEV[ch].MCMD.size();
					if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
				}
			}else if((C_C.flg == 1)||(C_C.flg == 19)){
				struct mcmd * MX = new struct mcmd;
			//	struct mcmd MX;
				MX->cmd = new char[strlen(C_C.text)+1];
				strcpy_s(MX->cmd,strlen(C_C.text)+1,C_C.text);
				cgetchar(F);/*次の文字列を読み出し*/
				while((C_C.flg != 1)&&(C_C.flg != 19)&&(C_C.flg != 0x0a))cgetchar(F);
				if(C_C.flg == 1){
					MX->type = 0;
					if(!(strcmp(C_C.text,"P1"))){
						MX->param = -4;
					}else if(!(strcmp(C_C.text,"P2"))){
						MX->param = -5;
					}else if(!(strcmp(C_C.text,"P3"))){
						MX->param = -6;
					}
				}else{
					MX->type = 0;
					MX->param = 0;
				}
				last = (long)MDEV[ch].MCMD.size();
				if(last>0)MDEV[ch].MCMD[MDEV[ch].MCMD.size()-1].CMD.push_back(*MX);
			}
		}
	}
	if(C_C.flg == EOF){
		return 0;
	}else{
     	return 1;
	}
}

/**********************************************************************************/
std::map<int, char[32]> enumerateSerialPorts2()
{
    std::map<int, char[32]> result;
	char test_string[32];
	HANDLE	hTestCom;
	hTestCom = 0;
	DCB	Testdcb;			//シリアルポートのDCB構造体
	BOOL fTestSuccess;		//シリアルポート設定エラーフラグ
  	WCHAR HandleName[32];
	size_t ret4;
   
	sprintf_s(test_string,"\\\\.\\COM1");

	for(int i=2;i<50;i++){
		mbstowcs_s(&ret4,HandleName,32,test_string,32);
	   	hTestCom = CreateFileW(HandleName,
    		GENERIC_READ | GENERIC_WRITE,
        	0,
        	NULL,
        	OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED, /*非同期読み書き*/
	        NULL);							//Comm初期化
    	if (hTestCom == INVALID_HANDLE_VALUE){	//Comm初期化失敗
		//	MessageBoxW(NULL,test_string,L"NG1", MB_ICONHAND);
		}else{
			fTestSuccess = GetCommState(hTestCom,&Testdcb);
    		if (!fTestSuccess) {//ステータス取得失敗
		//		MessageBoxW(NULL,test_string,L"NG2", MB_ICONHAND);
			}else{
				sprintf_s(result[i],32,test_string);
			}
		}
		if(hTestCom !=NULL)CloseHandle(hTestCom);
		hTestCom = 0;
		sprintf_s(test_string,32,"\\\\.\\COM%d",i);
		mbstowcs_s(&ret4,HandleName,32,test_string,32);
	}
    return result;
}


HANDLE init_commport(struct serial_param * COMD)
{
	int pipeflg;
	DWORD      dwStoredFlags;
 	WCHAR HandleName[32];
	size_t ret4;
	HANDLE hCom;
	
	pipeflg = 0;
    dwStoredFlags = EV_RXCHAR;

	if(!(strcmp(COMD->comm_string,"\\\\.\\pipe\\P1"))){/*USB*/
		mbstowcs_s(&ret4,HandleName,32,COMD->comm_string,32);
		hCom = CreateNamedPipeW(HandleName,		/* パイプ名 */
			PIPE_ACCESS_INBOUND|FILE_FLAG_WRITE_THROUGH,	/* 入力 */
			PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
			1,					/* インスタンス数*/
			4096,				/* 出力バッファ・サイズ */
			4096,				/* 入力バッファ・サイズ */
			0,					/* タイムアウト */
			NULL);				/* セキュリティ属性なし */
		pipeflg = 1;
	}else if(!(strcmp(COMD->comm_string,"\\\\.\\pipe\\P2"))){/*QPSK*/
		mbstowcs_s(&ret4,HandleName,32,COMD->comm_string,32);
		hCom = CreateNamedPipeW(HandleName,		/* パイプ名 */
			PIPE_ACCESS_INBOUND|FILE_FLAG_WRITE_THROUGH,	/* 入力 */
			PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
			1,					/* インスタンス数*/
			4096,				/* 出力バッファ・サイズ */
			4096,				/* 入力バッファ・サイズ */
			0,					/* タイムアウト */
			NULL);				/* セキュリティ属性なし */
		pipeflg = 1;
	}else if(!(strcmp(COMD->comm_string,"\\\\.\\pipe\\P3"))){/*CMD*/
		mbstowcs_s(&ret4,HandleName,32,COMD->comm_string,32);
		hCom = CreateNamedPipeW(HandleName,		/* パイプ名 */
			PIPE_ACCESS_OUTBOUND | FILE_FLAG_WRITE_THROUGH,	/* 入力 */
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,					/* インスタンス数*/
			4096,				/* 出力バッファ・サイズ */
			4096,				/* 入力バッファ・サイズ */
			0,					/* タイムアウト */
			NULL);				/* セキュリティ属性なし */
		pipeflg = 1;
	}else{
		mbstowcs_s(&ret4,HandleName,32,COMD->comm_string,32);
		hCom = CreateFileW(HandleName,
    		GENERIC_READ | GENERIC_WRITE,
       	 	0,
       	 	NULL,
        	OPEN_EXISTING,
		//	FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, /*非同期読み書き*/
			FILE_FLAG_OVERLAPPED, /*非同期読み書き*/
        	NULL);							//Comm初期化
		pipeflg = 0;
	}
    if (hCom == INVALID_HANDLE_VALUE){	//Comm初期化失敗
    //	MessageBoxW(NULL,L"Can not Open COM Port",COMD->comm_string,MB_ICONHAND);
        hCom = 0;
		return 0;
    }
	fSuccess = GetCommState(hCom,&dcb);
	if(! fSuccess){					//Comm1ステータス取得失敗
		if(pipeflg){

		}else{
	//	MessageBoxW(NULL,L"Can't Get State of COM Port",COMD->comm_string,MB_ICONHAND);
			CloseHandle(hCom);
			hCom = 0;
			return(0);
		}
	}
// 	MessageBoxW(NULL,L"Open COM Port",COMD->comm_string,MB_ICONHAND);
	dcb.DCBlength = sizeof(DCB);
	if(COMD->BaudRate == 1200){
		dcb.BaudRate = CBR_1200;
		dcb.fOutxCtsFlow = 1;      // CTS output flow control
	}else if(COMD->BaudRate == 38400){
		dcb.BaudRate = CBR_38400;
		dcb.fOutxCtsFlow = 0;      // CTS output flow control
	}else if(COMD->BaudRate == 2400){
		dcb.BaudRate = CBR_2400;
		dcb.fOutxCtsFlow = 0;      // CTS output flow control
	}else{
		dcb.BaudRate = COMD->BaudRate;
		dcb.fOutxCtsFlow = 0;      // CTS output flow control
	}
	dcb.fBinary = TRUE;
//	if(COMMPARAM.fParity){
	if(COMMPARAM[COMD->no].fParity){
		dcb.fParity = TRUE;       // enable parity checking
	}else{
		dcb.fParity = FALSE;       // enable parity checking
	}
	if(modem_flg){
	    dcb.fOutxCtsFlow = TRUE;      // CTS output flow control
   		dcb.fOutxDsrFlow = TRUE;      // DSR output flow control
 	    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;      // RTS flow control 
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;   	  // DTR flow control
	}else{
	    dcb.fOutxCtsFlow = 0;      // CTS output flow control
   		dcb.fOutxDsrFlow = 0;      // DSR output flow control
 	    dcb.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
		dcb.fDtrControl = 0;	   //DTR_CONTROL_DISABLE DTR flow control type 
	}
    dcb.fDsrSensitivity = 0;   // DSR sensitivity 
    dcb.fTXContinueOnXoff = 1; // XOFF continues Tx 
    dcb.fOutX = 0;            // XON/XOFF out flow control 
    dcb.fInX = 0;             // XON/XOFF in flow control 
    dcb.fErrorChar = 0;       // enable error replacement 
    dcb.fNull = 0;            // enable null stripping 
    dcb.fAbortOnError = 0;     // abort reads/writes on error 
    dcb.ByteSize = 8;			//
	switch (COMMPARAM[COMD->no].Parity){
		case COM_P_NONE:
  			dcb.Parity = NOPARITY;
			break;
		case COM_P_ODD:
  			dcb.Parity = ODDPARITY;
			break;
		case COM_P_EVEN:
  			dcb.Parity = EVENPARITY;
			break;
		case COM_P_SPACE:
  			dcb.Parity = SPACEPARITY;
			break;
		case COM_P_MARK:
  			dcb.Parity = MARKPARITY;
			break;
		default:
  			dcb.Parity = NOPARITY;
			break;
	}
	switch (COMMPARAM[COMD->no].StopBits){
		case COM_S_ONE:
		    dcb.StopBits = ONESTOPBIT;
			break;
		case COM_S_ONEHALF:
		    dcb.StopBits = ONE5STOPBITS;
			break;
		case COM_S_TWO:
		    dcb.StopBits = TWOSTOPBITS;
			break;
		default:
		    dcb.StopBits = ONESTOPBIT;
			break;
	}
	fSuccess = SetCommState(hCom,&dcb);
	if(!fSuccess){					//Comm1パラメータ設定失敗
		if(pipeflg){

		}else{
	//	MessageBoxW(NULL,"Can't Set State of COM Port","Error",MB_ICONHAND);
			CloseHandle(hCom);
       		hCom = 0;
			return(0);
		}
    }
    GetCommTimeouts(hCom,&cmto);
    cmto.ReadIntervalTimeout 		= 1;
    cmto.ReadTotalTimeoutMultiplier = 0;
    cmto.ReadTotalTimeoutConstant 	= 1;
    cmto.WriteTotalTimeoutMultiplier = 0;
	cmto.WriteTotalTimeoutConstant   = 0;
    SetCommTimeouts(hCom,&cmto);		//Comm1タイムアウト設定
    if (!SetCommMask(hCom,dwStoredFlags))return 0;
	return hCom;
}

//------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int			i,j,k,n;
	size_t ret4;

 	// TODO: ここにコードを挿入してください。
	//--------------------------------------------------
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	WSADATA wsaData;
	std::ignore = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
//  wtpoint = 0;				//文字終端位置初期化
//  wwtpoint = 0;				//文字終端位置初期化
	Save_flg = 0;				/*受信データセーブフラグ*/
	send_save_flg = 0;			/*送信データセーブフラグ*/
	fkazu = 0;
	RtnCnt = 0;
	RtnThd = 60;
	part_counter = 0;
	sub_counter = 0;
	Display_flg = 1;
	LogSave_flg = 0;
	LogSave_Count = 0;
	DataSave_Count = 0;
	cmd_count = 0;
	C_count = 0;
	label_count = 0;
	label_flg = 0;
	interval_flg = 0;
	Interval = 1000;
	Lua_flg = 0;
	measure_onflg = 0;
	check_interval = 0;
	cmd_format_flg = 0; /*short*/
	P_count = 0;
	first_flg = 1;
	for(P_count = 0;P_count < 5;P_count++){
		COMMPARAM[P_count].no = P_count;
		COMMPARAM[P_count].BaudRate = 9600;
		COMMPARAM[P_count].fParity = 0;
		COMMPARAM[P_count].Parity = COM_P_NONE;
		COMMPARAM[P_count].StopBits = COM_S_ONE;
		COMMPARAM[P_count].modem_flg = 0;
		COMMPARAM[P_count].delimiter = 0;
	}
 	WCHAR HandleName[32];
	WCHAR Wdummy[32];
	WCHAR Wdummy2[64];
	SND.len = 0;
	spline = 0;
	spline2 = 0;
	key_outbuflen = 0;
	key.spnflg = 0;
	key.spline = 0;
	key.spchar = 0;
	key.lastparam = 0;
//	COMBUF.len = 0;
//	COMBUF.flg = 0;
	for(i=0;i<5;i++){
		COMBUF[i].len = 0;
		COMBUF[i].flg = 0;
	}	
	FIXParam_Cnt = 0;
	for(i=0;i<MAKER_MAX;i++)devicemaker_num[i] = 0;
	BUSON_flg = 0;
//	WSIZE_WIDTH = 558;//540+2+16
//	WSIZE_HEIGHT = 530;
	WSIZE_WIDTH = 586;//570+16
	WSIZE_HEIGHT = 586;//39line

	OpenPort_mode = OPENMODE_ONCE;
	sprintf_s(comm_string,"\\\\.\\COM1"); /*config.txtで指示が無ければ*/
	strcpy_s(SCRIPT_file, 20, "script.lua");
	strcpy_s(REG_file, 20, "reg.txt");
	strcpy_s(CMD_file, 20, "command.txt");

	setlocale(LC_CTYPE, "ja_JP.UTF-8");

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

	/*イベントは手動リセット*/
#if 0
//	hEventObject = CreateEvent(NULL,TRUE,TRUE,LPCWSTR(""));
	hEventObject = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
	COMOverlapped.hEvent = hEventObject;
	COMOverlapped.Offset = 0;
	COMOverlapped.OffsetHigh = 0;
//	hEventObjectW = CreateEvent(NULL,TRUE,TRUE,LPCWSTR(""));
	hEventObjectW = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
	COMOverlappedW.hEvent = hEventObjectW;
	COMOverlappedW.Offset = 0;
	COMOverlappedW.OffsetHigh = 0;
#endif
	for(i=0;i<5;i++){
		hEventObject[i] = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
		COMOverlapped[i].hEvent = hEventObject[i];
		COMOverlapped[i].Offset = 0;
		COMOverlapped[i].OffsetHigh = 0;
		hEventObjectW[i] = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
		COMOverlappedW[i].hEvent = hEventObjectW[i];
		COMOverlappedW[i].Offset = 0;
		COMOverlappedW[i].OffsetHigh = 0;
	}
//	MessageBoxW(NULL,L"A0",L"Debug",MB_ICONHAND);

	fopen_s(&F,"config.txt","rt");
	if(F != NULL){
		while(mzParser(F));
		fclose(F);
	}
	
	getUSBVID();

//	MessageBoxW(NULL,L"A0A",L"Debug",MB_ICONHAND);

	fopen_s(&F, REG_file, "rt");
	if (F != NULL) {
		while (RegParser(F));
		fclose(F);
	}

//	MessageBoxW(NULL,L"A0B",L"Debug",MB_ICONHAND);

	fopen_s(&F,CMD_file,"rt");
	if(F != NULL){
		while(mxParser(F));
		fclose(F);
	}

//	MessageBoxW(NULL,L"A0C",L"Debug",MB_ICONHAND);

	fopen_s(&F,TLM_file,"rt");
	if(F != NULL){
		while(TlmParser(F));
		fclose(F);
	}
	

	fopen_s(&F,MEASURE_DEVICE_file,"rt");
	if(F != NULL){
		while(measureParser(F));
		fclose(F);
	}	
	
	for(i=0;i<MDEV.size();i++){
		fopen_s(&F,MDEV[i].setting_file_name,"rt");
		if(F != NULL){
			while(measureCMDParser(F,i));
			fclose(F);
		}
	}
	sprintf_s(COMMPARAM[0].comm_string,32,comm_string);
//	sprintf_s(COMMPARAM.comm_string,32,comm_string);
	
	luaL_openlibs(L);

//	MessageBoxW(NULL,L"A1",L"Debug",MB_ICONHAND);

	if(LM.serch("BUSON") != -1){
		if(REG[0].DATA != nullptr){
			REG[0].DATA[(LM.LU[(LM.serch("BUSON"))].addr)] = BUSON_flg;
		}
	}
//	_ltow_s(LM.LU[(LM.serch("BUSON"))].addr,Wdummy,32,16);
//	MessageBoxW(NULL,Wdummy,L"Debug",MB_ICONHAND);

	lua_pushcfunction(L, DispStrP);
	lua_setglobal(L, "DispStrP");
	lua_pushcfunction(L, DispParamlP);
	lua_setglobal(L, "DispParamlP");
	lua_pushcfunction(L, DispParamfP);
	lua_setglobal(L, "DispParamfP");
	lua_pushcfunction(L, SerialInit);
	lua_setglobal(L, "SerialInit");
	lua_pushcfunction(L, SerialSend);
	lua_setglobal(L, "SerialSend");
	lua_pushcfunction(L, SerialRecv);
	lua_setglobal(L, "SerialRecv");
	lua_pushcfunction(L, SerialRecv2);
	lua_setglobal(L, "SerialRecv2");
	lua_pushcfunction(L, SerialWaitRecv);
	lua_setglobal(L, "SerialWaitRecv");
	lua_pushcfunction(L, Sleep);
	lua_setglobal(L, "Sleep");
	lua_pushcfunction(L, uSleep);
	lua_setglobal(L, "uSleep");
	lua_pushcfunction(L, mSleep);
	lua_setglobal(L, "mSleep");
	lua_pushcfunction(L, RegRead);
	lua_setglobal(L, "RegRead");
	lua_pushcfunction(L, RegWrite);
	lua_setglobal(L, "RegWrite");
	lua_pushcfunction(L, HIDSend);
	lua_setglobal(L, "HIDSend");
	lua_pushcfunction(L, HIDSendS);
	lua_setglobal(L, "HIDSendS");
	lua_pushcfunction(L, HIDRecv);
	lua_setglobal(L, "HIDRecv");
	lua_pushcfunction(L, HIDRecv2);
	lua_setglobal(L, "HIDRecv2");
	lua_pushcfunction(L, CRC16);
	lua_pushcfunction(L, NetInitCheck);
	lua_setglobal(L, "NetInitCheck");
	lua_pushcfunction(L, NetConnect);
	lua_setglobal(L, "NetConnect");
	lua_pushcfunction(L, NetDisConnect);
	lua_setglobal(L, "NetDisConnect");
	lua_pushcfunction(L, NetSend);
	lua_setglobal(L, "NetSend");
	lua_pushcfunction(L, NetWaitRecv);
	lua_setglobal(L, "NetWaitRecv");
	lua_pushcfunction(L, NetRecv);
	lua_setglobal(L, "NetRecv");
	lua_pushcfunction(L, DeviceSeq);
	lua_setglobal(L, "DeviceSeq");
	lua_pushcfunction(L, RegRead);
	lua_setglobal(L, "RegRead");
	lua_pushcfunction(L, RegWrite);
	lua_setglobal(L, "RegWrite");
	lua_pushcfunction(L, CRC16);
	lua_setglobal(L, "CRC16");
	lua_pushcfunction(L, mkframe);
	lua_setglobal(L, "mkframe");
	lua_pushcfunction(L, GetAddr);
	lua_setglobal(L, "GetAddr");
	lua_pushcfunction(L, KeyWait);
	lua_setglobal(L, "KeyWait");
	lua_pushcfunction(L, SetC);
	lua_setglobal(L, "SetC");
	lua_pushcfunction(L, GetC);
	lua_setglobal(L, "GetC");
	lua_pushcfunction(L, HDS2102S_getwaveform2);
	lua_setglobal(L, "HDS2102S_getwaveform2");
	lua_pushcfunction(L, HDS2102S_getwavebmp2);
	lua_setglobal(L, "HDS2102S_getwavebmp2");
	
	if(luaL_loadfile(L, SCRIPT_file)|| lua_pcall(L, 0, 0, 0) ) {
	//	strcpy_s(dummy, 20, SCRIPT_file);		//フォントタイプ
		if(strlen(SCRIPT_file)==0){
			wcscpy_s(Wdummy,L"ScriptFilename undefined");
		}else{
			mbstowcs_s(&ret4,Wdummy,32,SCRIPT_file,32);
		}
		Lua_flg = 0;
		const char* str = lua_tostring(L, -1);
		MultiByteToWideChar(CP_UTF8, 0, str, 64, Wdummy2, 64);
		MessageBoxW(NULL, Wdummy2,Wdummy,MB_ICONHAND);
	//	MessageBoxW(NULL, Wdummy,L"Error", MB_ICONHAND);
	//	MessageBox(NULL, "Script File Not Open","Error", MB_ICONHAND);
	}else{
		Lua_flg = 1;
	}
	
	PortList = enumerateSerialPorts2();

//	MessageBoxW(NULL,L"A2",L"Debug",MB_ICONHAND);

	int rst;
	getUSBVID();
	for(k=0;k<5;k++){
		if(COMMPARAM[k].USB == 1){
			for(i=0;i<USBSER.size();i++){
				if((COMMPARAM[k].VID == USBSER[i].vid)
				 &&(COMMPARAM[k].PID == USBSER[i].pid)){
				//	if(!(wcscmp(COMMPARAM[k].SERIAL,USBSER[i].ser))){
					rst = wcscmp(COMMPARAM[k].SERIAL,USBSER[i].ser);
					if(rst == 0){
						sprintf_s(COMMPARAM[k].comm_string,32,"\\\\.\\COM%d",
							USBSER[i].port_num);
					}
				}
			}
		}
	}

	PortList = enumerateSerialPorts2();

	/*USB HIDデバイスのアクセスに必要なデバイス種別、エンドポイント情報を識別する*/
  	libusb_device **list;
	struct libusb_device_descriptor desc;
	libusb_device_handle *handle;
  	libusb_init(NULL);
  	int cnt = (int)libusb_get_device_list(NULL, &list);/*USBデバイスリストを取得する*/

	for(i=0;i<MDEV.size();i++){
		if(MDEV[i].interface_type == 1){//serial
			if(MDEV[i].serial_port_num > 100){
				MDEV[i].init = 0;
				for(j=0;j<USBSER.size();j++){
					if(MDEV[i].usb_ser != NULL){
						if((MDEV[i].usb_vid == USBSER[j].vid)
						 &&(MDEV[i].usb_pid == USBSER[j].pid)){
							if(!(wcscmp(MDEV[i].usb_ser,USBSER[j].ser))){
								sprintf_s(comm_string2,32,"\\\\.\\COM%d",
														USBSER[j].port_num);
								MDEV[i].serial_port = new char[strlen(comm_string2)+1];
								strcpy_s(MDEV[i].serial_port,strlen(comm_string2)+1,
														comm_string2);
								MDEV[i].init = 1;
							//	portset_flg = 1;
							}
						}
					}else{
						if((MDEV[i].usb_vid == USBSER[j].vid)
						 &&(MDEV[i].usb_pid == USBSER[j].pid)){
							sprintf_s(comm_string2,32,"\\\\.\\COM%d",USBSER[j].port_num);
							MDEV[i].serial_port = new char[strlen(comm_string2)+1];
						strcpy_s(MDEV[i].serial_port,strlen(comm_string2)+1,comm_string2);
							MDEV[i].init = 1;
							portset_flg = 1;
						}
					}
				}
			}
		}else if(MDEV[i].interface_type == 2){//1:serial 2:IVI 3:USB HID 4:LAN 5:GPIB
		//	keysight_ivi_device VIS;
		//	ViRsrc VISA_ADDRESS = MDEV[i].VISA_Addr;
		//	viOpenDefaultRM(&resourceManager);
		//	ViStatus error=viOpen(resourceManager,VISA_ADDRESS,VI_NO_LOCK,0,&VIS.session);
		//	MDEV[i].init = 0;
		//	if (error >= VI_SUCCESS){
		//		strcpy_s(VIS.name,sizeof(VIS.name),MDEV[i].name);
		//		VIS.delimiter = MDEV[i].delimiter;
		//		VISESS.push_back(VIS);
		//		VISESS[VISESS.size()-1].ident = (int)VISESS.size() + 'K';
		//		MDEV[i].init = 1;
		//	}
		}else if((MDEV[i].interface_type == 3)||(MDEV[i].interface_type == 7)){
						//1:serial 2:IVI 3:USB HID 4:LAN 5:GPIB 6: 7:HID DP100
			for (n = 0; n < cnt; n++) {
				libusb_device* dev = list[n];
				libusb_get_device_descriptor(dev, &desc);
				MDEV[i].init = 0;
				int ret = libusb_open(dev, &handle);
				if(ret==0){/*Open成功*/
					/*定義計測器リストをなめてVID,PIDの一致を見る*/
					if((desc.idVendor == MDEV[i].usb_vid)
					 &&(desc.idProduct == MDEV[i].usb_pid)){
						usbhid_dev U;
     					libusb_config_descriptor *config;
     					const libusb_interface_descriptor *interdesc;
     					const libusb_endpoint_descriptor *epdesc;
     					const libusb_interface *inter;
     					libusb_get_config_descriptor(dev, 0, &config);
     					for(int ii = 0; ii < (int) config->bNumInterfaces; ii++) {
	  						inter = &config->interface[ii];
	  	  					for(int jj = 0; jj < inter->num_altsetting; jj++) {
	       						interdesc = &inter->altsetting[jj];
	       						for(int kk = 0;kk < (int)interdesc->bNumEndpoints; kk++) {
									epdesc = &interdesc->endpoint[kk];
									if(epdesc->bEndpointAddress & 0x80){
										U.EPIN = epdesc->bEndpointAddress;
									}else{
										U.EPOUT = epdesc->bEndpointAddress;
									}
//	MessageBoxW(NULL,_itow(epdesc->bEndpointAddress,Wtmp0,10),_itow(epdesc->bmAttributes,Wtmp,10), MB_ICONHAND);
//OWON HDS2102S OutEndpoint 0x01 InEndpoint 0x81
								}
							}
						}
						U.handle = handle;
						U.MDEV_num = i;/*接続デバイスが定義計測器の何番であるか*/
						int ret = libusb_claim_interface(U.handle,0);
						UHDEV.push_back(U);
						MDEV[i].init = 1;
//	MessageBoxW(NULL,_itow(desc.idVendor,Wtmp0,10),_itow(desc.idProduct,Wtmp,10), MB_ICONHAND);
					}else{
						libusb_close(handle);
					}
				}
			}
		}else if(MDEV[i].interface_type == 4){//1:serial 2:IVI 3:USB HID 4:LAN 5:GPIB
			Socket Nport;
			Nport.init_client((u_short)MDEV[i].lan_port,MDEV[i].lan_addr);
			Nport.MDEV_num = i;
			Nport.Initflg = 1;
			NET.push_back(Nport);
		}else if(MDEV[i].interface_type == 6){//6:FTDI Bitbang
		//	i = (int)BITBANG.size();
		//	if(i > 0){
		//	}
		}
	}

	sprintf_s(COMMPARAM[0].comm_string,32,comm_string);
#if 0
	init_commport(&COMMPARAM[0]);
	if(hCom){
		COMMPARAM[0].Init = 1;
	}else{
		COMMPARAM[0].Init = 0;
	}
#else
	COMMPARAM[0].Init = 0;
	mbstowcs_s(&ret4,HandleName,32, COMMPARAM[0].comm_string,32);
	hCom[0] = CreateFileW(HandleName,
    	GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED, /*非同期読み書き*/
//      FILE_ATTRIBUTE_NORMAL,
        NULL);							//Comm初期化
    if (hCom[0] == INVALID_HANDLE_VALUE){	//Comm初期化失敗
    	MessageBoxW(NULL,L"Can not Create File of COM Port",L"Error",MB_ICONHAND);
        hCom[0] = 0;
    }else{
	//	ClearCommError(hCom,&DErr,NULL);
		SetupComm(hCom[0],COMMBUFSIZE,COMMBUFSIZE);
		fSuccess = GetCommState(hCom[0],&dcb);
    	if (! fSuccess) {					//Comm1ステータス取得失敗
    		MessageBoxW(NULL,L"Can not Get Comm State of COM Port",L"Error",MB_ICONHAND);
    	    CloseHandle(hCom[0]);
        	hCom[0] = 0;
    	}
	}
	dcb.DCBlength = sizeof(DCB);
/*  dcb.BaudRate = CBR_9600;*/	//9600bps
    dcb.BaudRate = COMMPARAM[0].BaudRate;
	dcb.fBinary = TRUE;
/*	dcb.fParity = FALSE;*/       // enable parity checking
	if(COMMPARAM[0].fParity){
		dcb.fParity = TRUE;       // enable parity checking
	}else{
		dcb.fParity = FALSE;       // enable parity checking
	}
	if(modem_flg){
	    dcb.fOutxCtsFlow = TRUE;      // CTS output flow control
   		dcb.fOutxDsrFlow = TRUE;      // DSR output flow control
 	    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;      // RTS flow control 
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;   	  // DTR flow control
	}else{
	    dcb.fOutxCtsFlow = 0;      // CTS output flow control
   		dcb.fOutxDsrFlow = 0;      // DSR output flow control
 	    dcb.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
		dcb.fDtrControl = 0;	   //DTR_CONTROL_DISABLE DTR flow control type 
	}
    dcb.fDsrSensitivity = 0;   // DSR sensitivity 
    dcb.fTXContinueOnXoff = 1; // XOFF continues Tx 
    dcb.fOutX = 0;            // XON/XOFF out flow control 
    dcb.fInX = 0;             // XON/XOFF in flow control 
    dcb.fErrorChar = 0;       // enable error replacement 
    dcb.fNull = 0;            // enable null stripping 
    dcb.fAbortOnError = 0;     // abort reads/writes on error 
    dcb.ByteSize = 8;			//
 //   dcb.Parity = NOPARITY;		//…見ての通り
 //   dcb.StopBits = ONESTOPBIT;	//
	switch (COMMPARAM[0].Parity){
		case COM_P_NONE:
  			dcb.Parity = NOPARITY;
			break;
		case COM_P_ODD:
  			dcb.Parity = ODDPARITY;
			break;
		case COM_P_EVEN:
  			dcb.Parity = EVENPARITY;
			break;
		case COM_P_SPACE:
  			dcb.Parity = SPACEPARITY;
			break;
		case COM_P_MARK:
  			dcb.Parity = MARKPARITY;
			break;
		default:
  			dcb.Parity = NOPARITY;
			break;
	}
	switch (COMMPARAM[0].StopBits){
		case COM_S_ONE:
		    dcb.StopBits = ONESTOPBIT;
			break;
		case COM_S_ONEHALF:
		    dcb.StopBits = ONE5STOPBITS;
			break;
		case COM_S_TWO:
		    dcb.StopBits = TWOSTOPBITS;
			break;
		default:
		    dcb.StopBits = ONESTOPBIT;
			break;
	}
/*	if(BaudRate_read)dcb.BaudRate = BaudRate_read;*/
	if(hCom[0] != 0){
		fSuccess = SetCommState(hCom[0],&dcb);
    	if (! fSuccess) {					//Comm1パラメータ設定失敗
    		MessageBoxW(NULL,L"Can't Set Comm State",L"Error",MB_ICONHAND);
        	CloseHandle(hCom[0]);
        	hCom[0] = 0;
    	}
    }
	if(hCom[0]){
	    GetCommTimeouts(hCom[0],&cmto);
	    cmto.ReadIntervalTimeout 		= 500;
	    cmto.ReadTotalTimeoutMultiplier = 0;
	    cmto.ReadTotalTimeoutConstant 	= 1;
	    cmto.WriteTotalTimeoutMultiplier = 0;
		cmto.WriteTotalTimeoutConstant   = 500;
	    SetCommTimeouts(hCom[0],&cmto);		//Comm1タイムアウト設定
		COMMPARAM[0].Init = 1;
	}else{
		COMMPARAM[0].Init = 0;
	}
#endif
    for(j=0;j<36;j++){
	   	for(i=0;i<31;i++)CDSP.str_Name[j][i] = ' ';
	   	for(i=0;i<31;i++)CDSP.str_Val[j][i] = ' ';
	   	CDSP.str_Name[j][31] = 0;
	   	CDSP.str_Val[j][31] = 0;
	   	CDSP.str_U[j] = 0;
	}
    for(i=0;i<100;i++){
	//	frm_U[i] = 0;//ASCII
		frm_U[i] = (Display_flg&0x01);//ASCII
		RtnPos[i] = -1;//改行位置
		sRtnPos[i] = -1;//改行位置
	}
    for(i=0;i<20;i++){
	//	sfrm_U[i] = 0;//ASCII
		sfrm_U[i] = (Display_flg&0x01);//ASCII
	}

	RtnPos[0] = 0;
	RtnN = 0;
	FstN = 0;
	sRtnPos[0] = 0;
	sRtnN = 0;
	sFstN = 0;
	retcnt = 0;
	frm_U[0] = (Display_flg&0x01);
	for(j=0;j<LINGBUF_SIZE;j++)frmm[j] = 0x20;
	for(j=0;j<SENDBUF_SIZE;j++)sfrmm[j] = 0x20;
	stpoint = 0; //表示先頭位置
	edpoint = 0; //表示末尾位置
	lnpoint = 0;
	sstpoint = 0; //表示先頭位置
	sedpoint = 0; //表示末尾位置
	//--------------------------------------------------
	MSG msg;
	HACCEL hAccelTable;

//	MessageBoxW(NULL,L"A3",L"Debug",MB_ICONHAND);

	// グローバル文字列を初期化しています。
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BINCOM40, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// アプリケーションの初期化を実行します:
	hogeWnd = InitInstance (hInstance, nCmdShow);
	if (!hogeWnd){
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BINCOM40));

	if(Lua_flg == 1){
		lua_getglobal(L, "Startup");
		lua_pcall(L, 0, 0, 0);
	}
//	_ASSERTE(_CrtCheckMemory());

//	MessageBoxW(NULL,L"A4",L"Debug",MB_ICONHAND);

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0)){
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
//	_ASSERTE(_CrtCheckMemory());
	
	while (!REG.empty()) {
    	if(REG.back().name != nullptr){delete[] REG.back().name; REG.back().name= NULL;}
    	if(REG.back().DATA != nullptr){delete[] REG.back().DATA; REG.back().DATA= NULL;}
    	REG.pop_back();
	}	
//	_ASSERTE(_CrtCheckMemory());
	j = (int)USBSER.size();
	for (n = 0; n < j; n++)USBSER.pop_back();
//	j = (int)VISESS.size();
//	for (n = 0; n < j; n++) {
//		viClose(VISESS[VISESS.size()-1].session);
//		VISESS.pop_back();
//	}
	j = (int)LL.size();
	for(n=0;n<j;n++){delete[] LL.LV[n].name;LL.LV[n].name=NULL;}
	j = (int)LM.size();
	for(n=0;n<j;n++){delete[] LM.LU[n].name;LM.LU[n].name=NULL;}
	j = (int)TLM.size();
	for(n=0;n<j;n++){delete[] TLM[n].name;TLM[n].name=NULL;}
	
	j = (int)CV.size();
	for(n=0;n<j;n++){
		delete CV[n].data;
		CV[n].data =NULL;
		delete CV[n].label;
		CV[n].label=NULL;
		delete CV[n].name;
		CV[n].name=NULL;
	}
	CV.clear();/*全削除*/
	
	libusb_free_device_list(list,1);
	libusb_exit(NULL);
	
//	_ASSERTE(_CrtCheckMemory());

	lua_settop(L, 0);
	if(L != NULL){
		lua_close(L);
		L = NULL;
	}
//	viClose(resourceManager);
//	CoUninitialize();
	
	return (int) msg.wParam;
}

/*-----------------------------------------------------------------------*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_BINCOM40));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCEW(IDC_BINCOM40);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
/*-----------------------------------------------------------------------*/
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	RECT	rect;

	rect.left   = 0;
	rect.top    = 0;
//	rect.right  = 542;
//	rect.bottom = 550;
	rect.right  = WSIZE_WIDTH;
	rect.bottom = WSIZE_HEIGHT;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。
	/* フレームの作成 */
	hWnd = CreateWindowW(szWindowClass, L"バイナリモニタ",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right-rect.left),
		(rect.bottom-rect.top),
		nullptr,
		nullptr,
		hInstance,
		nullptr);	//幅530,高さ420ドット

	if(!hWnd)return hWnd;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return hWnd;
}
/*-----------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HINSTANCE	hInstance;
    static HANDLE hThread[10];  /*静的変数にする */
	WCHAR szName[256];
	char szDir[1024];
	WCHAR LABELName[200];
	WCHAR szModel[] = L"CMD File (*.TXT)\0*.TXT\0All Files (*.*)\0*.*\0";
    char dummy[200] = {0};
    WCHAR Wdummy[1024] = {0};
	int i,j;
	int size;
	DWORD dwID;
	size_t ret2,ret5;
	char filenamebuf[250];
	DWORD DErr;

	switch (message){
		case WM_CREATE:
		{
			HDC		hdc;
			RECT	rr;	//テキスト範囲構造体
			wcscpy_s(szName,L"");
			GetCurrentDirectory(512,LPTSTR(szDir));
			memset(&lpofn, 0x00, sizeof(OPENFILENAME));
//			lpofn.lStructSize	= OPENFILENAME_SIZE_VERSION_400;
			lpofn.lStructSize	= sizeof(OPENFILENAME);
			lpofn.hwndOwner = hWnd;
			lpofn.lpstrFilter = szModel;
			lpofn.lpstrCustomFilter = NULL;
			lpofn.nMaxCustFilter = 0;
			lpofn.nFilterIndex = 1;
			lpofn.lpstrFile = szName;
			lpofn.nMaxFile = sizeof( szName );
			lpofn.lpstrFileTitle = NULL;
			lpofn.nMaxFileTitle = 0;
			lpofn.lpstrInitialDir = (LPCWSTR)szDir;
			lpofn.lpstrTitle = L"OPENFILENAME";
			lpofn.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
			lpofn.nFileOffset = 0;
			lpofn.nFileExtension = 0;
			lpofn.lpstrDefExt = L"*";
			lpofn.lCustData = NULL;
			lpofn.lpfnHook = NULL;
			lpofn.lpTemplateName = NULL;
			
			if(COMMPARAM[0].Init == 0){
				sprintf_s(PortText,18,"COMX");
			}else{
				for(i=0;i<strlen(COMMPARAM[0].comm_string);i++){
					if(COMMPARAM[0].comm_string[i] == 'C'){
						sprintf_s(PortText,18,COMMPARAM[0].comm_string+i);
					}
				}
			}
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			//メニュー初期化
			hMenu = CreateMenu();
		    hSubMenu0=CreatePopupMenu();
		    hSubMenu1=CreatePopupMenu();
		    hSubMenu2=CreatePopupMenu();
	    	mbstowcs_s(&ret5,LABELName,32,PortText,32);
			AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu1,LABELName);
		//	if(PortList.size() > 0){
			if(PortList.size() > 1){
				for(i=0;i<50;i++){
					if(PortList.find(i) != PortList.end()){
						sprintf_s(dummy,20,PortList[i]);
						mbstowcs_s(&ret5,LABELName,200,dummy,200);
						AppendMenuW (hSubMenu1,MF_STRING,((long long int)(0x9000))+i,LABELName);
					}
				}
			}
			AppendMenuW (hMenu,MF_STRING,IDM_EDIT,L"Baud...");
			AppendMenuW (hMenu,MF_STRING,IDM_CMD,L"REG...");
			AppendMenuW (hMenu,MF_STRING,IDM_SAVE,L"SAVE");
			//もしcommand.txtに内容があったら
			size = (int)(CV.size());
			if(size){
				AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu0,L"SEND");
				for(i=0;i<size;i++){
					for(j=0;j<CV[i].name_len;j++)Wdummy[j]= CV[i].name[j];
					Wdummy[CV[i].name_len] = 0;
					//コマンドは300番から順番に割り振る
					AppendMenuW (hSubMenu0,MF_STRING,((long long int)(0x8000))+i,Wdummy);
				}
				AppendMenuW (hSubMenu0,MF_DISABLED|MF_SEPARATOR,
					IDM_SPARE,Wdummy);
				AppendMenuW (hSubMenu0,MF_STRING,IDM_SSAVE,
					L"SaveSendData");
				AppendMenuW (hSubMenu0,MF_DISABLED|MF_SEPARATOR,
					IDM_SPARE,Wdummy);
				AppendMenuW (hSubMenu0,MF_STRING,IDM_READ,
					L"ReadCMDFile");
			}
			switch(Display_flg){
				case 0:
					frm_U[RtnN] = 0;
					AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Text");
					break;
				case 1:
					frm_U[RtnN] = 1;
					AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Hex");
					break;
				case 2:
					frm_U[RtnN] = 2;
					AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Lua");
					break;
				default:
					Display_flg = 1;
					frm_U[RtnN] = 1;
					AppendMenuW (hMenu,MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Hex");
					break;
				
			}
			AppendMenuW (hSubMenu2,MF_STRING,((long long int)(0xA000))+0,L"Txt");
			AppendMenuW (hSubMenu2,MF_STRING,((long long int)(0xA000))+1,L"Hex");
			AppendMenuW (hSubMenu2,MF_STRING,((long long int)(0xA000))+2,L"Lua");
			switch(BUSON_flg){
				case 0:
					AppendMenuW (hMenu,MF_STRING,IDM_BUSON,L"BUS ON");
					break;
				case 1:
					AppendMenuW (hMenu,MF_STRING,IDM_BUSON,L"BUS OFF");
					break;
				default:
					BUSON_flg = 1;
					AppendMenuW (hMenu,MF_STRING,IDM_BUSON,L"BUS OFF");
					break;
			}
			SetMenu(hWnd,hMenu);		//メニューが一つだけ
		//	SetTimer(hWnd,1,1000,NULL);	//タイマイベントを1秒おきに
			SetTimer(hWnd,1,Interval,NULL);	//タイマイベントを1秒おきに
			
			hdc = GetDC(hWnd);
			SetBkMode(hdc,TRANSPARENT);
			SetTextColor(hdc,RGB(0,0,0));
			GetClientRect(hWnd,&rr);
			FillRect(hdc, &rr, (HBRUSH)(COLOR_WINDOW+1));
			ReleaseDC(hWnd,hdc);

			InitializeCriticalSection(&cscom);

		//	hThread = CreateThread(NULL,0,ThreadFunc,(LPVOID)hWnd,0,&dwID);
		//	if(hThread!=NULL)SetThreadPriority(hThread,THREAD_PRIORITY_LOWEST);
			if(hCom[0] != NULL){
				hThread[0] = CreateThread(NULL,0,ThreadFunc,(LPVOID)hWnd,0,&dwID);
				if(hThread[0]!=NULL)SetThreadPriority(hThread[0],THREAD_PRIORITY_LOWEST);
			}
			if(hCom[1] != NULL){
				hThread[1] = CreateThread(NULL,0,ThreadFunc2,(LPVOID)hWnd,0,&dwID);
				if(hThread[1]!=NULL)SetThreadPriority(hThread[1],THREAD_PRIORITY_LOWEST);
			}
			if(hCom[2] != NULL){
				hThread[2] = CreateThread(NULL,0,ThreadFunc3,(LPVOID)hWnd,0,&dwID);
				if(hThread[2]!=NULL)SetThreadPriority(hThread[2],THREAD_PRIORITY_LOWEST);
			}
			if(hCom[3] != NULL){
				hThread[3] = CreateThread(NULL,0,ThreadFunc4,(LPVOID)hWnd,0,&dwID);
				if(hThread[3]!=NULL)SetThreadPriority(hThread[3],THREAD_PRIORITY_LOWEST);
			}
			if(hCom[4] != NULL){
				hThread[4] = CreateThread(NULL,0,ThreadFunc5,(LPVOID)hWnd,0,&dwID);
				if(hThread[4]!=NULL)SetThreadPriority(hThread[4],THREAD_PRIORITY_LOWEST);
			}
			if((NET.size() > 0)&&(NET[0].Initflg == 1)){
				hThread[5] = CreateThread(NULL,0,ThreadFunc6,(LPVOID)hWnd,0,&dwID);
				if(hThread[5] != 0)SetThreadPriority(hThread[5],THREAD_PRIORITY_LOWEST);
			}
			
		//	SetCommMask(hCom,EV_RXCHAR);
		}
		break;
		case WM_COMMAND:
			wcscpy_s(szName,L"");
			GetCurrentDirectory(512,LPTSTR(szDir));
			ZeroMemory(&lpofn, sizeof(OPENFILENAME));
//			lpofn.lStructSize	= OPENFILENAME_SIZE_VERSION_400;
			lpofn.lStructSize	= sizeof(OPENFILENAME);
			lpofn.hwndOwner = hWnd;
			lpofn.lpstrFilter = szModel;
			lpofn.lpstrCustomFilter = NULL;
			lpofn.nMaxCustFilter = 0;
			lpofn.nFilterIndex = 1;
			lpofn.lpstrFile = szName;
			lpofn.nMaxFile = sizeof( szName );
			lpofn.lpstrFileTitle = NULL;
			lpofn.nMaxFileTitle = 0;
			lpofn.lpstrInitialDir = (LPCWSTR)szDir;
			lpofn.lpstrTitle = L"OPENFILENAME";
			lpofn.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
			lpofn.nFileOffset = 0;
			lpofn.nFileExtension = 0;
			lpofn.lpstrDefExt = L"*";
			lpofn.lCustData = NULL;
			lpofn.lpfnHook = NULL;
			lpofn.lpTemplateName = NULL;
			
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// 選択されたメニューの解析:
			switch (wmId){
				case	IDM_EDIT:
					//エディットダイアログボックス表示
					DialogBoxW(hInstance,L"DIALOG1",
								HWND_DESKTOP,(DLGPROC)dlgProc);
					return 0;
				case	IDM_CMD:
					//コマンドダイアログボックス表示
					DialogBoxW(hInstance,L"DIALOG2",
								HWND_DESKTOP,(DLGPROC)dlgProc2);
					return 0;
				case	IDM_BUSON:
					switch(BUSON_flg){
						case 0:
						ModifyMenuW(hMenu,6,MF_BYPOSITION|MF_STRING,IDM_BUSON,L"BUS OFF");
							if(LM.serch("BUSON") != -1){
								if(REG[0].DATA != nullptr){
									BUSON_flg = 1;
									REG[0].DATA[(LM.LU[(LM.serch("BUSON"))].addr)] = 1;
								}
							}
							break;
						case 1:
						ModifyMenuW(hMenu,6,MF_BYPOSITION|MF_STRING,IDM_BUSON,L"BUS ON");
							if(LM.serch("BUSON") != -1){
								if(REG[0].DATA != nullptr){
									BUSON_flg = 0;
									REG[0].DATA[(LM.LU[(LM.serch("BUSON"))].addr)] = 0;
									lua_getglobal(L, "Startup");
									lua_pcall(L, 0, 0, 0);
								}
							}
							break;
						default:
						ModifyMenuW(hMenu,6,MF_BYPOSITION|MF_STRING,IDM_BUSON,L"BUS ON");
							if(LM.serch("BUSON") != -1){
								if(REG[0].DATA != nullptr){
									REG[0].DATA[(LM.LU[(LM.serch("BUSON"))].addr)] = 0;
									BUSON_flg = 0;
									lua_getglobal(L, "Startup");
									lua_pcall(L, 0, 0, 0);
								}
							}
							break;
					}
					return 0;
				case	IDM_READ:
					//コマンド設定ファイルを読む
					if(GetOpenFileNameW(&lpofn)){
						size = (int)(CV.size());/*削除すべき数を把握*/
						if(size){
							for(i=0;i<size;i++){
								delete CV[i].data;
								CV[i].data=NULL;
								delete CV[i].label;
								CV[i].label=NULL;
								delete CV[i].name;
								CV[i].name=NULL;
								DeleteMenu(hSubMenu0,((long long int)(0x8000))+i,MF_BYCOMMAND);
							}
							CV.clear();/*全削除*/
						}
						DeleteMenu(hSubMenu0,IDM_SPARE,MF_BYCOMMAND);
						DeleteMenu(hSubMenu0,IDM_SSAVE,MF_BYCOMMAND);
						DeleteMenu(hSubMenu0,IDM_SPARE,MF_BYCOMMAND);
						DeleteMenu(hSubMenu0,IDM_READ,MF_BYCOMMAND);
						wcstombs_s(&ret2,filenamebuf,100,lpofn.lpstrFile,100);
						_wfopen_s(&F,lpofn.lpstrFile,L"rt");
#if 0
						if(F != NULL){
							C_count = 0;
							while(mxParser(F));
							fclose(F);
						}
#endif
						size = (int)(CV.size());/*登録すべき数を把握*/
						if(size){
							for(i=0;i<size;i++){
								for(j=0;j<CV[i].name_len;j++){
									Wdummy[j]= CV[i].name[j];/*メニューラベル*/
								}
								Wdummy[CV[i].name_len] = 0;
								AppendMenuW (hSubMenu0,MF_STRING,
									((long long int)(0x8000))+i,Wdummy);
							}
						}
						mbstowcs_s(&ret5,LABELName,200,dummy,200);
						AppendMenuW (hSubMenu0,MF_DISABLED|MF_SEPARATOR,
							IDM_SPARE,LABELName);/*区切り*/
						AppendMenuW (hSubMenu0,MF_STRING,IDM_SSAVE,
							L"SaveSendData");/*送信データセーブ*/
						mbstowcs_s(&ret5,LABELName,200,dummy,200);
						AppendMenuW (hSubMenu0,MF_DISABLED|MF_SEPARATOR,
							IDM_SPARE,LABELName);/*区切り*/
						AppendMenuW (hSubMenu0,MF_STRING,IDM_READ,
							L"ReadCMDFile");/*コマンドファイル変更*/
					}else{
						DErr = CommDlgExtendedError();
						_ltow_s(DErr,Wdummy,200,16);
						MessageBoxW(NULL,L"Err",Wdummy,MB_ICONHAND);
					}
					return 0;
				case	IDM_SSAVE:/*送信データセーブ*/
					if(send_save_flg){
						send_save_flg = 0;
        	       		CloseHandle(hSSF);   //ファイルを閉じる
						ModifyMenuW(hSubMenu0,(UINT)(CV.size()+1),
							MF_BYPOSITION|MF_STRING,
							IDM_SSAVE,L"SaveSendData");
							DrawMenuBar(hWnd);
					}else{
						hSSF = CreateFileW(L"SENDDATA.dat",/*ファイル名固定*/
						   	GENERIC_READ | GENERIC_WRITE,
							0, NULL, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL, NULL);
						if (hSSF == INVALID_HANDLE_VALUE){	//初期化失敗
							send_save_flg = 0;
					     	MessageBoxW(NULL,L"File Not Open",
								L"Error",MB_ICONHAND);
						}else{/*セーブ開始*/
							ModifyMenuW(hSubMenu0,(UINT)(CV.size()+1),
								MF_BYPOSITION|MF_STRING,
								IDM_SSAVE,L"NowSaving..");
							send_save_flg = 1;
							DrawMenuBar(hWnd);
						}
					}
					return 0;
				case	IDM_SAVE:/*受信データセーブ*/
					if(Save_flg){/*終了*/
						daytime_write(hSF);	//データ終端に日付書き込み
        	       		CloseHandle(hSF);   //ファイルを閉じる
						ModifyMenuW(hMenu,3,MF_BYPOSITION|MF_STRING,
							IDM_SAVE,L"SAVE");
						Save_flg = 0;
						DrawMenuBar(hWnd);
					}else{		/*開始*/
						ModifyMenuW(hMenu,3,MF_BYPOSITION|MF_STRING,
							IDM_SAVE,L"NowSaving..");
						Save_flg = 1;
						DrawMenuBar(hWnd);
						_itow_s(fkazu,Wdummy,20,10);
						wcscpy_s(WFilename,20,Wdummy);
						wcscat_s(WFilename,L".dat");
						while (_waccess_s(WFilename,00) == 0 ) {
										//同じファイル名があれば
        					fkazu ++;	//ファイル名の数字を増やす
							_itow_s(fkazu,WFilename,10,10);
							wcscat_s(WFilename,L".dat");
				        }
						hSF = CreateFileW(WFilename,
						   	GENERIC_READ | GENERIC_WRITE,
					        0, NULL, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL, NULL);
						if (hSF == INVALID_HANDLE_VALUE){	//初期化失敗
					     	MessageBoxW(NULL,L"File Not Open",
								L"Error",MB_ICONHAND);
						}
						daytime_write(hSF);	//データ先頭に日付書き込み
					}
					return 0;
				case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
			}
			if(hCom[0] != NULL){
				if((wmId >(0x8000-1))&&(wmId < (0x9000))){
					int i;
					DWORD	len,l;
					l = ((DWORD)(wParam)-0x8000);/*コマンド番号取得*/
					if(CV[l].flg == 0){
						for(i=0;i<CV[l].len;i++)SND.buf[i]= CV[l].data[i];
						SND.len = CV[l].len;
				
						EnterCriticalSection(&cscom);
						WriteFile(hCom[0],SND.buf,SND.len,&len,&COMOverlappedW[0]);
						LeaveCriticalSection(&cscom);
						if(CV[l].interval > 0){
							sendtimer.flg = 1;
							sendtimer.interval = CV[l].interval;
							sendtimer.num = l;
							sendtimer.local = 0;
						}else{
							sendtimer.flg = 0;
						}
						SendDataP(hogeWnd,SND.buf,SND.len);
						if(send_save_flg){/*送信データセーブ*/
						    WriteFile(hSSF,SND.buf,SND.len,&len,NULL);
						}
					}else{
						//----------------------------
					}
				}else if((wmId >(0x9000-1))&&(wmId < (0xA000))){
					DWORD l = ((DWORD)(wParam)-0x9000);/*コマンド番号取得*/
					char bkup_string[32];
					if(!(strcmp(COMMPARAM[0].comm_string,LPCSTR(PortList[l])))){
    		//	MessageBoxW(NULL,LPCSTR(PortList[l]),COMMPARAM.comm_string,MB_ICONHAND);
					}else{
						sprintf_s(bkup_string,32,COMMPARAM[0].comm_string);
						sprintf_s(COMMPARAM[0].comm_string,32,PortList[l]);
						CloseHandle(hCom[0]);
						init_commport(&COMMPARAM[0]);
						if(hCom[0]){
							COMMPARAM[0].Init = 1;
							for(i=0;i<strlen(COMMPARAM[0].comm_string);i++){
								if(COMMPARAM[0].comm_string[i] == 'C'){
									sprintf_s(PortText,18,COMMPARAM[0].comm_string+i);
								}
							}
							mbstowcs_s(&ret5,LABELName,200,PortText,200);
						ModifyMenuW(hMenu,0,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu1,LABELName);
							DrawMenuBar(hWnd);
						}else{
							sprintf_s(COMMPARAM[0].comm_string,32,bkup_string);
							if(hCom[0]==NULL)CloseHandle(hCom[0]);
							init_commport(&COMMPARAM[0]);
							if(hCom){
								COMMPARAM[0].Init = 1;
							}else{
								if(hCom[0]==NULL)CloseHandle(hCom[0]);
							ModifyMenuW(hMenu,0,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu1,L"COMX");
								DrawMenuBar(hWnd);
								COMMPARAM[0].Init = 0;
							}
						}
					}
				}else if (wmId >(0xA000-1)){
					DWORD l = ((DWORD)(wParam)-0xA000);/*コマンド番号取得*/
					switch(l){
						case 0://Text
							Display_flg = 0;
							frm_U[RtnN] = 0;
						ModifyMenuW(hMenu,5,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Txt");
							break;
						case 1://Hex
							Display_flg = 1;
							frm_U[RtnN] = 1;
						ModifyMenuW(hMenu,5,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Hex");
							break;
						case 2://Lua
							Display_flg = 2;
							frm_U[RtnN] = 2;
						ModifyMenuW(hMenu,5,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Lua");
							break;
						default:
							Display_flg = 0;
							frm_U[RtnN] = 0;
						ModifyMenuW(hMenu,5,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu2,L"Txt");
							break;
					}
				}
			}
			break;
		case WM_READ_END:/*データ受信*/
		{
			DWORD 			len,len2,lenp,lenq;
			DWORD           len_return;
			BOOL			rtnflg;
			BOOL			nflg;
			int ch;
			unsigned int k;
			char  CDummy[20] = {0};
			wmId    = LOWORD(wParam);
			
			rtnflg = FALSE;
		//	ch = wParam & 0x0f;
			ch = wmId & 0x0f;
			if(ch > 5)ch = 0;
			len = COMBUF[ch].len;
			len2 = COMBUF[ch].len2;

			if (len>0) {	//何かComから受け取ったなら
				if(ch >0){
					for(k=0;k<len2;k++){
						if(COMBUF[ch].Buf2[k] == ','){
							CDummy[0] = COMBUF[ch].Buf2[k+1];
							CDummy[1] = COMBUF[ch].Buf2[k+2];
							CDummy[2] = COMBUF[ch].Buf2[k+3];
							CDummy[3] = COMBUF[ch].Buf2[k+4];
							CDummy[4] = 0;
							if(!(strcmp(CDummy,"DCV "))){
								j = LM.serch("DCVFlg");
								if(j != -1){
									LM.LU[j].num = 1;
								}
								nflg = FALSE;
								for(j=k+4;j<COMMBUFSIZE;j++){
									if(nflg){
										if((COMBUF[ch].Buf2[j] == 0x0d)
										 ||(COMBUF[ch].Buf2[j] == 0x0a)){
										}else{
											COMBUF[ch].Buf2[j] = 0;
										}
									}
									if((COMBUF[ch].Buf2[j] == 0x0d)
									 ||(COMBUF[ch].Buf2[j] == 0x0a)){
										nflg = TRUE;
									}
								}
								i=len2;
								lua_getglobal(L, "Loop");
								lua_pcall(L, 0, 0, 0);
							}
						}
					}
					if((Debug_flg - 1) == ch){
						WriteFile(hSAVE1F,COMBUF[ch].Buf,len2,&len_return,NULL);
					}
					COMBUF[ch].len = 0;
				}else{
				//	lenq = len;
				//	if(len > 368){
				//		lenp = (len & 0x1fff) - 368;
				//		lenq = 368;
				//	}else{
				//		lenp = 0;
				//	}/*lenqは表示バイト、lenpはバッファ先頭からのシフト値*/
					if(Save_flg){
						WriteFile(hSF,COMBUF[0].Buf,len,&len_return,NULL);
					}
				//	COMBUF[ch].len -= len;
				//	if(COMBUF[ch].len > COMMBUFSIZE)COMBUF[ch].len = COMMBUFSIZE;
				//	if(COMBUF[ch].len){
				//		for(i=0;i<COMBUF[ch].len;i++){
				//			COMBUF[ch].Buf[i] = COMBUF[ch].Buf[len+i];
				//		}
				//	}
					COMBUF[ch].len = 0;
					COMBUF[ch].flg = 0;
					if(Display_flg == 0){
						DisplayData(hWnd,COMBUF[ch].Buf,len);
					}else if(Display_flg == 1){
					//	HexDataP(hWnd,COMBUF[ch].Buf+lenp,lenq);
						HexDataP(hWnd,COMBUF[ch].Buf,len);
					}else{
					
					}
					RtnCnt = 0;
				//	HexDataP(hWnd,COMBUF[ch].Buf+lenp,lenq);
				}
			}
		}
		break;
		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			HexDataREP(hWnd);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_CLOSE:
		{
			int i;
		//	DeleteObject(hNewFont);
			if(Save_flg)CloseHandle(hSF);   //ファイルを閉じる
			for(i=0;i<5;i++){
		    	if(hCom[i]!=0)CloseHandle(hCom[i]);
            	hCom[i] = NULL;
			}
		//	if(L != NULL){
		//		lua_close(L);
		//		L = NULL;
		//	}
			DestroyWindow (hWnd);
		}
		break;
		case WM_DESTROY:
		{
			int i;
			DeleteCriticalSection(&cscom);
        //	if(hCom[0]!=0)CloseHandle(hCom[0]);
		//	for(i=0;i<5;i++){
		//  	if(hCom[i]!=0)CloseHandle(hCom[i]);
		//  }
			PostQuitMessage (0);
		}
		break;
		case WM_CHAR:/*キーボードからの直接入力*/
		{
			DWORD	len;
			if(wParam > 31 && wParam < 127){
				if(key_outbuflen < 1024){/*改行までバッファに*/
					SND.buf[SND.len] = (unsigned char)wParam;
					SND.len++;
					sfrm_U[sRtnN] = 0;
					SendDataPK(hogeWnd,(unsigned char)(wParam));
				}
			}else if(wParam == 0x0a){
				if(key_outbuflen < 1024){/*改行までバッファに*/
					SND.buf[SND.len] = (unsigned char)wParam;
					SND.len++;
					sfrm_U[sRtnN] = 0;
					SendDataPK(hogeWnd,(unsigned char)(wParam));
				}
			}else if(wParam == 0x0d){/*改行コード*/
				if(key.lastparam == 0x0d){
				//	SND.len = key.baklen;
					SND.len = 0;
					sfrm_U[sRtnN] = 1;
				//	sfrm_U[sRtnN] = 0;
					SendDataP(hogeWnd,SND.buf,SND.len);
					if(COMMPARAM[0].newline == 0){
						SND.len-=1;
					}else if(COMMPARAM[0].newline == 1){
						SND.len-=2;
					}else if(COMMPARAM[0].newline == 2){
						SND.len-=1;
					}else if(COMMPARAM[0].newline == 3){
						SND.len-=1;
					}
				//	if(hCom[0] != NULL){
				//		if(key.lastparam == 0x0d){
				//			if(keywait_flg == 1){
				//				keywait_flg = 0;
				//			}
				//		}
				//		key.baklen = SND.len;
				//		SND.len = 0;
				//		key.spnflg = 0;
				//	}else{
				//		if(keywait_flg == 1){
				//			keywait_flg = 0;
				//		}
				//	}
				}else if(wParam == 0x1b){/*ESC*/
					i = LM.serch("Escape");
					if(i != -1){
						LM.LU[i].num = 1;
					}
				}else{
					sfrm_U[sRtnN] = 0;
					SendDataPK(hogeWnd,(unsigned char)(wParam));
				}
				key.lastparam = (int)(wParam);
				
				if(SND.len){/*改行で送信*/
					if(COMMPARAM[0].newline == 0){
						SND.buf[SND.len] = 0x0a;/*LF*/
						SND.len++;
					}else if(COMMPARAM[0].newline == 1){
						SND.buf[SND.len] = 0x0d;/*CRLF*/
						SND.len++;
						SND.buf[SND.len] = 0x0a;/*LF*/
						SND.len++;
					}else if(COMMPARAM[0].newline == 2){
						SND.buf[SND.len] = 0x0a;/*LF*/
						SND.len++;
					}else if(COMMPARAM[0].newline == 3){
						SND.buf[SND.len] = 0x0d;/*CR*/
						SND.len++;
					}
					EnterCriticalSection(&cscom);
					WriteFile(hCom[0],SND.buf,SND.len,&len,&COMOverlappedW[0]);
					LeaveCriticalSection(&cscom);
				}
				key.baklen = SND.len;
				SND.len = 0;
				key.spnflg = 0;
			}
			HexDataREP(hWnd);
		}
		break;
		case WM_TIMER:
		{
			switch(wParam){
				case	1:
				{
					int i;
					DWORD	len,l;
					if(Lua_flg == 1){
					//	if(BUSON_flg == 0){
						if(LM.serch("BUSON") != -1){
							if(REG[0].DATA != nullptr){
								if(REG[0].DATA[(LM.LU[(LM.serch("BUSON"))].addr)] == 0){
									lua_getglobal(L, "Loop");
									lua_pcall(L, 0, 0, 0);
								}else{
									BUSON_flg = 1;
							ModifyMenuW(hMenu,6,MF_BYPOSITION|MF_STRING,IDM_BUSON,L"BUS OFF");
								}
							}
							DrawMenuBar(hWnd);
						}
					}
					if(sendtimer.flg){/*インターバル送信用*/
						sendtimer.local++;
						if(sendtimer.interval == sendtimer.local){
							l = sendtimer.num;
							for(i=0;i<CV[l].len;i++)SND.buf[i]= CV[l].data[i];
							SND.len = CV[l].len;
							EnterCriticalSection(&cscom);
						WriteFile(hCom[0],SND.buf,SND.len,&len,&COMOverlappedW[0]);
							LeaveCriticalSection(&cscom);
							SendDataP(hogeWnd,SND.buf,SND.len);
							sendtimer.local = 0;
						}
					}
					if(hCom[0] != NULL){
						fSuccess = GetCommState(hCom[0],&dcb);
						if(! fSuccess){					//Comm1ステータス取得失敗
	//	MessageBoxW(NULL,L"Can't Get State of COM Port",COMD->comm_string,MB_ICONHAND);
							CloseHandle(hCom[0]);
							hCom[0] = 0;
							COMMPARAM[0].Init = 0;
							sprintf_s(PortText,18,"COMX");
							mbstowcs_s(&ret5,LABELName,200,PortText,200);
							ModifyMenuW(hMenu,0,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu1,LABELName);
							DrawMenuBar(hWnd);
						}
					}else{
						if((check_interval % 3) == 1){
							init_commport(&COMMPARAM[0]);
							if(hCom[0]){
								COMMPARAM[0].Init = 1;
								check_interval = 0;
								for(i=0;i<strlen(COMMPARAM[0].comm_string);i++){
									if(COMMPARAM[0].comm_string[i] == 'C'){
										sprintf_s(PortText,18,COMMPARAM[0].comm_string+i);
									}
								}
	   						 	mbstowcs_s(&ret5,LABELName,32,PortText,32);
								ModifyMenuW(hMenu,0,MF_BYPOSITION|MF_POPUP|MF_STRING,(UINT_PTR)hSubMenu1,LABELName);
								DrawMenuBar(hWnd);
							}else{
								COMMPARAM[0].Init = 0;
							}
						}
						if(COMMPARAM[0].Init == 0){
							check_interval++;
						}
					}
				}
			}
		}
		break;
#if 1
		case WM_SIZE:
		{
            RECT rect;
        	GetWindowRect(hWnd, &rect);
			WSIZE_WIDTH  = LOWORD(lParam);
			WSIZE_HEIGHT = HIWORD(lParam);
			if((WSIZE_WIDTH < 28)||(WSIZE_WIDTH > 1587)
			 ||(WSIZE_HEIGHT < 80)||(WSIZE_HEIGHT > 1849)){
				if(WSIZE_WIDTH < 28)WSIZE_WIDTH = 28;
				if(WSIZE_WIDTH > 1587)WSIZE_WIDTH = 1587;
				if(WSIZE_HEIGHT < 80)WSIZE_HEIGHT = 80;
				if(WSIZE_HEIGHT > 1849)WSIZE_HEIGHT = 1849;
			//	rect.right  = rect.left + WSIZE_WIDTH;
			//	rect.bottom = rect.top + WSIZE_HEIGHT;
				rect.right  = WSIZE_WIDTH;
				rect.bottom = WSIZE_HEIGHT;
				SetWindowPos(hWnd,HWND_TOP,rect.left,rect.top,rect.right,rect.bottom,0);
			}
        //	GetClientRect(hWnd, &rect);
        //  InvalidateRect(hWnd, NULL, TRUE);
            InvalidateRect(hWnd, &rect, TRUE);
		}
		break;
#endif
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラです。
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
//-------------ボーレート変更用ダイアログのプロシージャ--------------------
BOOL   CALLBACK dlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	char cbuf[100];
	WCHAR Wbuf[100];

	switch(msg){
		case WM_INITDIALOG:
			_ltow_s(dcb.BaudRate,Wbuf,100, 10);
		//	SetDlgItemText(hwndDlg,ID_TA,LPCWSTR(cbuf));
			SetDlgItemTextW(hwndDlg,ID_TA,Wbuf);
			break;
		case WM_COMMAND:
			switch(wParam){
				case	ID_OK:
					GetDlgItemTextW(hwndDlg,ID_TA,Wbuf,100);
					dcb.BaudRate = _wtol(Wbuf);
					SetCommState(hCom[0],&dcb);
					EndDialog(hwndDlg,FALSE);
					return TRUE;
				case	IDCANCEL:
					EndDialog(hwndDlg,FALSE);
					return TRUE;
			}
			break;
	}
	return FALSE;
}
//-------------コマンド用ダイアログのプロシージャ--------------------
BOOL   CALLBACK dlgProc2(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	WCHAR Wbuf[100];
	unsigned char getbuf[4];
//  DWORD	len;
	unsigned int inlen;
	int addr;

	SND.buf[0] = 0x00;
	SND.buf[1] = 0x00;
	SND.buf[2] = 0x00;
	SND.buf[3] = 0x00;
	Wbuf[0] = 0;

	switch(msg){
		case WM_INITDIALOG:
			wcscpy_s(Wbuf,L"0x0004");
			SetDlgItemTextW(hwndDlg,ID_TB,Wbuf);
			wcscpy_s(Wbuf,L"0xff");
			SetDlgItemTextW(hwndDlg,ID_TC,Wbuf);
			break;
		case WM_COMMAND:
			switch(wParam){
				case	ID_WRITE:
					if(REG.size() > 0){
						inlen = GetDlgItemTextW(hwndDlg,ID_TB,Wbuf,100);
						if(axtolW(Wbuf,getbuf,inlen)){
							addr = (getbuf[2] << 8| getbuf[3]) & 0xffff;
						}else{
							addr = 0x0000;	/*Addr = 0x0000*/
						}
						inlen = GetDlgItemTextW(hwndDlg,ID_TC,Wbuf,100);
						if(axtolW(Wbuf,getbuf,inlen)){
							REG[0].DATA[addr] = (getbuf[2] << 8| getbuf[3]) & 0xffff;
						}else{
							REG[0].DATA[addr] = 0xff;	/*Data = 0xff*/
						}
					}
					return TRUE;
#if 0
				case	ID_READ:
					inlen = GetDlgItemTextW(hwndDlg,ID_TB,Wbuf,100);
					if(axtolW(Wbuf,getbuf,inlen)){
						SND.buf[1] = getbuf[3];
					}else{
						SND.buf[1] = 0x04;	/*Addr = 0x04*/
					}
					inlen = GetDlgItemTextW(hwndDlg,ID_TC,Wbuf,100);
					if(axtolW(Wbuf,getbuf,inlen)){
						SND.buf[2] = getbuf[3];
					}else{
						SND.buf[2] = 0xff;	/*Data = 0xff*/
					}
					SND.buf[0] = 0x01;		/*READ*/
					SND.len = 4;
					EnterCriticalSection(&cscom);
				    WriteFile(hCom[0],SND.buf,SND.len,&len,&COMOverlappedW);
					LeaveCriticalSection(&cscom);
					SendDataP(hogeWnd,SND.buf,SND.len);
					if(send_save_flg){
					    WriteFile(hSSF,SND.buf,SND.len,&len,NULL);
					}
					EndDialog(hwndDlg,FALSE);
					return TRUE;
#endif
				case	IDCANCEL:
					EndDialog(hwndDlg,FALSE);
					return TRUE;
			}
			break;
	}
	return FALSE;
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc(LPVOID vdParam) {
	unsigned int i;
	DWORD RtnParam,EvParam,len,len2;
	LPTSTR lpMessageBuffer = NULL;

	while(1){
		//データ受信のイベントで待つ
		std::ignore =ReadFile(hCom[0],GetBufin[0],131072,&EvParam,&COMOverlapped[0]);
		/*ReadFileからのイベントオブジェクトシグナルを調べる*/
		RtnParam = WaitForSingleObject(COMOverlapped[0].hEvent,100);
		if(RtnParam == WAIT_OBJECT_0){
			GetOverlappedResult(hCom[0],&COMOverlapped[0],&len, FALSE);
			if(len > 0){
				len2 = len;
				if((COMBUF[0].len + len)<COMMBUFSIZE){
					COMBUF[0].len += len;
					len = COMMBUFSIZE - COMBUF[0].len;
				}else{
					COMBUF[0].len = COMMBUFSIZE;
				}
				for(i=0;i<len;i++)COMBUF[0].Buf[COMBUF[0].len + i] = GetBufin[0][i];
				if((COMBUF[0].len2 + len)<COMMBUFSIZE){
					COMBUF[0].len2 += len2;
					len2 = COMMBUFSIZE - COMBUF[0].len2;
				}else{
					COMBUF[0].len2 = COMMBUFSIZE;
				}
				for(i=0;i<len2;i++)COMBUF[0].Buf2[COMBUF[0].len2 + i] = GetBufin[0][i];
				COMBUF[0].flg++;
				PostMessage(hogeWnd,WM_READ_END,0,0);/*メインスレッドで処理*/
			}
		}else if (RtnParam == WAIT_FAILED) {
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				LANG_USER_DEFAULT,
				(LPTSTR)&lpMessageBuffer,
				0,
				NULL);
		//	MessageBox(NULL, lpMessageBuffer, "Hoge", MB_ICONHAND);
		}
		ResetEvent(COMOverlapped[0].hEvent);/*イベントは手動リセット*/
	}
	ExitThread(TRUE);
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc2(LPVOID vdParam) {
	unsigned int i;
//	BOOL ret;
	DWORD RtnParam,EvParam,len;
	LPTSTR lpMessageBuffer = NULL;

	while(1){
		//データ受信のイベントで待つ
		std::ignore = ReadFile(hCom[1],GetBufin[1],131072,&EvParam,&COMOverlapped[1]);
		/*ReadFileからのイベントオブジェクトシグナルを調べる*/
		RtnParam = WaitForSingleObject(COMOverlapped[1].hEvent,100);
		if(RtnParam == WAIT_OBJECT_0){
			GetOverlappedResult(hCom[1],&COMOverlapped[1],&len, FALSE);
			if(len > 0){
				for(i=0;i<len;i++)COMBUF[1].Buf[COMBUF[1].len + i] = GetBufin[1][i];
				for(i=0;i<len;i++)COMBUF[1].Buf2[COMBUF[0].len2 + i] = GetBufin[1][i];
				COMBUF[1].len += len;
				COMBUF[1].len2 += len;
				COMBUF[1].flg++;
				PostMessage(hogeWnd,WM_READ_END,1,0);/*メインスレッドで処理*/
			}
		}else if (RtnParam == WAIT_FAILED) {
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				LANG_USER_DEFAULT,
				(LPTSTR)&lpMessageBuffer,
				0,
				NULL);
		//	MessageBox(NULL, lpMessageBuffer, "Hoge", MB_ICONHAND);
		}
		ResetEvent(COMOverlapped[1].hEvent);/*イベントは手動リセット*/
	}
	ExitThread(TRUE);
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc3(LPVOID vdParam) {
	unsigned int i;
//	BOOL ret;
	DWORD RtnParam,EvParam,len;
	LPTSTR lpMessageBuffer = NULL;

	while(1){
		//データ受信のイベントで待つ
		std::ignore = ReadFile(hCom[2],GetBufin[2],131072,&EvParam,&COMOverlapped[2]);
		/*ReadFileからのイベントオブジェクトシグナルを調べる*/
		RtnParam = WaitForSingleObject(COMOverlapped[2].hEvent,100);
		if(RtnParam == WAIT_OBJECT_0){
			GetOverlappedResult(hCom[2],&COMOverlapped[2],&len, FALSE);
			if(len > 0){
				for(i=0;i<len;i++)COMBUF[2].Buf[COMBUF[2].len + i] = GetBufin[2][i];
				for(i=0;i<len;i++)COMBUF[2].Buf2[COMBUF[2].len2 + i] = GetBufin[2][i];
				COMBUF[2].len += len;
				COMBUF[2].len2 += len;
				COMBUF[2].flg++;
				PostMessage(hogeWnd,WM_READ_END,2,0);/*メインスレッドで処理*/
			}
		}else if (RtnParam == WAIT_FAILED) {
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				LANG_USER_DEFAULT,
				(LPTSTR)&lpMessageBuffer,
				0,
				NULL);
		//	MessageBox(NULL, lpMessageBuffer, "Hoge", MB_ICONHAND);
		}
		ResetEvent(COMOverlapped[2].hEvent);/*イベントは手動リセット*/
	}
	ExitThread(TRUE);
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc4(LPVOID vdParam) {
	unsigned int i;
//	BOOL ret;
	DWORD RtnParam,EvParam,len;
	LPTSTR lpMessageBuffer = NULL;

	while(1){
		//データ受信のイベントで待つ
		std::ignore = ReadFile(hCom[3],GetBufin[3],131072,&EvParam,&COMOverlapped[3]);
		/*ReadFileからのイベントオブジェクトシグナルを調べる*/
		RtnParam = WaitForSingleObject(COMOverlapped[3].hEvent,100);
		if(RtnParam == WAIT_OBJECT_0){
			GetOverlappedResult(hCom[3],&COMOverlapped[3],&len, FALSE);
			if(len > 0){
				for(i=0;i<len;i++)COMBUF[3].Buf[COMBUF[3].len + i] = GetBufin[3][i];
				for(i=0;i<len;i++)COMBUF[3].Buf2[COMBUF[3].len2 + i] = GetBufin[3][i];
				COMBUF[3].len += len;
				COMBUF[3].len2 += len;
				COMBUF[3].flg++;
				PostMessage(hogeWnd,WM_READ_END,3,0);/*メインスレッドで処理*/
			}
		}else if (RtnParam == WAIT_FAILED) {
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				LANG_USER_DEFAULT,
				(LPTSTR)&lpMessageBuffer,
				0,
				NULL);
		//	MessageBox(NULL, lpMessageBuffer, "Hoge", MB_ICONHAND);
		}
		ResetEvent(COMOverlapped[3].hEvent);/*イベントは手動リセット*/
	}
	ExitThread(TRUE);
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc5(LPVOID vdParam) {
	unsigned int i;
//	BOOL ret;
	DWORD RtnParam,EvParam,len;
	LPTSTR lpMessageBuffer = NULL;

	while(1){
		//データ受信のイベントで待つ
		std::ignore = ReadFile(hCom[4],GetBufin[4],131072,&EvParam,&COMOverlapped[4]);
		/*ReadFileからのイベントオブジェクトシグナルを調べる*/
		RtnParam = WaitForSingleObject(COMOverlapped[4].hEvent,100);
		if(RtnParam == WAIT_OBJECT_0){
			GetOverlappedResult(hCom[4],&COMOverlapped[4],&len, FALSE);
			if(len > 0){
				for(i=0;i<len;i++)COMBUF[4].Buf[COMBUF[4].len + i] = GetBufin[4][i];
				for(i=0;i<len;i++)COMBUF[4].Buf2[COMBUF[4].len2 + i] = GetBufin[4][i];
				COMBUF[4].len += len;
				COMBUF[4].len2 += len;
				COMBUF[4].flg++;
				PostMessage(hogeWnd,WM_READ_END,4,0);/*メインスレッドで処理*/
			}
		}else if (RtnParam == WAIT_FAILED) {
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				GetLastError(),
				LANG_USER_DEFAULT,
				(LPTSTR)&lpMessageBuffer,
				0,
				NULL);
		//	MessageBox(NULL, lpMessageBuffer, "Hoge", MB_ICONHAND);
		}
		ResetEvent(COMOverlapped[4].hEvent);/*イベントは手動リセット*/
	}
	ExitThread(TRUE);
}
/*----------------------------------------------------------------*/
DWORD WINAPI ThreadFunc6(LPVOID vdParam) {
	int len;
	
	Sleep(1000);
	NET[0].Connect();
	
	while(1){
		if(NET[0].IsActive()){
			len = NET[0].Recv((char *)(COMBUF[5].Buf+COMBUF[5].len));
			if(len > 0){
				if(len != -1){
					COMBUF[5].len += len;
					COMBUF[5].flg++;
					PostMessage(hogeWnd,WM_READ_END,10,0);
				/*メインスレッドで処理*/
				}
			}else if(len == 0){
				NET[0].Close();
				Sleep(10);
				NET[0].Connect();
			}
			Sleep(10);
		}else{
			NET[0].Close();
			Sleep(10);
		//	NET[0].init_client((u_short)HDR_CTRL_PORT,HDR_ip);
			NET[0].Connect();
			if(NET[0].IsActive()){
			//	WSAAsyncSelect(HDR_Cport.sc,hogeWnd,WSOCK_SELECT_CTM,
			//		FD_READ|FD_CLOSE);
			//	strcpy_s(dummy,"Connect Cortex TM port with ");
			//	inet_ntop(AF_INET,NET[0].RemoteAddr(),dummy2,sizeof(dummy2));
			//	strcat_s(dummy,100,dummy2);
			//	strcat_s(dummy,inet_ntoa(HDR_Cport.RemoteAddr()));
			//	DisplayData(hogeWnd,(unsigned char *)dummy,
			//		DWORD(strlen(dummy)));
			}
			Sleep(10);
		}
	}
}
