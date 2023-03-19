/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */



#ifndef _MAIN_LENS_H

#define _MAIN_LENS_H

#include "lens_list.h"
#include <linux/ioctl.h>

/* TabA7 Lite code for SR-AX3565-01-906  by chenjun at 2022/02/19 start */
#define MAX_NUM_OF_LENS 40
/* TabA7 Lite code for SR-AX3565-01-906  by chenjun at 2022/02/19 end */

#define AF_MAGIC 'A'

#ifdef CONFIG_MACH_MT6779
#define SUPPORT_GETTING_LENS_FOLDER_NAME 1
#else
#define SUPPORT_GETTING_LENS_FOLDER_NAME 0
#endif
/* A03s code for CAM-AL5625-01-247 by lisizhou at 2021/04/22 start */
/*hs03s_NM code for SL6215DEV-4183 by liluling at 2022/4/15 start*/
#ifdef CONFIG_HQ_PROJECT_HS03S
#define AFDRV_GT9772AF "GT9772AF"
#define AFDRV_GT9773AF "GT9773AF"
#define AFDRV_FP5519AF "FP5519AF"
/* A03s code for CAM-AL5625-01-247 by xuxianwei at 2021/05/18 start */
/*hs03s_NM code for SL6215DEV-4183 by liluling at 2022/4/15 end*/
#define AFDRV_CN3927OFILMAF "CN3927OFILMAF"
#define AFDRV_CN3927DDAF "CN3927DDAF"
#define AFDRV_CN3927XLAF "CN3927XLAF"
/* A03s code for CAM-AL5625-01-247 by xuxianwei at 2021/05/18 end */
#endif
/* A03s code for CAM-AL5625-01-247 by lisizhou at 2021/04/22 end */

/*hs04 code for DEVAL6398A-46 by renxinglin at  2022/10/14 start*/
#ifdef CONFIG_HQ_PROJECT_HS04
    #define AFDRV_O2101_GT9772AF "O2101_GT9772AF"
    #define AFDRV_O2102_GT9769AF "O2102_GT9769AF"
    #define AFDRV_O2103_PE916AF  "O2103_PE916AF"
    #define AFDRV_O2104_GT9769AF "O2104_GT9769AF"
#endif
/*hs04 code for DEVAL6398A-46 by renxinglin at  2022/10/14 end*/
/*hs14 code for SR-AL5628-01-161 Universal macro adaptation by lisizhou at 2022/9/23 start*/
#define AFDRV_A1401AW8601WAF "A1401AW8601WAF"
#define AFDRV_A1402DW9767AF "A1402DW9767AF"
#define AFDRV_A1403GT9778AF "A1403GT9778AF"
/*hs14 code for SR-AL5628-01-161 Universal macro adaptation by lisizhou at 2022/9/23 end*/
/* hs04 code for SR-AL6398A-01-16 by liluling at 2022/07/05 end */
/*HS04 code for DEVAL6398A-9 Universal macro adaptation by chenjun at 2022/7/2 end*/

