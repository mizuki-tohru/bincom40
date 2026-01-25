-- Luaスクリプト
--日本語コメントはUTF-8で

--グローバル変数
count = 0
Buf2 ="                         "
Send ="                                                                                                                                                                                                                                                                                                                                                                                                   "
SPIflg = 0
OBCCnt = 0
OBCRCnt = 0
STRXCnt = 0
SA_V_L = 0.0
SA_V_R = 0.0
SA_I_L = 0.0
SA_I_R = 0.0
BAT_V = 0.0
BAT_I = 0.0
BAT_T = 0.0
SA_V_L_GOAL = 0.0
SA_V_R_GOAL = 0.0
BAT_V_GOAL = 0.0
P5V = 0.0
P15V = 0.0
M15V = 0.0
P48V = 0.0
BUS_V = 0.0
LOAD_I = 0.0
XPA_I = 0.0
PCDU_T = 0.0
CMDTbl = {0,0,0,0,0,0,0,0,0,0,0,0}
CMDTbl_Cnt = 1
CtrlMode = 0
CtrlModeOld = 0
CtrlMode_Cnt = 0
CtrlMode_For = 0
PWMMode = 0
StepDuty = 0
OldDuty_L = 0
OldDuty_R = 0
DiffDuty_L = 0
DiffDuty_R = 0
DiffDuty_La = 0
DiffDuty_Ra = 0
OldPWR_L = 0
OldPWR_R = 0
STRX_First_flg = 0
TLMDigit_flg = 0
UVC_Cnt = 0
XUVC_Cnt = 0
BAT_HTR_ERR = 0
BAT_HTR_Cnt = 0
MTQ_XD = 0
MTQ_YD = 0
MTQ_ZD = 0
MTQ_XC = 0
MTQ_YC = 0
MTQ_ZC = 0
MTQ_XCnt = 0
MTQ_YCnt = 0
MTQ_ZCnt = 0
MTQ_PWR = 0
MTQ_FF = 0
MTQ_XF = 0
MTQ_YF = 0
MTQ_ZF = 0
--テーブル定義
TlmTbl = {}
rly = {{"Err X0Y0","AHDRM#1 ON","AHDRM#2 ON","AHDRM#3 ON","AHDRM#4 ON",
        "OBP ON","OBP OFF","Err X0Y7","Err X0Y8","Err X0Y9"},
       {"Err X1Y0","BUS OFF","HW_Fix ENA","GYRO ON","AHDRM_P ON",
        "AHDRM_R ON","PPU1 ON","PPU2 ON","FOG ON","MTQ ON"},
       {"Err X2Y0","SepSW OFF","HW_Fix DIS","GYRO OFF","AHDRM_P OFF",
        "AHDRM_R OFF","PPU1 OFF","PPU2 OFF","FOG OFF","MTQ OFF"},
       {"Err X3Y0","HTR1 ON","HTR5 ON","HTR8 ON","RW_4 ON",
        "PPU6 ON","PPU5 ON","MAGS2 ON","GPSR1 ON","GPSR2 ON"},
       {"Err X4Y0","HTR1 OFF","HTR5 OFF","HTR8 OFF","RW_4 OFF",
        "PPU6 OFF","PPU5 OFF","MAGS2 OFF","GPSR1 OFF","GPSR2 OFF"},
       {"PPU3 ON","HTR2 ON","HTR6 ON","STT3 ON","IDRS ON",
        "S-ELU ON","S-ELU OFF","XPA OFF","STT1 ON","STT2 ON"},
       {"PPU3 OFF","HTR2 OFF","HTR6 OFF","STT3 OFF","Err X6Y4",
        "BUS OFF","XPA ON","XTX ON","STT1 OFF","STT2 OFF"},
       {"PPU4 ON","HTR3 ON","HTR7 ON","RW_1 ON","RW_3 ON",
        "Err X7Y5","IDRS OFF","XTX OFF","MAGS1 ON","MDR ON"},
       {"PPU4 OFF","HTR3 OFF","HTR7 OFF","RW_1 OFF","RW_3 OFF",
        "OBC_MAIN ON","STRX ON","BUS ON","MAGS1 OFF","MDR OFF"},
       {"Err X9Y0","HTR4 ON","HTR4 OFF","RW_2 ON","RW_2 OFF",
        "SepSW ON","OBC_MAIN OFF","STRX OFF","OBC_RTC ON","OBC_RTC OFF"}}

onoff = {{  0,  0,  0,  0,  0, 36,-36,  0,  0,  0},
         {  0,  0, 49, 30, 25, 26, 27, 28, 29,  0},
         {  0,  0,-49,-30,-25,-26,-27,-28,-29,  0},
         {  0,  5,  9, 13, 21, 51, 50, 32, 33, 34},
         {  0, -5, -9,-13,-21,-51,-50,-32,-33,-34},
         {  4,  6, 11, 37, 35,  3, -3, -1, 16, 17},
         { -4, -6,-11,-37,  0,  0,  1,  2,-16,-17},
         { 10,  7, 12, 18, 20,  0,-35, -2, 31, 24},
         {-10, -7,-12,-18,-20, 15, 23,  0,-31,-24},
         {  0,  8, -8, 19,-19,  0,-15,-23, 14,-14}}

function str(x)
  return x .. ""
end

--初期化関数。この名前の関数は必ず置く。この関数は最初に一度だけ実行される
function Startup()
	RegWrite(1,0x0000,21)
	for i = 0, 209, 1 do
		TlmTbl[i] = 0
	end
	TlmTbl[1] = 0x78
	TlmTbl[2] = 0x01
	TlmTbl[3] = 387
	TlmTbl[4] = 0
	TlmTbl[5] = 0
	TlmTbl[6] = 0
	TlmTbl[7] = 0x71 --制御モード 日陰時制御ﾓｰﾄﾞ | PPT制御
	TlmTbl[8] = 0x33 --初期Duty値
	TlmTbl[9] = 0 --HDRM_CMD_COUNT
	TlmTbl[10] = 0xFF --固定Duty
	TlmTbl[11] = 2 --Duty刻み幅
	TlmTbl[12] = 0 --PCDU駆動Duty値_SAP-L
	TlmTbl[13] = 0 --PCDU駆動Duty値_SAP-R
	TlmTbl[14] = 0x041A --CD比
	TlmTbl[15] = 50*100 --UVC閾値
	TlmTbl[16] = 48*100 --XUVC閾値
	TlmTbl[17] = 45*100 --OT High閾値
	TlmTbl[18] = 35*100 --OT Low閾値
	TlmTbl[19] = 25*100 --CC値
	TlmTbl[20] = 64*100 --CV値
	TlmTbl[21] = 150 --CCヒステリシス値
	TlmTbl[22] = 5*100 --CVヒステリシス値
	TlmTbl[23] = 50 --駆動Duty切替回数
	TlmTbl[24] = 0x01 + 0x04 + 0x08
	TlmTbl[26] = 0x01 + 0x02 + 0x04 + 0x20
	TlmTbl[28] = 10 --日陰判定電流閾値
	TlmTbl[29] = 0x02F0 --VOCオフセット
	TlmTbl[30] = 0x4680 --VOC

	TlmTbl[36] = 0 --無コマンド時間
	TlmTbl[37] = 0 --OBC OFF時間
	TlmTbl[38] = 5*100 --+5Vバス電圧
	TlmTbl[39] = 15*100 --+15Vバス電圧
	TlmTbl[40] = -15*100 ---15Vバス電圧
	TlmTbl[41] = 48*100 --+48Vバス電圧
	P5V = 5*100
	P15V = 15*100
	M15V = -15*100
	P48V = 48*100
