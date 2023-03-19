// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#  include <linux/string.h>
#  include <linux/kernel.h>
#endif

#include "lcm_drv.h"
#include <linux/regulator/consumer.h>
#include <linux/string.h>
#include <linux/kernel.h>

#ifdef CONFIG_HQ_SET_LCD_BIAS
#include <lcm_bias.h>
#endif

#ifdef BUILD_LK
#  include <platform/upmu_common.h>
#  include <platform/mt_gpio.h>
#  include <platform/mt_i2c.h>
#  include <platform/mt_pmic.h>
#  include <string.h>
#elif defined(BUILD_UBOOT)
#  include <asm/arch/mt_gpio.h>
#endif

#include <linux/pinctrl/consumer.h>

#ifdef BUILD_LK
#  define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#  define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#  define LCM_LOGI(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#  define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

static struct LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)    lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
#  include <linux/kernel.h>
#  include <linux/module.h>
#  include <linux/fs.h>
#  include <linux/slab.h>
#  include <linux/init.h>
#  include <linux/list.h>
#  include <linux/i2c.h>
#  include <linux/irq.h>
#  include <linux/uaccess.h>
#  include <linux/interrupt.h>
#  include <linux/io.h>
#  include <linux/platform_device.h>
#endif

#define LCM_DSI_CMD_MODE  0

#define FRAME_WIDTH               (1080)
#define FRAME_HEIGHT              (2408)

/* physical size in um */
#define LCM_PHYSICAL_WIDTH        (68430)
#define LCM_PHYSICAL_HEIGHT       (152570)
#define LCM_DENSITY               (450)

#define REGFLAG_DELAY             0xFFFC
#define REGFLAG_UDELAY            0xFFFB
#define REGFLAG_END_OF_TABLE      0xFFFD
#define REGFLAG_RESET_LOW         0xFFFE
#define REGFLAG_RESET_HIGH        0xFFFF

