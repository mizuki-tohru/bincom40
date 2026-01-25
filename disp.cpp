//
//
#include "disp.h"
#define MODPOLY 0xA001

void scroll2(unsigned int);
//void scroll4(unsigned int);
void HexDataREP2(HWND);
//void HexDataREP4(HWND);
void DisplayParam2(HWND hWnd);
//void DisplayParam4(HWND hWnd);

int HDS2102S_getwaveform2(lua_State * );
int HDS2102S_getwavebmp2(lua_State * );
void HDS2102S_getwaveform3(char *);
int SerialInit(lua_State * );
int SerialSend(lua_State * );
int SerialWaitRecv(lua_State * );
int SerialRecv(lua_State * );
int SerialRecv2(lua_State * );
int Sleep(lua_State * );
int uSleep(lua_State * );
int mSleep(lua_State * );
int HIDSend(lua_State * );
int HIDSendS(lua_State * );
//int HIDSendS3(char *,unsigned char *);
int HIDSendS3(char *,char *);
int HIDRecv(lua_State * );
int HIDRecv2(lua_State * );
int HIDRecv3(char *,unsigned char *);
int NetInitCheck(lua_State * );
int NetConnect(lua_State * );
int NetDisConnect(lua_State * );
int NetSend(lua_State * );
int NetWaitRecv(lua_State *);
int NetRecv(lua_State * );
int NetRecv2(lua_State * );
int DeviceSeq(lua_State * );
int RegRead(lua_State * );
int RegWrite(lua_State * );
int CRC16(lua_State * );
int mkframe(lua_State * );
int KeyWait(lua_State * L);
int SetC(lua_State * L);
int GetC(lua_State * L);
int GetAddr(lua_State * );

struct rcv_data2 SND[6];
//struct comm_data2 	COMMPARAM;
//struct serial_param2 COMMPARAM;
//struct ComReadBuf3 COMBUF;
//struct dispframe2 CDSP2;
//vector<reg_struct> REG;
//map<int,char[32]> PortList;

vector<reg_main> REG;
struct EXBITMAPINFO BmInfo;
BITMAPFILEHEADER    BmFileHed;
unsigned char pBit[320*240];



unsigned char crc_ccitt16_LSB_tabH[256] = {
	0x00, 0x11, 0x23, 0x32, 0x46, 0x57, 0x65, 0x74,
	0x8c, 0x9d, 0xaf, 0xbe, 0xca, 0xdb, 0xe9, 0xf8,
	0x10, 0x01, 0x33, 0x22, 0x56, 0x47, 0x75, 0x64,
	0x9c, 0x8d, 0xbf, 0xae, 0xda, 0xcb, 0xf9, 0xe8,
	0x21, 0x30, 0x02, 0x13, 0x67, 0x76, 0x44, 0x55,
	0xad, 0xbc, 0x8e, 0x9f, 0xeb, 0xfa, 0xc8, 0xd9,
	0x31, 0x20, 0x12, 0x03, 0x77, 0x66, 0x54, 0x45,
	0xbd, 0xac, 0x9e, 0x8f, 0xfb, 0xea, 0xd8, 0xc9,
	0x42, 0x53, 0x61, 0x70, 0x04, 0x15, 0x27, 0x36,
	0xce, 0xdf, 0xed, 0xfc, 0x88, 0x99, 0xab, 0xba,
	0x52, 0x43, 0x71, 0x60, 0x14, 0x05, 0x37, 0x26,
	0xde, 0xcf, 0xfd, 0xec, 0x98, 0x89, 0xbb, 0xaa,
	0x63, 0x72, 0x40, 0x51, 0x25, 0x34, 0x06, 0x17,
	0xef, 0xfe, 0xcc, 0xdd, 0xa9, 0xb8, 0x8a, 0x9b,
	0x73, 0x62, 0x50, 0x41, 0x35, 0x24, 0x16, 0x07,
	0xff, 0xee, 0xdc, 0xcd, 0xb9, 0xa8, 0x9a, 0x8b,
	0x84, 0x95, 0xa7, 0xb6, 0xc2, 0xd3, 0xe1, 0xf0,
	0x08, 0x19, 0x2b, 0x3a, 0x4e, 0x5f, 0x6d, 0x7c,
	0x94, 0x85, 0xb7, 0xa6, 0xd2, 0xc3, 0xf1, 0xe0,
	0x18, 0x09, 0x3b, 0x2a, 0x5e, 0x4f, 0x7d, 0x6c,
	0xa5, 0xb4, 0x86, 0x97, 0xe3, 0xf2, 0xc0, 0xd1,
	0x29, 0x38, 0x0a, 0x1b, 0x6f, 0x7e, 0x4c, 0x5d,
	0xb5, 0xa4, 0x96, 0x87, 0xf3, 0xe2, 0xd0, 0xc1,
	0x39, 0x28, 0x1a, 0x0b, 0x7f, 0x6e, 0x5c, 0x4d,
	0xc6, 0xd7, 0xe5, 0xf4, 0x80, 0x91, 0xa3, 0xb2,
	0x4a, 0x5b, 0x69, 0x78, 0x0c, 0x1d, 0x2f, 0x3e,
	0xd6, 0xc7, 0xf5, 0xe4, 0x90, 0x81, 0xb3, 0xa2,
	0x5a, 0x4b, 0x79, 0x68, 0x1c, 0x0d, 0x3f, 0x2e,
	0xe7, 0xf6, 0xc4, 0xd5, 0xa1, 0xb0, 0x82, 0x93,
	0x6b, 0x7a, 0x48, 0x59, 0x2d, 0x3c, 0x0e, 0x1f,
	0xf7, 0xe6, 0xd4, 0xc5, 0xb1, 0xa0, 0x92, 0x83,
	0x7b, 0x6a, 0x58, 0x49, 0x3d, 0x2c, 0x1e, 0x0f
};

unsigned char crc_ccitt16_LSB_tabL[256] = {
	0x00, 0x89, 0x12, 0x9b, 0x24, 0xad, 0x36, 0xbf,
	0x48, 0xc1, 0x5a, 0xd3, 0x6c, 0xe5, 0x7e, 0xf7,
	0x81, 0x08, 0x93, 0x1a, 0xa5, 0x2c, 0xb7, 0x3e,
	0xc9, 0x40, 0xdb, 0x52, 0xed, 0x64, 0xff, 0x76,
	0x02, 0x8b, 0x10, 0x99, 0x26, 0xaf, 0x34, 0xbd,
	0x4a, 0xc3, 0x58, 0xd1, 0x6e, 0xe7, 0x7c, 0xf5,
	0x83, 0x0a, 0x91, 0x18, 0xa7, 0x2e, 0xb5, 0x3c,
	0xcb, 0x42, 0xd9, 0x50, 0xef, 0x66, 0xfd, 0x74,
	0x04, 0x8d, 0x16, 0x9f, 0x20, 0xa9, 0x32, 0xbb,
	0x4c, 0xc5, 0x5e, 0xd7, 0x68, 0xe1, 0x7a, 0xf3,
	0x85, 0x0c, 0x97, 0x1e, 0xa1, 0x28, 0xb3, 0x3a,
	0xcd, 0x44, 0xdf, 0x56, 0xe9, 0x60, 0xfb, 0x72,
	0x06, 0x8f, 0x14, 0x9d, 0x22, 0xab, 0x30, 0xb9,
	0x4e, 0xc7, 0x5c, 0xd5, 0x6a, 0xe3, 0x78, 0xf1,
	0x87, 0x0e, 0x95, 0x1c, 0xa3, 0x2a, 0xb1, 0x38,
	0xcf, 0x46, 0xdd, 0x54, 0xeb, 0x62, 0xf9, 0x70,
	0x08, 0x81, 0x1a, 0x93, 0x2c, 0xa5, 0x3e, 0xb7,
	0x40, 0xc9, 0x52, 0xdb, 0x64, 0xed, 0x76, 0xff,
	0x89, 0x00, 0x9b, 0x12, 0xad, 0x24, 0xbf, 0x36,
	0xc1, 0x48, 0xd3, 0x5a, 0xe5, 0x6c, 0xf7, 0x7e,
	0x0a, 0x83, 0x18, 0x91, 0x2e, 0xa7, 0x3c, 0xb5,
	0x42, 0xcb, 0x50, 0xd9, 0x66, 0xef, 0x74, 0xfd,
	0x8b, 0x02, 0x99, 0x10, 0xaf, 0x26, 0xbd, 0x34,
	0xc3, 0x4a, 0xd1, 0x58, 0xe7, 0x6e, 0xf5, 0x7c,
	0x0c, 0x85, 0x1e, 0x97, 0x28, 0xa1, 0x3a, 0xb3,
	0x44, 0xcd, 0x56, 0xdf, 0x60, 0xe9, 0x72, 0xfb,
	0x8d, 0x04, 0x9f, 0x16, 0xa9, 0x20, 0xbb, 0x32,
	0xc5, 0x4c, 0xd7, 0x5e, 0xe1, 0x68, 0xf3, 0x7a,
	0x0e, 0x87, 0x1c, 0x95, 0x2a, 0xa3, 0x38, 0xb1,
	0x46, 0xcf, 0x54, 0xdd, 0x62, 0xeb, 0x70, 0xf9,
	0x8f, 0x06, 0x9d, 0x14, 0xab, 0x22, 0xb9, 0x30,
	0xc7, 0x4e, 0xd5, 0x5c, 0xe3, 0x6a, 0xf1, 0x78
};


const static struct ScaleS Scalet[] = {{"10.0mV",0.01},{"20.0mV",0.02},{"50.0mV",0.05},{"100mV",0.1},{"200mV" ,0.2},
	{"500mV" ,0.5},{"1.00V" ,1.0},{"2.00V" ,2.0},{"5.00V" ,5.0},{"10.0V" ,10.0},
	{"20.0V" ,20.0},{"50.0V" ,50.0},{"100V"  ,100.0},{"200V"  ,200.0},{"500V"  ,500.0}};

struct DataView {
//	double * DATA;
	double scale;
	double shift;
	int len;
};

void bmpparam(void)
{
	int i;
    ZeroMemory((PVOID)&BmInfo,sizeof(struct EXBITMAPINFO));
    BmFileHed.bfType=0x4D42;
    BmFileHed.bfSize=14+40+(256*4)+(320*240);
    BmFileHed.bfReserved1=0;
    BmFileHed.bfReserved2=0;
    BmFileHed.bfOffBits=14+40+(256*4);//310
    BmInfo.BmInfoHed.biSize=sizeof(BITMAPINFOHEADER);
    BmInfo.BmInfoHed.biWidth		= 320;		// DIB横幅
    BmInfo.BmInfoHed.biHeight		= 240;
    BmInfo.BmInfoHed.biPlanes		= 1;			// プレーンの数(基本は1)
    BmInfo.BmInfoHed.biBitCount		= 8;			// プレーンの色数
    BmInfo.BmInfoHed.biCompression	= BI_RGB;
    BmInfo.BmInfoHed.biSizeImage	= 0;
    BmInfo.BmInfoHed.biXPelsPerMeter= 0;
    BmInfo.BmInfoHed.biYPelsPerMeter= 0;
    BmInfo.BmInfoHed.biClrUsed      = 0;
    BmInfo.BmInfoHed.biClrImportant = 0;
	//---カラーパレット設定---
	// 0 :00,00,00 BLACK
	// 5 :ff,00,00 RED
	// 30:00,ff,00 GREEN
	// 35:ff,ff,00 YELLOW
	//180:00,00,ff BLUE
	//185:ff,00,ff MAZENDA
	//210:00,ff,ff CYAN
	//215:ff,ff,ff WHITE
    for (i=0;i<216;i++){
        BmInfo.bmiColors[i].rgbRed   = (0x33 *      (i%6))&0xff;
        BmInfo.bmiColors[i].rgbGreen = (0x33 * ((i/ 6)%6))&0xff;
     	BmInfo.bmiColors[i].rgbBlue  = (0x33 * ((i/36)%6))&0xff;
    }
    for (i=216;i<256;i++){
        BmInfo.bmiColors[i].rgbRed   = 0xff;
        BmInfo.bmiColors[i].rgbGreen = 0xff;
     	BmInfo.bmiColors[i].rgbBlue  = 0xff;
    }
}

void fillblack(void)
{
	int i,j;
	for(i=0;i<240;i++){
		for(j=0;j<320;j++){
			pBit[i*320+j] = 0;//Black
		}
	}
}

void bmpgraph(void)
{
	int i,j;
	//300/12=25
	//200/8=25
	for(j=0;j<7;j++){
		for(i=0;i<300;i++){
			if(i%5 == 0){
			//	pBit[((25+(25*j))*320)+10+i] = 210;//CYAN
				pBit[((20+25+(25*j))*320)+10+i] = 210;//CYAN
			}
		}
	}
	for(j=0;j<11;j++){
		for(i=0;i<200;i++){
			if(i%5 == 0){
				pBit[((20+i)*320)+10+25+(j*25)] = 210;//CYAN
			}
		}
	}
	
	for(i=0;i<300;i++){
		pBit[( 20*320)+10+i] = 215;//White
		pBit[(220*320)+10+i] = 215;//White
	}
	for(i=0;i<200;i++){
		pBit[((20+i)*320)+ 10] = 215;//White
		pBit[((20+i)*320)+310] = 215;//White
	}
}

