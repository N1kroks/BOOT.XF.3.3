#ifndef UFS_PHY_SEQ_H
#define UFS_PHY_SEQ_H
 /**********************************************************************
 * ufs_phy_sequence.h
 *
 * UFS PHY sequence 
 *
 * Copyright (c) 2018-19
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *********************************************************************

                             Edit History

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2019-07-31   jt      Fix RX_MODE_01 settings  
2019-05-14   jt      Kona Updates 
2019-02-14   jt      Update for HSG4 
2018-09-20   jt      Initial version.
-----------------------------------------------------------------------------*/

#include "ufs_bsp.h"
#include "ufs_phy_hwio_v2.h"
#include "aang_phy_hwio.h"

// Constant table for UFS-PHY initialization
static struct ufs_mphy_init_item ufs_bsp_mphy_init_table[] = {
   { 0x00, HWIO_UFS_MEM_UFS_PHY_SW_RESET_OFFS,                          0x01},
   { 0x00, HWIO_UFS_MEM_UFS_PHY_POWER_DOWN_CONTROL_OFFS,                0x01},
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_SYSCLK_EN_SEL_OFFS,                 0xD9},
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_HSCLK_SEL_OFFS,                     0x11}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_HSCLK_HS_SWITCH_SEL_OFFS,           0x00}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP_EN_OFFS,                   0x01},
   { CONDITIONAL_FLAG(RATE_A, 0), 
           HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_OFFS,                  0x02},
   { CONDITIONAL_FLAG(RATE_B, 0),
           HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_OFFS,                  0x06}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_PLL_IVCO_OFFS,                      0x0F}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_INITVAL2_OFFS,             0x00}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_BIN_VCOCAL_HSCLK_SEL_OFFS,          0x11}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE0_OFFS,               0x82}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE0_OFFS,                 0x06}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE0_OFFS,               0x16}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE0_OFFS,               0x36}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE0_OFFS,               0xFF}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE0_OFFS,               0x0C}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_BIN_VCOCAL_CMP_CODE1_MODE0_OFFS,    0xAC}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_BIN_VCOCAL_CMP_CODE2_MODE0_OFFS,    0x1E}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_DEC_START_MODE1_OFFS,               0x98}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_CP_CTRL_MODE1_OFFS,                 0x06}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_PLL_RCTRL_MODE1_OFFS,               0x16}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_PLL_CCTRL_MODE1_OFFS,               0x36},
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP1_MODE1_OFFS,               0x32}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_LOCK_CMP2_MODE1_OFFS,               0x0F}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_BIN_VCOCAL_CMP_CODE1_MODE1_OFFS,    0xDD}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_COM_BIN_VCOCAL_CMP_CODE2_MODE1_OFFS,    0x23},
   { 0x00, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_1_DIVIDER_BAND0_1_OFFS,    0x06}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_2_DIVIDER_BAND0_1_OFFS,    0x03}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_3_DIVIDER_BAND0_1_OFFS,    0x01}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_TX0_PWM_GEAR_4_DIVIDER_BAND0_1_OFFS,    0x00},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_OFFS,                   0xE5}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_QSERDES_TX0_LANE_MODE_1_OFFS,                   0x35}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_TX0_TRAN_DRVR_EMP_EN_OFFS,              0x0C}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_LVL_OFFS,                    0x24}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_CNTRL_OFFS,                  0x0F}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_SIGDET_DEGLITCH_CNTRL_OFFS,         0x1E},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_BAND_OFFS,                       0x18}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_FO_GAIN_OFFS,         0x0A}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_SATURATION_AND_ENABLE_OFFS, 0x5A},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CONTROLS_OFFS,              0xF1},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FASTLOCK_COUNT_LOW_OFFS,       0x80},
   { CONDITIONAL_FLAG(0, HS_G4),
           HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CTRL2_OFFS,                 0x81},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
           HWIO_UFS_MEM_QSERDES_RX0_UCDR_PI_CTRL2_OFFS,                 0x80},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_FO_GAIN_OFFS,                  0x0E},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_UCDR_SO_GAIN_OFFS,                  0x04},
   { CONDITIONAL_FLAG(0, HS_G4),
           HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_OFFS,                    0x6F},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
           HWIO_UFS_MEM_QSERDES_RX0_RX_TERM_BW_OFFS,                    0x1B},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL1_OFFS,         0x04},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_OFFS,         0x00},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL2_OFFS,         0x06},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_OFFS,         0x09},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL3_OFFS,         0x04},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_OFFS,         0x07},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQU_ADAPTOR_CNTRL4_OFFS,         0x1D},
   { CONDITIONAL_FLAG(0, HS_G4), 
           HWIO_UFS_MEM_QSERDES_RX0_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFS,   0x17},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_OFFSET_ADAPTOR_CNTRL2_OFFS,      0x00},
   { CONDITIONAL_FLAG(0, HS_G4),
           HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_OFFS,          0x20},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
           HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_MEASURE_TIME_OFFS,          0x10},
   { CONDITIONAL_FLAG(0, HS_G4),
          HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_OFFS,            0x80}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
          HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_LOW_OFFS,            0xC0}, 
   { CONDITIONAL_FLAG(0, HS_G4),
         HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_OFFS,            0x01},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
         HWIO_UFS_MEM_QSERDES_RX0_RX_IDAC_TSETTLE_HIGH_OFFS,            0x00},
   { CONDITIONAL_FLAG(0, HS_G4), 
         HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_LOW_OFFS,                  0x3F},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
         HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_LOW_OFFS,                  0x6D},
   { CONDITIONAL_FLAG(0, HS_G4),
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH_OFFS,                  0xFF}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH_OFFS,                  0x6D}, 
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH2_OFFS,                 0xFF},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH2_OFFS,                 0xED},
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH3_OFFS,                 0x7F},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH3_OFFS,                 0x3B},
   { CONDITIONAL_FLAG(0, HS_G4),
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH4_OFFS,                 0x2C},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_00_HIGH4_OFFS,                 0x3C},
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_LOW_OFFS,                   0x6D},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_LOW_OFFS,                   0xE0},
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH_OFFS,                  0x6D}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH_OFFS,                  0xC8},
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH2_OFFS,                 0xED},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH2_OFFS,                 0xC8},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH3_OFFS,              0x3B},
   { CONDITIONAL_FLAG(0, HS_G4), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH4_OFFS,                 0x3C},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
        HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_01_HIGH4_OFFS,                 0xB1},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_LOW_OFFS,                0xE0},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_HIGH_OFFS,               0xC8}, 
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_HIGH2_OFFS,              0xC8},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_HIGH3_OFFS,              0x3B},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_RX_MODE_10_HIGH4_OFFS,              0xB1},
   { 0x00, HWIO_UFS_MEM_QSERDES_RX0_DCC_CTRL1_OFFS,                     0x0C},  
   { CONDITIONAL_FLAG(0, HS_G4),
           HWIO_UFS_MEM_QSERDES_RX0_GM_CAL_OFFS,                        0x0F},
   { STOP_CONDITION, 0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_init_end_table[] = {
   { 0x00, HWIO_UFS_MEM_UFS_PHY_RX_SIGDET_CTRL2_OFFS,              0x6D},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
           HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_OFFS,         0x0A},
   { CONDITIONAL_FLAG(0, HS_G4),
           HWIO_UFS_MEM_UFS_PHY_TX_LARGE_AMP_DRV_LVL_OFFS,         0x10},
   { 0x00, HWIO_UFS_MEM_UFS_PHY_TX_SMALL_AMP_DRV_LVL_OFFS,         0x02},
   { 0x00, HWIO_UFS_MEM_UFS_PHY_TX_MID_TERM_CTRL1_OFFS,            0x43},
   { 0x00, HWIO_UFS_MEM_UFS_PHY_DEBUG_BUS_CLKSEL_OFFS,             0x1F},
   { 0x00, HWIO_UFS_MEM_UFS_PHY_RX_MIN_HIBERN8_TIME_OFFS,          0xFF},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_UFS_PHY_PLL_CNTL_OFFS,                     0x03},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
           HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_OFFS, 0x16}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
           HWIO_UFS_MEM_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_OFFS, 0xD8}, 
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
          HWIO_UFS_MEM_UFS_PHY_TX_PWM_GEAR_BAND_OFFS,              0xAA},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
          HWIO_UFS_MEM_UFS_PHY_TX_HS_GEAR_BAND_OFFS,               0x06},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
          HWIO_UFS_MEM_UFS_PHY_TX_HSGEAR_CAPABILITY_OFFS,          0x03},
   { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
          HWIO_UFS_MEM_UFS_PHY_RX_HSGEAR_CAPABILITY_OFFS,          0x03}, 
   { CONDITIONAL_FLAG(0, HS_G4),
          HWIO_UFS_MEM_UFS_PHY_BIST_FIXED_PAT_CTRL_OFFS,           0x0A}, 
   { 0x00, HWIO_UFS_MEM_UFS_PHY_SW_RESET_OFFS,                     0x00}, 
   { STOP_CONDITION, 0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_init_table_rumi[] = {
     { 0x00, HWIO_UFS_PHY_PHY_START_OFFS,                     0x00}, 
     { 0x00, HWIO_UFS_PHY_POWER_DOWN_CONTROL_OFFS,            0x00}, 
     { 0x00, HWIO_UFS_PHY_RX_SYM_RESYNC_CTRL_OFFS,            0x03}, 
     { 0x00, HWIO_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_OFFS,  0x0F}, 
     { 0x00, HWIO_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_OFFS,  0x00}, 
     { 0x00, HWIO_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_OFFS,       0x3F}, 
     { 0x00, HWIO_QSERDES_COM_SYSCLK_EN_SEL_OFFS,             0x03}, 
     { 0x00, HWIO_QSERDES_COM_SYS_CLK_CTRL_OFFS,              0x16}, 
     { 0x00, HWIO_QSERDES_COM_RES_CODE_TXBAND_OFFS,           0xC0}, 
     { 0x00, HWIO_QSERDES_COM_PLL_VCOTAIL_EN_OFFS,            0x03}, 
     { 0x00, HWIO_QSERDES_COM_PLL_CNTRL_OFFS,                 0x24}, 
     { 0x00, HWIO_QSERDES_COM_PLL_CLKEPDIV_OFFS,              0x03}, 
     { 0x00, HWIO_QSERDES_COM_RESETSM_CNTRL_OFFS,             0x10}, 
     { 0x00, HWIO_QSERDES_COM_PLL_RXTXEPCLK_EN_OFFS,          0x13}, 
     { 0x00, HWIO_QSERDES_COM_PLL_CRCTRL_OFFS,                0x43}, 
     { 0x00, HWIO_QSERDES_TX0_CLKBUF_ENABLE_OFFS,             0x29},
     { 0x00, HWIO_QSERDES_TX1_CLKBUF_ENABLE_OFFS,             0x29},
     { 0x00, HWIO_QSERDES_COM_DEC_START1_OFFS,                0x98}, 
     { 0x00, HWIO_QSERDES_COM_DEC_START2_OFFS,                0x03}, 
     { 0x00, HWIO_QSERDES_COM_DIV_FRAC_START1_OFFS,           0x80}, 
     { 0x00, HWIO_QSERDES_COM_DIV_FRAC_START2_OFFS,           0x80}, 
     { 0x00, HWIO_QSERDES_COM_DIV_FRAC_START3_OFFS,           0x10}, 
     { 0x00, HWIO_QSERDES_COM_PLLLOCK_CMP1_OFFS,              0x65}, 
     { 0x00, HWIO_QSERDES_COM_PLLLOCK_CMP2_OFFS,              0x1E}, 
     { 0x00, HWIO_QSERDES_COM_PLLLOCK_CMP3_OFFS,              0x00}, 
     { 0x00, HWIO_QSERDES_COM_PLLLOCK_CMP_EN_OFFS,            0x03}, 
     { 0x00, HWIO_QSERDES_COM_PLL_IP_SETI_OFFS,               0x07}, 
     { 0x00, HWIO_QSERDES_COM_PLL_CP_SETI_OFFS,               0x0F}, 
     { 0x00, HWIO_QSERDES_COM_PLL_IP_SETP_OFFS,               0x07}, 
     { 0x00, HWIO_QSERDES_COM_PLL_CP_SETP_OFFS,               0x01}, 
     { 0x00, HWIO_QSERDES_RX0_PWM_CNTRL1_OFFS,                0x08}, 
     { 0x00, HWIO_QSERDES_RX0_PWM_CNTRL2_OFFS,                0x40}, 
     { 0x00, HWIO_QSERDES_RX0_PWM_NDIV_OFFS,                  0x30}, 
     { 0x00, HWIO_QSERDES_RX0_CDR_CONTROL_OFFS,               0x40}, 
     { 0x00, HWIO_QSERDES_RX0_CDR_CONTROL_HALF_OFFS,          0x0C}, 
     { 0x00, HWIO_QSERDES_RX0_CDR_CONTROL_QUARTER_OFFS,       0x12}, 
     { 0x00, HWIO_QSERDES_RX0_SIGDET_CNTRL_OFFS,              0xC0}, 
     { 0x00, HWIO_QSERDES_RX0_SIGDET_CNTRL2_OFFS,             0x07}, 
     { 0x00, HWIO_QSERDES_RX0_RX_BAND_OFFS,                   0x06}, 
     { 0x00, HWIO_QSERDES_RX0_UFS_CNTRL_OFFS,                 0x00}, 
     { 0x00, HWIO_QSERDES_RX0_RX_IQ_RXDET_EN_OFFS,            0xF3},
     { 0x00, HWIO_UFS_PHY_POWER_DOWN_CONTROL_OFFS,            0x01}, 
     { STOP_CONDITION, 0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_2_lane_init_table[] = {
     { 0x00, HWIO_UFS_MEM_QSERDES_TX1_PWM_GEAR_1_DIVIDER_BAND0_1_OFFS,        0x06}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_TX1_PWM_GEAR_2_DIVIDER_BAND0_1_OFFS,        0x03}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_TX1_PWM_GEAR_3_DIVIDER_BAND0_1_OFFS,        0x01}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_TX1_PWM_GEAR_4_DIVIDER_BAND0_1_OFFS,        0x00},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_TX1_LANE_MODE_1_OFFS,                       0xE5},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_TX1_LANE_MODE_1_OFFS,                       0x35},
     { 0x00, HWIO_UFS_MEM_QSERDES_TX1_TRAN_DRVR_EMP_EN_OFFS,                  0x0C},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_SIGDET_LVL_OFFS,                        0x24},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_SIGDET_CNTRL_OFFS,                      0x0F},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_SIGDET_DEGLITCH_CNTRL_OFFS,             0x1E},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_BAND_OFFS,                           0x18}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_FASTLOCK_FO_GAIN_OFFS,             0x0A},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_SO_SATURATION_AND_ENABLE_OFFS,     0x5A},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_PI_CONTROLS_OFFS,                  0xF1},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_FASTLOCK_COUNT_LOW_OFFS,           0x80},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_UCDR_PI_CTRL2_OFFS,                     0x81},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_UCDR_PI_CTRL2_OFFS,                     0x80},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_FO_GAIN_OFFS,                      0x0E},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_UCDR_SO_GAIN_OFFS,                      0x04}, 
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_TERM_BW_OFFS,                        0x6F},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_TERM_BW_OFFS,                        0x1B},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL1_OFFS,             0x04},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL2_OFFS,             0x00},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL2_OFFS,             0x06},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL3_OFFS,             0x09},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL3_OFFS,             0x04},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL4_OFFS,             0x07},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQU_ADAPTOR_CNTRL4_OFFS,             0x1D},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFS,       0x17},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_OFFSET_ADAPTOR_CNTRL2_OFFS,          0x00},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_MEASURE_TIME_OFFS,              0x20},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_MEASURE_TIME_OFFS,              0x10},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_TSETTLE_LOW_OFFS,               0x80}, 
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_TSETTLE_LOW_OFFS,               0xC0}, 
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_TSETTLE_HIGH_OFFS,              0x01},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_IDAC_TSETTLE_HIGH_OFFS,              0x00},
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_LOW_OFFS,                    0x3F},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_LOW_OFFS,                    0x6D},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH_OFFS,                   0xFF}, 
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH_OFFS,                   0x6D}, 
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH2_OFFS,                  0xFF},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH2_OFFS,                  0xED},
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH3_OFFS,                  0x7F},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH3_OFFS,                  0x3B},
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH4_OFFS,                  0x2C},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR),
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_00_HIGH4_OFFS,                  0x3C},
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_LOW_OFFS,                    0x6D},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_LOW_OFFS,                    0xE0},
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH_OFFS,                   0x6D}, 
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH_OFFS,                   0xC8},
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH2_OFFS,                  0xED}, 
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH2_OFFS,                  0xC8},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH3_OFFS,                  0x3B}, 
     { CONDITIONAL_FLAG(0, HS_G4), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH4_OFFS,                  0x3C},
     { CONDITIONAL_FLAG(0, DEFAULT_GEAR), 
             HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_01_HIGH4_OFFS,                  0xB1},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_10_LOW_OFFS,                    0xE0},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_10_HIGH_OFFS,                   0xC8}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_10_HIGH2_OFFS,                  0xC8}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_10_HIGH3_OFFS,                  0x3B}, 
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_RX_MODE_10_HIGH4_OFFS,                  0xB1},
     { 0x00, HWIO_UFS_MEM_QSERDES_RX1_DCC_CTRL1_OFFS,                         0x0C}, 
     { CONDITIONAL_FLAG(0, HS_G4),
             HWIO_UFS_MEM_QSERDES_RX1_GM_CAL_OFFS,                            0x0F},
     { 0x00, HWIO_UFS_MEM_UFS_PHY_MULTI_LANE_CTRL1_OFFS,                      0x02},
     { STOP_CONDITION, 0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_2_lane_init_table_rumi[] = {
     { 0x00, HWIO_QSERDES_RX1_PWM_CNTRL1_OFFS,                        0x08},
     { 0x00, HWIO_QSERDES_RX1_PWM_CNTRL2_OFFS,                        0x40},
     { 0x00, HWIO_QSERDES_RX1_PWM_NDIV_OFFS,                          0x30},
     { 0x00, HWIO_QSERDES_RX1_CDR_CONTROL_OFFS,                       0x40},
     { 0x00, HWIO_QSERDES_RX1_CDR_CONTROL_HALF_OFFS,                  0x0C},
     { 0x00, HWIO_QSERDES_RX1_CDR_CONTROL_QUARTER_OFFS,               0x12},
     { 0x00, HWIO_QSERDES_RX1_SIGDET_CNTRL_OFFS,                      0xc0},
     { 0x00, HWIO_QSERDES_RX1_SIGDET_CNTRL2_OFFS,                     0x07},
     { 0x00, HWIO_QSERDES_RX1_RX_BAND_OFFS,                           0x06},
     { 0x00, HWIO_QSERDES_RX1_UFS_CNTRL_OFFS,                         0x00},
     { 0x00, HWIO_QSERDES_RX1_RX_IQ_RXDET_EN_OFFS,                    0xf3},
     { 0x00, HWIO_UFS_PHY_MULTI_LANE_CTRL1_OFFS,                      0x02},
     { STOP_CONDITION, 0,0}};

#endif /* #ifndef UFS_PHY_SEQ_H */