--日照時
--	TlmTbl[42] = (60.0-0.0)/0.04                -- SAP-L_V
--	TlmTbl[43] = (10.0-0.326563122)/0.001632816 -- SAP-L_I
--	TlmTbl[44] = (60.0-0.0)/0.04                -- SAP-R_V
--	TlmTbl[45] = (10.0-0.326563122)/0.001632816 -- SAP-R_I
--	TlmTbl[46] = (60.0-0.0)/0.04 -- 一次バス電圧
--	TlmTbl[42] = 60*100 -- SAP-L_V
--	TlmTbl[43] = 10*100 -- SAP-L_I
--	TlmTbl[44] = 60*100 -- SAP-R_V
--	TlmTbl[45] = 10*100 -- SAP-R_I
--	TlmTbl[46] = 60*100 -- 一次バス電圧
--日陰時
--	TlmTbl[42] = (0.0-0.0)/0.04  -- SAP-L_V
--	TlmTbl[43] = (0.0-0.326563122)/0.001632816 -- SAP-L_I
--	TlmTbl[44] = (0.0-0.0)/0.04  -- SAP-R_V
--	TlmTbl[45] = (0.0-0.326563122)/0.001632816 -- SAP-R_I
--	TlmTbl[46] = (48.0-0.0)/0.04 -- 一次バス電圧
-----------------

	TlmTbl[42] = 0 -- SAP-L_V
	TlmTbl[43] = 0 -- SAP-L_I
	TlmTbl[44] = 0 -- SAP-R_V
	TlmTbl[45] = 0 -- SAP-R_I
--------------試験時
--	TlmTbl[42] = 60*100 -- SAP-L_V
--	TlmTbl[43] = 5*100 -- SAP-L_I
--	TlmTbl[44] = 60*100 -- SAP-R_V
--	TlmTbl[45] = 5*100 -- SAP-R_I
------------------
	TlmTbl[46] = 48*100 -- 一次バス電圧
	BUS_V = 48*100
--
--	TlmTbl[47] = (10.0-75.0)/0.028571429 -- LOAD_I
--	TlmTbl[48] = (10.0-75.0)/0.028571429 -- XPA_I
--	TlmTbl[49] = (48.0-0.0)/0.0205 -- BAT_V
--	TlmTbl[50] = (10.0-75.0)/0.028571429 -- BAT_I
	TlmTbl[47] = 10*100 -- LOAD_I
	TlmTbl[48] = 10*100 -- XPA_I
	LOAD_I = 10*100
	XPA_I  = 10*100
-----------------
	TlmTbl[49] = 0 -- BAT_V
	TlmTbl[50] = 0 -- BAT_I
--------------試験時
--	TlmTbl[49] = 60*100 -- BAT_V
--	TlmTbl[50] = 2*100 -- BAT_I
-------------------
--	TlmTbl[51] = (21.0-259.740297)/0.103896104 --PCDU温度
--	TlmTbl[52] = (21.0-32.14356721)/0.021322551 --BAT1温度
--	TlmTbl[53] = (21.0-32.14356721)/0.021322551 --BAT2温度
	TlmTbl[51] = 21*100 --PCDU温度
	TlmTbl[52] = 21*100 --BAT1温度
	TlmTbl[53] = 21*100 --BAT2温度
	TlmTbl[54] = 72 --SAT ENADIS
	TlmTbl[56] = 32+64  --OBC_Main OBC_RTC ON
	TlmTbl[183] = 200 --Duty強制加算連続回数閾値
	TlmTbl[184] = 150 --CC Over値
	TlmTbl[185] = 1*100 --CVオーバー値
	TlmTbl[186] = 2*100 --Duty刻み幅増幅数
	TlmTbl[187] = 0 --0ACC値
	TlmTbl[188] = 150 --0ACC Hyst値
	TlmTbl[189] = 150 --0ACC Over値
	TlmTbl[202] = 10
	TlmTbl[203] = 0
	TlmTbl[204] = -500
	TlmTbl[205] = 5000
	TlmTbl[206] = -2000
	PCDU_T = 21*100
	RegWrite(0,0x0040,4800)
	RegWrite(0,0x0050,1000)
	RegWrite(0,0x0060,3500)
	RegWrite(0,0x0100,TlmTbl[42])
	RegWrite(0,0x0110,TlmTbl[44])
--	BAT_V_GOAL = BAT_V
	SerialInit(0,"X",19200)
	DispStrP("Init OK")
end

function Loop()
--	len,Buf = SerialRecv(0,Buf)
	len,Buf2 = SerialRecv(0,Buf2)
	if(len > 0) then
	--	SerialSend(0,Buf,len)
--		DispStrP(str(len))
--		if(len > 7) then
		for i=0,12,1 do
			TlmTbl[191+i] = 0
		end
		for i=0,len,8 do
			local Buf = Buf2:sub(i*8,i*8+8)
			MID = string.byte(Buf,1,1)
			CID = string.byte(Buf,2,2)
			CRC = string.byte(Buf,7,7) + (string.byte(Buf,8,8) << 8 )
			CRC2,Buf = CRC16(Buf,6)
--			DispStrP(str(CRC2))
			if(CRC2 == CRC)then
				if(MID == 0x78)then