void bmpdraw(double * DATA1,struct DataView DV1, double * DATA2, struct DataView DV2)
{
	int i,n,m;
	double scale1,scale2;
	double shift1,shift2;
	double ch1,ch2;

	scale1 = (DV1.scale/2.0); //1.0V DV.scale=5.0V 5.0/25 * 
	scale2 = (DV2.scale/2.0); //1.0V DV.scale=5.0V 5.0/25 * 
	shift1 = (120 + DV1.shift);         //0 
	shift2 = (120 + DV2.shift);         //-50 

	bmpgraph();
//	for(i=0;i<300;i++){
	for(i=0;i<DV1.len;i++){
		ch1 = ((scale1*DATA1[i])*25);  //((5.0/25)*5.0)*25=25 
		if((shift1+ch1) > 220)ch1 = 220 - shift1;
		if((shift1+ch1) <  20)ch1 = 20  - shift1;
		ch2 = ((scale2*DATA2[i])*25);
		if((shift2+ch2) > 220)ch2 =  220 - shift2;
		if((shift2+ch2) <  20)ch2 =  20  - shift2;
		pBit[((int)(shift1+ch1)*320)+10+i] = 35;//YEROW
		pBit[((int)(shift2+ch2)*320)+10+i] = 180;//BLUE
	}
	for(n=0;n<8;n++){
		for(m=0;m<8;m++){
			if(((MFont[96][n] >> m) & 0x01) == 1){
			//	pBit[(320*(239 - (int)shift1 - n +4))-8+(8-m)] = 35;//YEROW;
			//	pBit[(320*(239 - (int)shift2 - n +4))-8+(8-m)] = 180;//BLUE;
				pBit[(320*((int)shift1 - n +4))+(8-m)] = 35;//YEROW;
				pBit[(320*((int)shift2 - n +4))+(8-m)] = 180;//BLUE;
			}
		}
	}
}

/*******************************************************************************/
void bmpdrawTRG(int ch, int level)
{
	int n,m;

	for(n=0;n<8;n++){
		for(m=0;m<8;m++){
			if(((MFont[97][n] >> m) & 0x01) == 1){
				if(ch == 1){
					pBit[(320*(239 - level - n +4))+307+(8-m)] = 35;//YEROW;
				}else{
					pBit[(320*(239 - level - n +4))+307+(8-m)] = 180;//BLUE;
				}
			}
		}
	}
}
/*******************************************************************************/
int bmpprint(std::string text,int x,int y)
{
	int j,m,n;
	int r = 0;
	int i = (int)text.length();
	if((x > 8)&&(y < (240-8))){
		for(j=0;j<i;j++){
			for(n=0;n<8;n++){
				for(m=0;m<8;m++){
					if(((MFont[(char(text[j]) - ' ')][n] >> m) & 0x01) == 1){
				//	if(((MFont[16][n] >> m) & 0x01) == 1){
						pBit[(320*(239 - y - n))+x-8+(j*8)+(8-m)] = 215;
					}
				}
			}
		}
		r = 1;
	}else{
		r = 0;
	}
	return r;
}

int HDS2102S_getwaveform2(lua_State * L)
{
	int len,len2,i,j;
	double k;
	int r = 0;
	double scale1 = 0.0;
	double scale2 = 0.0;
	double offset1 = 0.0;
	double offset2 = 0.0;
	char Buf[1030];
	char Buf2[1030];
	char dummy[40];
	size_t sp1,sp2,sp3,sp4,sp5,sp6,sp7,sp8;
//	size_t len2;
	errno_t error;
	FILE * F;

//	char * filename = (char *)lua_tolstring(L, 1,&len2);
	char * filename = (char *)lua_tostring(L, 1);
	error = fopen_s(&F,filename,"wt");
	if(F != NULL){
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		Buf[0] = 0;
		HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:HEAD?");
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		len2 = Buf[0]|(Buf[1]<<8);
		strcpy_s(Buf2,1024,Buf+5);
		len2 = len2 - len;

		while(len2 > 0){
			Sleep(1);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			strcat_s(Buf2,1024,Buf);
			len2 = len2 - len;
		}
		
		if(strlen(Buf2)!=0){
			std::string SBuf2 = Buf2;
			sp1 = SBuf2.find("SCALE");
			sp2 = sp1 + 5;
			std::string SBuf0 = SBuf2.substr(sp2+2);
			sp1 = SBuf0.find("SCALE");
			sp2 = sp1 + 5;
			std::string SBuf3 = SBuf0.substr(sp2+2);
			sp3 = SBuf3.find("SCALE");
			sp4 = sp3 + 5;
			std::string SBuf4  = SBuf0.substr(sp2+2);
			std::string SBuf6;
			std::string SBuf9;
			std::string SBuf10 = SBuf0.substr(sp2+4);
			SBuf10 = SBuf10.erase(sp2+10);
			std::string SBuf11 = SBuf3.substr(sp4+4);
			SBuf11 = SBuf11.erase(sp4+10);
			std::string SBuf12;
			std::string SBuf13;

			for(i=0;i<7;i++){
				if(SBuf10[i] == 34){//0x22 "
					SBuf12 = SBuf10.erase(i);
					break;
				}
			}
			for(i=0;i<7;i++){
				if(SBuf11[i] == 34){
					SBuf13 = SBuf11.erase(i);
					break;
				}
			}

			for(i=0;i<15;i++){
				sp1 = SBuf12.find(Scalet[i].name);
				if(sp1 != string::npos){
					scale1 = Scalet[i].scale;
					break;
				}
			}
			for(i=0;i<15;i++){
				sp1 = SBuf13.find(Scalet[i].name);
				if(sp1 != string::npos){
					scale2 = Scalet[i].scale;
					break;
				}
			}

			sp5 = SBuf4.find("OFFSET");
			sp6 = sp5 + 6;
			std::string SBuf5 = SBuf4.substr(sp6+2);
		//	std::string SBuf5 = SBuf0.substr(sp6+3,sp6+6);
		//	SBuf5 = SBuf5.erase(sp6+6);
			for(i=0;i<10;i++){
			//	if(SBuf5[i] == 44){
				if(SBuf5[i] == ','){//0x22 "
				//	SBuf6 = SBuf5.erase(i);
					SBuf6 = SBuf5.substr(0,i);//OFFSET:0
					break;
				}
			}
			offset1 = std::stof(SBuf6);

			std::string SBuf7 = SBuf4.substr(sp6+3,SBuf4.size());
			sp7 = SBuf7.find("OFFSET");
			sp8 = sp7 + 6;
		//	std::string SBuf8 = SBuf7.substr(sp8+3,sp8+6);
			std::string SBuf8 = SBuf7.substr(sp8+2);
		//	SBuf8 = SBuf8.erase(sp8+6);
			for(i=0;i<10;i++){
			//	if(SBuf8[i] == 44){
				if(SBuf8[i] == ','){
				//	SBuf9 = SBuf8.erase(i);
					SBuf9 = SBuf8.substr(0,i);//OFFSET:0
					break;
				}
			}
			offset2 = std::stof(SBuf9);
			
			SBuf2 = SBuf2 + '\n';
			fwrite(SBuf2.c_str(), 1, strlen(SBuf2.c_str()), F);

			HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH1?");
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);

			len2 = Buf[0]|(((int)Buf[1])<<8);
			for(i=4;i<(len-2);i+=2){
				j = (((int)Buf[i+1])<<8)|Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset1/25)) * scale1;
				sprintf_s(dummy,40, "%f,",k);
			//	ftoa(dummy,j,10);
			//	dummy[strlen(dummy)] = ',';
			//	dummy[strlen(dummy)+1] = 0;
				fwrite(dummy, 1, strlen(dummy), F);
			}

			len2 = (len2 - len);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			while(len2 > 0){
				for(i=0;i<(len-1);i+=2){
					j = (((int)Buf[i+1])<<8)|Buf[i];
					if(j > 32767){
						j = -(65535 - j);
					}
					k = (((double)j / 6400) - (offset1/25)) * scale1;
				//	ftoa(dummy,j,10);
					sprintf_s(dummy,40, "%f,",k);
				//	dummy[strlen(dummy)] = ',';
				//	dummy[strlen(dummy)+1] = 0;
					fwrite(dummy, 1, strlen(dummy), F);
				}
				Sleep(1);
				len2 = (len2 - len);
				len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			}

			fwrite("\n", 1, 1, F);

			HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH2?");
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			len2 = Buf[0]|(((int)Buf[1])<<8);
			for(i=4;i<(len-2);i+=2){
				j = (((int)Buf[i+1])<<8)|Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset2/25)) * scale2;
				sprintf_s(dummy,40, "%f,",k);
			//	ftoa(dummy,j,10);
			//	dummy[strlen(dummy)] = ',';
			//	dummy[strlen(dummy)+1] = 0;
				fwrite(dummy, 1, strlen(dummy), F);
			}

			len2 = (len2 - len);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			while (len2 > 0){
				for(i=0;i<(len-1);i+=2){
					j = (((int)Buf[i+1])<<8)|Buf[i];
					if(j > 32767){
						j = -(65535 - j);
					}
					k = (((double)j / 6400) - (offset2/25)) * scale2;
					sprintf_s(dummy,40, "%f,",k);
				//	ftoa(dummy,j,10);
				//	dummy[strlen(dummy)] = ',';
				//	dummy[strlen(dummy)+1] = 0;
					fwrite(dummy, 1, strlen(dummy), F);
				}
				Sleep(1);
				len2 = len2 - len;
				len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			}

			fwrite("\n", 1, 1, F);
			fclose(F);
		}else{
			r = 1;
		}
	}else{
		r = 1;
	}
	//	DispStrP("WaveformData " .. datetime)
	lua_pop(L, lua_gettop(L));
   	lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int HDS2102S_getwavebmp2(lua_State * L)
{
	int len,len2,i,ch,level;
	long j;
	int n = 0;
	int m = 0;
	double k;
	int r = 0;
	double scale1 = 0.0;
	double scale2 = 0.0;
	double offset1 = 0.0;
	double offset2 = 0.0;
	double probe1 = 0.0;
	double probe2 = 0.0;
	double tlevel = 0.0;
	char Buf[2060];
	char Buf2[2060];
	char tmp1[30];
	char tmp2[30];
	double DATA1[800];
	double DATA2[800];
	size_t sp1,sp2,sp7,sp8;
	errno_t error;
	FILE * F;
	struct DataView DV1;
	struct DataView DV2;

	char * filename = (char *)lua_tostring(L, 1);
	error = fopen_s(&F,filename,"wb");
	if(F != NULL){
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		Buf[0] = 0;
		Sleep(1);
		HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:HEAD?");
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		len2 = Buf[0]|(Buf[1]<<8);
		strcpy_s(Buf2,1024,Buf+5);
		len2 = len2 - len;

		while(len2 > 0){
			Sleep(1);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			strcat_s(Buf2,1024,Buf);
			len2 = len2 - len;
		}
		if(strlen(Buf2)!=0){
			std::string SBuf2 = Buf2;
			sp1 = SBuf2.find("SCALE");
			sp2 = sp1 + 5;
			std::string SBuf0 = SBuf2.substr(sp2+3);
			std::string TSCALE;
			for(i=0;i<7;i++){
				if(SBuf0[i] == 34){//0x22 "
					TSCALE = SBuf0.substr(0,i);//SCALE:10uS
					break;
				}
			}
			sp1 = SBuf2.find("SAMPLERATE");
			sp2 = sp1 + 10;
			std::string SBuf15 = SBuf2.substr(sp2+3);
			std::string SRATE;
			for(i=0;i<10;i++){
				if(SBuf15[i] == 34){//0x22 "
					SRATE = SBuf15.substr(0,i);//SAMPLERATE:25MSa/s
					break;
				}
			}
			sp1 = SBuf15.find("PROBE");
			sp2 = sp1 + 5;
			std::string SBuf17 = SBuf15.substr(sp2+3);
			std::string SBuf18;
			for(i=0;i<10;i++){
			//	if(SBuf17[i] == 34){//0x22 "
				if(SBuf17[i] == 'X'){//0x22 "
					SBuf18 = SBuf17.substr(0,i);//PROBE:10X
					break;
				}
			}
			probe1 = std::stof(SBuf18);//---------------------------

			sp1 = SBuf15.find("SCALE");
			sp2 = sp1 + 5;
			std::string SBuf19 = SBuf15.substr(sp2+3);
			std::string SBuf20;
			for(i=0;i<7;i++){
				if(SBuf19[i] == 34){//0x22 "
					SBuf20 = SBuf19.substr(0,i);//SCALE:500mV
					break;
				}
			}
			for(i=0;i<15;i++){
				sp1 = SBuf20.find(Scalet[i].name);
				if(sp1 != string::npos){
					scale1 = Scalet[i].scale;
					break;
				}
			}
			
			sp1 = SBuf15.find("OFFSET");
			sp2 = sp1 + 6;
			std::string SBuf21 = SBuf15.substr(sp2+2);
			std::string SBuf22;
			for(i=0;i<10;i++){
				if(SBuf21[i] == ','){//0x22 "
					SBuf22 = SBuf21.substr(0,i);//OFFSET:0
					break;
				}
			}
			offset1 = std::stof(SBuf22);
			
			sp1 = SBuf19.find("PROBE");
			sp2 = sp1 + 5;
			std::string SBuf23 = SBuf19.substr(sp2+3);
			std::string SBuf24;
			for(i=0;i<10;i++){
			//	if(SBuf23[i] == 34){//0x22 "
				if(SBuf23[i] == 'X'){//0x22 "
					SBuf24 = SBuf23.substr(0,i);//PROBE:10X
					break;
				}
			}
			probe2 = std::stof(SBuf24);
			
			sp1 = SBuf23.find("SCALE");
			sp2 = sp1 + 5;
			std::string SBuf25 = SBuf23.substr(sp2+3);
			std::string SBuf26;
			for(i=0;i<7;i++){
				if(SBuf25[i] == 34){//0x22 "
					SBuf26 = SBuf25.substr(0,i);//SCALE:500mV
					break;
				}
			}
			for(i=0;i<15;i++){
				sp1 = SBuf26.find(Scalet[i].name);
				if(sp1 != string::npos){
					scale2 = Scalet[i].scale;
					break;
				}
			}
			sp7 = SBuf23.find("OFFSET");
			sp8 = sp7 + 6;
			std::string SBuf27 = SBuf23.substr(sp8+2);
			std::string SBuf28;
			for(i=0;i<10;i++){
				if(SBuf27[i] == ','){
					SBuf28 = SBuf27.substr(0,i);//OFFSET:0
					break;
				}
			}
			offset2 = std::stof(SBuf28);
			sp7 = SBuf27.find("Channel");
			sp8 = sp7 + 7;
			std::string SBuf29 = SBuf27.substr(sp8+3);
			std::string TCH;
			for(i=0;i<7;i++){
				if(SBuf29[i] == 34){
					TCH = SBuf29.substr(0,i);//Chanel:CH1
					break;
				}
			}
			
			sp7 = SBuf27.find("Level");
			sp8 = sp7 + 5;
			std::string SBuf30 = SBuf27.substr(sp8+3);
			std::string TLV;
			for(i=0;i<7;i++){
				if(SBuf30[i] == 34){
					TLV = SBuf30.substr(0,i);//Level:0.00mV
					break;
				}
			}
			
			sp7 = TLV.find("uV");
			std::string TLVM;
			if(sp7 == string::npos){
				sp7 = TLV.find("mV");
				if(sp7 == string::npos){
					sp7 = TLV.find("V");
					if(sp7 == string::npos){
						
					}else{
						for(i=0;i<7;i++){
							if(TLV[i] == 'V'){
								TLVM = TLV.substr(0,i);//Level:0.00V
								break;
							}
						}
						tlevel = std::stof(TLVM);
					}
				}else{
					for(i=0;i<7;i++){
						if(TLV[i] == 'm'){
							TLVM = TLV.substr(0,i);//Level:0.00mV
							break;
						}
					}
					tlevel = std::stof(TLVM);
					tlevel = tlevel / 1000;
				}
			}else{
				for(i=0;i<7;i++){
					if(TLV[i] == 'u'){
						TLVM = TLV.substr(0,i);//Level:0.00uV
						break;
					}
				}
				tlevel = std::stof(TLVM);
				tlevel = tlevel / 1000000;
			}
			
			
			sp1 = SBuf27.find("RISE");
			std::string TED;
			if(sp1 != string::npos){
				TED = "/";
			}else{
				TED = "\\";
			}
			sp7 = SBuf27.find("Coupling");
			sp8 = sp7 + 8;
			std::string SBuf31 = SBuf27.substr(sp8+3);
			std::string TCP;
			for(i=0;i<7;i++){
				if(SBuf31[i] == 34){
					TCP = SBuf31.substr(0,i);//Coupling:DC
					break;
				}
			}
		//	TCH = TCH + ':' + TCP;
			
			sp7 = SBuf27.find("Sweep");
			sp8 = sp7 + 5;
			std::string SBuf32 = SBuf27.substr(sp8+3);
			std::string TSW;
			for(i=0;i<7;i++){
				if(SBuf32[i] == 34){
					TSW = SBuf32.substr(0,i);//Sweep:AUTO
					break;
				}
			}

			HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH1?");
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			n = 0;
			len2 = Buf[0]|(((int)Buf[1])<<8);
			for(i=4;i<(len-2);i+=2){
			//	j = (Buf[i+2]<<8)|Buf[i+1];
				j = (((int)Buf[i+1])<<8)+Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset1/25)) * scale1;
				DATA1[n] = k;
				if(n<800)n++;
			}

			len2 = (len2 - len);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			while(len2 > 0){
				for(i=0;i<(len-1);i+=2){
					j = (((int)Buf[i+1])<<8)+Buf[i];
					if(j > 32767){
						j = -(65535 - j);
					}
					k = (((double)j / 6400) - (offset1/25)) * scale1;
					DATA1[n] = k;
					if(n<800)n++;
				}
				Sleep(1);
				len2 = (len2 - len);
				len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			}

			HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH2?");
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			len2 = Buf[0]|(((int)Buf[1])<<8);
			m = 0;
			for(i=4;i<(len-2);i+=2){
				j = ((int)(Buf[i+1])<<8)+Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset2/25)) * scale2;
				DATA2[m] = k;
				if(m<800)m++;
			}

			len2 = (len2 - len);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			while (len2 > 0){
				for(i=0;i<(len-1);i+=2){
					j = (((int)Buf[i+1])<<8)+Buf[i];
					if(j > 32767){
						j = -(65535 - j);
					}
					k = (((double)j / 6400) - (offset2/25)) * scale2;
					DATA2[m] = k;
					if(m<800)m++;
				}
				Sleep(1);
				len2 = len2 - len;
				len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			}
	
			bmpparam();
			fwrite(&BmFileHed,14,1, F);
			fwrite(&BmInfo,40+(256*4),1, F);
			fillblack();

			scale1 = scale1 * probe1;
			scale2 = scale2 * probe2;

			DV1.scale = scale1;
			DV1.shift = offset1;
			DV1.len = n;
			DV2.scale = scale2;
			DV2.shift = offset2;
			DV2.len = m;

			sprintf_s(tmp1,30,"1:%.2fV",scale1);
			sprintf_s(tmp2,30,"2:%.2fV",scale2);
			
			if(TCH == "CH1"){
				ch = 1;
				level = (int)(120+offset1+((tlevel/scale1)*25));
			}else{
				ch = 2;
				level = (int)(120+offset1+((tlevel/scale2)*25));
			}
			bmpdrawTRG(ch, level);
			bmpdraw(DATA1,DV1,DATA2,DV2);
			
			bmpprint(std::string(tmp1),10,225);
			bmpprint(std::string(tmp2),75,225);
			
			bmpprint(TSW,10,5);
			bmpprint(TSCALE,50,5);
			bmpprint(SRATE,140,225);
		//	bmpprint(TCH,215,225);
			bmpprint(TCH + ':' + TCP,215,225);
			bmpprint(TED,265,225);
			bmpprint(TLV,275,225);
			
			fwrite(&pBit,(320*240), 1, F);
		}else{
			r = 1;
		}
		fclose(F);
	}else{
		r = 1;
	}
	//	DispStrP("WaveformData " .. datetime)
	lua_pop(L, lua_gettop(L));
   	lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

