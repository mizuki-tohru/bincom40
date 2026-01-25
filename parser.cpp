#include "stdafx.h"
#include "bincom40.h"
#include "parser.h"

/*--------------------------------------------------------------------------*/
using namespace std;
/*--------------------------------------------------------------------------*/
void rchar(FILE *);
void cgetchar(FILE *);
/*--------------------------------------------------------------------------*/
int scene_label;
int nest_level;

/*-------------------------------------------------------------------------*/
void rchar(FILE * F)
{
	char ch,ch2,ch3,ch4;

	ch = (char)(fgetc(F) &0xff);
	if(ch > 0x1f && ch < 0x7f){			/*もし1バイト文字なら*/
		if( ((ch >= 'a')&&(ch <= 'z'))||((ch >= 'A')&&(ch <= 'Z'))
		  ||(ch == '_')){	/*文字*/
			C_C.flg = 1;
			int i = (int)(strlen(C_C.text));
            C_C.text[i] = ch;
			C_C.text[i+1] = 0;
			i = (int)(wcslen(C_C.Wtext));
            C_C.Wtext[i] = ch;
			C_C.Wtext[i+1] = 0;
			rchar(F);
		}else if((ch >= '0')&&(ch <= '9')){	/*数*/
			if(C_C.flg != 1)C_C.flg = 2;
			int i = (int)(strlen(C_C.text));
            C_C.text[i] = ch;
			C_C.text[i+1] = 0;
			i = (int)(wcslen(C_C.Wtext));
            C_C.Wtext[i] = ch;
			C_C.Wtext[i+1] = 0;
			rchar(F);
		}else{
			/*ドットは数列に含める*/
			if((C_C.flg == 1)||(C_C.flg == 2)){
				if((C_C.flg == 2)&&(ch == '.')){
					int i = (int)(strlen(C_C.text));
		            C_C.text[i] = ch;
					C_C.text[i+1] = 0;
					i = (int)(wcslen(C_C.Wtext));
        		    C_C.Wtext[i] = ch;
					C_C.Wtext[i+1] = 0;
					rchar(F);
				}else{
					ungetc(ch,F);
				}
			}else{
				C_C.text[0] = 0;
				if((ch == ' ')||(ch == '\t')){ /*空白*/
					C_C.flg = 0;
					for(;;){
						ch = (char)(fgetc(F) &0xff);
						if((ch != ' ')&&(ch != '\t')){
							ungetc(ch,F);
							break;
						}
					}
					rchar(F);
				}else if(ch == '#'){	/*コメント*/
					while(((char)(fgetc(F) &0xff))!=0x0a);
					C_C.flg = 0;
					/*行末まで空読み*/
					rchar(F);
				}else if(ch == '='){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '='){
						C_C.flg = 11;	/* '==' */
						C_C.level = 4;
					}else{
						ungetc(ch,F);
						C_C.flg = '=';
						C_C.level = 1;
					}
				}else if(ch == '!'){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '='){
						C_C.flg = 12;	/* '!=' */
						C_C.level = 4;
					}else{
						ungetc(ch,F);
						C_C.flg = '!';
						C_C.level = 8;
					}
				}else if(ch == '>'){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '='){
						C_C.flg = 13;	/* '>=' */
					}else{
						ungetc(ch,F);
						C_C.flg = '>';
					}
					C_C.level = 5;
				}else if(ch == '<'){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '='){
						C_C.flg = 14;	/* '<=' */
					}else{
						ungetc(ch,F);
						C_C.flg = '<';
					}
					C_C.level = 5;
				}else if(ch == '('){	/*丸括弧はネストだけ記憶して*/
					if(scene_label == 1){
						scene_label = 2;
						nest_level = 0;
					}else{
						nest_level++;
					}
					C_C.flg = 0;
					rchar(F);			/*読み飛ばす*/
				}else if(ch == ')'){
					nest_level--;
					if(nest_level < 0)nest_level = 0;
					if(nest_level == 0 && scene_label){
						C_C.flg = 18;
					}else{
						C_C.flg = 0;
						rchar(F);			/*読み飛ばす*/
					}
				}else if(ch == '&'){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '&'){
						C_C.flg = 15;	/* '&&' */
						C_C.level = 2;
					}else{
						ungetc(ch,F);
						C_C.flg = '&';
						C_C.level = 3;
					}
				}else if(ch == '|'){
					ch = (char)(fgetc(F) &0xff);
					if(ch == '|'){
						C_C.flg = 16;	/* '||' */
						C_C.level = 2;
					}else{
						ungetc(ch,F);
						C_C.flg = '|';
						C_C.level = 3;
					}
				}else if(ch == '-'){
					C_C.flg = '-';
					C_C.level = 6;
				}else if(ch == '+'){
					C_C.flg = '+';
					C_C.level = 6;
				}else if(ch == '*'){
					C_C.flg = '*';
					C_C.level = 7;
				}else if(ch == '/'){
					C_C.flg = '/';
					C_C.level = 7;
				}else if(ch == '%'){
					C_C.flg = '%';
					C_C.level = 7;
				}else if(ch == '{'){
                    scene_label = 0;
					C_C.flg = '{';
					C_C.level = 0;
				}else if(ch == '}'){
					C_C.flg = '}';
					C_C.level = 0;
				}else if(ch == '.'){
					C_C.flg = '.';
					C_C.level = 0;
				}else if(ch == '"'){
					int i = 0;
					C_C.flg = 19; /*囲った範囲は文字列*/
					ch = (char)(fgetc(F) &0xff);
					while(ch != '"'){
			            C_C.text[i] = ch;
						if(ch > 0x1f && ch < 0x7f){			/*もし1バイト文字なら*/
							i = (int)(wcslen(C_C.Wtext));
        				    C_C.Wtext[i] = ch;
							C_C.Wtext[i+1] = 0;
						}else{
							if(((ch & 0xff) >= 0xC2) && ((ch & 0xff) <= 0xDF)){
							/*UTF-8の2バイトキャラなら*/
								int i = (int)(strlen(C_C.text));
  								C_C.text[i] = ch;
								ch2 = (char)(fgetc(F) &0xff);/*一気に2バイト*/
 								C_C.text[i+1] = ch2;
								C_C.text[i+2] = 0;
								i = (int)wcslen(C_C.Wtext);
							    C_C.Wtext[i] = ((ch & 0x1f)<< 6)|(ch2 & 0x3f);
								C_C.Wtext[i+1] = 0;
							}else if(((ch & 0xff) >= 0xE0) && ((ch & 0xff) <= 0xEF)){
	 						/*UTF8の3バイトキャラなら*/
								int i = (int)(strlen(C_C.text));
            					C_C.text[i] = ch;
								ch2 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 								C_C.text[i+1] = ch2;
								ch3 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 								C_C.text[i+2] = ch3;
								C_C.text[i+3] = 0;
								i = (int)wcslen(C_C.Wtext);
	        					C_C.Wtext[i] = ((ch & 0x0f) << 12)
	        								|((ch2 & 0x3f) << 6)|(ch3 & 0x3f);
								C_C.Wtext[i+1] = 0;
							}else if(((ch & 0xff) >= 0xF0) && ((ch & 0xff) <= 0xF4)){
	 						/*UTF8の4バイトキャラなら*/
								int i = (int)(strlen(C_C.text));
            					C_C.text[i] = ch;
								ch2 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 								C_C.text[i+1] = ch2;
								ch3 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 								C_C.text[i+2] = ch3;
								ch4 = (char)(fgetc(F) &0xff);/*一気に4バイト*/
 								C_C.text[i+3] = ch4;
								C_C.text[i+4] = 0;
								i = (int)wcslen(C_C.Wtext);
		    					C_C.Wtext[i] = ((ch & 0x07) << 18)|((ch2 & 0x3f) << 12)
											 |((ch3 & 0x3f) << 6)|(ch4 & 0x3f);
								C_C.Wtext[i+1] = 0;
			            	}
			            }
						ch = (char)(fgetc(F) &0xff);
						i++;
					}
					C_C.text[i] = 0;
				}else{
					C_C.text[0] = 0;
					C_C.flg = 0;
					rchar(F);
				}
			}
		}
	}else{
	//	if(((ch & 0xff) > 0x80) && ((ch & 0xff) < 0x9a)){
	//	/*シフトJISの2バイトキャラなら*/
		if(((ch & 0xff) >= 0xC2) && ((ch & 0xff) <= 0xDF)){
		/*UTF-8の2バイトキャラなら*/
			C_C.flg = 1;
			int i = (int)(strlen(C_C.text));
            C_C.text[i] = ch;
			ch2 = (char)(fgetc(F) &0xff);/*一気に2バイト*/
 			C_C.text[i+1] = ch2;
			C_C.text[i+2] = 0;
			i = (int)wcslen(C_C.Wtext);
		    C_C.Wtext[i] = ((ch & 0x1f)<< 6)|(ch2 & 0x3f);
			C_C.Wtext[i+1] = 0;
            rchar(F);
	//	}else if(((ch & 0xff) > 0xdf) && ((ch & 0xff) < 0xf0)){
	//	/*シフトJISの2バイトキャラなら*/
	//		C_C.flg = 1;
	//		int i = strlen(C_C.text);
    //      C_C.text[i] = ch;
	//		ch = (char)(fgetc(F) &0xff);/*一気に2バイト*/
 	//		C_C.text[i+1] = ch;
	//		C_C.text[i+2] = 0;
	//		rchar(F);
		}else if(((ch & 0xff) >= 0xE0) && ((ch & 0xff) <= 0xEF)){
	 	/*UTF8の3バイトキャラなら*/
			C_C.flg = 1;
			int i = (int)(strlen(C_C.text));
            C_C.text[i] = ch;
			ch2 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 			C_C.text[i+1] = ch2;
			ch3 = (char)(fgetc(F) &0xff);/*一気に3バイト*/
 			C_C.text[i+2] = ch3;
			C_C.text[i+3] = 0;
			i = (int)wcslen(C_C.Wtext);
	        C_C.Wtext[i] = ((ch & 0x0f) << 12)|((ch2 & 0x3f) << 6)|(ch3 & 0x3f);
			C_C.Wtext[i+1] = 0;
            rchar(F);
		}else if(((ch & 0xff) >= 0xF0) && ((ch & 0xff) <= 0xF4)){
	 	/*UTF8の4バイトキャラなら*/
			C_C.flg = 1;
			int i = (int)(strlen(C_C.text));
            C_C.text[i] = ch;
			ch2 = (char)(fgetc(F) &0xff);/*一気に4バイト*/
 			C_C.text[i+1] = ch2;
			ch3 = (char)(fgetc(F) &0xff);/*一気に4バイト*/
 			C_C.text[i+2] = ch3;
			ch4 = (char)(fgetc(F) &0xff);/*一気に4バイト*/
 			C_C.text[i+3] = ch4;
			C_C.text[i+4] = 0;
			i = (int)wcslen(C_C.Wtext);
		    C_C.Wtext[i] = ((ch & 0x07) << 18)|((ch2 & 0x3f) << 12)
					 |((ch3 & 0x3f) << 6)|(ch4 & 0x3f);
			C_C.Wtext[i+1] = 0;
            rchar(F);
		}else{
			if((C_C.flg == 1)||(C_C.flg == 2)){
				ungetc(ch,F);
			}else{
				if(ch == 0x0a){					/*改行文字なら*/
					C_C.flg = 0x0a;
				}else if(ch == EOF){			/*ファイル終端で*/
					C_C.flg = EOF;
					return;						/*終了*/
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------*/
void cgetchar(FILE * F)
{
	long len,count,i,j;
	count = 0;

	char asc[]={'0','1','2','3','4','5','6','7','8','9',
				'A','B','C','D','E','F',
				'a','b','c','d','e','f','Q'};

	C_C.flg = 0;
	C_C.text[0] = 0;
	C_C.Wtext[0] = 0;
	C_C.data = 0;

	rchar(F);
//	if(C_C.flg == 1){/*文字列だったら*/
	if((C_C.flg == 1)||(C_C.flg == 19)){/*文字列だったら*/
		if(C_C.text[0] == '0'){
			if((C_C.text[1] == 'x')||(C_C.text[1] == 'X')){
				len = ((long)(strlen(C_C.text))) - 2;
				for(i=0;i<len;i++){
					if((C_C.text[2+i] >= '0')&&(C_C.text[2+i] <= '9')){	/*数*/
						count++;
					}else if((C_C.text[2+i] >= 'a')&&(C_C.text[2+i] <= 'f')){							count++;
					}else if((C_C.text[2+i] >= 'A')&&(C_C.text[2+i] <= 'F')){							count++;
					}
				}
				if(count == len){
					for(i=0;i<len;i++){
						for(j=0;j<23;j++){
							if(asc[j] == C_C.text[2+i])break;
						}
						if(j>15){
							j-=6;
						}
						C_C.data |= j <<(((len-1)-i)*4);
					}
					C_C.flg = SC_NUM;
				}
			}else if((C_C.text[1] == 'b')||(C_C.text[1] == 'B')){
				len = ((long)(strlen(C_C.text))) - 2;
				for(i=0;i<len;i++){
					if((C_C.text[2+i] == '0')||(C_C.text[2+i] == '1')){
						count++;
					}
				}
				if(count == len){
				//	for(i=0;i<(len*8);i++){
					for(i=0;i<len;i++){
						if(C_C.text[2+i] == '1'){
							C_C.data |= 1<<((len-1)-i);
						}
					}
					C_C.flg = SC_NUM;
				}
			}
		}
	}else if(C_C.flg == SC_NUM){
		C_C.data = atol(C_C.text);
	}
}