--					DispStrP(str(CID))
				--	TlmTbl[190+0] = CID
					CMDTbl[CMDTbl_Cnt] = CID
					if(CMDTbl_Cnt < 13)then
						CMDTbl_Cnt = CMDTbl_Cnt + 1
					else
						CMDTbl_Cnt = 1
					end
					for j=0,12,1 do
						if(CMDTbl_Cnt + j + 1 < 13)then
							TlmTbl[190+j] = CMDTbl[CMDTbl_Cnt + j + 1]
						else
							TlmTbl[190+j] = CMDTbl[(CMDTbl_Cnt + j + 1)-12]
						end
					end
					if(CID == 0x10)then
						X = string.byte(Buf,3,3)
						Y = string.byte(Buf,4,4)
						DispStrP(rly[X+1][Y+1])
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						if((X == 10)and(Y== 7))then
							TlmTbl[37] = 0 --OBC OFF
						end
						if((X == 1)and(Y== 9))then --MTQ ON
							TlmTbl[57] = TlmTbl[57] | 0x20 --X
							TlmTbl[61] = TlmTbl[61] | 0x08 --Y
							TlmTbl[61] = TlmTbl[61] | 0x10 --Z
							MTQ_PWR = 1
						end
						if((X == 2)and(Y== 9))then --MTQ Off
							TlmTbl[57] = TlmTbl[57] & ~0x20 --X
							TlmTbl[61] = TlmTbl[61] & ~0x08 --Y
							TlmTbl[61] = TlmTbl[61] & ~0x10 --Z
							MTQ_PWR = 0
						end
						if((X == 1)and(Y== 1))then --BUS OFF
							if((TlmTbl[26] & 0x10) == 0x10)then
								DispStrP("-- BUS OFF --")
								RegWrite(0,0x0080,1)
							end
						end