void HDS2102S_getwaveform3(char * filename)
{
	int len,len2,i,j;
	double k;
	double scale1 = 0.0;
	double scale2 = 0.0;
	double offset1 = 0.0;
	double offset2 = 0.0;
	char Buf[1030];
	char Buf2[1030];
	char dummy[40];
	size_t sp1,sp2,sp3,sp4,sp5,sp6,sp7,sp8;
	errno_t error;
	FILE * F;

	error = fopen_s(&F,filename,"wt");
	if(F != NULL){
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		Buf[0] = 0;
		Sleep(1);
		HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:HEAD?");
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		len2 = Buf[0]|(((int)Buf[1])<<8);
		strcpy_s(Buf2,1024,Buf+5);
		len2 = len2 - len;
		while(len2 > 0){
			Sleep(1);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
			strcat_s(Buf2,1024,Buf);
			len2 = len2 - len;
		}

		std::string SBuf2 = Buf2;
		sp1 = SBuf2.find("SCALE");
		sp2 = sp1 + 5;
		std::string SBuf0 = SBuf2.substr(sp2+3);
		sp1 = SBuf0.find("SCALE");
		sp2 = sp1 + 5;
		std::string SBuf3 = SBuf0.substr(sp2+3);
		sp3 = SBuf3.find("SCALE");
		sp4 = sp3 + 5;
		std::string SBuf4  = SBuf0.substr(sp2+3);
		std::string SBuf6;
		std::string SBuf9;
		std::string SBuf10 = SBuf0.substr(sp2+4);
		SBuf10 = SBuf10.erase(sp2+10);
		std::string SBuf11 = SBuf3.substr(sp4+4);
		SBuf11 = SBuf11.erase(sp4+10);
		std::string SBuf12;
		std::string SBuf13;

		for(i=0;i<7;i++){
			if(SBuf10[i] == 34){
			//	SBuf12 = SBuf10.erase(i);
				SBuf12 = SBuf10.substr(0,i);
				break;
			}
		}
		for(i=0;i<7;i++){
			if(SBuf11[i] == 34){
			//	SBuf13 = SBuf11.erase(i);
				SBuf13 = SBuf11.substr(0,i);
				break;
			}
		}

		for(i=0;i<15;i++){
			sp1 = SBuf12.find(Scalet[i].name);
			if(sp1 != string::npos){
				scale1 = Scalet[i].scale;
				break;
			}
		}
		for(i=0;i<15;i++){
			sp1 = SBuf13.find(Scalet[i].name);
			if(sp1 != string::npos){
				scale2 = Scalet[i].scale;
				break;
			}
		}

		sp5 = SBuf4.find("OFFSET");
		sp6 = sp5 + 6;
	//	std::string SBuf5 = SBuf0.substr(sp6+3,sp6+6);
		std::string SBuf5 = SBuf0.substr(sp6+2);
	//	SBuf5 = SBuf5.erase(sp6+6);
		for(i=0;i<10;i++){
		//	if(SBuf5[i] == 44){
			if(SBuf5[i] == ','){
			//	SBuf6 = SBuf5.erase(i);
				SBuf6 = SBuf5.substr(0,i);
				break;
			}
		}
		offset1 = std::stof(SBuf6);
		
		std::string SBuf7 = SBuf4.substr(sp6+3,SBuf4.size());
		sp7 = SBuf7.find("OFFSET");
		sp8 = sp7 + 6;
	//	std::string SBuf8 = SBuf7.substr(sp8+3,sp8+6);
		std::string SBuf8 = SBuf7.substr(sp8+2);
	//	SBuf8 = SBuf8.erase(sp8+6);
		for(i=0;i<10;i++){
		//	if(SBuf8[i] == 44){
			if(SBuf8[i] == ','){
			//	SBuf9 = SBuf8.erase(i);
				SBuf9 = SBuf8.substr(0,i);
				break;
			}
		}
		offset2 = std::stof(SBuf9);

		SBuf2 = SBuf2 + '\n';
		fwrite(SBuf2.c_str(), 1, strlen(SBuf2.c_str()), F);

		HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH1?");
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);

		len2 = Buf[0]|(((int)Buf[1])<<8);
		for(i=4;i<(len-2);i+=2){
			j = (((int)Buf[i+1])<<8)|Buf[i];
			if(j > 32767){
				j = -(65535 - j);
			}
			k = (((double)j / 6400) - (offset1/25)) * scale1;
			sprintf_s(dummy,40, "%f,",k);
		//	ftoa(dummy,j,10);
		//	dummy[strlen(dummy)] = ',';
		//	dummy[strlen(dummy)+1] = 0;
			fwrite(dummy, 1, strlen(dummy), F);
		}

		len2 = (len2 - len);
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		while(len2 > 0){
			for(i=0;i<(len-1);i+=2){
				j = (((int)Buf[i+1])<<8)|Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset1/25)) * scale1;
			//	ftoa(dummy,j,10);
				sprintf_s(dummy,40, "%f,",k);
			//	dummy[strlen(dummy)] = ',';
			//	dummy[strlen(dummy)+1] = 0;
				fwrite(dummy, 1, strlen(dummy), F);
			}
			Sleep(1);
			len2 = (len2 - len);
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		}

		fwrite("\n", 1, 1, F);

		HIDSendS3((char *)"HDS2102S_1",(char *)":DATa:WAVe:SCReen:CH2?");
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		len2 = Buf[0]|(((int)Buf[1])<<8);
		for(i=4;i<(len-2);i+=2){
			j = (((int)Buf[i+1])<<8)|Buf[i];
			if(j > 32767){
				j = -(65535 - j);
			}
			k = (((double)j / 6400) - (offset2/25)) * scale2;
			sprintf_s(dummy,40, "%f,",k);
		//	ftoa(dummy,j,10);
		//	dummy[strlen(dummy)] = ',';
		//	dummy[strlen(dummy)+1] = 0;
			fwrite(dummy, 1, strlen(dummy), F);
		}

		len2 = (len2 - len);
		len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		while (len2 > 0){
			for(i=0;i<(len-1);i+=2){
				j = (((int)Buf[i+1])<<2)|Buf[i];
				if(j > 32767){
					j = -(65535 - j);
				}
				k = (((double)j / 6400) - (offset2/25)) * scale2;
				sprintf_s(dummy,40, "%f,",k);
			//	ftoa(dummy,j,10);
			//	dummy[strlen(dummy)] = ',';
			//	dummy[strlen(dummy)+1] = 0;
				fwrite(dummy, 1, strlen(dummy), F);
			}
			Sleep(1);
			len2 = len2 - len;
			len = HIDRecv3((char *)"HDS2102S_1",(unsigned char *)Buf);
		}

		fwrite("\n", 1, 1, F);
		fclose(F);
	}
//	DispStrP("WaveformData " .. datetime)
}