#define LCM_ID_TD4375 0x23
/*hs14 code for SR-AL6528A-01-435 by duanyaoming at 20220914 start*/
/*backlight from 23ma to 20ma*/
#define BACKLIGHT_MAX_REG_VAL   (4095/15*20)
#define BACKLIGHT_MAX_APP_VAL   (255/15*23)
/*hs14 code for SR-AL6528A-01-435 by duanyaoming at 20220914 end*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif



struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[256];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 20, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 100, {} },
};
/*hs14 code for SR-AL6528A-01-423 by duanyaoming at 20220926 start*/
static struct LCM_setting_table lcm_diming_disable_setting[] = {
    {0x53, 0x01, {0x24}},
    {REGFLAG_DELAY, 10,{}},
};
/*hs14 code for SR-AL6528A-01-423 by duanyaoming at 20220926 end*/
static struct LCM_setting_table init_setting_cmd[] = {
    {0xB0, 01,{0x00}},
    {0xB6, 10,{0x30,0x6b,0x00,0x86,0x02,0x0a,0x13,0x1a,0x00,0x00}},
    {0xB7, 04,{0x51,0x00,0x00,0x00}},
    {0xB8, 07,{0x57,0x3d,0x19,0xbe,0x1e,0x0a,0x0a}},
    {0xB9, 07,{0x6f,0x3d,0x28,0xbe,0x3c,0x14,0x0a}},
    {0xBA, 07,{0xb5,0x33,0x41,0xbe,0x64,0x23,0x0a}},
    {0xBB, 11,{0x44,0x26,0xc3,0x1f,0x19,0x06,0x03,0xc0,0x00,0x00,0x10}},
    {0xBC, 11,{0x32,0x4c,0xc3,0x52,0x32,0x1f,0x03,0xf2,0x00,0x00,0x13}},
    {0xBD, 11,{0x24,0x68,0xc3,0xaa,0x3f,0x32,0x03,0xff,0x00,0x00,0x25}},
    {0xBE, 12,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xC0, 23,{0x00,0xd1,0x01,0x2c,0x0a,0x09,0x68,0x00,0x08,0x22,0x00,0x08,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02}},
    {0xC1, 52,{0x30,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x22,0x00,0x05,0x20,0x00,0x80,0xfa,0x40,0x00,0x84,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x2c,0x08,0x08,0x00,0x0a,0x28,0x0f,0x3c,0x14,0x50}},
    {0xC2, 141,{0x00,0xc0,0xcc,0x01,0x01,0x10,0x01,0x02,0x00,0x00,0xc0,0xcc,0x01,0x01,0x09,0x72,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x60,0xD1,0x05,0x05,0x01,0x03,0xc1,0x00,0x00,0x05,0x00,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xC3, 108,{0x00,0x00,0x00,0x01,0x00,0x09,0x92,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xaa,0xaa,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xC4, 109,{0x52,0x00,0x4F,0x44,0x00,0x06,0x02,0x12,0x11,0x10,0x0F,0x0E,0x61,0x61,0x5F,0x5F,0x5D,0x5D,0x61,0x61,0x5F,0x5F,0x5D,0x5D,0x00,0x00,0x4F,0x44,0x00,0x06,0x02,0x12,0x11,0x10,0x0F,0x0E,0x61,0x61,0x5F,0x5F,0x5D,0x5D,0x61,0x61,0x5F,0x5F,0x5D,0x5D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x85,0xFF,0xFF,0x94,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xC5, 05,{0x08,0x00,0x00,0x00,0x00}},
    {0xC6, 62,{0x02,0x0a,0x08,0xff,0xff,0xff,0xff,0x00,0x00,0x13,0x01,0xaf,0x0f,0x01,0x11,0x3D,0x3D,0x3C,0x00,0x00,0x00,0x01,0x05,0x09,0x00,0x28,0x01,0x00,0x48,0x42,0x41,0x01,0x0b,0x00,0x00,0x00,0x1e,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x22,0x22,0x00,0x00,0x00,0x00,0x10,0x00}},
    {0xC7, 76,{0x00,0x00,0x00,0xEE,0x01,0x71,0x01,0x93,0x01,0xA4,0x01,0xAA,0x01,0xAC,0x01,0x9F,0x01,0xB5,0x01,0x82,0x01,0xCA,0x01,0x7E,0x01,0xBA,0x01,0x69,0x01,0xC8,0x01,0xBB,0x02,0x37,0x02,0xA2,0x02,0xD4,0x00,0x00,0x00,0xEE,0x01,0x71,0x01,0x93,0x01,0xA4,0x01,0xAA,0x01,0xAC,0x01,0x9F,0x01,0xB5,0x01,0x82,0x01,0xCA,0x01,0x7E,0x01,0xBA,0x01,0x69,0x01,0xC8,0x01,0xBB,0x02,0x37,0x02,0xA2,0x02,0x56}},	//gamma
    {0xCB, 14,{0x20,0x80,0x70,0x80,0x20,0x00,0x00,0x2d,0x41,0x00,0x00,0x00,0x00,0xff}},
    {0xCD, 01,{0x00}},
    {0xCE, 35,{0x5D,0x40,0x49,0x53,0x59,0x5E,0x63,0x68,0x6E,0x74,0x7E,0x8A,0x98,0xA8,0xBB,0xD0,0xE7,0xFF,0x00,0xB0,0x04,0x04,0x45,0x44,0x69,0x5A,0x40,0x40,0x00,0x00,0x00,0x84,0xFA,0x00,0x00}},
    {0xCF, 06,{0x00,0x00,0x80,0x46,0x61,0x00}},
    {0xD0, 18,{0xcc,0x19,0xd5,0x66,0x09,0x90,0x00,0xcf,0x0f,0x05,0xd5,0x06,0x52,0xfe,0x09,0x09,0xcc,0x00}},
    {0xD1, 43,{0xdb,0xdb,0x1b,0xb3,0x07,0x07,0x3b,0x44,0x34,0x44,0x34,0x05,0x33,0x7b,0x07,0x33,0x7b,0x78,0xd2,0xd0,0x06,0x96,0x13,0x93,0x22,0x22,0x22,0xb3,0xbb,0x80,0x00,0xbb,0xbb,0x11,0x08,0xf0,0x84,0xc0,0xc0,0x77,0x77,0x77,0x55}},
    {0xD2, 02,{0x00,0x00}},
    {0xD4, 23,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xD7, 74,{0x21,0x10,0x52,0x52,0x00,0xd1,0x00,0x06,0x00,0xd1,0x04,0xfd,0x01,0x00,0x03,0x00,0x05,0x05,0x00,0x03,0x04,0x05,0x00,0x04,0x00,0x08,0x02,0x08,0x06,0x03,0x08,0x04,0x08,0x0A,0x0c,0x0b,0x0a,0x0a,0x0a,0x07,0x07,0x06,0x06,0x00,0x08,0x08,0x04,0x05,0x09,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x02,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x06,0x06,0x05,0x05,0x05,0x00}},
    {0xD8, 62,{0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xD9, 22,{0x40,0x99,0x26,0xed,0x12,0x73,0x16,0x6c,0x16,0x6c,0x00,0x14,0x00,0x14,0x00,0x14,0x01,0x0c,0x00,0x00,0x01,0x00}},
    {0xDD, 04,{0x30,0x06,0x23,0x65}},
    {0xDE, 10,{0x00,0x00,0x00,0x0f,0xff,0x00,0x00,0x00,0x00,0x10}},
    {0xE6, 10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xE7, 182,{0x11,0x00,0x89,0x30,0x80,0x09,0x68,0x04,0x38,0x00,0x08,0x02,0x1c,0x02,0x1c,0x02,0x00,0x02,0x0e,0x20,0x00,0xbb,0x00,0x07,0x0c,0x0d,0xb7,0x0c,0xb7,0x18,0x00,0x10,0xf0,0x03,0x0c,0x20,0x00,0x06,0x0b,0x0b,0x33,0x0e,0x1c,0x2a,0x38,0x46,0x54,0x62,0x69,0x70,0x77,0x79,0x7b,0x7d,0x7e,0x01,0x02,0x01,0x00,0x09,0x40,0x09,0xbe,0x19,0xfc,0x19,0xfa,0x19,0xf8,0x1a,0x38,0x1a,0x78,0x1a,0xb6,0x2a,0xf6,0x2b,0x34,0x2b,0x74,0x3b,0x74,0x6b,0xf4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x20,0x00,0x06,0x0F,0x0F,0x33,0x0E,0x1C,0x2A,0x38,0x46,0x54,0x62,0x69,0x70,0x77,0x79,0x7B,0x7D,0x7E,0x01,0xC2,0x22,0x00,0x2A,0x40,0x2A,0xBE,0x3A,0xFC,0x3A,0xFA,0x3A,0xF8,0x3B,0x38,0x3B,0x78,0x3B,0x76,0x4B,0xB6,0x4B,0xB6,0x4B,0xF4,0x63,0xF4,0x7C,0x34,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xE8, 04,{0x00,0x30,0x63,0x00}},
    {0xEA, 30,{0x01,0x0e,0x0a,0x00,0x0a,0x80,0x00,0x00,0x09,0x08,0x01,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xc2,0x00,0x12,0x00,0xd1,0x0d,0x10,0x86,0x00}},
    {0xEB, 07,{0x00,0x00,0x00,0x00,0x01,0x00,0x11}},
    {0xEC, 10,{0x04,0xb0,0x00,0x10,0xa8,0x0a,0x80,0x00,0x02,0x3a}},
    {0xED, 32,{0x01,0x01,0x02,0x02,0x08,0x08,0x09,0x09,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd0,0x00,0x00,0x00,0x00,0x00,0xec,0x10,0x00}},
    {0xEE, 96,{0x01,0x03,0x30,0x00,0x00,0x71,0x01,0x00,0x00,0x00,0x00,0xd2,0xff,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd7,0x5d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0xEF, 83,{0x01,0x70,0x54,0x09,0x10,0x00,0x00,0x00,0x00,0x35,0x35,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x70,0x54,0x09,0x10,0x35,0x35,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x01,0x10,0x01,0x00,0x10,0x00,0x10,0x00,0x0a,0x06,0x00,0x00,0x00,0x00,0x00,0x4f,0x00,0x03,0x51,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01}},
    {0xF9, 8,{0x44,0x3f,0x00,0x8d,0xbf,0x00,0x09,0x1d}},
    {0xB0, 1,{0x03}},
    {0x51, 2, {0x00, 0x01} },
    {0x55, 1, {0x00}},
    {0x53, 1, {0x2C}},

    {0x11, 0, {} },
    {REGFLAG_DELAY, 100, {} },

    {0x29, 0, {} },
    {REGFLAG_DELAY, 20, {} },

};

static struct LCM_setting_table bl_level[] = {
    {0x51, 0x02, {0xFF, 0xFF} },
    {REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(void *cmdq, struct LCM_setting_table *table,
               unsigned int count, unsigned char force_update)
{
    unsigned int i;
    unsigned int cmd;

    for (i = 0; i < count; i++) {
        cmd = table[i].cmd;
        switch (cmd) {
        case REGFLAG_DELAY:
            if (table[i].count <= 10)
                MDELAY(table[i].count);
            else
                MDELAY(table[i].count);
            break;
        case REGFLAG_UDELAY:
            UDELAY(table[i].count);
            break;
        case REGFLAG_END_OF_TABLE:
            break;
        default:
            dsi_set_cmdq_V22(cmdq, cmd, table[i].count,
                     table[i].para_list, force_update);
            break;
        }
    }
}

static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static void lcm_get_params(struct LCM_PARAMS *params)
{
    memset(params, 0, sizeof(struct LCM_PARAMS));

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->physical_width = LCM_PHYSICAL_WIDTH / 1000;
    params->physical_height = LCM_PHYSICAL_HEIGHT / 1000;
    params->physical_width_um = LCM_PHYSICAL_WIDTH;
    params->physical_height_um = LCM_PHYSICAL_HEIGHT;
    params->density = LCM_DENSITY;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode = CMD_MODE;
    params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode = SYNC_EVENT_VDO_MODE;
    // params->dsi.switch_mode = CMD_MODE;
#endif

    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_FOUR_LANE;
    /* The following defined the fomat for data coming from LCD engine. */
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 15;
    params->dsi.vertical_frontporch = 30;
    // params->dsi.vertical_frontporch_for_low_power = 620;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 8;
    params->dsi.horizontal_backporch = 25;
    params->dsi.horizontal_frontporch = 40;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    // params->dsi.noncont_clock           = 1;
    // params->dsi.noncont_clock_period    = 1;
    params->dsi.ssc_range = 4;
    params->dsi.ssc_disable = 1;
    /* params->dsi.ssc_disable = 1; */
#ifndef CONFIG_FPGA_EARLY_PORTING
    /* this value must be in MTK suggested table */
    params->dsi.PLL_CLOCK = 550;
    // params->dsi.PLL_CK_VDO = 440;
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif
    /*hs14 code for SR-AL6528A-01-415 by tangzhen at 20220913 start*/
    // params->dsi.CLK_HS_POST = 36;
    params->dsi.clk_lp_per_line_enable = 0;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x1c;
    /*hs14 code for SR-AL6528A-01-415 by tangzhen at 20220913 end*/

    /* for ARR 2.0 */
    params->max_refresh_rate = 60;
    params->min_refresh_rate = 60;

}

static void lcm_bias_enable(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);
#ifdef CONFIG_HQ_SET_LCD_BIAS
    lcd_bias_set_vspn(ON, VSP_FIRST_VSN_AFTER, 5500);  //open lcd bias
#endif
    MDELAY(10);
}

static void lcm_bias_disable(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);
#ifdef CONFIG_HQ_SET_LCD_BIAS
    lcd_bias_set_vspn(OFF, VSN_FIRST_VSP_AFTER, 5500);  //open lcd bias
#endif
}



/* turn on gate ic & control voltage to 5.5V */
/* equle display_bais_enable ,mt6768 need +/-5.5V */
static void lcm_init_power(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);
    if ((lcd_pinctrl1 == NULL) || (tp_rst_high == NULL)) {
        pr_err("lcd_pinctrl1/tp_rst_high is invaild\n");
        return;
    }

    pinctrl_select_state(lcd_pinctrl1, tp_rst_high);
    MDELAY(5);
    lcm_bias_enable();
}

static void lcm_suspend_power(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);

    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(5);
    if ((lcd_pinctrl1 == NULL) || (tp_rst_low == NULL)) {
        pr_err("lcd_pinctrl1/tp_rst_low is invaild\n");
        return;
    }

    pinctrl_select_state(lcd_pinctrl1, tp_rst_low);
    lcm_bias_disable();
}

/* turn on gate ic & control voltage to 5.5V */
static void lcm_resume_power(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);
    lcm_init_power();
}

static void lcm_init(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);

    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(3);
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(3);
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(30);

    push_table(NULL, init_setting_cmd, ARRAY_SIZE(init_setting_cmd), 1);
    LCM_LOGI("td4375_cw_inx----tps6132----lcm mode = vdo mode :%d----\n",lcm_dsi_mode);
}

static void lcm_suspend(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);
    push_table(NULL, lcm_suspend_setting, ARRAY_SIZE(lcm_suspend_setting), 1);
}

static void lcm_resume(void)
{
    pr_notice("[Kernel/LCM] %s enter\n", __func__);

    lcm_init();
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
    /* A14 code for AL6528A-15 by duanyaoming at 20220903 start */
    LCM_LOGI("%s,td4375_cw_inx backlight: level = %d\n", __func__, level);
    /*hs14 code for SR-AL6528A-01-423 by duanyaoming at 20220926 start*/
    if (level == 0) {
        pr_notice("[LCM],td4375_cw_inx backlight off, diming disable first\n");
        push_table(NULL,
            lcm_diming_disable_setting,
            sizeof(lcm_diming_disable_setting) / sizeof(struct LCM_setting_table),
            1);
    }
    /*hs14 code for SR-AL6528A-01-423 by duanyaoming at 20220926 end*/
    /* Backlight data is mapped from 8 bits to 12 bits,The default scale is BACKLIGHT_MAX_REG_VAL=7(255):BACKLIGHT_MAX_REG_VAL=273(4095) */
    level = mult_frac(level, BACKLIGHT_MAX_REG_VAL, BACKLIGHT_MAX_APP_VAL);
    bl_level[0].para_list[0] = level >> 8;
    bl_level[0].para_list[1] = level & 0xFF;
    push_table(handle, bl_level, ARRAY_SIZE(bl_level), 1);
    /* A14 code for AL6528A-15 by duanyaoming at 20220903 end */
}

struct LCM_DRIVER lcd_td4375_cw_inx_mipi_fhd_video_lcm_drv = {
    .name = "lcd_td4375_cw_inx_mipi_fhd_video",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .init_power = lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
    .set_backlight_cmdq = lcm_setbacklight_cmdq,
};