--					DispStrP(str(onoff[X+1][Y+1]))
						if(onoff[X+1][Y+1] > 0)then
							local j = math.ceil(onoff[X+1][Y+1] / 8) - 1
							TlmTbl[55+j] = TlmTbl[55+j] | (1 << ((math.ceil(onoff[X+1][Y+1])-1) %8))
				--	DispStrP(str((1 << ((math.ceil(onoff[X+1][Y+1])-1) %8))))
				--	DispStrP(str(TlmTbl[55+j]))
				--	DispStrP(str(j))
						--	TlmTbl[55] = TlmTbl[55] &  (1 << (56 - onoff[X][Y]))
						else
							local j = math.ceil((-1 * onoff[X+1][Y+1]) / 8) - 1
							TlmTbl[55+j] = TlmTbl[55+j] & ~(1 << ((-1 * (math.ceil(onoff[X+1][Y+1]))-1) %8))
				--	DispStrP(str(1 << ((-1 * (onoff[X+1][Y+1])-1) %8)))
				--	DispStrP(str(TlmTbl[55+j]))
				--	DispStrP(str(j))
						--	TlmTbl[55] = TlmTbl[55] & ~(1 << (56 + onoff[X][Y]))
						end
					elseif(CID == 0x11)then --MTQ電流方向
						if((TlmTbl[25] & 0x08) == 0x08)then --シリアルコマンド
							MTQ_XC = string.byte(Buf,3,3)
							MTQ_YC = string.byte(Buf,4,4)
							MTQ_ZC = string.byte(Buf,5,5)
							if(MTQ_XC > 127)then
								MTQ_XC = (255 - MTQ_XC)+1
								MTQ_XD = 1
							else
								MTQ_XD = 0
							end
							MTQ_XCnt = MTQ_XC
							if(MTQ_XCnt == 0)then
							--	TlmTbl[57] = TlmTbl[57] & ~0x20 --X
								TlmTbl[59] = TlmTbl[59] & ~0x20 --X
							end
							if(MTQ_YC > 127)then
								MTQ_YC = (255-MTQ_YC)+1
								MTQ_YD = 1
							else
								MTQ_YD = 0
							end
							MTQ_YCnt = MTQ_YC
							if(MTQ_YCnt == 0)then
							--	TlmTbl[61] = TlmTbl[61] & ~0x08 --Y
								TlmTbl[59] = TlmTbl[59] & ~0x40 --Y
							end
							if(MTQ_ZC > 127)then
								MTQ_ZC = (255 - MTQ_ZC)+1
								MTQ_ZD = 1
							else
								MTQ_ZD = 0
							end
							MTQ_ZCnt = MTQ_ZC
							if(MTQ_ZCnt == 0)then
							--	TlmTbl[61] = TlmTbl[61] & ~0x10 --Z
								TlmTbl[59] = TlmTbl[59] & ~0x80 --Z
							end
						end
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("MTQ CMD")
					elseif(CID == 0x20)then --初期Duty
						TlmTbl[8] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("First Duty")
					elseif(CID == 0x21)then --固定Duty
						TlmTbl[10] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("Fx Duty")
					elseif(CID == 0x22)then --Duty刻み幅
						TlmTbl[11] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("DUTY_DELTA")
					elseif(CID == 0x23)then --UVC閾値
						TlmTbl[15] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("UVC_THSD")
					elseif(CID == 0x24)then --XUVC閾値
						TlmTbl[16] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("XUVC_THSD")
					elseif(CID == 0x26)then --OTHigh閾値
						TlmTbl[17] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("OTH_THSD")
					elseif(CID == 0x27)then --OTLow閾値
						TlmTbl[18] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("OTL_THSD")
					elseif(CID == 0x28)then --CC値
						TlmTbl[19] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CC_I")
					elseif(CID == 0x29)then --CV値
						TlmTbl[20] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CV_V")
					elseif(CID == 0x2A)then --CCヒステリシス値
						TlmTbl[21] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CC_HYST")
					elseif(CID == 0x2B)then --CVヒステリシス値
						TlmTbl[22] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CV_HYST")
					elseif(CID == 0x2C)then --駆動Duty値切替回数
						TlmTbl[23] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("DUTY_CNTR")
					elseif(CID == 0x2E)then --CD比
						TlmTbl[14] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CD_RATE")
					elseif(CID == 0x31)then --日陰判定電流閾値
						TlmTbl[28] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("SHADE_LIMIT")
					elseif(CID == 0x34)then --VOCオフセット値
						TlmTbl[29] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("OFFSET_VOC")
					elseif(CID == 0x35)then --PCUDタイム
						TlmTbl[4] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8) + (string.byte(Buf,5,5) << 16) + (string.byte(Buf,6,6) << 24)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("PCDUTIME_SET")
					elseif(CID == 0x36)then --PCUD0タイムリセット
						TlmTbl[4] = 0
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("PCDUTIME_CLR")
					elseif(CID == 0x37)then --モード判定カウント数
						TlmTbl[32] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("MODE_INTERVAL")
					elseif(CID == 0x38)then --OT判定カウント数
						TlmTbl[33] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("OT_INTERVAL")
					elseif(CID == 0x39)then --UVC判定カウント数
						TlmTbl[34] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("UVC_INTERVAL")
					elseif(CID == 0x3A)then --XUVC判定カウント数
						TlmTbl[35] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("XUVC_INTERVAL")
					elseif(CID == 0x3E)then --Duty強制加算連続回数
						TlmTbl[183] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("FORCED_INTERVAL")
					elseif(CID == 0x3F)then --CCオーバー値
						TlmTbl[184] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CC_OVER")
					elseif(CID == 0x40)then --CVオーバー値
						TlmTbl[185] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CV_OVER")
					elseif(CID == 0x42)then --Duty刻み幅増幅数
						TlmTbl[186] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("DUTY_DELTA_MULTI_NUM")
					elseif(CID == 0x46)then --OACC値
						TlmTbl[187] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_0ACC_I")
					elseif(CID == 0x47)then --OACCヒステリシス値
						TlmTbl[188] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_0ACC_HYST")
					elseif(CID == 0x48)then --OACCオーバー値
						TlmTbl[189] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_0ACC_OVER")
					elseif(CID == 0x49)then --BATヒーターONOFF判定カウント数
						TlmTbl[202] = string.byte(Buf,3,3)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_BATHTR_INTERVAL")
					elseif(CID == 0x4A)then --BATヒーターOFF閾値
						TlmTbl[203] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_BATHTR_OFF_THSD")
					elseif(CID == 0x4B)then --BATヒーターON閾値
						TlmTbl[204] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_BATHTR_ON_THSD")
					elseif(CID == 0x4C)then --BAT温度センサ異常上限閾値
						TlmTbl[205] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_BATTEMP_ABN_H_THSD")
					elseif(CID == 0x4D)then --BAT温度センサ異常下限閾値
						TlmTbl[206] = string.byte(Buf,3,3) + (string.byte(Buf,4,4) << 8)
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_BATTEMP_ABN_L_THSD")
					elseif(CID == 0x50)then ---CC充電制御 有効
						TlmTbl[24] = TlmTbl[24] | 0x04
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CC_ENA")
					elseif(CID == 0x51)then --CC充電制御 無効
						TlmTbl[24] = TlmTbl[24] & ~0x04
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CC_DIS")
					elseif(CID == 0x52)then --CV充電制御 有効
						TlmTbl[24] = TlmTbl[24] | 0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CV_ENA")
					elseif(CID == 0x53)then --CV充電制御 無効
						TlmTbl[24] = TlmTbl[24] & ~0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_CV_DIS")
					elseif(CID == 0x54)then --CC充電制御 Duty再演算有効
						TlmTbl[24] = TlmTbl[24] | 0x20
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("JMPDUTY_CC_ENA")
					elseif(CID == 0x55)then --CC充電制御 Duty再演算無効
						TlmTbl[24] = TlmTbl[24] & ~0x20
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("JMPDUTY_CC_DIS")
					elseif(CID == 0x56)then --固定Duty制御 有効
						TlmTbl[24] = TlmTbl[24] | 0x10
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_MODE_FIX")
					elseif(CID == 0x57)then --固定Duty制御 無効
						TlmTbl[24] = TlmTbl[24] & ~0x10
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_MODE_AUTO")
					elseif(CID == 0x58)then --SW固定Duty制御 有効
						TlmTbl[26] = TlmTbl[26] | 0x20
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("SOFT_FIX_ENA")
					elseif(CID == 0x59)then --SW固定Duty制御 無効
						TlmTbl[26] = TlmTbl[26] & ~0x20
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("SOFT_FIX_ENA")
					elseif(CID == 0x5C)then --OT検出ENADISフラグ ENA
						TlmTbl[26] = TlmTbl[26] | 0x01
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_OT_ENA")
					elseif(CID == 0x5D)then --OT検出ENADISフラグ DIS
						TlmTbl[26] = TlmTbl[26] & ~0x01
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_OT_DIS")
					elseif(CID == 0x5E)then --UVC検出ENADISフラグ ENA
						TlmTbl[26] = TlmTbl[26] | 0x02
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_UVC_ENA")
					elseif(CID == 0x5F)then --UVC検出ENADISフラグ DIS
						TlmTbl[26] = TlmTbl[26] & ~0x02
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_UVC_DIS")
					elseif(CID == 0x60)then --XUVC検出ENADISフラグ ENA
						TlmTbl[26] = TlmTbl[26] | 0x04
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_XUVC_ENA")
					elseif(CID == 0x61)then --XUVC検出ENADISフラグ DIS
						TlmTbl[26] = TlmTbl[26] & ~0x04
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("POW_XUVC_DIS")
					elseif(CID == 0x64)then --BATヒーター制御ENADISフラグ ENA
						TlmTbl[24] = TlmTbl[24] | 0x01
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("BATHTR_CTRL_ENA")
					elseif(CID == 0x65)then --BATヒーター制御ENADISフラグ DIS
						TlmTbl[24] = TlmTbl[24] & ~0x01
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
						TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
						DispStrP("BATHTR_CTRL_DIS")
					elseif(CID == 0x70)then --PCUD CPUリセット 有効
						TlmTbl[24] = TlmTbl[24] | 0x40
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("RST_ENA")
					elseif(CID == 0x71)then --PCUD CPUリセット 無効
						TlmTbl[24] = TlmTbl[24] & ~0x40
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("RST_DIS")
					elseif(CID == 0x72)then --PCUD CPUリセット
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("RST_PROC")
					elseif(CID == 0x73)then --CRCエラー発生フラグクリア
						TlmTbl[25] = TlmTbl[25] & ~0x40
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("CRC_ERR_CNT_CLT")
					elseif(CID == 0x74)then --BUS_OFF許可フラグ ENA
						TlmTbl[26] = TlmTbl[26] | 0x10
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("BUS_OFF_ENA")
					elseif(CID == 0x75)then --BUS_OFF許可フラグ DIS
						TlmTbl[26] = TlmTbl[26] & ~0x10
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("BUS_OFF_DIS")
					elseif(CID == 0x76)then --MTQ駆動方式をABコマンドに
						TlmTbl[25] = TlmTbl[25] & ~0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						MTQ_XCnt = 0
						MTQ_YCnt = 0
						MTQ_ZCnt = 0
						DispStrP("MTQ_DRIV_AB_CMD")
					elseif(CID == 0x77)then --MTQ駆動方式をシリアルコマンドに
						TlmTbl[25] = TlmTbl[25] | 0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						MTQ_XCnt = 0
						MTQ_YCnt = 0
						MTQ_ZCnt = 0
						DispStrP("MTQ_DRIV_SIO_CMD")
					elseif(CID == 0xF0)then --ダミーコマンド
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("Dummy CMD")
					elseif(CID == 0xF1)then --テレメDigit切替がENAの場合 AAPAにDigit値
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						if((TlmTbl[26] & 0x08) == 0x08)then
							TLMDigit_flg = 1
							TlmTbl[207] = 1
						end
						DispStrP("DBG_TLMDIGIT_START")
					elseif(CID == 0xF2)then --テレメAA,PA値を工学値に戻す
						TlmTbl[26] = TlmTbl[26] & ~0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						TLMDigit_flg = 0
						TlmTbl[207] = 0
						DispStrP("DBG_TLMDIGIT_STOP")
					elseif(CID == 0xF3)then --テレメDigit切替 ENA
						TlmTbl[26] = TlmTbl[26] | 0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("TLMDIGIT_SWITCH_ENA")
					elseif(CID == 0xF4)then --テレメDigit切替 DIS
						TlmTbl[26] = TlmTbl[26] & ~0x08
						TlmTbl[5] = TlmTbl[5] + 1
						TlmTbl[36] = 0
						DispStrP("TLMDIGIT_SWITCH_DIS")
					else
						DispStrP("CMD Error")
					end
				else
					DispStrP("CMD Error")
				end
			else
				DispStrP("CRC Error")
			end
		end
	end