unsigned short CalCRC_CMD(unsigned short len, unsigned char bufp[])
{
	unsigned short i;
	unsigned char luc_temp = 0;
	unsigned char uc_CrcL = 0xff;
	unsigned char uc_CrcH = 0xff;

	for(i = 0;i < len;i++)
	{
		uc_CrcL ^= bufp[i];
		luc_temp = uc_CrcH;
		uc_CrcH = crc_ccitt16_LSB_tabH[uc_CrcL];
		uc_CrcL = crc_ccitt16_LSB_tabL[uc_CrcL]^luc_temp;
	}
	return (uc_CrcL | (uc_CrcH << 8));
}

unsigned short CRC_MODBUS_MSBfirst_ALL1(unsigned short len, unsigned char bufp[]){
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
				crc ^= MODPOLY;
			}else{
				crc = crc << 1;
			}
		}
	}
	return crc;
}

unsigned short CRC_MODBUS_LSBfirst_ALL1(unsigned short len, unsigned char bufp[]){
	unsigned char i,j;
	unsigned char ch = 0;
	unsigned short crc = 0xffff;

	for(j = 0;j < len;j++){
		ch = bufp[j];
		crc ^= (unsigned short)ch;
		for (i = 0; i < 8; i++){
			if(crc & 0x0001){
				crc = crc >> 1;
				crc ^= MODPOLY;
			}else{
				crc = crc >> 1;
			}
		}
	}
	return crc;
}
//------------------------------------------------------------------------
//	long値実数から16進ASCIIへ。ltoa()を使わないのは、出力を大文字にしたい
//だけの理由から
//------------------------------------------------------------------------
int ltohex2(unsigned long l,unsigned char * buf,unsigned char i)
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
//------------------------------------------------------------------------
//　キャラクタのスクロール
//------------------------------------------------------------------------
void scroll2(unsigned int kai)
{
    unsigned int j;
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	int ws = ((WSIZE_WIDTH-12)/8)-1;

    for(j=0;j<kai;j++){
		if(frm_U[RtnN] == 1){//Hex
			stpoint+= ws;//表示先頭の行終端へ
			if(stpoint>=(LINGBUF_SIZE-1))stpoint=(stpoint-(LINGBUF_SIZE-1));
			if((int)(FstN+1) >= ((hs*4)+(3-wh))){
				FstN = 0;
			}else{
				if((FstN+1) == RtnN){
					if((int)(RtnN+1) >= ((hs*4)+(3-wh))){
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
			//ここでstpointは次の行先頭を指している
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
void send_scroll2(unsigned int kai)
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
//------------------------------------------------------------------------
//	特殊アスキーコード及び未定義分を"*"に置き換える
//------------------------------------------------------------------------
unsigned char charset2(unsigned char in)
{									//ASCIIコードを受け取り
    if (in<32 || in>126) in = 42;	//フツーの文字以外は皆"*"に
    return in;
}
//------------------------------------------------------------------------
//  16進文字列表記をキャラクターに直す
//	特殊アスキーコード及び未定義分を"*"に置き換える
//------------------------------------------------------------------------
unsigned char Incharset2(unsigned char * in)
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
/*-------------------------------------------------------------------------------------*/
/*int UTF8Len(const char * Buf,int * nlen)                                             */
/* Unsigned char バイト列で渡されたUTF-8文字列の文字数を返す                           */
/* またUTF-8文字列長が引数2のwidec文字目に達した時のバイト数を引数の3番目で返す        */
/*これはUTF-8文字列を画面一行の表示で収まるバイト文字列長を返すことを意図している      */
/*-------------------------------------------------------------------------------------*/
int UTF8Len2(const unsigned char * Buf,int widec,unsigned long * nlen)
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
					*nlen = j;
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
					*nlen = j;
				}
			}else if(((Buf[i] & 0xff) >= 0xF0)&&((Buf[i] & 0xff) <= 0xF4)){
				j = (int)wcslen(WBUF);
				if(j<255){
					WBUF[(j&0xff)] = ((Buf[i] & 0x07) << 18)|((Buf[i+1] & 0x3f) << 12)
						 |((Buf[i+2] & 0x3f) << 6)|(Buf[i+3] & 0x3f);
					WBUF[(j+1)&0xff] = 0;
				}
				j = (int)wcslen(WBUF);
				i+=4;
				if(j == widec){
					*nlen = i;
				}else{
					*nlen = j;
				}
			}else{
				j = (int)wcslen(WBUF);
				if(j == widec){
					*nlen = i;
				}else{
					*nlen = j;
				}
				if(j<255){
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
		if(j>255)break;
	}
	return (int)wcslen(WBUF);
}
//------------------------------------------------------------------------
//汎用表示
// UTF-8対応 引数2に1指定でUTF-8文字列と判別 0もしくは指定なしでASCII
//------------------------------------------------------------------------
int DispStrP(lua_State *L)
{
	unsigned int r,k;
	unsigned long n;
	int m,flg;
	size_t len;
	unsigned int i,wtpoint2,kai;
	
	unsigned char * Bufin = (unsigned char *)lua_tolstring(L, 1, &len);
	flg = 0;
	flg = (int)lua_tonumber(L, 2);
	int ws = ((WSIZE_WIDTH-12)/8)-1;//65
	int vs = ((WSIZE_WIDTH-12)/8);//66
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);

	r = 0;
	//前回の文字終端+今回の追加分
	wtpoint2 = (edpoint + (unsigned int)(len*3));	//前回の文字終端+今回の追加分
	if(long(wtpoint2) > (((hs*4)+(3-wh))*ws)){
		kai = int((wtpoint2 - (((hs*4)+(3-wh))*ws))/ws);
		scroll2(kai);		//収まるようにスクロール
	}

	for(i=0;i<len;i++){
		/*改行条件*/
		if ((Bufin[i] == '\n')||(Bufin[i] == '\r')) {
			if(edpoint!=0){
				if(frmm[edpoint-1] != '\n'){
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
					if(stpoint > LINGBUF_SIZE){
						stpoint = 0;
					}
					retcnt++;
				}
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
				if(stpoint > LINGBUF_SIZE){
					stpoint = 0;
				}
				retcnt++;
			}
			if(retcnt >= ((hs*4)+(3-wh)-1)) {
				scroll2(1);			//収まるようにスクロール
				retcnt--;
			}
			m = UTF8Len2((const unsigned char *)&frmm[lnpoint],ws,&n);
			if(m == strlen(&frmm[lnpoint])){//ASCII
			//	frm_U[RtnN] = 0;
				frm_U[RtnN] = (Display_flg & 0x01);
			}else{
				frm_U[RtnN] = 2;
			}
			if(m>ws){
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
			HexDataREP2(hogeWnd);
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
			m = UTF8Len2((const unsigned char *)&frmm[lnpoint],ws,&n);
			if(m == strlen(&frmm[lnpoint])){//ASCII
			//	frm_U[RtnN] = 0;
				frm_U[RtnN] = (Display_flg & 0x01);
			}else{
				frm_U[RtnN] = 2;
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
			HexDataREP2(hogeWnd);
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
#if 1
			m = UTF8Len2((const unsigned char *)&frmm[lnpoint],ws,&n);
			if((long)n >= ws){
			//	m = UTF8Len2((const unsigned char *)&frmm[lnpoint],ws,&n);
				for(k=edpoint;k>(lnpoint+n);k--){
					frmm[(k+1)&0x7fff] = frmm[k&0x7fff];
				}
				frmm[(lnpoint+n+1)&0x7fff] = 0;
				RtnPos[RtnN] = lnpoint+n+2;/*書き直し*/
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
#endif
			if(retcnt >= ((hs*4)+(3-wh)-1)) {
				scroll2(1);			//収まるようにスクロール
				retcnt--;
			}
		}
	}
	r = 1;
	
//	MessageBoxW(NULL,L"B2",L"Debug",MB_ICONHAND);
	
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
//------------------------------------------------------------------------
//パラメータ表示
//------------------------------------------------------------------------
int DispParamlP(lua_State *L)
{
	int i,r;
	r = 0;
	int ch = (int)lua_tointeger(L, 1);
	char * Buf = (char *)lua_tostring(L, 2);
	long Value = (long)lua_tonumber(L, 3);
	
	if((ch >= 0)&&(ch < 36)){
		strcpy_s((char*)CDSP.str_Name[ch],32, Buf);
		sprintf_s((char*)CDSP.str_Val[ch],32,"%d",Value);
		for(i=(int)strlen(CDSP.str_Val[ch]);i<31;i++)CDSP.str_Val[ch][i] = 0x20;
		CDSP.str_Val[ch][31] = 0;
	}
	DisplayParam2(hogeWnd);
	r = 1;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
//------------------------------------------------------------------------
//パラメータ表示(浮動小数点)
//------------------------------------------------------------------------
int DispParamfP(lua_State *L)
{
	int r;
	r = 0;
	int ch = (int)lua_tointeger(L, 1);
	char * Buf = (char *)lua_tostring(L, 2);
	double Value = (double)lua_tonumber(L, 3);
	
	if((ch >= 0)&&(ch < 36)){
		strcpy_s((char*)CDSP.str_Name[ch],32, Buf);
		sprintf_s((char*)CDSP.str_Val[ch],32,"%f",Value);
	}
	DisplayParam2(hogeWnd);
	r = 1;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
//---テレメトリ全体再描画---------------------------------------------------
void HexDataREP2(HWND hWnd)
{
	HDC		   hdc;	//ハンドル
	HFONT hFont;
	unsigned char line[2000] = {0};	//バイナリ>キャラクタ変換用バッファ
    size_t ret;
	int i,j,k,n,ws,m,p;
	int hs = ((WSIZE_HEIGHT-80)/75);
	int wh = FIXParam_Cnt/(int)((WSIZE_WIDTH-2)/135);
	int vs = (((((WSIZE_WIDTH-12)/8)-2)/4)*3);//48
	if(WSIZE_WIDTH<28){
		WSIZE_WIDTH = 28;
		ws = 4;
	}else{
		ws = ((WSIZE_WIDTH-12)/8)-1;
	}
	WCHAR * Disp = new WCHAR[ws+1];

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
					//	line[vs+1+(n/3)] = charset2(frmm[RtnPos[j]+(n/3)]);
						line[vs+(n/3)] = Incharset2((unsigned char *)(frmm+RtnPos[j]+n));
					} //アスキー表示も右端におまけする
				}
				for(i=n+1;i<vs+1;i++)line[i] = 0x20;
			//	line[n+1] = 0x20;
			//	line[n+1+(vs/3)] = 0;
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
				TextOutW(hdc,4,(k*15)+4+((wh+1)*30),Disp,ws);
			}
		}
	}
	
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
    				ltohex2((unsigned char)(sfrmm[i]),&line[(m*3)],2);
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
	
    SelectObject(hdc,hNewFont);
    DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
}
//---固定パラメータ描画---------------------------------------------------
void DisplayParam2(HWND hWnd)
{
	HDC		   hdc;	//ハンドル
	hdc = GetDC(hWnd);
//	WCHAR * Disp = new WCHAR[((WSIZE_WIDTH-12)/8)+1];
	HFONT hFont;
    size_t ret;
//	int ws = ((WSIZE_WIDTH-12)/8)-1;
	int wh = (WSIZE_WIDTH-2)/135;
	int ch,ws;
	if (WSIZE_WIDTH < 332) {
		WSIZE_WIDTH = 332;
		ws = 64;
	}else {
		ws = ((WSIZE_WIDTH - 12) / 8) - 1;
	}
	WCHAR* Disp = new WCHAR[ws+1];

//  hFont = (HFONT)SelectObject(hdc, hDispFont);
    hFont = (HFONT)SelectObject(hdc, hUFont);
	for(ch = 0;ch < FIXParam_Cnt;ch++){
		mbstowcs_s(&ret,Disp,32,CDSP.str_Name[ch],32);
		TextOutW(hdc,(int(ch%wh))*135+2,(int(ch/wh))*30   ,Disp,(int)wcslen(Disp));
		mbstowcs_s(&ret,Disp,32,CDSP.str_Val[ch],32);
		TextOutW(hdc,(int(ch%wh))*135+2,(int(ch/wh))*30+15,Disp,(int)wcslen(Disp));
	}
    SelectObject(hdc,hNewFont);
    DeleteObject(hFont);
	ReleaseDC(hWnd,hdc);
}

std::map<int, char[32]> enumerateSerialPorts3()
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
/*--------------------------------------------------------------------------*/
/*シリアルポートの初期化                                                    */
/*シリアルポート番号をあらわす文字列を持っているパラメータ構造体を与えると  */
/*Openして初期化する                                                        */
/*実はメッセージパイプも初期化できる                                        */
/*--------------------------------------------------------------------------*/
HANDLE init_commport2(struct serial_param * COMD)
{
	COMMTIMEOUTS cmto;			//Comm Timeout略語化
	BOOL 	fSuccess;		//シリアルポート設定エラーフラグ
	DCB		dcb;			//シリアルポートのDCB構造体
	int pipeflg;
	HANDLE hCom = {0};
	DWORD      dwStoredFlags;
 	WCHAR HandleName[32];
	size_t ret;
	
	pipeflg = 0;
    dwStoredFlags = EV_RXCHAR;
	if(COMD->Init == 0){
		if(!(strcmp(COMD->comm_string,"\\\\.\\pipe\\P1"))){/*USB*/
			mbstowcs_s(&ret,HandleName,32,COMD->comm_string,32);
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
			mbstowcs_s(&ret,HandleName,32,COMD->comm_string,32);
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
			mbstowcs_s(&ret,HandleName,32,COMD->comm_string,32);
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
			mbstowcs_s(&ret,HandleName,32,COMD->comm_string,32);
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
    		MessageBoxW(NULL,L"Can not Open COM Port",L"Error",MB_ICONHAND);
    	//	MessageBox(NULL,COMD->comm_string,"Error",MB_ICONHAND);
        	hCom = 0;
			return 0;
    	}else{
			fSuccess = GetCommState(hCom,&dcb);
			if(! fSuccess){					//Comm1ステータス取得失敗
				if(pipeflg){

				}else{
				MessageBoxW(NULL,L"Can't Get State of COM Port",L"Error",MB_ICONHAND);
					CloseHandle(hCom);
					hCom = 0;
					return(0);
				}
			}
		}
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
		dcb.fParity = FALSE;       // enable parity checking
	    dcb.fOutxDsrFlow = 0;      // DSR output flow control
		dcb.fDtrControl = 0;	   //DTR_CONTROL_DISABLE DTR flow control type 
	    dcb.fDsrSensitivity = 0;   // DSR sensitivity 
	    dcb.fTXContinueOnXoff = 1; // XOFF continues Tx 
	    dcb.fOutX = 0;            // XON/XOFF out flow control 
	    dcb.fInX = 0;             // XON/XOFF in flow control 
	    dcb.fErrorChar = 0;       // enable error replacement 
	    dcb.fNull = 0;            // enable null stripping 
	    dcb.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
	    dcb.fAbortOnError = 0;     // abort reads/writes on error 
	    dcb.ByteSize = 8;			//
	    dcb.Parity = NOPARITY;		//…見ての通り
	    dcb.StopBits = ONESTOPBIT;	//
		fSuccess = SetCommState(hCom,&dcb);
		if(hCom != 0){
			if(!fSuccess){					//Comm1パラメータ設定失敗
				if(pipeflg){

				}else{
				MessageBoxW(NULL,L"Can't Set State of COM Port",L"Error",MB_ICONHAND);
					CloseHandle(hCom);
       				hCom = 0;
					return(0);
				}
			}
    	}
    	if(hCom != 0){
    		GetCommTimeouts(hCom,&cmto);
    		cmto.ReadIntervalTimeout 		= 1;
    		cmto.ReadTotalTimeoutMultiplier = 0;
   		 	cmto.ReadTotalTimeoutConstant 	= 1;
    		cmto.WriteTotalTimeoutMultiplier = 0;
			cmto.WriteTotalTimeoutConstant   = 0;
    		SetCommTimeouts(hCom,&cmto);		//Comm1タイムアウト設定
    		if (!SetCommMask(hCom,dwStoredFlags))return 0;
		}
	}
	return hCom;
}
/*-----------------------------------------------------------------------------*/
/*Lua拡張関数としてのシリアルポート初期化                                      */
/*Lua上の引数 1:チャンネル番号(0～4)                                           */
/*            2:シリアルポート番号、またはデバイス仮名、またはデバイス名       */
/* デバイス仮名とデバイス名は USBシリアル変換を使用時に、device_config.txtに   */
/* 必要なパラメータを事前に定義してくことで使用できる                          */
/*            3:ボーレート                                                     */
/*初期化されたシリアルポートは以後チャンネル番号でアクセスする                 */
/*Lua上の戻り値 r 0で成功 1で失敗                                              */
/*-----------------------------------------------------------------------------*/
int SerialInit(lua_State * L)
{
	unsigned int r,i;
	int delimiter = 0;
	char testcomm_string[32] ={0};
	char testcomm_string2[32] ={0};
	int MDEVch = 0;	
	int ch = (int)lua_tonumber(L, 1);
//	int Port = (int)lua_tonumber(L, 2);
	long baud = (long)lua_tonumber(L, 3);
	LPTSTR lpMessageBuffer = NULL;	
	DWORD dwID;
	r = 1;
	
	if(lua_isnumber(L, 2)){
		int Port = (int)lua_tonumber(L, 2);
		sprintf_s(testcomm_string,16,"\\\\.\\COM%d",Port);
	}else{
		char * Buf = (char *)lua_tostring(L, 2);
		if(Buf[1] == 0){
			if((Buf[0] >= 'A')&&(Buf[0] <= 'J')){
				for(i=0;i<MDEV.size();i++){
					if((MDEV[i].serial_port_num - 100)==Buf[0]){
						if(MDEV[i].init){
							sprintf_s(testcomm_string,MDEV[i].serial_port);
						}
						delimiter = MDEV[i].delimiter;
						MDEVch = i;
					}
				}
			}else if(Buf[0] >= 'X'){
				sprintf_s(testcomm_string,COMMPARAM[ch].comm_string);
			}else{
				COMMPARAM[ch].Init = -1;
			}
		}else{
			for(i=0;i<MDEV.size();i++){
				if(!(strcmp(MDEV[i].name,Buf))){
					if(MDEV[i].serial_port != NULL){
						sprintf_s(testcomm_string,MDEV[i].serial_port);
						delimiter = MDEV[i].delimiter;
						MDEVch = i;
					}
				}
			}
		}
	}

	PortList = enumerateSerialPorts3();

	if(COMMPARAM[ch].Init == 0){
		int Port = (int)lua_tonumber(L, 2);
		sprintf_s(testcomm_string2,16,"\\\\.\\COM%d",Port);
		if(!(strcmp(COMMPARAM[ch].comm_string,testcomm_string2))){
		
		}else{
			strcpy_s(COMMPARAM[ch].comm_string,16,testcomm_string);
			COMMPARAM[ch].BaudRate = baud;
		//	COMMPARAM[ch].newline = delimiter;
			if(MDEVch != 0){
				COMMPARAM[ch].no = ch;
				COMMPARAM[ch].fParity = 0;
				COMMPARAM[ch].Parity = 0;
				COMMPARAM[ch].StopBits = 0;//COM_S_ONE;
				COMMPARAM[ch].modem_flg = 0;
				COMMPARAM[ch].newline = MDEV[MDEVch].delimiter;
				COMMPARAM[ch].VID = MDEV[MDEVch].usb_vid;
				COMMPARAM[ch].PID = MDEV[MDEVch].usb_pid;
			}
			hCom[ch] = init_commport2(&COMMPARAM[ch]);
			switch(ch){
				case 0:
				hThread[ch] = CreateThread(NULL,0,ThreadFunc,(LPVOID)hogeWnd,0,&dwID);
				break;
				case 1:
				hThread[ch] = CreateThread(NULL,0,ThreadFunc2,(LPVOID)hogeWnd,0,&dwID);
				break;
				case 2:
				hThread[ch] = CreateThread(NULL,0,ThreadFunc3,(LPVOID)hogeWnd,0,&dwID);
				break;
				case 3:
				hThread[ch] = CreateThread(NULL,0,ThreadFunc4,(LPVOID)hogeWnd,0,&dwID);
				break;
				case 4:
				hThread[ch] = CreateThread(NULL,0,ThreadFunc5,(LPVOID)hogeWnd,0,&dwID);
				break;
				default:
				break;
			}
			if(hThread[ch]!=NULL){
				SetThreadPriority(hThread[ch],THREAD_PRIORITY_LOWEST);
			}else{
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					GetLastError(),
					LANG_USER_DEFAULT,
					(LPTSTR)&lpMessageBuffer,
					0,
					NULL);
					MessageBoxW(NULL, lpMessageBuffer, L"Hoge", MB_ICONHAND);
			}
		}
		if(hCom){
			COMMPARAM[ch].Init = 1;
			r = 0;
		}else{
			r = 1;
		}
	}else{
		r = 1;
	}
	if(MDEVch != 0){
		MDEV[MDEVch].ser_ch = ch;
	}
	hEventObject[ch] = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
	COMOverlapped[ch].hEvent = hEventObject[ch];
	COMOverlapped[ch].Offset = 0;
	COMOverlapped[ch].OffsetHigh = 0;
	hEventObjectW[ch] = CreateEventW(NULL,TRUE,TRUE,LPCWSTR(L""));
	COMOverlappedW[ch].hEvent = hEventObjectW[ch];
	COMOverlappedW[ch].Offset = 0;
	COMOverlappedW[ch].OffsetHigh = 0;	
    // 結果をスタックに戻す
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
/*--------------------------------------------------------------------------------*/
/*シリアルポート送信                                                              */
/*Lua上の引数 1:シリアルチャンネル番号またはデバイス名                            */
/*            2:送信文字列       3:送信文字列長                                   */
/*Lua上の戻り値 r 常に1                                                           */
/*--------------------------------------------------------------------------------*/
int SerialSend(lua_State * L)
{
	unsigned int r;
	int i,j,len,ch;
	LPDWORD lenR = 0;
//	size_t len2;
	ch = 0;
	char Buf2[4] = {0};
	
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				if(MDEV[i].init){
					for(j=0;j<10;j++){
						if(!(strcmp(COMMPARAM[ch].comm_string,MDEV[i].serial_port))){
							ch = j;
						}
					}
				}
			}
		}
	}
	char * Buf = (char *)lua_tostring(L, 2);
	len = (int)lua_tonumber(L, 3);
//	if(len > len2)len = (int)len2;

	for(i=0;i<len;i++)SND[ch].buf[i] = (unsigned char)Buf[i];
	SND[ch].len = len;
	switch(COMMPARAM[ch].newline){
		case 3:
			SND[ch].buf[SND[ch].len] = '\r';
			SND[ch].buf[SND[ch].len+1] = '\n';
			SND[ch].buf[SND[ch].len+2] = 0;
			SND[ch].len+=2;
			break;
		case 2:
			SND[ch].buf[SND[ch].len] = '\n';
			SND[ch].buf[SND[ch].len+1] = 0;
			SND[ch].len++;
			break;
		case 1:
			SND[ch].buf[SND[ch].len] = '\r';
			SND[ch].buf[SND[ch].len+1] = 0;
			SND[ch].len++;
			break;
		default:
			break;
	}
	EnterCriticalSection(&cscom);
	WriteFile(hCom[ch],SND[ch].buf,SND[ch].len,lenR,&COMOverlappedW[ch]);
	LeaveCriticalSection(&cscom);
	r = 1;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
int SerialWaitRecv(lua_State * L)
{
	unsigned int r = 0;
	int len = 0;
	int i;
	r = 0;
	size_t len2;
	char Buf2[512];
	
	int ch = (int)lua_tonumber(L, 1);
	char * Buf = (char *)lua_tolstring(L, 2,&len2);
	int timeout = (int)lua_tonumber(L, 2);
	
//	while((COMBUF[ch].len2 == 0)&&(timeout > 0)) {
	while((r == 0)&&(timeout > 0)) {
		if (COMBUF[ch].len2>0) {	//何かComから受け取ったなら
			for(i=0;i<COMBUF[ch].len2;i++)Buf2[i] = COMBUF[ch].Buf2[i];
			if(!(strcmp(Buf2,Buf))){
				r = 1;
			}else{
				r = 0;
			}
		}else{
			r = 0;
		}
		Sleep(1000);
		timeout--;
	//	if(timeout == 0)r = 1;
	}
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int SerialRecv(lua_State * L)
{
	unsigned int r;
	int len = 0;
	int i;
	r = 0;
	size_t len2;
	int ch = (int)lua_tonumber(L, 1);
	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 2, &len2);

	if (COMBUF[ch].len2>0) {	//何かComから受け取ったなら
	//	if(len2<COMBUF[ch].len2)COMBUF[ch].len2 = (long)len2;
		r = COMBUF[ch].len2;
	//	if(COMMBUFSIZE<COMBUF[ch].len2)COMBUF.len2 = COMMBUFSIZE;
		for(i=0;i<COMBUF[ch].len2;i++)Buf[i] = COMBUF[ch].Buf2[i];
	}else{
		r = 0;
	}
//	r = 1;
	COMBUF[ch].Buf2[0] = 0;
	COMBUF[ch].len2 = 0;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
	lua_pushlstring (L,(char *)Buf,r);
    return 2; // 戻り値の数を返す
}
/*--------------------------------------------------------------------------------*/
/* シリアルポート受信 Lua文字列内部生成版                                         */
/*Lua上の引数 1:シリアルチャンネル番号                                            */
/*Lua上の戻り値 Buf,len  BufはLua文字列、lenは文字列長                            */
/*Bufは事前に宣言する必要は無い。内部で生成される。あとはluaのガベコレで処理される*/
/*--------------------------------------------------------------------------------*/
int SerialRecv2(lua_State * L)
{
	unsigned int r,r2;
	int len = 0;
	int i;
	r = 0;
	char * Buf = {0};
	luaL_Buffer Buff;
	
	int ch = (int)lua_tonumber(L, 1);

	if (COMBUF[ch].len2>0) {	//何かComから受け取ったなら
		r = COMBUF[ch].len2;
		luaL_buffinitsize(L, &Buff,r);
		Buf = luaL_prepbuffer(&Buff);
		for(i=0;i<COMBUF[ch].len2;i++)Buf[i] = COMBUF[ch].Buf2[i];
		r2 = r;
	}else{
		luaL_buffinitsize(L, &Buff,0);
		r = 0;
	}
	COMBUF[ch].Buf2[0] = 0;
	COMBUF[ch].len2 = 0;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
	luaL_pushresultsize(&Buff,r);
    lua_pushinteger(L, r);
    return 2; // 戻り値の数を返す
}

int Sleep(lua_State * L)
{
	int r = 0;
	int time = (int)lua_tonumber(L, 1);
	Sleep(1000 * time);
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int HIDSend(lua_State * L)
{
	unsigned int r;
	int i,j,ch,len;
	ch = 0;
	int data_len=1;
//	char Buf2[4] = {0};
	unsigned char Buf2[512] = {0};
	size_t len2;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
//	unsigned char * Buf = (unsigned char *)lua_tostring(L, 2);
	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 2, &len2);
//	char * Buf = (char *)lua_tostring(L, 2);
	len = (int)lua_tonumber(L, 3);
//	strcpy_s((char *)Buf2,512,(char *)Buf);
   	strcpy_s((char *)Buf2,sizeof(Buf2),(char *)Buf);
	if(len2<len)len = (int)len2;
	if(len>509)len = 509;
	switch(MDEV[UHDEV[ch].MDEV_num].delimiter){
		case 3:
		//	Buf2[0] = '\r';
		//	Buf2[1] = '\n';
		//	Buf2[2] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[len] = '\r';
			Buf2[len+1] = '\n';
			Buf2[len+2] = 0;
			len+=2;
			break;
		case 2:
		//	Buf2[0] = '\n';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[len] = '\n';
			Buf2[len+1] = 0;
			len++;
			break;
		case 1:
		//	Buf2[0] = '\r';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[len] = '\r';
			Buf2[len+1] = 0;
			len++;
			break;
		default:
			Buf2[len] = 0;
			break;
	}
//  uint8_t tlen = (uint8_t)strlen((char *)Buf);
//  if(Buf[len-1] != '\n'){
//  if((Buf[len-1] != '\n')&&(Buf[len-1] != '?')){
//		strcpy_s((char *)Buf2,512,(char *)Buf);
	//	Buf2[len] = '\n';
//		Buf2[len] = 0;
//		Buf2[len+1] = 0;
	//	len++;
//	libusb_bulk_transfer(UHDEV[ch].handle, LIBUSB_ENDPOINT_OUT|1, Buf2, len,&data_len, 1000);
//		libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,Buf2, len,&data_len, 1000);
//	}else{
//	libusb_bulk_transfer(UHDEV[ch].handle, LIBUSB_ENDPOINT_OUT|1, Buf, len,&data_len, 1000);
	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
	//	libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT, Buf, len,&data_len, 1000);
		libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT, Buf2, len,&data_len, 1000);
		r = 1;
		}else{
			r = 0;
		}
	}else{
		r = 0;
	}