/* AFDRV_XXXX be the same as AF_DRVNAME in (*af).c */
#define AFDRV_AD5820AF "AD5820AF"
#define AFDRV_AD5823 "AD5823"
#define AFDRV_AD5823AF "AD5823AF"
#define AFDRV_AK7345AF "AK7345AF"
#define AFDRV_AK7371AF "AK7371AF"
#define AFDRV_BU63165AF "BU63165AF"
#define AFDRV_BU63169AF "BU63169AF"
#define AFDRV_BU6424AF "BU6424AF"
#define AFDRV_BU64253GWZAF "BU64253GWZAF"
#define AFDRV_BU6429AF "BU6429AF"
#define AFDRV_BU64748AF "BU64748AF"
#define AFDRV_BU64745GWZAF "BU64745GWZAF"
#define AFDRV_DW9714A "DW9714A"
#define AFDRV_DW9714AF "DW9714AF"
#define AFDRV_DW9718AF "DW9718AF"
#define AFDRV_DW9718SAF "DW9718SAF"
#define AFDRV_DW9800WAF "DW9800WAF"
#define AFDRV_DW9719TAF "DW9719TAF"
#define AFDRV_DW9763AF "DW9763AF"
#define AFDRV_DW9814AF "DW9814AF"
#define AFDRV_DW9839AF "DW9839AF"
#define AFDRV_FP5510E2AF "FP5510E2AF"
#define AFDRV_FP5529AF "FP5529AF"
#define AFDRV_FM50AF "FM50AF"
#define AFDRV_GAF001AF "GAF001AF"
#define AFDRV_GAF002AF "GAF002AF"
#define AFDRV_GAF008AF "GAF008AF"
#define AFDRV_GT9764AF "GT9764AF"
#define AFDRV_LC898122AF "LC898122AF"
#define AFDRV_LC898212AF "LC898212AF"
#define AFDRV_LC898212XDAF "LC898212XDAF"
#define AFDRV_LC898212XDAF_TVC700 "LC898212XDAF_TVC700"
#define AFDRV_LC898212XDAF_F "LC898212XDAF_F"
#define AFDRV_LC898214AF "LC898214AF"
#define AFDRV_LC898217AF "LC898217AF"
#define AFDRV_LC898217AFA "LC898217AFA"
#define AFDRV_LC898217AFB "LC898217AFB"
#define AFDRV_LC898217AFC "LC898217AFC"
#define AFDRV_LC898229AF "LC898229AF"
#define AFDRV_MT9P017AF "MT9P017AF"
#define AFDRV_OV8825AF "OV8825AF"
#define AFDRV_WV511AAF "WV511AAF"
#define AFDRV_DW9718TAF "DW9718TAF"

/*HS04 code for DEVAL6398A-9 Universal macro adaptation by chenjun at 2022/7/2 start*/
#ifdef CONFIG_HQ_PROJECT_OT8
/*gaozhenyu add for af start*/
#define AFDRV_GT9769AF "GT9769AF"
/*gaozhenyu add for af end*/
/*TabA7 Lite code for SR-AX3565-01-320 by liuchengfei at 20201127 start*/
#define AFDRV_GT9772AFHLT "GT9772AFHLT"
/*TabA7 Lite code for SR-AX3565-01-320 by liuchengfei at 20201127 end*/
/* TabA7 Lite  code for SR-AX3565-01-320 by lisizhou at 20201129 start */
#define AFDRV_VA26X802AF "VA26X802AF"
/* TabA7 Lite  code for SR-AX3565-01-320 by lisizhou at 20201129 end */
/*  TabA7 Lite code for SR-AX3565-01-875 by gaozhenyu at 2021/11/19 start */
#define AFDRV_GT9769sAF "GT9769sAF"
/*  TabA7 Lite code for SR-AX3565-01-875 by gaozhenyu at 2021/11/19 end */
/* TabA7 Lite code for SR-AX3565-01-906  by chenjun at 2022/02/19 start */
#define AFDRV_FP5510MAIN6AF "FP5510MAIN6AF"
/* TabA7 Lite code for SR-AX3565-01-906  by chenjun at 2022/02/19 end */
#endif
#define CONVERT_CCU_TIMESTAMP 0x1000
/*HS04 code for DEVAL6398A-9 Universal macro adaptation by chenjun at 2022/7/2 end*/

/* Structures */
struct stAF_MotorInfo {
	/* current position */
	u32 u4CurrentPosition;
	/* macro position */
	u32 u4MacroPosition;
	/* Infinity position */
	u32 u4InfPosition;
	/* Motor Status */
	bool bIsMotorMoving;
	/* Motor Open? */
	bool bIsMotorOpen;
	/* Support SR? */
	bool bIsSupportSR;
};

/* Structures */
struct stAF_MotorCalPos {
	/* macro position */
	u32 u4MacroPos;
	/* Infinity position */
	u32 u4InfPos;
};

#define STRUCT_MOTOR_NAME 32
#define AF_MOTOR_NAME 31

/* Structures */
struct stAF_MotorName {
	u8 uMotorName[STRUCT_MOTOR_NAME];
};

/* Structures */
struct stAF_MotorCmd {
	u32 u4CmdID;
	u32 u4Param;
};

/* Structures */
struct stAF_CtrlCmd {
	long long i8CmdID;
	long long i8Param[2];
};

/* Structures */
struct stAF_MotorOisInfo {
	int i4OISHallPosXum;
	int i4OISHallPosYum;
	int i4OISHallFactorX;
	int i4OISHallFactorY;
};