--	SA_V_L = RegRead(0,0x0000)
--	SA_V_R = RegRead(0,0x0010)
	SA_I_L = RegRead(0,0x0020)
	SA_I_R = RegRead(0,0x0030)
--	BAT_V = RegRead(0,0x0040)
	BAT_I = RegRead(0,0x0050)
	BAT_T = RegRead(0,0x0060)
	if(BAT_T > 0x8000)then
		BAT_T = -(0xffff - BAT_T + 1)
	end
	SA_V_L_GOAL = RegRead(0,0x0000)
	SA_V_R_GOAL = RegRead(0,0x0010)
	BAT_V_GOAL = RegRead(0,0x0040)
	if((TlmTbl[25] & 0x08) == 0x00)then --MTQ ABコマンド
		if(RegRead(0,0x0100) == 1)then
			MTQ_XCnt = RegRead(0,0x0110)
			MTQ_YCnt = RegRead(0,0x0120)
			MTQ_ZCnt = RegRead(0,0x0130)
			RegWrite(0,0x0100,0)
			if(MTQ_XCnt < 0)then
				MTQ_XCnt = MTQ_XCnt * -1
			end
			if(MTQ_YCnt < 0)then
				MTQ_YCnt = MTQ_YCnt * -1
			end
			if(MTQ_ZCnt < 0)then
				MTQ_ZCnt = MTQ_ZCnt * -1
			end
		end
	end


	if(SA_V_L ~= SA_V_L_GOAL)then
		if(SA_V_L > SA_V_L_GOAL)then
			if((SA_V_L - SA_V_L_GOAL) < 201)then
				SA_V_L = SA_V_L - 100
			else
				SA_V_L = SA_V_L - 200
			end
		else
			if((SA_V_L_GOAL - SA_V_L) < 201)then
				SA_V_L = SA_V_L + 100
			else
				SA_V_L = SA_V_L + 200
			end
		end
	end

	if(SA_V_R ~= SA_V_R_GOAL)then
		if(SA_V_R > SA_V_R_GOAL)then
			if((SA_V_R - SA_V_R_GOAL) < 201)then
				SA_V_R = SA_V_R - 100
			else
				SA_V_R = SA_V_R - 200
			end
		else
			if((SA_V_R_GOAL - SA_V_R) < 201)then
				SA_V_R = SA_V_R + 100
			else
				SA_V_R = SA_V_R + 200
			end
		end
	end

	if(BAT_V ~= BAT_V_GOAL)then
		if((math.abs(BAT_V - BAT_V_GOAL)) > 1000)then
			BAT_V = BAT_V_GOAL
		else
			if(BAT_V > BAT_V_GOAL)then
				if((BAT_V - BAT_V_GOAL) < 201)then
					BAT_V = BAT_V - 100
				else
					BAT_V = BAT_V - 200
				end
			else
				if((BAT_V_GOAL - BAT_V) < 201)then
					BAT_V = BAT_V + 100
				else
					BAT_V = BAT_V + 200
				end
			end
		end
	end

	if(MTQ_PWR == 1)then --MTQ駆動
--	DispStrP("MTQ " .. str(MTQ_XCnt))
		if(MTQ_XCnt > 0)then
			if(MTQ_XCnt < 101)then
				MTQ_XCnt = MTQ_XCnt - 1
			end
			if(MTQ_XCnt == 0)then
			--	TlmTbl[57] = TlmTbl[57] & ~0x20 --X
				TlmTbl[59] = TlmTbl[59] & ~0x20 --X
	DispStrP("MTQ X STOP")
			else
			--	TlmTbl[57] = TlmTbl[57] | 0x20 --X
				TlmTbl[59] = TlmTbl[59] | 0x20 --X
	DispStrP("MTQ X DRV" .. str(MTQ_XCnt))
			end
		end
		if(MTQ_YCnt > 0)then
			if(MTQ_YCnt < 101)then
				MTQ_YCnt = MTQ_YCnt - 1
			end
			if(MTQ_YCnt == 0)then
			--	TlmTbl[61] = TlmTbl[61] & ~0x08 --Y
				TlmTbl[59] = TlmTbl[59] & ~0x40 --Y
	DispStrP("MTQ Y STOP")
			else
			--	TlmTbl[61] = TlmTbl[61] | 0x08 --Y
				TlmTbl[59] = TlmTbl[59] | 0x40 --Y
	DispStrP("MTQ Y DRV" .. str(MTQ_YCnt))
			end
		end
		if(MTQ_ZCnt > 0)then
			if(MTQ_ZCnt < 101)then
				MTQ_ZCnt = MTQ_ZCnt - 1
			end
			if(MTQ_ZCnt == 0)then
			--	TlmTbl[61] = TlmTbl[61] & ~0x10 --Z
				TlmTbl[59] = TlmTbl[59] & ~0x80 --Z
	DispStrP("MTQ Z STOP")
			else
			--	TlmTbl[61] = TlmTbl[61] | 0x10 --Z
				TlmTbl[59] = TlmTbl[59] | 0x80 --Z
	DispStrP("MTQ Z DRV" .. str(MTQ_ZCnt))
			end
		end
	end

	if(TLMDigit_flg == 1)then
		TlmTbl[38] = (P5V -2.469485-0.5)/ 0.1501858
		TlmTbl[39] = (P15V +0.9555759-0.5) / 0.5025653
		TlmTbl[40] = (M15V +1998.072-0.5) / 0.4953704
		TlmTbl[41] = (P48V +2.33378-0.5) / 1.496715
		TlmTbl[42] = SA_V_L / 0.04
		TlmTbl[43] = (SA_I_L +0.326563122) / 0.001632816
		TlmTbl[44] = SA_V_R / 0.04
		TlmTbl[45] = (SA_I_R +0.326563122)/ 0.001632816
		TlmTbl[46] = (BUS_V -156.851) / 1.996115
		TlmTbl[47] = (LOAD_I +113.0727) / 0.5754367
		TlmTbl[48] = (XPA_I +471.5411) / 1.84434
		TlmTbl[49] = (BAT_V / 0.0205)
		TlmTbl[50] = (BAT_I +7851.01)/ 2.917411
		TlmTbl[51] = (PCDU_T +25661.08)/10.40321
		TlmTbl[52] = (BAT_T -21.75193196)/ 0.001007294
		TlmTbl[53] = (BAT_T -21.28885463)/ 0.001029204
	else
		TlmTbl[38] = P5V  --+5V
		TlmTbl[39] = P15V --+15V
		TlmTbl[40] = M15V ---15V
		TlmTbl[41] = P48V --+48V
		TlmTbl[42] = SA_V_L
		TlmTbl[43] = SA_I_L
		TlmTbl[44] = SA_V_R
		TlmTbl[45] = SA_I_R
		TlmTbl[46] = BUS_V
		TlmTbl[47] = LOAD_I
		TlmTbl[48] = XPA_I
		TlmTbl[49] = BAT_V
		TlmTbl[50] = BAT_I
		TlmTbl[51] = PCDU_T
		TlmTbl[52] = BAT_T
		TlmTbl[53] = BAT_T
	end

	OldDuty_L = TlmTbl[12]
	OldDuty_R = TlmTbl[13]