//  uint8_t bmReqType = 32 + 1;// the request type (direction of transfer)
//  uint8_t bmReqType = 0x20 + 0x01;// the request type (direction of transfer)
//  uint8_t bmReqType = 0xC0 + 0x00;// the request type (direction of transfer)
//  uint8_t bReq = 1;// the request field for this packet
//  uint16_t wVal = 0x300;// the value field for this packet
//  uint16_t wIndex = 0x400;// the index field for this packet
//	libusb_control_transfer(UHDEV[ch].handle,
//			bmReqType,bReq,wVal,wIndex,(unsigned char *)Buf,tlen,1000);
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
	return 1;
}

int HIDSendS(lua_State * L)
{
	unsigned int r;
	int i,j,ch;
	ch = 0;
	int data_len=1;
//	char Buf2[4] = {0};
	unsigned char Buf2[512] = {0};
	size_t len2;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	char * Buf = (char *)lua_tolstring(L, 2, &len2);
//	char * Buf = (char *)lua_tostring(L, 2);
//  uint8_t tlen = (uint8_t)strlen(Buf);
    int tlen = (int)strlen(Buf);
	if(len2<tlen)tlen = (uint8_t)len2;
	if(tlen>509)tlen = 509;
//	strcpy_s((char *)Buf2,512,(char *)Buf);
   	strcpy_s((char *)Buf2,sizeof(Buf2),(char *)Buf);
	switch(MDEV[UHDEV[ch].MDEV_num].delimiter){
		case 3:
		//	Buf2[0] = '\r';
		//	Buf2[1] = '\n';
		//	Buf2[2] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\r';
			Buf2[tlen+1] = '\n';
			Buf2[tlen+2] = 0;
			tlen+=2;
			break;
		case 2:
		//	Buf2[0] = '\n';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\n';
			Buf2[tlen+1] = 0;
			tlen++;
			break;
		case 1:
		//	Buf2[0] = '\r';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\r';
			Buf2[tlen+1] = 0;
			tlen++;
			break;
		default:
			Buf2[tlen] = 0;
			break;
	}
	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
		//	libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
		//				(unsigned char *)Buf,tlen,&data_len, 1000);
			libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
						(unsigned char *)Buf2,tlen,&data_len, 1000);
			r = 1;
		}else{
			r = 0;
		}
	}else{
		r = 0;
	}
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
	return 1;
}