/* Structures */
#define OIS_DATA_NUM 8
#define OIS_DATA_MASK (OIS_DATA_NUM - 1)
struct stAF_OisPosInfo {
	int64_t TimeStamp[OIS_DATA_NUM];
	int i4OISHallPosX[OIS_DATA_NUM];
	int i4OISHallPosY[OIS_DATA_NUM];
};

/* Structures */
struct stAF_DrvList {
	u8 uEnable;
	u8 uDrvName[32];
	int (*pAF_SetI2Cclient)(struct i2c_client *pstAF_I2Cclient,
				spinlock_t *pAF_SpinLock, int *pAF_Opened);
	long (*pAF_Ioctl)(struct file *a_pstFile, unsigned int a_u4Command,
			  unsigned long a_u4Param);
	int (*pAF_Release)(struct inode *a_pstInode, struct file *a_pstFile);
	int (*pAF_GetFileName)(unsigned char *pFileName);
	int (*pAF_OisGetHallPos)(int *PosX, int *PosY);
};

#define I2CBUF_MAXSIZE 10

struct stAF_CCUI2CFormat {
	u8 I2CBuf[I2CBUF_MAXSIZE];
	u8 BufSize;
};

#define I2CDATA_MAXSIZE 4
/* Structures */
struct stAF_DrvI2CFormat {
	/* Addr Format */
	u8 Addr[I2CDATA_MAXSIZE];
	u8 AddrNum;
	u8 CtrlData[I2CDATA_MAXSIZE]; /* Control Data */
	u8 BitRR[I2CDATA_MAXSIZE];
	u8 Mask1[I2CDATA_MAXSIZE];
	u8 BitRL[I2CDATA_MAXSIZE];
	u8 Mask2[I2CDATA_MAXSIZE];
	u8 DataNum;
};

#define I2CSEND_MAXSIZE 4
/* Structures */
struct stAF_MotorI2CSendCmd {
	u8 Resolution;
	u8 SlaveAddr;
	/* I2C Send */
	struct stAF_DrvI2CFormat I2CFmt[I2CSEND_MAXSIZE];
	u8 I2CSendNum;
};

/* Control commnad */
/* S means "set through a ptr" */
/* T means "tell by a arg value" */
/* G means "get by a ptr" */
/* Q means "get by return a value" */
/* X means "switch G and S atomically" */
/* H means "switch T and Q atomically" */
#define AFIOC_G_MOTORINFO _IOR(AF_MAGIC, 0, struct stAF_MotorInfo)

#define AFIOC_T_MOVETO _IOW(AF_MAGIC, 1, u32)

#define AFIOC_T_SETINFPOS _IOW(AF_MAGIC, 2, u32)

#define AFIOC_T_SETMACROPOS _IOW(AF_MAGIC, 3, u32)

#define AFIOC_G_MOTORCALPOS _IOR(AF_MAGIC, 4, struct stAF_MotorCalPos)

#define AFIOC_S_SETPARA _IOW(AF_MAGIC, 5, struct stAF_MotorCmd)

#define AFIOC_G_MOTORI2CSENDCMD _IOR(AF_MAGIC, 6, struct stAF_MotorI2CSendCmd)

#define AFIOC_S_SETDRVNAME _IOW(AF_MAGIC, 10, struct stAF_MotorName)

#define AFIOC_S_SETPOWERDOWN _IOW(AF_MAGIC, 11, u32)

#define AFIOC_G_MOTOROISINFO _IOR(AF_MAGIC, 12, struct stAF_MotorOisInfo)

#define AFIOC_S_SETPOWERCTRL _IOW(AF_MAGIC, 13, u32)

#define AFIOC_S_SETLENSTEST _IOW(AF_MAGIC, 14, u32)

#define AFIOC_G_OISPOSINFO _IOR(AF_MAGIC, 15, struct stAF_OisPosInfo)

#define AFIOC_S_SETDRVINIT _IOW(AF_MAGIC, 16, u32)

#define AFIOC_G_GETDRVNAME _IOWR(AF_MAGIC, 17, struct stAF_MotorName)

#define AFIOC_X_CTRLPARA _IOWR(AF_MAGIC, 18, struct stAF_CtrlCmd)

#endif