--	DispStrP(str(BAT_V) .. str(TlmTbl[15]))
--  UVC
	if(BAT_V < TlmTbl[15])then
		UVC_Cnt = UVC_Cnt + 1
		if(UVC_Cnt > TlmTbl[34])then
			if((TlmTbl[26] & 0x02) == 0x02)then --UVC flg
				TlmTbl[25] = (TlmTbl[25] & 0xfd) | 0x02 --UVC flg
			end
		end
	else
		if((TlmTbl[25] & 0x02) == 0x02)then
			if((TlmTbl[26] & 0x02) == 0x02)then --UVC flg
--	DispStrP("UVC_Cnt " .. str(UVC_Cnt))
				if(UVC_Cnt > 0)then
					UVC_Cnt = 0
				else
					UVC_Cnt = UVC_Cnt - 1
					if(UVC_Cnt < -(TlmTbl[34]))then
						TlmTbl[25] = (TlmTbl[25] & 0xfd) --UVC flg clear
						UVC_Cnt = 0
					end
				end
			end
		else
			UVC_Cnt = 0
		end
	end

	--XUVC
	if(BAT_V < TlmTbl[16])then
		XUVC_Cnt = XUVC_Cnt + 1
		if(XUVC_Cnt > TlmTbl[35])then
			if((TlmTbl[26] & 0x04) == 0x04)then --XUVC flg
				TlmTbl[25] = (TlmTbl[25] & 0xfb) | 0x04 --XUVC flg
				TlmTbl[24] = TlmTbl[24] & 0xfe --BAT HTR DIS
				TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
				TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
			end
		end
	else
		if((TlmTbl[25] & 0x04) == 0x04)then
			if((TlmTbl[26] & 0x04) == 0x04)then --XUVC flg
				if(XUVC_Cnt > 0)then
					XUVC_Cnt = 0
				else
					XUVC_Cnt = XUVC_Cnt - 1
					if(XUVC_Cnt < -(TlmTbl[35]))then
						TlmTbl[25] = (TlmTbl[25] & 0xfb) --XUVC flg clear
						XUVC_Cnt = 0
					end
				end
			end
		else
			XUVC_Cnt = 0
		end
	end

	-- BAT HTR制御
	if((TlmTbl[24] & 0x01) == 0x01)then --BAT HTR
		if(BAT_T > TlmTbl[205])then --異常上限
			BAT_HTR_ERR = BAT_HTR_ERR | 0x01
			TlmTbl[26] = (TlmTbl[26] & 0xbf) | 0x40
		end
		if(BAT_T < TlmTbl[206])then --異常下限
			BAT_HTR_ERR = BAT_HTR_ERR | 0x01
			TlmTbl[26] = (TlmTbl[26] & 0xbf) | 0x40
		end
		if(BAT_T > TlmTbl[205])then --異常上限
			BAT_HTR_ERR = BAT_HTR_ERR | 0x02
			TlmTbl[26] = (TlmTbl[26] & 0x7f) | 0x80
		end
		if(BAT_T < TlmTbl[206])then --異常下限
			BAT_HTR_ERR = BAT_HTR_ERR | 0x02
			TlmTbl[26] = (TlmTbl[26] & 0x7f) | 0x80
		end
		if((BAT_HTR_ERR & 0x03) == 0x03)then
			TlmTbl[24] = TlmTbl[24] & 0xfe --BAT HTR DIS
			TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
			TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
		else
			if((BAT_HTR_ERR & 0x01) == 0x00)then
				if(BAT_T > TlmTbl[203])then --off閾値
					if((BAT_HTR_ERR & 0x02) == 0x00)then
						if(BAT_T < TlmTbl[204])then --on閾値
							if(BAT_HTR_Cnt > 10)then
								TlmTbl[55] = (TlmTbl[55] & 0x7f) | 0x80
								TlmTbl[56] = (TlmTbl[56] & 0xfe) | 0x01
							else
								BAT_HTR_Cnt = BAT_HTR_Cnt + 1
							end
						else
							if(BAT_HTR_Cnt > 10)then
								TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
								TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
							else
								BAT_HTR_Cnt = BAT_HTR_Cnt + 1
							end
						end
					else
						if(BAT_HTR_Cnt > 10)then
							TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
							TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
						else
							BAT_HTR_Cnt = BAT_HTR_Cnt + 1
						end
					end
				elseif(BAT_T < TlmTbl[204])then --on閾値
					if(BAT_HTR_Cnt > 10)then
						TlmTbl[55] = (TlmTbl[55] & 0x7f) | 0x80
						TlmTbl[56] = (TlmTbl[56] & 0xfe) | 0x01
					else
						BAT_HTR_Cnt = BAT_HTR_Cnt + 1
					end
				else
					if((BAT_HTR_ERR & 0x02) == 0x00)then
						if(BAT_T > TlmTbl[203])then --off閾値
							if(BAT_HTR_Cnt > 10)then
								TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
								TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
							else
								BAT_HTR_Cnt = BAT_HTR_Cnt + 1
							end
						else
							BAT_HTR_Cnt = 0
						end
					else
						BAT_HTR_Cnt = 0
					end
				end
			else
				if(BAT_T > TlmTbl[203])then --off閾値
					TlmTbl[55] = TlmTbl[55] & 0x7f --HTR4 Clear
					TlmTbl[56] = TlmTbl[56] & 0xfe --HTR5 Clear
				else
					if(BAT_T < TlmTbl[204])then --on閾値
						if(BAT_HTR_Cnt > 10)then
							TlmTbl[55] = (TlmTbl[55] & 0x7f) | 0x80
							TlmTbl[56] = (TlmTbl[56] & 0xfe) | 0x01
						else
							BAT_HTR_Cnt = BAT_HTR_Cnt + 1
						end
					else
						BAT_HTR_Cnt = 0
					end
				end
			end
		end
	end