//int HIDSendS3(char * NAME,unsigned char * Buf)
int HIDSendS3(char * NAME,char * Buf)
{
	int i,j,ch,r;
	ch = 0;
	int data_len=1;
//	char Buf2[4] = {0};
	unsigned char Buf2[512] = {0};
//	size_t len2;

	if((NAME[0] >= '0')&&(NAME[0] <= '9')){
		ch = NAME[0] - '0';
	}else{
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,NAME))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}

    int tlen = (int)strlen((char *)Buf);
	if(tlen > 509)tlen = 509;
   	strcpy_s((char *)Buf2,sizeof(Buf2),(char *)Buf);
//	if(len2<tlen)tlen = (uint8_t)len2;
	switch(MDEV[UHDEV[ch].MDEV_num].delimiter){
		case 3:
		//	Buf2[0] = '\r';
		//	Buf2[1] = '\n';
		//	Buf2[2] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\r';
			Buf2[tlen+1] = '\n';
			Buf2[tlen+2] = 0;
			tlen+=2;
			break;
		case 2:
		//	Buf2[0] = '\n';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\n';
			Buf2[tlen+1] = 0;
			tlen++;
			break;
		case 1:
		//	Buf2[0] = '\r';
		//	Buf2[1] = 0;
		//	luaL_addstring((luaL_Buffer *)&Buf,Buf2);
			Buf2[tlen] = '\r';
			Buf2[tlen+1] = 0;
			tlen++;
			break;
		default:
			Buf2[tlen] = 0;
			break;
	}

	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
		//	libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
		//				(unsigned char *)Buf,tlen,&data_len, 1000);
			libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
						(unsigned char *)Buf2,tlen,&data_len, 1000);
			r = 1;
		}else{
			r = 0;
		}
	}else{
		r = 0;
	}
	return r;
}

int HIDRecv(lua_State * L)
{
//	unsigned int r;
	int i,j,ch,len,ret;
	ch = 0;
	unsigned char data[600]={0};
	int data_len=1;
	size_t len2;
	
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	} else if(lua_isstring(L, 1)){
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}else{
//      lua_pushinteger(L, -1);
//      lua_pushstring(L, "");
        return 2;		
	}
//  unsigned char * Buf = reinterpret_cast<unsigned char*>(const_cast<void*>(lua_topointer(L, 2)));
//	unsigned char * Buf = (unsigned char *)lua_tostring(L, 2);
	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 2, &len2);
	Buf[0] = 0;
	len = -1;
//	if(libusb_bulk_transfer(UHDEV[ch].handle, LIBUSB_ENDPOINT_IN|1, 
	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		data, sizeof(data), &data_len, 1000);
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		Buf, sizeof(Buf), &data_len, 1000);
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		Buf, 64, &data_len, 1000);
			ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
				data, sizeof(data), &data_len, 1000);
			if(ret == 0){
          		len = data_len > 599 ? 599 : data_len;
				for(i=0;i<len;i++)Buf[i] = data[i];
				Buf[len] = 0;
          	}else{
				len = 0;
				if (ret == LIBUSB_ERROR_TIMEOUT ){
				//	MessageBoxW(NULL,L"LIBUSB_ERROR_TIMEOUT",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_PIPE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_PIPE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_OVERFLOW ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_OVERFLOW",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_DEVICE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_DEVICE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_BUSY ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_BUSY",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INVALID_PARAM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INVALID_PARAM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_IO ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_IO",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_TRANSFER_ERROR ){
					MessageBoxW(NULL,L"LIBUSB_TRANSFER_ERROR",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INTERRUPTED ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INTERRUPTED",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_ACCESS ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_ACCESS",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_MEM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_MEM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_COUNT ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_COUNT",L"A19A",MB_ICONHAND);
				}
			}
		}else{
			len = -1;
		}
	}else{
		len = -1;
	}
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, len);
	lua_pushlstring (L,(char *)Buf,len);
//	lua_pushlstring (L,(char *)data,len);
	return 2;
}
/*--------------------------------------------------------------------------------*/
/* シリアルHID受信 Lua文字列内部生成版  　                                        */
/*Lua上の引数 1:デバイス名  またはチャンネル番号                                  */
/*Lua上の戻り値 Buf,len  BufはLua文字列、lenは文字列長                            */
/*Bufは事前に宣言する必要は無い。内部で生成される。あとはluaのガベコレで処理される*/
/*--------------------------------------------------------------------------------*/
int HIDRecv2(lua_State * L)
{
	int i,j,ch,len,ret;
	ch = 0;
	char * Buf = {0};
	luaL_Buffer Buff;
	int data_len=1;
	unsigned char data[600]={0};
	size_t len2;
	
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	len = 0;
	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		(unsigned char *)&Buf, 512, &data_len, 1000);
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		(unsigned char *)&Buf, 64, &data_len, 1000);
			ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
				data, sizeof(data), &data_len, 1000);
		//	ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
		//		data, 64, &data_len, 1000);
			if(ret == 0){
				if(data_len > 599){
					len2 = 599;
				}else{
					len2 = data_len;
				}
			//	luaL_buffinitsize(L, &Buff,64);
				luaL_buffinitsize(L, &Buff,len2);
				Buf = luaL_prepbuffer(&Buff);
			//	for(i=0;i<64;i++)Buf[i] = data[i];
				for(i=0;i<len2;i++)Buf[i] = data[i];
			//	len = data_len;
			//	Buf[64] = 0;
			//	len = 64;
				len = (int)len2;
			}else{
				len = 0;
				if (ret == LIBUSB_ERROR_TIMEOUT ){
				//	MessageBoxW(NULL,L"LIBUSB_ERROR_TIMEOUT",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_PIPE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_PIPE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_OVERFLOW ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_OVERFLOW",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_DEVICE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_DEVICE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_BUSY ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_BUSY",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INVALID_PARAM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INVALID_PARAM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_IO ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_IO",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_TRANSFER_ERROR ){
					MessageBoxW(NULL,L"LIBUSB_TRANSFER_ERROR",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INTERRUPTED ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INTERRUPTED",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_ACCESS ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_ACCESS",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_MEM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_MEM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_COUNT ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_COUNT",L"A19A",MB_ICONHAND);
				}
			}
		}else{
			len = -1;
		}
	}else{
		len = -1;
	}
    // 結果をスタックに戻す
//	if(len < 64){
//		len3 = 64-len;
//	 	luaL_buffsub(&Buff,len3);
//  }
  	lua_pop(L, lua_gettop(L));
	luaL_pushresultsize(&Buff,len);
    lua_pushinteger(L, len);
	return 2;
}

int HIDRecv3(char * NAME,unsigned char * Buf)
{
	int i,j,ch,len,ret;
	ch = 0;
	unsigned char data[600]={0};
	int data_len=1;
	size_t len2;
	
	if((NAME[0] >= '0')&&(NAME[0] <= '9')){
		ch = NAME[0] - '0';
	}else{
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,NAME))){
				for(j=0;j<UHDEV.size();j++){
					if(UHDEV[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	Buf[0] = 0;
	len = 0;
	if(UHDEV.size() > ch){
		if(UHDEV[ch].handle != NULL){
			ret = libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPIN,
				data, sizeof(data), &data_len, 1000);
			if(ret == 0){
				if(data_len > 599){
					len2 = 599;
				}else{
					len2 = data_len;
				}
				for(i=0;i<len2;i++)Buf[i] = data[i];
				Buf[data_len] = 0;
				len = data_len;
			}else{
				len = 0;
				if (ret == LIBUSB_ERROR_TIMEOUT ){
				//	MessageBoxW(NULL,L"LIBUSB_ERROR_TIMEOUT",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_PIPE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_PIPE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_OVERFLOW ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_OVERFLOW",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_DEVICE ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_DEVICE",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_BUSY ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_BUSY",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INVALID_PARAM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INVALID_PARAM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_IO ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_IO",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_TRANSFER_ERROR ){
					MessageBoxW(NULL,L"LIBUSB_TRANSFER_ERROR",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_INTERRUPTED ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_INTERRUPTED",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_ACCESS ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_ACCESS",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_NO_MEM ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_NO_MEM",L"A19A",MB_ICONHAND);
				}else if ( ret == LIBUSB_ERROR_COUNT ){
					MessageBoxW(NULL,L"LIBUSB_ERROR_COUNT",L"A19A",MB_ICONHAND);
				}
			}
		}else{
			len = -1;
		}
	}else{
		len = -1;
	}
	return len;
}

int NetInitCheck(lua_State * L)
{
	int ch = 0;
	int i,j;
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	int r = 0;
    // 結果をスタックに戻す
	if(NET.size() > ch){
		r = NET[ch].Initflg;
	}else{
		r = 0;
	}
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L,r);
    return 1; // 戻り値の数を返す
}
int NetConnect(lua_State * L)
{
	int r = 0;
	int ch = 0;
	int i,j;
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	if(NET.size() > ch){
		NET[ch].Connect();
		if(NET[ch].IsActive()){
			r = 0;
		}else{
			r = 1;
		}
	}else{
		r = 0;
	}
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
int NetDisConnect(lua_State * L)
{
	int r = 0;
	int ch = 0;
	int i,j;
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	if(NET.size() > ch){
		NET[ch].Close();
	}
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
int NetSend(lua_State * L)
{
	int i,j,len = 0;
	int r = 0;
	int ch = 0;
	size_t len2;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
//	unsigned char * Buf = (unsigned char *)lua_tostring(L, 2);
	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 2, &len2);
	len = (int)lua_tonumber(L, 3);
//	if(len > 1024)len = 1024;
	if(len > len2)len = (int)len2;
	if(ch>4)ch = 0;
	for(i=0;i<len;i++)SND[5+ch].buf[i]= Buf[i];
	if(!NET[ch].SendB((unsigned char *)SND[5+ch].buf,len)){
		r = 1;
	}
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
int NetWaitRecv(lua_State * L)
{
	unsigned int r = 0;
	int i,j;
	int ch = 0;
	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
	int timeout = (int)lua_tonumber(L, 2);
	while((COMBUF[10+ch].len == 0)&&(timeout > 0)) {
		Sleep(1000);
		timeout--;
		if(timeout == 0)r = 1;
	}
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}
int NetRecv(lua_State * L)
{
	unsigned int r;
	int len = 0;
	int i,j;
	int ch = 0;
//	size_t len2;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}
//	unsigned char * Buf = (unsigned char *)lua_tostring(L, 2);
//	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 2, &len2);
//	COMBUF[10+ch].len = NET[ch].Recv(Buf);

	if (COMBUF[5+ch].len>0) {	//何かIP Portから受け取ったなら
		r = COMBUF[5+ch].len;
	}else{
		r = 0;
	}
	r = 1;
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
	lua_pushlstring (L,(char *)COMBUF[5+ch].Buf,len);
    return 2; // 戻り値の数を返す
}
/*--------------------------------------------------------------------------------*/
/* ネット受信 Lua文字列内部生成版                                                 */
/*Lua上の引数 1:デバイス名  またはチャンネル番号                                  */
/*Lua上の戻り値 Buf,len  BufはLua文字列、lenは文字列長                            */
/*Bufは事前に宣言する必要は無い。内部で生成される。あとはluaのガベコレで処理される*/
/*--------------------------------------------------------------------------------*/
int NetRecv2(lua_State * L)
{
	unsigned int r;
	int len = 0;
	int i,j;
	int ch = 0;
	
	char * Buf = {0};
	luaL_Buffer Buff;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				for(j=0;j<NET.size();j++){
					if(NET[j].MDEV_num == i){
						ch = j;
					}
				}
			}
		}
	}

	if (COMBUF[10+ch].len>0) {	//何かIP Portから受け取ったなら
		r = COMBUF[10+ch].len;
		luaL_buffinitsize(L, &Buff,r);
		Buf = luaL_prepbuffer(&Buff);
		for(i=0;i<COMBUF[10+ch].len;i++)Buf[i] = COMBUF[10+ch].Buf[i];
	}else{
		luaL_buffinitsize(L, &Buff,0);
		r = 0;
	}
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
	luaL_pushresultsize(&Buff,r);
    lua_pushinteger(L, r);
//	lua_pushlstring (L,(char *)COMBUF[10+ch].Buf,len);
    return 2; // 戻り値の数を返す
}
/*--------------------------------------------------------------------------------*/
/* 計測器登録済みシーケンス実行                                                   */
/*Lua上の引数 1:デバイス名  またはチャンネル番号                                  */
/*            2:定義済みシーケンス名                                              */
/*Lua上の戻り値 1で成功、0で失敗　　　　　　　　　　　                            */
/* 基本的にはシーケンスで定義されたSCPIコマンドが送出される                       */
/* SCPI以外の定義コマンド1: SLEEP 秒単位でスリープ処理                            */
/*                       2: MAWIT 100ミリ秒単位でスリープ処理                     */
/*--------------------------------------------------------------------------------*/
int DeviceSeq(lua_State * L)
{
	int r = 0;
	int len = 0;
	int i,j;
	int ch,num,cmdn,param;
	int data_len=1;
	double fparam;
	unsigned char data[64]={0};
	char Buf2[4] = {0};
	LPDWORD lenR = 0;
	unsigned short CRC16;
	ch = 0;
	num = -1;

	if(lua_isnumber(L, 1)){
		ch = (int)lua_tointeger(L, 1);
	}else{
		char * name = (char *)lua_tostring(L, 1);
		for(i=0;i<MDEV.size();i++){
			if(!(strcmp(MDEV[i].name,name))){
				num = i;
				switch (MDEV[i].interface_type){
					case 1://serial
					//	ch = 必ずinit時の数字で指定
						ch = MDEV[i].ser_ch;
						break;
					case 2://IVI
					//	for(j=0;j<VISESS.size();j++){
					//		if(!(strcmp(VISESS[j].name,name))){
					//			ch = j;
					//		}
					//	}
						break;
					case 3://USB HID
						for(j=0;j<UHDEV.size();j++){
							if(UHDEV[j].MDEV_num == i){
								ch = j;
							}
						}
						break;
					case 4://LAN
						for(j=0;j<NET.size();j++){
							if(NET[j].MDEV_num == i){
								ch = j;
							}
						}
						break;
					case 5://GPIB
						break;
					case 7://USB HID
						for(j=0;j<UHDEV.size();j++){
							if(UHDEV[j].MDEV_num == i){
								ch = j;
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
	char * Buf = (char *)lua_tostring(L, 2);
	if(num > -1){
		cmdn = 0;
		for(i=0;i<MDEV[num].MCMD.size();i++){
			if(!(strcmp(MDEV[num].MCMD[i].name,Buf))){
				cmdn = i;
			}
		}
		for(i=0;i<MDEV[num].MCMD[cmdn].CMD.size();i++){
			if(MDEV[num].MCMD[cmdn].CMD[i].type == 0){
				switch (MDEV[num].interface_type){
					case 1://serial
						if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
							if(MDEV[num].MCMD[cmdn].CMD[i].param > -4){
								fparam = (double)lua_tonumber(L,
									2 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)SND[ch].buf,64,
									"%f %s",fparam,MDEV[num].MCMD[cmdn].CMD[i].cmd);
							}else{
								fparam = (double)lua_tonumber(L,
									-1 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)SND[ch].buf,64,
									"%s %f",MDEV[num].MCMD[cmdn].CMD[i].cmd,fparam);
							}
						}else{
							sprintf_s((char *)SND[ch].buf,64,
								"%s",MDEV[num].MCMD[cmdn].CMD[i].cmd);
						}
						SND[ch].len = (DWORD)strlen((char *)SND[ch].buf);
						switch(MDEV[num].delimiter){
							case 3:
								SND[ch].buf[SND[ch].len] = '\r';
								SND[ch].buf[SND[ch].len+1] = '\n';
								SND[ch].buf[SND[ch].len+2] = 0;
								SND[ch].len+=2;
								break;
							case 2:
								SND[ch].buf[SND[ch].len] = '\n';
								SND[ch].buf[SND[ch].len+1] = 0;
								SND[ch].len++;
								break;
							case 1:
								SND[ch].buf[SND[ch].len] = '\r';
								SND[ch].buf[SND[ch].len+1] = 0;
								SND[ch].len++;
								break;
							default:
								break;
						}
						EnterCriticalSection(&cscom);
					WriteFile(hCom[ch],SND[ch].buf,SND[ch].len,lenR,&COMOverlappedW[ch]);
						LeaveCriticalSection(&cscom);
						r = 1;
						break;
					case 2://IVI
						if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
							if(MDEV[num].MCMD[cmdn].CMD[i].param > -4){
								param = (int)lua_tointeger(L,
									2 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)SND[ch].buf,64,
									"%s %d",MDEV[num].MCMD[cmdn].CMD[i].cmd,param);
							}else{
								param = (int)lua_tointeger(L,
									-1 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)SND[ch].buf,64,
									"%d %s",param,MDEV[num].MCMD[cmdn].CMD[i].cmd);
							}
						}else{
						sprintf_s((char *)data,64,"%s",MDEV[num].MCMD[cmdn].CMD[i].cmd);
						}
						len = (int)strlen((char *)data);
						switch(MDEV[num].delimiter){
							case 3:
								data[len] = '\r';
								data[len+1] = '\n';
								data[len+2] = 0;
								len+=2;
								break;
							case 2:
								data[len] = '\n';
								data[len+1] = 0;
								len++;
								break;
							case 1:
								data[len] = '\r';
								data[len+1] = 0;
								len++;
								break;
							default:
								break;
						}
					//	if(VISESS.size() > 0){
					//		viPrintf(VISESS[ch].session,(char *)data);
					//		r = 1;
					//	}
						break;
					case 3://HID
						if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
							if(MDEV[num].MCMD[cmdn].CMD[i].param > -4){
								param = (int)lua_tointeger(L,
									2 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)data,64,
									"%s %d",MDEV[num].MCMD[cmdn].CMD[i].cmd,param);
							}else{
								param = (int)lua_tointeger(L,
									-1 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)data,64,
									"%d %s",param,MDEV[num].MCMD[cmdn].CMD[i].cmd);
							}
						}else{
							sprintf_s((char *)data,64,
							"%s",MDEV[num].MCMD[cmdn].CMD[i].cmd);
						}
						len = (int)strlen((char *)data);
						switch(MDEV[num].delimiter){
							case 3:
								data[len] = '\r';
								data[len+1] = '\n';
								data[len+2] = 0;
								len+=2;
								break;
							case 2:
								data[len] = '\n';
								data[len+1] = 0;
								len++;
								break;
							case 1:
								data[len] = '\r';
								data[len+1] = 0;
								len++;
								break;
							default:
								break;
						}
						if(UHDEV.size() > 0){
							if(UHDEV[ch].handle != NULL){
								libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
								 data,len,&data_len, 1000);
								r = 1;
							}
						}
						break;
					case 4://LAN
						if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
							if(MDEV[num].MCMD[cmdn].CMD[i].param > -4){
								param = (int)lua_tointeger(L,
									2 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)data,64,
									"%s %d",MDEV[num].MCMD[cmdn].CMD[i].cmd,param);
							}else{
								param = (int)lua_tointeger(L,
									-1 - (MDEV[num].MCMD[cmdn].CMD[i].param));
								sprintf_s((char *)SND[ch].buf,64,
									"%d %s",param,MDEV[num].MCMD[cmdn].CMD[i].cmd);
							}
						}else{
							sprintf_s((char *)data,64,
								"%s",MDEV[num].MCMD[cmdn].CMD[i].cmd);
						}
						len = (int)strlen((char *)data);
						if(NET.size() > 0){
							if(!NET[ch].SendB((unsigned char *)data,len)){
								r = 1;
							}
						}
						break;
					case 7://HID DP100
						if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
							fparam = (double)lua_tonumber(L,
								-1 - (MDEV[num].MCMD[cmdn].CMD[i].param));
							data[0] = 0xfb;
							len = 64;
							if(!(strcmp(MDEV[num].MCMD[cmdn].CMD[i].cmd,"VOLT"))){
								data[1] = 0x35;
								data[2] = 0;
								data[3] = 10;
								data[4] = 0x20;
								data[5] = 1;
								data[6] =  long(fparam * 1000)& 0xff;
								data[7] = (long(fparam * 1000)>> 8) & 0xff;
								data[8] =  (5 * 1000) & 0xff;
								data[9] = ((5 * 1000) >> 8) & 0xff;
								data[10] = long(30.5 * 1000)& 0xff;
								data[11] =(long(30.5 * 1000)>> 8) & 0xff;
								data[12] = long(5.05 * 1000) & 0xff;
								data[13] =(long(5.05 * 1000)>> 8) & 0xff;
								data[14] = 0;
								data[15] = 0;
								CRC16 = CRC_MODBUS_LSBfirst_ALL1(14,data);
								data[14] =  CRC16 & 0xff;
								data[15] = (CRC16 >> 8)& 0xff;
								data[16] = 0;
							}
						}else{
							data[0] = 0xfb;
							len = 64;
							if(!(strcmp(MDEV[num].MCMD[cmdn].CMD[i].cmd,"STOP"))){
								data[1] = 0x35;
								data[2] = 0;
								data[3] = 0;
								data[4] = 0xa0;
								data[5] = 0;
								data[6] =  (0 * 1000)& 0xff;
								data[7] = ((0 * 1000)>> 8) & 0xff;
								data[8] =  (0 * 1000) & 0xff;
								data[9] = ((0 * 1000) >> 8) & 0xff;
								data[10] = (30 * 1000)& 0xff;
								data[11] =((30 * 1000)>> 8) & 0xff;
								data[12] = (5 * 1000) & 0xff;
								data[13] =((5 * 1000)>> 8) & 0xff;
								data[14] = 0;
								data[15] = 0;
								CRC16 = CRC_MODBUS_LSBfirst_ALL1(14,data);
								data[14] =  CRC16 & 0xff;
								data[15] = (CRC16 >> 8)& 0xff;
								data[16] = 0;
							}else if(!(strcmp(MDEV[num].MCMD[cmdn].CMD[i].cmd,"INFO"))){
								data[1] = 0x10;
								data[2] = 0;
								data[3] = 0;
								data[4] = 0;
								data[5] = 0;
								CRC16 = CRC_MODBUS_LSBfirst_ALL1(4,data);
								data[4] =  CRC16 & 0xff;
								data[5] = (CRC16 >> 8)& 0xff;
								data[6] = 0;
							}else if(!(strcmp(MDEV[num].MCMD[cmdn].CMD[i].cmd,"ACT"))){
								data[1] = 0x35;
								data[2] = 0;
								data[3] = 1;
								data[4] = 0x80;
								data[5] = 0;
								data[6] = 0;
								CRC16 = CRC_MODBUS_LSBfirst_ALL1(5,data);
								data[5] =  CRC16 & 0xff;
								data[6] = (CRC16 >> 8)& 0xff;
								data[7] = 0;
							}else if(!(strcmp(MDEV[num].MCMD[cmdn].CMD[i].cmd,"READ"))){
								data[1] = 0x30; //BASIC INFO
								data[2] = 0;
								data[3] = 0;
								data[4] = 0;
								data[5] = 0;
								CRC16 = CRC_MODBUS_LSBfirst_ALL1(4,data);
								data[4] =  CRC16 & 0xff;
								data[5] = (CRC16 >> 8)& 0xff;
								data[6] = 0;
							}
						}
						if(UHDEV.size() > 0){
							if(UHDEV[ch].handle != NULL){
								libusb_bulk_transfer(UHDEV[ch].handle,UHDEV[ch].EPOUT,
								 data,len,&data_len, 1000);
								r = 1;
							}
						}
						break;
					default:
						r = 0;
						break;
				}
			}else if(MDEV[num].MCMD[cmdn].CMD[i].type == 1){/*SLEEP*/
				if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
				Sleep(1000 * (int)lua_tointeger(L,2-(MDEV[num].MCMD[cmdn].CMD[i].param)));
				}else{
				Sleep(1000 * MDEV[num].MCMD[cmdn].CMD[i].param);
				}
			}else if(MDEV[num].MCMD[cmdn].CMD[i].type == 3){/*MWAIT*/
				//	timewait_flg = SCN[num].S[list].data;
				//	timewait_flg = timewait_flg*(100/Interval);
				//	timewait_flg = 1000;
			//	Sleep(100);
				if(MDEV[num].MCMD[cmdn].CMD[i].param < 0){
				Sleep(100 * (int)lua_tointeger(L,2-(MDEV[num].MCMD[cmdn].CMD[i].param)));
		//	timewait_flg = (int)lua_tointeger(L,2-(MDEV[num].MCMD[cmdn].CMD[i].param));
				}else{
				Sleep(100 * MDEV[num].MCMD[cmdn].CMD[i].param);
			//		timewait_flg =	MDEV[num].MCMD[cmdn].CMD[i].param;
				}
			}else if(MDEV[num].MCMD[cmdn].CMD[i].type == 2){/*FUNC*/
				//
			}
		}
	}
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int uSleep(lua_State * L)
{
	int r = 0;
	int time = (int)lua_tonumber(L, 1);

//	timewait_flg = time*(1000/Interval);
	
	Sleep((1000/Interval) * time);
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int mSleep(lua_State * L)
{
	int r = 0;
	int time = (int)lua_tonumber(L, 1);

	timewait_flg = time*(100/Interval);
	
//	Sleep(1000 * time);
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

/*--------------------------------------------------------------------------------*/
/* Lua実行を一時停止し、特定のキー入力(リターン）待ち状態にする                   */
/* 本体タイマーイベントで毎回Lua内のLoop関数を呼び出して実行しているが、これを    */
/*停止する。キー入力があると keywit_flgが0にクリアされる                          */
/*--------------------------------------------------------------------------------*/
int KeyWait(lua_State * L)
{
	int r = 0;
	keywait_flg = 1;
	
//	if(keywait_flg > 0) r = 1;
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}


int RegRead(lua_State * L)
{
	long data;
	int RID;
	int i;
//	int i,j;
	if(lua_type(L,1) == LUA_TNUMBER){
		RID = (int)lua_tonumber(L, 1);
	}else if(lua_type(L,1) == LUA_TNUMBER){
		RID = 0;
		for(i=0;i<REG.size();i++){
			if(!(strcmp(REG[i].name,lua_tostring(L, 1)))){
				RID = i;
			}
		}
	}else{
		RID = 0;
	}
#if 0
	int MID = (int)lua_tonumber(L, 2);
	int addr = (int)lua_tonumber(L, 3);
#else
	int addr = (int)lua_tonumber(L, 2);
#endif
//	int data = (long)lua_tonumber(L, 4);
	data = 0;
#if 0
	if((MID > 0)&&(MID < 7)){
		for(i=0;i<REG[RID].PART[MID].isize;i++){
			if((addr >= REG[RID].PART[MID].SUB[i].start)
			 &&(addr <= REG[RID].PART[MID].SUB[i].start 
			  + REG[RID].PART[MID].SUB[i].isize)){
				j = addr - REG[RID].PART[MID].SUB[i].start;
				data = REG[RID].PART[MID].SUB[i].DATA[j];
			}
		}
	}
#else
	if(RID < REG.size()){
		if((REG[RID].start <= addr)&&((REG[RID].start + REG[RID].isize) > addr)){
			data = REG[RID].DATA[addr];
		}
	}
#endif
    // 結果をスタックに戻す
    lua_pushinteger(L, data);
    return 1; // 戻り値の数を返す
}
int RegWrite(lua_State * L)
{
	unsigned int r;
//	int i,j,k;
    int i;
	int RID;
	if(lua_type(L,1) == LUA_TNUMBER){
		RID = (int)lua_tonumber(L, 1);
	}else if(lua_type(L,1) == LUA_TNUMBER){
		RID = 0;
		for(i=0;i<REG.size();i++){
			if(!(strcmp(REG[i].name,lua_tostring(L, 1)))){
				RID = i;
			}
		}
	}else{
		RID = 0;
	}
#if 0
	int MID = (int)lua_tonumber(L, 2);
	int addr = (int)lua_tonumber(L, 3);
	int data = (long)lua_tonumber(L, 4);
#else
	int addr = (int)lua_tonumber(L, 2);
	long data = (long)lua_tonumber(L, 3);
#endif
	r = 0;
#if 0
	if((MID > 0)&&(MID < 7)){
		for(i=0;i<REG[RID].PART[MID].isize;i++){
			if((addr >= REG[RID].PART[MID].SUB[i].start)
			 &&(addr <  REG[RID].PART[MID].SUB[i].start 
			  + REG[RID].PART[MID].SUB[i].isize)){
				j = addr - REG[RID].PART[MID].SUB[i].start;
				REG[RID].PART[MID].SUB[i].DATA[j] = data;
				r = 1;
			}
		}
	}else{
		if(MID == 0x0F){
			for(k=0;k<REG[RID].isize;k++){
				for(i=0;i<REG[RID].PART[k].isize;i++){
					if((addr >= REG[RID].PART[k].SUB[i].start)
					 &&(addr <  REG[RID].PART[k].SUB[i].start 
					  + REG[RID].PART[k].SUB[i].isize)){
						j = addr - REG[RID].PART[k].SUB[i].start;
						REG[RID].PART[k].SUB[i].DATA[j] = data;
						r = 1;
					}
				}
			}
		}
	}
#else
	if(RID < REG.size()){
		if((REG[RID].start <= addr)&&((REG[RID].start + REG[RID].isize) > addr)){
			REG[RID].DATA[addr] = data;
			r = 1;
		}
	}
#endif
    // 結果をスタックに戻す
    lua_pushinteger(L, r);
    return 1; // 戻り値の数を返す
}

int CRC16(lua_State * L)
{
//	int i,n,len;
	int len;
//	unsigned char C_dummy[512];
	unsigned short CRC16;
//	unsigned short n;
	size_t len2;

	unsigned char * Buf = (unsigned char *)lua_tolstring(L, 1,&len2);
	len = (int)lua_tonumber(L, 2);
//	n = len - 2;
//	n = len;
	if(len > len2)len = (int)len2;
//	for(i=0;i<n;i++)C_dummy[i] = Buf[i];
//	for(i=n;i<n+2;i++)C_dummy[i] = 0;
//	CRC16 = CRC_CCITT_LSBfirst_ALL1(n+2, C_dummy);
//	CRC16 = CRC_CCITT_LSBfirst_ALL1(n, C_dummy);
//	CRC16 = CRC_CCITT_LSBfirst_ALL1(len, Buf);
	CRC16 = CalCRC_CMD(len, Buf);
	Buf[len  ] = (char)( CRC16      & 0xff);
	Buf[len+1] = (char)((CRC16 >> 8)& 0xff);
	Buf[len+2] = 0xff;
#if 0
	C_dummy[n  ] = (char)( CRC16      & 0xff);
	C_dummy[n+1] = (char)((CRC16 >> 8)& 0xff);
//	strcpy_s(Buf,n+2,(char *)C_dummy);
	strcpy_s(Buf,((size_t)n)+2,(char *)C_dummy);
#endif
    // 結果をスタックに戻す
  	lua_pop(L, lua_gettop(L));
    lua_pushinteger(L, CRC16);
	lua_pushlstring (L,(char *)Buf,len+3);
    return 2; // 戻り値の数を返す
}
#if 1
/*呼び出すと1フレームづつテレメトリを作る*/
//void mkframe(unsigned long long * TLMDATA, unsigned char * buf)
int mkframe(lua_State * L)
{
	int i,j,r;
	r = 0;
	long long TLMD;
	size_t len,len2;
	
//	char * Buf = (char *)lua_tostring(L, 1);
//	len = (int)lua_tonumber(L, 2);
	len = (size_t)lua_tonumber(L, 2);
	char * Buf = (char *)lua_tolstring(L, 1, &len2);
//	char * Buf = (char *)lua_tostring(L, 1);
//	long long * TLMD = (long long *)lua_topointer(L, 3);
//	long long * TLMD = (long long *)lua_tostring(L, 3);
	for(i=0;i<len2;i++)Buf[i] = 0;
	for(i=0;(unsigned int)i<TLM.size();i++){
	//	size = TLM[i].size;
		for(j=0;j<TLM[i].size;j++){
			lua_pushinteger(L,i+1);
			lua_gettable(L, 3);
			TLMD = (__int64)lua_tonumber(L,-1);
			lua_pop(L,1);
		//	Buf[(TLM[i].PART[j].major)] |= ((TLMD[i] >> TLM[i].PART[j].shift)
		//				 	     		   & TLM[i].PART[j].mask);
			if((TLM[i].PART[j].major) < len2){
			Buf[(TLM[i].PART[j].major)] |= ((TLMD >> TLM[i].PART[j].shift)
						 	     		   & TLM[i].PART[j].mask);
			}
		}
	}
	r = (int)len;
  	lua_pop(L, lua_gettop(L));
	lua_pushlstring (L,(char *)Buf,r);
    return 1; // 戻り値の数を返す
}
#endif

int GetAddr(lua_State * L)
{
	int index;
	long addr = -1;
	size_t len2;

	char * Buf = (char *)lua_tolstring(L, 1, &len2);
	index = LM.serch(Buf);
	addr = LM.LU[index].addr;
  	lua_pop(L, lua_gettop(L));
	lua_pushnumber(L, addr);
	return 1;
}

/*--------------------------------------------------------------------------------*/
/* LMクラスに登録した変数に値をセットする                                         */
/* 引数1:変数名 2:種別 3:値                                                       */
/* これでLuaスクリプト内からC++に値を渡す                                         */
/* 変数名はシナリオスクリプト内にDEFキーワードの後ろに書いて定義する              */
/* 種別:0 整数 1:浮動小数点数                                                     */
/*--------------------------------------------------------------------------------*/
int SetC(lua_State * L)
{
	int index,type;
	int r = 0;
	long num = 0;
	double fnum = 0.0;
	size_t len2;

	char * Buf = (char *)lua_tolstring(L, 1, &len2);
	type = (int)lua_tonumber(L, 2);
	if(type == 0){
		num = (long)lua_tonumber(L, 3);
	}else{
		fnum = (double)lua_tonumber(L, 3);
	}
	index = LM.serch(Buf);
	LM.LU[index].num = num;
	LM.LU[index].fnum = fnum;
	
//	if(!(strcmp(Buf,"RcvWait"))){
//		keywait_flg = (int)lua_tonumber(L, 3);
//	}

  	lua_pop(L, lua_gettop(L));
	lua_pushnumber(L, r);
	return 1;
}
/*--------------------------------------------------------------------------------*/
/* LMクラスに登録した変数から値を取り出す                                         */
/* 引数1:変数名                                                                   */
/* 戻り値 num 値 値は登録された種別に従って整数か浮動小数点数が返る               */
/*--------------------------------------------------------------------------------*/
int GetC(lua_State * L)
{
	int index;
	long num = 0;
	double fnum = 0.0;
	size_t len2;

	char * Buf = (char *)lua_tolstring(L, 1, &len2);
	index = LM.serch(Buf);
  	lua_pop(L, lua_gettop(L));
	if(index != -1){
		if(LM.LU[index].type == 0){
			num = LM.LU[index].num;
  		//	lua_pushnumber(L, num);
  			lua_pushinteger(L, num);
		}else{
			fnum = LM.LU[index].fnum;
			lua_pushnumber(L, fnum);
		}
	}else{
	//	lua_pushnumber(L, num);
		lua_pushinteger(L, num);
	}
	return 1;
}