--	DispStrP("CtrlMode " .. str(CtrlMode))
	
	if(((TlmTbl[28] > SA_I_L)and((BAT_V + 150) > SA_V_L))and((TlmTbl[28] > SA_I_R)and((BAT_V + 150) > SA_V_R)))then --日照日陰判定
		CtrlMode =  0x01	-- 日陰 Duty 100%
	else
	-- 日照
		if((TlmTbl[17] < BAT_T)or(TlmTbl[18] > BAT_T))then --OT閾値 NG
			if((TlmTbl[26] & 0x01) == 0x01)then --OT ENA
				--0A定電流(CC)モード
				CtrlMode = 0x07
			end
		else
			if((TlmTbl[7] & 0x0f ) == 0x02) then --CV ENA
				if(((TlmTbl[20]-TlmTbl[22])<BAT_V)and((TlmTbl[24] & 0x08)==0x08))then
					--定電圧充電(CV)モード
					CtrlMode = 0x02
				end
			else
				if((TlmTbl[20]<BAT_V)and((TlmTbl[24] & 0x08)==0x08))then
					--定電圧充電(CV)モード
					CtrlMode = 0x02
				else
					if((TlmTbl[24] & 0x10)==0x10)then
						CtrlMode = 0x04
					else
						CtrlMode = 0x05
					end
				end
			end
			if((TlmTbl[7] & 0x0f ) == 0x03) then --CC ENA
				if(((TlmTbl[19]-TlmTbl[21])<BAT_I)and((TlmTbl[24] & 0x04)==0x04))then
					--定電流充電(CC)モード
					CtrlMode = 0x03
				end
			else
				if((TlmTbl[19]<BAT_I)and((TlmTbl[24] & 0x04)==0x04))then
					--定電流充電(CC)モード
					CtrlMode = 0x03
				else
					if((TlmTbl[24] & 0x10)==0x10)then
						CtrlMode = 0x04
					else
						CtrlMode = 0x05
					end
				end
			end
		end
	end

--	DispStrP("CtrlMode " .. str(CtrlMode))

	if((TlmTbl[7] & 0x0f) == 0x00)then
		if((TlmTbl[24] & 0x10)==0x10)then --Fix Duty
			CtrlMode = 0x04 --Fix Duty mode
		else
			CtrlMode = 0x05 --Light mode
		end
	end

--	DispStrP("CtrlMode " .. str(CtrlMode))

	if(CtrlMode == CtrlModeOld)then
		CtrlMode_Cnt = CtrlMode_Cnt + 1
		if(((TlmTbl[7] & 0x0f) == 0x03)and(CtrlMode == 0x05))then --CC
			if(CtrlMode_Cnt >= (TlmTbl[32] * 6))then
				TlmTbl[7] = (TlmTbl[7] & 0xf0)|CtrlMode
				CtrlMode_Cnt = 0
				CtrlMode_For = 0
				CtrlModeOld = TlmTbl[7] & 0x0f
			end
		else
			if(CtrlMode_Cnt >= TlmTbl[32])then
				if((TlmTbl[7] & 0x0f) ~= CtrlMode)then
					CtrlMode_For = 0
				end
				TlmTbl[7] = (TlmTbl[7] & 0xf0)|CtrlMode
				CtrlMode_Cnt = 0
				CtrlModeOld = TlmTbl[7] & 0x0f
			end
		end
	else
		CtrlMode_Cnt = 0
	end
--	DispStrP("CtrlMode_For " .. str(CtrlMode_For))
	if(CtrlMode_For >= TlmTbl[183])then
		CtrlMode_For = 0
		CtrlMode = 0x05
		CtrlModeOld = TlmTbl[7] & 0x0f
		TlmTbl[7] = (TlmTbl[7] & 0xf0)|CtrlMode
		CtrlMode_Cnt = 0
	end
	CtrlModeOld = CtrlMode

--	DispStrP("CtrlMode " .. str(CtrlMode))
--	DispStrP("CtrlMode " .. str(TlmTbl[7] & 0x0f))

	if((TlmTbl[7] & 0x0f) == 0x01)then --SHADE
		PWMMode = 1 --SWEEP
		TlmTbl[12] = 0xff
		TlmTbl[13] = 0xff
	elseif((TlmTbl[7] & 0x0f) == 0x02)then --CV
		if(PWMMode ~= 2)then --初回
			TlmTbl[12] = TlmTbl[8]
			TlmTbl[13] = TlmTbl[8]
		end
		PWMMode = 2 --CV
	elseif((TlmTbl[7] & 0x0f) == 0x03)then --CC
		if(PWMMode ~= 3)then --初回
			if((TlmTbl[24] & 0x20) == 0x20)then
				if(TlmTbl[8] > 0xff)then
					TlmTbl[12] = 0xfe
					TlmTbl[13] = 0xfe
				else
					TlmTbl[12] = TlmTbl[8]
					TlmTbl[13] = TlmTbl[8]
				end
			else
				TlmTbl[12] = TlmTbl[8]
				TlmTbl[13] = TlmTbl[8]
			end
		end
		PWMMode = 3 --CC
	elseif((TlmTbl[7] & 0x0f) == 0x04)then --FIX
		if((TlmTbl[194] & 0x01) == 0x01)then
			PWMMode = 6 --SWFIX
			TlmTbl[12] = TlmTbl[10]
			TlmTbl[13] = TlmTbl[10]
		else
			PWMMode = 5 --HWFIX
		end
	elseif((TlmTbl[7] & 0x0f) == 0x05)then --LIGHT
		if(PWMMode ~= 7)then --初回
			TlmTbl[12] = TlmTbl[8]
			TlmTbl[13] = TlmTbl[8]
		end
		PWMMode = 7 --PPT
	elseif((TlmTbl[7] & 0x0f) == 0x07)then --0ACC
		if(PWMMode ~= 9)then --初回
			TlmTbl[12] = TlmTbl[8]
			TlmTbl[13] = TlmTbl[8]
		end
		PWMMode = 9 --0ACC
	else
		PWMMode = 10 --Err
	end


	if((TlmTbl[24] & 0x10) & 0x00)then --Disable
		if(PWMMode == 5)then
			TlmTbl[194] = (TlmTbl[194] & 0xfe) | 0x01
		else
			TlmTbl[194] = (TlmTbl[194] & 0xfe)
		end
	else
		TlmTbl[194] = (TlmTbl[194] & 0xfe) | 0x01
		PWMMode = 5 --HWFIX
	end

--	DispStrP("PWMMode " .. str(PWMMode))
	TlmTbl[7] = TlmTbl[7] & 0x0f | (PWMMode * 16)

	DiffDuty_L = (TlmTbl[12] - OldDuty_L)
	if(DiffDuty_L ~= DiffDuty_La)then
		DiffDuty_La = 0
	else
		if(DiffDuty_L < 0)then
			DiffDuty_La = DiffDuty_La - 1
		else
			DiffDuty_La = DiffDuty_La + 1
		end
	end
--	DispStrP("DiffDuty_L " .. str(DiffDuty_L) .. str(DiffDuty_La))

	DiffDuty_R = (TlmTbl[13] - OldDuty_R)
	if(DiffDuty_R ~= DiffDuty_Ra)then
		DiffDuty_Ra = 0
	else
		if(DiffDuty_R < 0)then
			DiffDuty_Ra = DiffDuty_Ra - 1
		else
			DiffDuty_Ra = DiffDuty_Ra + 1
		end
	end


	if((SA_V_L * SA_I_L) >= ((8200-57)*(480+27)))then
		StepDuty = (TlmTbl[11] + 1) / 2
	else
		StepDuty = TlmTbl[11]
	end
	if(StepDuty < 1)then
		StepDuty = 1
	end


--	if((TlmTbl[54] < 111)and(BAT_I < 2708))then
	if((TlmTbl[54] < 111)and(BAT_I < 50))then
		TlmTbl[12] = 0xff
	elseif(SA_I_L < (40+27))then
		TlmTbl[12] = TlmTbl[12] + StepDuty
	else
		if((DiffDuty_La >= TlmTbl[23])or(-DiffDuty_La >= TlmTbl[23]))then
			if(DiffDuty_La > 0)then
				if(DiffDuty_La > 0xE5)then
					TlmTbl[12] = DiffDuty_La - (StepDuty * 	TlmTbl[186] * 2)
				else
					TlmTbl[12] = DiffDuty_La - (StepDuty * 	TlmTbl[186])
				end
			else
				TlmTbl[12] = DiffDuty_La + StepDuty
			end
			DiffDuty_La = 0
		else
			if((((SA_V_L * SA_I_L) - OldPWR_L)*(TlmTbl[12] - OldDuty_L)) <= 0)then
				if(DiffDuty_La > 0xe5)then
					TlmTbl[12] = DiffDuty_La - (StepDuty * 	TlmTbl[186] * 2)
				else
					TlmTbl[12] = DiffDuty_La - (StepDuty * 	TlmTbl[186])
				end
			else
				TlmTbl[12] = DiffDuty_La + StepDuty
			end
		end
	end

	if((SA_V_R * SA_I_R) >= ((8200-287)*(480+31)))then
		StepDuty = (TlmTbl[11] + 1) / 2
	else
		StepDuty = TlmTbl[11]
	end
	if(StepDuty < 1)then
		StepDuty = 1
	end

--	if((TlmTbl[54] < 39)and(BAT_I < (50+7851)))then
	if((TlmTbl[54] < 111)and(BAT_I < 50))then
		TlmTbl[13] = 0xff
	elseif(SA_I_R < (40+27))then
		TlmTbl[13] = TlmTbl[13] + StepDuty
	else
		if((DiffDuty_Ra >= TlmTbl[23])or(-DiffDuty_Ra >= TlmTbl[23]))then
			if(DiffDuty_Ra > 0)then
				if(DiffDuty_Ra > 0xE5)then
					TlmTbl[13] = DiffDuty_Ra - (StepDuty * 	TlmTbl[186] * 2)
				else
					TlmTbl[13] = DiffDuty_Ra - (StepDuty * 	TlmTbl[186])
				end
			else
				TlmTbl[13] = DiffDuty_Ra + StepDuty
			end
			DiffDuty_Ra = 0
		else
			if((((SA_V_R * SA_I_R) - OldPWR_R)*(TlmTbl[13] - OldDuty_R)) <= 0)then
				if(DiffDuty_Ra > 0xe5)then
					TlmTbl[13] = DiffDuty_Ra - (StepDuty * 	TlmTbl[186] * 2)
				else
					TlmTbl[13] = DiffDuty_Ra - (StepDuty * 	TlmTbl[186])
				end
			else
				TlmTbl[13] = DiffDuty_Ra + StepDuty
			end
		end
	end

	OldPWR_L = (SA_V_L * SA_I_L)
	OldPWR_R = (SA_V_R * SA_I_R)
	TlmTbl[27] = ((SA_V_L * SA_I_L) + (SA_V_R * SA_I_R))

	Send = mkframe(Send,387,TlmTbl)
	local CRC3,Send2 = CRC16(Send,384)
	SerialSend(0,Send2,387)

	DispParamlP(0,"Time",TlmTbl[4])--PCUDタイム
	DispParamlP(1,"CMDCnt",TlmTbl[5])--コマンドカウンタ
	DispParamlP(2,"TLMCnt",TlmTbl[6])--テレメカウンタ
	DispParamlP(3,"MID2 PicExec",CRC3)--適当に変数をここに入れれば表示される(10進のみ)
	DispParamlP(4,"ABTLM OBC",TlmTbl[56])--適当に変数をここに入れれば表示される(10進のみ)
	DispParamlP(5,"OBCCnt",OBCCnt)--適当に変数をここに入れれば表示される(10進のみ)
	DispParamlP(6,"SA_V_L",RegRead(0,0x0000))
	DispParamlP(7,"SA_V_R",RegRead(0,0x0010))
	DispParamlP(8,"SA_I_L",RegRead(0,0x0020))
	DispParamlP(9,"SA_I_R",RegRead(0,0x0030))
	DispParamlP(10,"BAT_V",RegRead(0,0x0040))
--	DispParamlP(11,"BAT_I",RegRead(0,0x0050))
	DispParamlP(11,"BUSON",RegRead(0,0x0080))
	TlmTbl[4] = TlmTbl[4] +1
	TlmTbl[6] = TlmTbl[6] +1
	TlmTbl[36] = TlmTbl[36] +1
	TlmTbl[37] = TlmTbl[37] +1
	if(TlmTbl[4] == 60*30)then
		TlmTbl[57] = TlmTbl[57] | 64
		STRX_First_flg = 1
	end
--	DispStrP(str(TlmTbl[56] & 64))
	if((TlmTbl[56] & 64) == 0)then
		if(OBCCnt > 6)then
			TlmTbl[56] = TlmTbl[56] | 64
			DispStrP("OBC_MAIN ON")
			OBCCnt = 0
		else
			OBCCnt = OBCCnt + 1
		end
	end

	if((TlmTbl[56] & 32) == 0)then
		if(OBCRCnt > 6)then
			TlmTbl[56] = TlmTbl[56] | 32
			DispStrP("OBC_RTC ON")
			OBCRCnt = 0
		else
			OBCRCnt = OBCRCnt + 1
		end
	end

	if(STRX_First_flg == 1)then
		if((TlmTbl[57] & 64) == 0)then
			if(STRXCnt > 6)then
				TlmTbl[57] = TlmTbl[57] | 64
				DispStrP("STRX ON")
				STRXCnt = 0
			else
				STRXCnt = STRXCnt + 1
			end
		end
	end

	if(TlmTbl[36] > (100*60))then
--	if(TlmTbl[36] > (100))then
		Startup()
	end
--	Sleep(10)
end

