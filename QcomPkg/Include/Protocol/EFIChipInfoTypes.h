/**
  @file  EFIChipInfoTypes.h
  @brief ChipInfo EFI protocol interface.
*/
/*=============================================================================
  Copyright (c) 2016-2019 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/28/16   pbitra     Created.
=============================================================================*/

#ifndef __EFICHIPINFOTYPES_H__
#define __EFICHIPINFOTYPES_H__

/** @addtogroup efi_chipInfo_constants
@{ */
/** Macro to generate the hardware version number from a specified major
  and minor number. This is used when comparing against the return value of
  hw_version(). For example, to check if the version is at least Version 1.2: \n

  @code
  EFI_ChipInfo_GetVersion(h, &version);
  if (version >= EFI_CHIPINFO_VERSION(1,2)) ...
  @endcode
*/
#define EFI_CHIPINFO_VERSION(major, minor)  (((major) << 16) | (minor))
#define EFI_CHIPINFO_VERSION_UNKNOWN 0

/** Maximum length of a chip ID string. This can be used by clients when
  declaring space for the EFI_DALCHIPINFO_GETCHIPIDSTRING() function. */
#define EFICHIPINFO_MAX_ID_LENGTH 16

/**
  Maximum length of a processor name string. This can be used
  by clients when declaring space for the
  EFI_DALCHIPINFO_GETPROCESSORNAMESTRING() function. */
#define EFICHIPINFO_MAX_NAME_LENGTH EFICHIPINFO_MAX_ID_LENGTH

/**
  Maximum number of CPU clusters supported by the GetDefectiveParts API
 **/
#define EFICHIPINFO_MAX_CPU_CLUSTERS 4

/**
 * Other macros used to indicate unknown values
 */
#define EFICHIPINFO_RAW_VERSION_UNKNOWN        0x0
#define EFICHIPINFO_RAW_ID_UNKNOWN             0x0
#define EFICHIPINFO_SERIAL_NUM_UNKNOWN         0x0
#define EFICHIPINFO_RAW_DEVICE_FAMILY_UNKNOWN  0x0
#define EFICHIPINFO_RAW_DEVICE_NUMBER_UNKNOWN  0x0
#define EFICHIPINFO_QFPROM_CHIPID_UNKNOWN      0x0

/** @endcond */

/** @addtogroup efi_chipInfo_data_types
@{ */
/** Chip version with the major number in the upper 16 bits and the
  minor number in the lower 16 bits. For example: \n
  - Version 1.0 is 0x00010000
  - Version 2.3 is 0x00020003

  The #EFI_CHIPINFO_VERSION macro must be used to generate the
  appropriate comparison value because the format is not guaranteed
  to remain unchanged.
*/
typedef UINT32 EFIChipInfoVersionType;

/** Stores information about modem hardware supported on the
  platform. */
typedef UINT32 EFIChipInfoModemType;

/** Chip identification. */
/* Any new IDs must be added to the end. */
typedef enum
{
  /** @cond */
  EFICHIPINFO_ID_UNKNOWN            = 0,  /**< Unknown chip. */
  EFICHIPINFO_ID_MDM1000            = 1,  /**< MDM1000 chip. */
  EFICHIPINFO_ID_ESM6235            = 2,  /**< ESM6235 chip. */
  EFICHIPINFO_ID_QSC6240            = 3,  /**< QSC6240 chip. */
  EFICHIPINFO_ID_MSM6245            = 4,  /**< MSM6245 chip. */
  EFICHIPINFO_ID_MSM6255            = 5,  /**< MSM6255 chip. */
  EFICHIPINFO_ID_MSM6255A           = 6,  /**< MSM6255A chip. */
  EFICHIPINFO_ID_MSM6260            = 7,  /**< MSM6260 chip. */
  EFICHIPINFO_ID_MSM6246            = 8,  /**< MSM6246 chip. */
  EFICHIPINFO_ID_QSC6270            = 9,  /**< QSC6270 chip. */
  EFICHIPINFO_ID_MSM6280            = 10, /**< MSM6280 chip. */
  EFICHIPINFO_ID_MSM6290            = 11, /**< MSM6290 chip. */
  EFICHIPINFO_ID_MSM7200            = 12, /**< MSM7200 chip. */
  EFICHIPINFO_ID_MSM7201            = 13, /**< MSM7201 chip. */
  EFICHIPINFO_ID_ESM7205            = 14, /**< ESM7205 chip. */
  EFICHIPINFO_ID_ESM7206            = 15, /**< ESM7206 chip. */
  EFICHIPINFO_ID_MSM7200A           = 16, /**< MSM7200A chip. */
  EFICHIPINFO_ID_MSM7201A           = 17, /**< MSM7201A chip. */
  EFICHIPINFO_ID_ESM7205A           = 18, /**< ESM7205A chip. */
  EFICHIPINFO_ID_ESM7206A           = 19, /**< ESM7206A chip. */
  EFICHIPINFO_ID_ESM7225            = 20, /**< ESM7225 chip. */
  EFICHIPINFO_ID_MSM7225            = 21, /**< MSM7225 chip. */
  EFICHIPINFO_ID_MSM7500            = 22, /**< MSM7500 chip. */
  EFICHIPINFO_ID_MSM7500A           = 23, /**< MSM7500A chip. */
  EFICHIPINFO_ID_MSM7525            = 24, /**< MSM7525 chip. */
  EFICHIPINFO_ID_MSM7600            = 25, /**< MSM7600 chip. */
  EFICHIPINFO_ID_MSM7601            = 26, /**< MSM7601 chip. */
  EFICHIPINFO_ID_MSM7625            = 27, /**< MSM7625 chip. */
  EFICHIPINFO_ID_MSM7800            = 28, /**< MSM7800 chip. */
  EFICHIPINFO_ID_MDM8200            = 29, /**< MDM8200 chip. */
  EFICHIPINFO_ID_QSD8650            = 30, /**< QSD8650 chip. */
  EFICHIPINFO_ID_MDM8900            = 31, /**< MDM8900 chip. */
  EFICHIPINFO_ID_QST1000            = 32, /**< QST1000 chip. */
  EFICHIPINFO_ID_QST1005            = 33, /**< QST1005 chip. */
  EFICHIPINFO_ID_QST1100            = 34, /**< QST1100 chip. */
  EFICHIPINFO_ID_QST1105            = 35, /**< QST1105 chip. */
  EFICHIPINFO_ID_QST1500            = 40, /**< QST1500 chip. */
  EFICHIPINFO_ID_QST1600            = 41, /**< QST1600 chip. */
  EFICHIPINFO_ID_QST1700            = 42, /**< QST1700 chip. */
  EFICHIPINFO_ID_QSD8250            = 36, /**< QSD8250 chip. */
  EFICHIPINFO_ID_QSD8550            = 37, /**< QSD8550 chip. */
  EFICHIPINFO_ID_QSD8850            = 38, /**< QSD8850 chip. */
  EFICHIPINFO_ID_MDM2000            = 39, /**< MDM2000 chip. */
  EFICHIPINFO_ID_MSM7227            = 43, /**< MSM7227 chip. */
  EFICHIPINFO_ID_MSM7627            = 44, /**< MSM7627 chip. */
  EFICHIPINFO_ID_QSC6165            = 45, /**< QSC6165 chip. */
  EFICHIPINFO_ID_QSC6175            = 46, /**< QSC6175 chip. */
  EFICHIPINFO_ID_QSC6185            = 47, /**< QSC6185 chip. */
  EFICHIPINFO_ID_QSC6195            = 48, /**< QSC6195 chip. */
  EFICHIPINFO_ID_QSC6285            = 49, /**< QSC6285 chip. */
  EFICHIPINFO_ID_QSC6295            = 50, /**< QSC6295 chip. */
  EFICHIPINFO_ID_QSC6695            = 51, /**< QSC6695 chip. */
  EFICHIPINFO_ID_ESM6246            = 52, /**< ESM6246 chip. */
  EFICHIPINFO_ID_ESM6290            = 53, /**< ESM6290 chip. */
  EFICHIPINFO_ID_ESC6270            = 54, /**< ESC6270 chip. */
  EFICHIPINFO_ID_ESC6240            = 55, /**< ESC6240 chip. */
  EFICHIPINFO_ID_MDM8220            = 56, /**< MDM8220 chip. */
  EFICHIPINFO_ID_MDM9200            = 57, /**< MDM9200 chip. */
  EFICHIPINFO_ID_MDM9600            = 58, /**< MDM9600 chip. */
  EFICHIPINFO_ID_MSM7630            = 59, /**< MSM7630 chip. */
  EFICHIPINFO_ID_MSM7230            = 60, /**< MSM7230 chip. */
  EFICHIPINFO_ID_ESM7227            = 61, /**< ESM7227 chip. */
  EFICHIPINFO_ID_MSM7625D1          = 62, /**< MSM7625D1 chip. */
  EFICHIPINFO_ID_MSM7225D1          = 63, /**< MSM7225D1 chip. */
  EFICHIPINFO_ID_QSD8250A           = 64, /**< QSD8250A chip. */
  EFICHIPINFO_ID_QSD8650A           = 65, /**< QSD8650A chip. */
  EFICHIPINFO_ID_MSM7625D2          = 66, /**< MSM7625D2 chip. */
  EFICHIPINFO_ID_MSM7227D1          = 67, /**< MSM7227D1 chip. */
  EFICHIPINFO_ID_MSM7627D1          = 68, /**< MSM7627D1 chip. */
  EFICHIPINFO_ID_MSM7627D2          = 69, /**< MSM7627D2 chip. */
  EFICHIPINFO_ID_MSM8260            = 70, /**< MSM8260 chip. */
  EFICHIPINFO_ID_MSM8660            = 71, /**< MSM8660 chip. */
  EFICHIPINFO_ID_MDM8200A           = 72, /**< MDM8200A chip. */
  EFICHIPINFO_ID_QSC6155            = 73, /**< QSC6155 chip. */
  EFICHIPINFO_ID_MSM8255            = 74, /**< MSM8255 chip. */
  EFICHIPINFO_ID_MSM8655            = 75, /**< MSM8655 chip. */
  EFICHIPINFO_ID_ESC6295            = 76, /**< ESC6295 chip. */
  EFICHIPINFO_ID_MDM3000            = 77, /**< MDM3000 chip. */
  EFICHIPINFO_ID_MDM6200            = 78, /**< MDM6200 chip. */
  EFICHIPINFO_ID_MDM6600            = 79, /**< MDM6600 chip. */
  EFICHIPINFO_ID_MDM6210            = 80, /**< MDM6210 chip. */
  EFICHIPINFO_ID_MDM6610            = 81, /**< MDM6610 chip. */
  EFICHIPINFO_ID_QSD8672            = 82, /**< QSD8672 chip. */
  EFICHIPINFO_ID_MDM6215            = 83, /**< MDM6215 chip. */
  EFICHIPINFO_ID_MDM6615            = 84, /**< MDM6615 chip. */
  EFICHIPINFO_ID_APQ8055            = 85, /**< APQ8055 chip. */
  EFICHIPINFO_ID_APQ8060            = 86, /**< APQ8060 chip. */
  EFICHIPINFO_ID_MSM8960            = 87, /**< MSM8960 chip. */
  EFICHIPINFO_ID_MSM7225A           = 88, /**< MSM7225A chip. */
  EFICHIPINFO_ID_MSM7625A           = 89, /**< MSM7625A chip. */
  EFICHIPINFO_ID_MSM7227A           = 90, /**< MSM7227A chip. */
  EFICHIPINFO_ID_MSM7627A           = 91, /**< MSM7627A chip. */
  EFICHIPINFO_ID_ESM7227A           = 92, /**< ESM7227A chip. */
  EFICHIPINFO_ID_QSC6195D2          = 93, /**< QSC6195D2 chip. */
  EFICHIPINFO_ID_FSM9200            = 94, /**< FSM9200 chip. */
  EFICHIPINFO_ID_FSM9800            = 95, /**< FSM9800 chip. */
  EFICHIPINFO_ID_MSM7225AD1         = 96, /**< MSM7225AD1 chip. */
  EFICHIPINFO_ID_MSM7227AD1         = 97, /**< MSM7227AD1 chip. */
  EFICHIPINFO_ID_MSM7225AA          = 98,  /**< MSM7225AA chip. */
  EFICHIPINFO_ID_MSM7225AAD1        = 99,  /**< MSM7225AAD1 chip. */
  EFICHIPINFO_ID_MSM7625AA          = 100, /**< MSM7625AA chip. */
  EFICHIPINFO_ID_MSM7227AA          = 101, /**< MSM7227AA chip. */
  EFICHIPINFO_ID_MSM7227AAD1        = 102, /**< MSM7227AAD1 chip. */
  EFICHIPINFO_ID_MSM7627AA          = 103, /**< MSM7627AA chip. */
  EFICHIPINFO_ID_MDM9615            = 104, /**< MDM9615 chip. */
  EFICHIPINFO_ID_MDM9615M           = EFICHIPINFO_ID_MDM9615, /**< MDM9615M chip. */
  EFICHIPINFO_ID_MDM8215            = 106, /**< MDM8215 chip. */
  EFICHIPINFO_ID_MDM9215            = 107, /**< MDM9215 chip. */
  EFICHIPINFO_ID_MDM9215M           = EFICHIPINFO_ID_MDM9215,
  EFICHIPINFO_ID_APQ8064            = 109, /**< APQ8064 chip. */
  EFICHIPINFO_ID_QSC6270D1          = 110, /**< QSC6270D1 chip. */
  EFICHIPINFO_ID_QSC6240D1          = 111, /**< QSC6240D1 chip. */
  EFICHIPINFO_ID_ESC6270D1          = 112, /**< ESC6270D1 chip. */
  EFICHIPINFO_ID_ESC6240D1          = 113, /**< ESC6240D1 chip. */
  EFICHIPINFO_ID_MDM6270            = 114, /**< MDM6270 chip. */
  EFICHIPINFO_ID_MDM6270D1          = 115, /**< MDM6270D1 chip. */
  EFICHIPINFO_ID_MSM8930            = 116, /**< MSM8930 chip. */
  EFICHIPINFO_ID_MSM8630            = 117, /**< MSM8630 chip. */
  EFICHIPINFO_ID_MSM8230            = 118, /**< MSM8230 chip. */
  EFICHIPINFO_ID_APQ8030            = 119, /**< APQ8030 chip. */
  EFICHIPINFO_ID_MSM8627            = 120, /**< MSM8627 chip. */
  EFICHIPINFO_ID_MSM8227            = 121, /**< MSM8227 chip. */
  EFICHIPINFO_ID_MSM8660A           = 122, /**< MSM8660A chip. */
  EFICHIPINFO_ID_MSM8260A           = 123, /**< MSM8260A chip. */
  EFICHIPINFO_ID_APQ8060A           = 124, /**< APQ8060A chip. */
  EFICHIPINFO_ID_MPQ8062            = 125, /**< MPQ8062 chip. */
  /** @endcond */
  EFICHIPINFO_ID_MSM8974            = 126, /**< MSM8974 chip. */
  /** @cond */
  EFICHIPINFO_ID_MSM8225            = 127, /**< MSM8225 chip. */
  EFICHIPINFO_ID_MSM8225D1          = 128, /**< MSM8225D1 chip. */
  EFICHIPINFO_ID_MSM8625            = 129, /**< MSM8625 chip. */
  EFICHIPINFO_ID_MPQ8064            = 130, /**< MPQ8064 chip. */
  EFICHIPINFO_ID_MSM7225AB          = 131, /**< MSM7225AB chip. */
  EFICHIPINFO_ID_MSM7225ABD1        = 132, /**< MSM7225ABD1 chip. */
  EFICHIPINFO_ID_MSM7625AB          = 133, /**< MSM7625AB chip. */
  EFICHIPINFO_ID_MDM9625            = 134, /**< MDM9625 chip. */
  EFICHIPINFO_ID_MSM7125A           = 135, /**< MSM7125A chip. */
  EFICHIPINFO_ID_MSM7127A           = 136, /**< MSM7127A chip. */
  EFICHIPINFO_ID_MSM8125AB          = 137, /**< MSM8125AB chip. */
  EFICHIPINFO_ID_MSM8626            = 145, /**< MSM8626 chip. */
  EFICHIPINFO_ID_MPQ8092            = 146, /**< MPQ8092 chip. */
  EFICHIPINFO_ID_MSM8610            = 147, /**< MSM8610 chip. */
  EFICHIPINFO_ID_MDM8225            = 148, /**< MDM8225 chip. */
  EFICHIPINFO_ID_MDM9225            = 149, /**< MDM9225 chip. */
  EFICHIPINFO_ID_MDM9225M           = 150, /**< MDM9225M chip. */
  EFICHIPINFO_ID_MDM9624M           = 151, /**< MDM9624M chip. */
  EFICHIPINFO_ID_MDM9625M           = 152, /**< MDM9625M chip. */
  EFICHIPINFO_ID_MSM8226            = 158, /**< MSM8226 chip. */
  EFICHIPINFO_ID_MSM8826            = 159, /**< MSM8826 chip. */
  EFICHIPINFO_ID_APQ8030AA          = 160, /**< APQ8030AA chip. */
  EFICHIPINFO_ID_MSM8110            = 161, /**< MSM8110 chip. */
  EFICHIPINFO_ID_MSM8210            = 162, /**< MSM8210 chip. */
  EFICHIPINFO_ID_MSM8810            = 163, /**< MSM8810 chip. */
  EFICHIPINFO_ID_MSM8212            = 164, /**< MSM8212 chip. */
  EFICHIPINFO_ID_MSM8612            = 165, /**< MSM8612 chip. */
  EFICHIPINFO_ID_MSM8812            = 166, /**< MSM8812 chip. */
  EFICHIPINFO_ID_MSM8125            = 167, /**< MSM8125 chip. */
  EFICHIPINFO_ID_MSM8225Q           = 168, /**< MSM8225Q chip. */
  EFICHIPINFO_ID_MSM8625Q           = 169, /**< MSM8625Q chip. */
  EFICHIPINFO_ID_MSM8125Q           = 170, /**< MSM8125Q chip. */
  EFICHIPINFO_ID_MDM9310            = 171, /**< MDM9310 chip. */
  EFICHIPINFO_ID_APQ8064_SLOW_PRIME = 172, /**< APQ8064 (slow prime) chip. */
  EFICHIPINFO_ID_MDM8110M           = 173, /**< MDM8110M chip. */
  EFICHIPINFO_ID_MDM8615M           = 174, /**< MDM8615M chip. */
  EFICHIPINFO_ID_MDM9320            = 175, /**< MDM9320 chip. */
  EFICHIPINFO_ID_MDM9225_1          = 176, /**< MDM9225_1 chip. */
  EFICHIPINFO_ID_MDM9225M_1         = 177, /**< MDM9225M_1 chip. */
  EFICHIPINFO_ID_APQ8084            = 178, /**< APQ8084 chip. */
  EFICHIPINFO_ID_MSM8130            = 179, /**< MSM8130 chip. */
  EFICHIPINFO_ID_MSM8130AA          = 180, /**< MSM8130AA chip. */
  EFICHIPINFO_ID_MSM8130AB          = 181, /**< MSM8130AB chip. */
  EFICHIPINFO_ID_MSM8627AA          = 182, /**< MSM8627AA chip. */
  EFICHIPINFO_ID_MSM8227AA          = 183, /**< MSM8227AA chip. */
  EFICHIPINFO_ID_APQ8074            = 184, /**< APQ8074 chip. */
  EFICHIPINFO_ID_MSM8274            = 185, /**< MSM8274 chip. */
  EFICHIPINFO_ID_MSM8674            = 186, /**< MSM8674 chip. */
  EFICHIPINFO_ID_MDM9635            = 187, /**< MDM9635 chip. */
  EFICHIPINFO_ID_FSM9900            = 188, /**< FSM9900 chip. */
  EFICHIPINFO_ID_FSM9965            = 189, /**< FSM9965 chip. */
  EFICHIPINFO_ID_FSM9955            = 190, /**< FSM9955 chip. */
  EFICHIPINFO_ID_FSM9950            = 191, /**< FSM9950 chip. */
  EFICHIPINFO_ID_FSM9915            = 192, /**< FSM9915 chip. */
  EFICHIPINFO_ID_FSM9910            = 193, /**< FSM9910 chip. */
  EFICHIPINFO_ID_MSM8974_PRO        = 194, /**< MSM8974 (pro) chip. */
  EFICHIPINFO_ID_MSM8962            = 195, /**< MSM8962 chip. */
  EFICHIPINFO_ID_MSM8262            = 196, /**< MSM8262 chip. */
  EFICHIPINFO_ID_APQ8062            = 197, /**< APQ8062 chip. */
  EFICHIPINFO_ID_MSM8126            = 198, /**< MSM8126 chip. */
  EFICHIPINFO_ID_APQ8026            = 199, /**< APQ8026 chip. */
  EFICHIPINFO_ID_MSM8926            = 200, /**< MSM8926 chip. */
  EFICHIPINFO_ID_MSM8326            = 205, /**< MSM8326 chip. */
  EFICHIPINFO_ID_MSM8916            = 206, /**< MSM8916 chip. */
  EFICHIPINFO_ID_MSM8994            = 207, /**< MSM8994 chip. */
  EFICHIPINFO_ID_APQ8074_AA         = 208, /**< APQ8074AA chip. */
  EFICHIPINFO_ID_APQ8074_AB         = 209, /**< APQ8074AB chip. */
  EFICHIPINFO_ID_APQ8074_PRO        = 210, /**< APQ8074 (pro) chip. */
  EFICHIPINFO_ID_MSM8274_AA         = 211, /**< MSM8274AA chip. */
  EFICHIPINFO_ID_MSM8274_AB         = 212, /**< MSM8274AB chip. */
  EFICHIPINFO_ID_MSM8274_PRO        = 213, /**< MSM8274 (pro) chip. */
  EFICHIPINFO_ID_MSM8674_AA         = 214, /**< MSM8674AA chip. */
  EFICHIPINFO_ID_MSM8674_AB         = 215, /**< MSM8674AB chip. */
  EFICHIPINFO_ID_MSM8674_PRO        = 216, /**< MSM8674 (pro) chip. */
  EFICHIPINFO_ID_MSM8974_AA         = 217, /**< MSM8974AA chip. */
  EFICHIPINFO_ID_MSM8974_AB         = 218, /**< MSM8974AB chip. */
  EFICHIPINFO_ID_APQ8028            = 219, /**< APQ8028 chip. */
  EFICHIPINFO_ID_MSM8128            = 220, /**< MSM8128 chip. */
  EFICHIPINFO_ID_MSM8228            = 221, /**< MSM8228 chip. */
  EFICHIPINFO_ID_MSM8528            = 222, /**< MSM8528 chip. */
  EFICHIPINFO_ID_MSM8628            = 223, /**< MSM8628 chip. */
  EFICHIPINFO_ID_MSM8928            = 224, /**< MSM8928 chip. */
  EFICHIPINFO_ID_MSM8510            = 225, /**< MSM8510 chip. */
  EFICHIPINFO_ID_MSM8512            = 226, /**< MSM8512 chip. */
  EFICHIPINFO_ID_MDM9630            = 227,
  EFICHIPINFO_ID_MDM9635M           = EFICHIPINFO_ID_MDM9635,
  EFICHIPINFO_ID_MDM9230            = 228,
  EFICHIPINFO_ID_MDM9235M           = 229,
  EFICHIPINFO_ID_MDM8630            = 230,
  EFICHIPINFO_ID_MDM9330            = 231,
  EFICHIPINFO_ID_MPQ8091            = 232,
  EFICHIPINFO_ID_MSM8936            = 233,
  EFICHIPINFO_ID_MDM9240            = 234,
  EFICHIPINFO_ID_MDM9340            = 235,
  EFICHIPINFO_ID_MDM9640            = 236,
  EFICHIPINFO_ID_MDM9245M           = 237,
  EFICHIPINFO_ID_MDM9645M           = 238,
  EFICHIPINFO_ID_MSM8939            = 239,
  EFICHIPINFO_ID_APQ8036            = 240,
  EFICHIPINFO_ID_APQ8039            = 241,
  EFICHIPINFO_ID_MSM8236            = 242,
  EFICHIPINFO_ID_MSM8636            = 243,
  EFICHIPINFO_ID_APQ8064_AU         = 244,
  EFICHIPINFO_ID_MSM8909            = 245,
  EFICHIPINFO_ID_MSM8996            = 246,
  EFICHIPINFO_ID_APQ8016            = 247,
  EFICHIPINFO_ID_MSM8216            = 248,
  EFICHIPINFO_ID_MSM8116            = 249,
  EFICHIPINFO_ID_MSM8616            = 250,
  EFICHIPINFO_ID_MSM8992            = 251,
  EFICHIPINFO_ID_APQ8092            = 252,
  EFICHIPINFO_ID_APQ8094            = 253,
  EFICHIPINFO_ID_FSM9008            = 254,
  EFICHIPINFO_ID_FSM9010            = 255,
  EFICHIPINFO_ID_FSM9016            = 256,
  EFICHIPINFO_ID_FSM9055            = 257,
  EFICHIPINFO_ID_MSM8209            = 258,
  EFICHIPINFO_ID_MSM8208            = 259,
  EFICHIPINFO_ID_MDM9209            = 260,
  EFICHIPINFO_ID_MDM9309            = 261,
  EFICHIPINFO_ID_MDM9609            = 262,
  EFICHIPINFO_ID_MSM8239            = 263,
  EFICHIPINFO_ID_MSM8952            = 264,
  EFICHIPINFO_ID_APQ8009            = 265,
  EFICHIPINFO_ID_MSM8956            = 266,
  EFICHIPINFO_ID_QDF2432            = 267,
  EFICHIPINFO_ID_MSM8929            = 268,
  EFICHIPINFO_ID_MSM8629            = 269,
  EFICHIPINFO_ID_MSM8229            = 270,
  EFICHIPINFO_ID_APQ8029            = 271,
  EFICHIPINFO_ID_QCA9618            = 272,
  EFICHIPINFO_ID_IPQ4018            = EFICHIPINFO_ID_QCA9618,
  EFICHIPINFO_ID_QCA9619            = 273,
  EFICHIPINFO_ID_IPQ4019            = EFICHIPINFO_ID_QCA9619,
  EFICHIPINFO_ID_APQ8056            = 274,
  EFICHIPINFO_ID_MSM8609            = 275,
  EFICHIPINFO_ID_FSM9916            = 276,
  EFICHIPINFO_ID_APQ8076            = 277,
  EFICHIPINFO_ID_MSM8976            = 278,
  EFICHIPINFO_ID_MDM9650            = 279,
  EFICHIPINFO_ID_IPQ8065            = 280,
  EFICHIPINFO_ID_IPQ8069            = 281,
  EFICHIPINFO_ID_MSM8939_BC         = 282,
  EFICHIPINFO_ID_MDM9250            = 283,
  EFICHIPINFO_ID_MDM9255            = 284,
  EFICHIPINFO_ID_MDM9350            = 285,
  EFICHIPINFO_ID_MDM9655            = 286,
  EFICHIPINFO_ID_IPQ4028            = 287,
  EFICHIPINFO_ID_IPQ4029            = 288,
  EFICHIPINFO_ID_APQ8052            = 289,
  EFICHIPINFO_ID_MDM9607            = 290,
  EFICHIPINFO_ID_APQ8096            = 291,
  EFICHIPINFO_ID_MSM8998            = 292,
  EFICHIPINFO_ID_MSM8953            = 293,
  EFICHIPINFO_ID_MSM8937            = 294,
  EFICHIPINFO_ID_APQ8037            = 295,
  EFICHIPINFO_ID_MDM8207            = 296,
  EFICHIPINFO_ID_MDM9207            = 297,
  EFICHIPINFO_ID_MDM9307            = 298,
  EFICHIPINFO_ID_MDM9628            = 299,
  EFICHIPINFO_ID_MSM8909W           = 300,
  EFICHIPINFO_ID_APQ8009W           = 301,
  EFICHIPINFO_ID_MSM8996L           = 302,
  EFICHIPINFO_ID_MSM8917            = 303,
  EFICHIPINFO_ID_APQ8053            = 304,
  EFICHIPINFO_ID_MSM8996SG          = 305,
  EFICHIPINFO_ID_MSM8997            = 306,
  EFICHIPINFO_ID_APQ8017            = 307,
  EFICHIPINFO_ID_MSM8217            = 308,
  EFICHIPINFO_ID_MSM8617            = 309,
  EFICHIPINFO_ID_MSM8996AU          = 310,
  EFICHIPINFO_ID_APQ8096AU          = 311,
  EFICHIPINFO_ID_APQ8096SG          = 312,
  EFICHIPINFO_ID_MSM8940            = 313,
  EFICHIPINFO_ID_MDM9665            = 314,
  EFICHIPINFO_ID_MSM8996SGAU        = 315,
  EFICHIPINFO_ID_APQ8096SGAU        = 316,
  EFICHIPINFO_ID_SDM660             = 317,
  EFICHIPINFO_ID_SDM630             = 318,
  EFICHIPINFO_ID_APQ8098            = 319,
  EFICHIPINFO_ID_MSM8920            = 320,
  EFICHIPINFO_ID_SDM845             = 321,
  EFICHIPINFO_ID_MDM9206            = 322,
  EFICHIPINFO_ID_IPQ8074            = 323,
  EFICHIPINFO_ID_SDA660             = 324,
  EFICHIPINFO_ID_SDM658             = 325,
  EFICHIPINFO_ID_SDA658             = 326,
  EFICHIPINFO_ID_SDA630             = 327,
  EFICHIPINFO_ID_SDM830             = 328,
  EFICHIPINFO_ID_SDX50M             = 329,
  EFICHIPINFO_ID_QCA6290            = 330,
  EFICHIPINFO_ID_MSM8905            = 331,
  EFICHIPINFO_ID_SDC830             = 332,
  EFICHIPINFO_ID_MDM9660            = 333,
  EFICHIPINFO_ID_SDX24              = 334,
  EFICHIPINFO_ID_SDX24M             = 335,
  EFICHIPINFO_ID_SDM670             = 336,
  EFICHIPINFO_ID_SDA670             = 337,
  EFICHIPINFO_ID_SDM450             = 338,
  EFICHIPINFO_ID_SDM855             = 339,
  EFICHIPINFO_ID_SDM900             = 340,
  EFICHIPINFO_ID_SDM1000            = EFICHIPINFO_ID_SDM900,
  EFICHIPINFO_ID_SDC845             = 341,
  EFICHIPINFO_ID_IPQ8072            = 342,
  EFICHIPINFO_ID_IPQ8076            = 343,
  EFICHIPINFO_ID_IPQ8078            = 344,
  EFICHIPINFO_ID_SDA845             = EFICHIPINFO_ID_SDC845,
  EFICHIPINFO_ID_SDX20M             = EFICHIPINFO_ID_MDM9665,
  EFICHIPINFO_ID_SDM636             = 345,
  EFICHIPINFO_ID_SDA636             = 346,
  EFICHIPINFO_ID_QCS605             = 347,
  EFICHIPINFO_ID_SDM850             = 348,
  EFICHIPINFO_ID_SDM632             = 349,
  EFICHIPINFO_ID_SDA632             = 350,
  EFICHIPINFO_ID_SDA450             = 351,
  EFICHIPINFO_ID_QCS6200            = 352,
  EFICHIPINFO_ID_SDM439             = 353,
  EFICHIPINFO_ID_SDM429             = 354,
  EFICHIPINFO_ID_SDM640             = 355,
  EFICHIPINFO_ID_QCS405             = EFICHIPINFO_ID_QCS6200,
  EFICHIPINFO_ID_SDA865             = 356,
  EFICHIPINFO_ID_SDX55              = 357,
  EFICHIPINFO_ID_QCA6390            = 358,
  EFICHIPINFO_ID_MDM9150            = 359,
  EFICHIPINFO_ID_SDM710             = 360,
  EFICHIPINFO_ID_SDA855             = 361,
  EFICHIPINFO_ID_SDM855A            = 362,
  EFICHIPINFO_ID_SM8150             = EFICHIPINFO_ID_SDM855,
  EFICHIPINFO_ID_SM8150P            = EFICHIPINFO_ID_SDA855,
  EFICHIPINFO_ID_SM8250             = EFICHIPINFO_ID_SDA865,
  EFICHIPINFO_ID_SCX8180            = EFICHIPINFO_ID_SDM1000,
  EFICHIPINFO_ID_SDA439             = 363,
  EFICHIPINFO_ID_SDA429             = 364,
  EFICHIPINFO_ID_SM7150             = 365,
  EFICHIPINFO_ID_SM7150P            = 366,
  EFICHIPINFO_ID_SDA855A            = 367,
  EFICHIPINFO_ID_SA8150             = EFICHIPINFO_ID_SDM855A,
  EFICHIPINFO_ID_SA8150P            = EFICHIPINFO_ID_SDA855A,
  EFICHIPINFO_ID_SDX55M             = 368,
  EFICHIPINFO_ID_SM6150             = EFICHIPINFO_ID_SDM640,
  EFICHIPINFO_ID_SM6150P            = 369,
  EFICHIPINFO_ID_SXR1120            = 370,
  EFICHIPINFO_ID_SXR1130            = 371,
  EFICHIPINFO_ID_QCS401             = 372,
  EFICHIPINFO_ID_QCS403             = 373,
  EFICHIPINFO_ID_SA8155             = 374,
  EFICHIPINFO_ID_IPQ8070            = 375,
  EFICHIPINFO_ID_IPQ8071            = 376,
  EFICHIPINFO_ID_SA6155P            = 377,
  EFICHIPINFO_ID_SA6150P            = 378,
  EFICHIPINFO_ID_SA6145P            = 379,
  EFICHIPINFO_ID_SA4155P            = 380,
  EFICHIPINFO_ID_QCA6595            = 381,
  EFICHIPINFO_ID_QCN7605            = 382,
  EFICHIPINFO_ID_QCN7606            = 383,
  EFICHIPINFO_ID_SC8180X            = EFICHIPINFO_ID_SCX8180,
  EFICHIPINFO_ID_SA6155             = 384,
  EFICHIPINFO_ID_SDM455             = 385,
  EFICHIPINFO_ID_QM215              = 386,
  EFICHIPINFO_ID_APQ8096A           = 387,
  EFICHIPINFO_ID_MDM9205            = 388,
  EFICHIPINFO_ID_IPQ8072A           = 389,
  EFICHIPINFO_ID_IPQ8074A           = 390,
  EFICHIPINFO_ID_IPQ8076A           = 391,
  EFICHIPINFO_ID_IPQ8078A           = 392,
  EFICHIPINFO_ID_SDM712             = 393,
  EFICHIPINFO_ID_SM_NICOBAR         = 394,
  EFICHIPINFO_ID_IPQ8070A           = 395,
  EFICHIPINFO_ID_IPQ8071A           = 396,
  EFICHIPINFO_ID_IPQ8172            = 397,
  EFICHIPINFO_ID_IPQ8173            = 398,
  EFICHIPINFO_ID_IPQ8174            = 399,
  EFICHIPINFO_ID_SM_SAIPAN          = 400,
  EFICHIPINFO_ID_QCS610             = 401,
  EFICHIPINFO_ID_IPQ6018            = 402,
  EFICHIPINFO_ID_IPQ6028            = 403,
  EFICHIPINFO_ID_SC8180XP           = 404,
  EFICHIPINFO_ID_SA8195P            = 405,
  EFICHIPINFO_ID_QCS410             = 406,
  EFICHIPINFO_ID_SM_RENNELL         = 407,
  EFICHIPINFO_ID_SA415M             = 408,
  EFICHIPINFO_ID_QCA6490            = 409,
  EFICHIPINFO_ID_QCS404             = 410,
  EFICHIPINFO_ID_QCS407             = 411,
  EFICHIPINFO_ID_QCA6480            = 412,
  EFICHIPINFO_ID_QCA6481            = 413,
  EFICHIPINFO_ID_QCA6491            = 414,
  EFICHIPINFO_ID_SM_LAHAINA         = 415,
  EFICHIPINFO_ID_SDM429W            = 416,
  EFICHIPINFO_ID_SM_KAMORTA         = 417,
  EFICHIPINFO_ID_SA515M             = 418,
  EFICHIPINFO_ID_FSM10055           = 419,
  EFICHIPINFO_ID_SMP_KAMORTA        = 420,
  EFICHIPINFO_ID_IPQ6000            = 421,
  EFICHIPINFO_ID_IPQ6010            = 422,
  EFICHIPINFO_ID_QCN3018            = 423,
  EFICHIPINFO_ID_SMP_RENNEL         = 424,
  EFICHIPINFO_ID_SC7180             = 425,
  EFICHIPINFO_ID_QCN9000            = 426,
  EFICHIPINFO_ID_QCN9001            = 427,
  EFICHIPINFO_ID_QCN9002            = 428,
  EFICHIPINFO_ID_QCN9003            = 429,
  EFICHIPINFO_ID_QCN9010            = 430,
  EFICHIPINFO_ID_QCN9011            = 431,
  EFICHIPINFO_ID_QCN9012            = 432,
  EFICHIPINFO_ID_QCN9013            = 433,
  EFICHIPINFO_ID_SM_BITRA           = 434,
  EFICHIPINFO_ID_SMP_BITRA          = 435,
  EFICHIPINFO_ID_QCM2150            = 436,
  EFICHIPINFO_ID_SDA429W            = 437,
  EFICHIPINFO_ID_SDX_CHITWAN        = 438,
  EFICHIPINFO_ID_SMP_LAHAINA        = 439,
  EFICHIPINFO_ID_SM_SAIPAN_MODULE   = 440,
  EFICHIPINFO_ID_QM_AGATTI	        = 441,
  EFICHIPINFO_ID_WCN_MOSELLE	    = 442,
  EFICHIPINFO_ID_SM_RENNELL_AB	    = 443,
  EFICHIPINFO_ID_SM_KAMORTA_H	    = 444,
  EFICHIPINFO_ID_SMP_KAMORTA_H	    = 445,
  EFICHIPINFO_ID_IPQ5010	        = 446,
  EFICHIPINFO_ID_IPQ5018	        = 447,
  EFICHIPINFO_ID_IPQ5028	        = 448,
  EFICHIPINFO_ID_SC_MAKENA	        = 449,
  EFICHIPINFO_ID_SM_CEDROS	        = 450,
  EFICHIPINFO_ID_SA2145P	        = 451,
  EFICHIPINFO_ID_SA2150P	        = 452,
  EFICHIPINFO_ID_IPQ6005	        = 453,
  EFICHIPINFO_ID_SM_MANNAR	        = 454,
  EFICHIPINFO_ID_QRB5165	        = 455,

  EFICHIPINFO_NUM_IDS,
  EFICHIPINFO_ID_32BITS             = 0x7FFFFFF
/** @endcond */
} EFIChipInfoIdType;

/** Chip family. This is the family type of the chip on which the software is
  running. Each family may include multiple chip IDs.
*/
typedef enum
{
  /** @cond */
  EFICHIPINFO_FAMILY_UNKNOWN    = 0,  /**< Unknown family. */
  EFICHIPINFO_FAMILY_MSM6246    = 1,  /**< MSM6246 family. */
  EFICHIPINFO_FAMILY_MSM6260    = 2,  /**< MSM6260 family. */
  EFICHIPINFO_FAMILY_QSC6270    = 3,  /**< QSC6270 family. */
  EFICHIPINFO_FAMILY_MSM6280    = 4,  /**< MSM6280 family. */
  EFICHIPINFO_FAMILY_MSM6290    = 5,  /**< MSM6290 family. */
  EFICHIPINFO_FAMILY_MSM7200    = 6,  /**< MSM7200 family. */
  EFICHIPINFO_FAMILY_MSM7500    = 7,  /**< MSM7500 family. */
  EFICHIPINFO_FAMILY_MSM7600    = 8,  /**< MSM7600 family. */
  EFICHIPINFO_FAMILY_MSM7625    = 9,  /**< MSM7625 family. */
  EFICHIPINFO_FAMILY_MSM7X30    = 10, /**< MSM7X30 family. */
  EFICHIPINFO_FAMILY_MSM7800    = 11, /**< MSM7800 family. */
  EFICHIPINFO_FAMILY_MDM8200    = 12, /**< MDM8200 family. */
  EFICHIPINFO_FAMILY_QSD8650    = 13, /**< QSD8650 family. */
  EFICHIPINFO_FAMILY_MSM7627    = 14, /**< MSM7627 family. */
  EFICHIPINFO_FAMILY_QSC6695    = 15, /**< QSC6695 family. */
  EFICHIPINFO_FAMILY_MDM9X00    = 16, /**< MDM9X00 family. */
  EFICHIPINFO_FAMILY_QSD8650A   = 17, /**< QSD8650A family. */
  EFICHIPINFO_FAMILY_MSM8X60    = 18, /**< MSM8X60 family. */
  EFICHIPINFO_FAMILY_MDM8200A   = 19, /**< MDM8200A family. */
  EFICHIPINFO_FAMILY_QSD8672    = 20, /**< QSD8672 family. */
  EFICHIPINFO_FAMILY_MDM6615    = 21, /**< MDM6615 family. */
  EFICHIPINFO_FAMILY_MSM8660    = EFICHIPINFO_FAMILY_MSM8X60,
/** @endcond */
  EFICHIPINFO_FAMILY_MSM8960    = 22, /**< MSM8960 family. */
  EFICHIPINFO_FAMILY_MSM7625A   = 23, /**< MSM7625A family. */
  EFICHIPINFO_FAMILY_MSM7627A   = 24, /**< MSM7627A family. */
  EFICHIPINFO_FAMILY_MDM9X15    = 25, /**< MDM9X15 family. */
  EFICHIPINFO_FAMILY_MSM8930    = 26, /**< MSM8930 family. */
  EFICHIPINFO_FAMILY_MSM8630    = EFICHIPINFO_FAMILY_MSM8930,
  EFICHIPINFO_FAMILY_MSM8230    = EFICHIPINFO_FAMILY_MSM8930,
  EFICHIPINFO_FAMILY_APQ8030    = EFICHIPINFO_FAMILY_MSM8930,
  EFICHIPINFO_FAMILY_MSM8627    = 30, /**< MSM8627 family. */
  EFICHIPINFO_FAMILY_MSM8227    = EFICHIPINFO_FAMILY_MSM8627,
/** @endcond */
  EFICHIPINFO_FAMILY_MSM8974    = 32, /**< MSM8974 family. */
/** @cond */
  EFICHIPINFO_FAMILY_MSM8625    = 33, /**< MSM8625 family. */
  EFICHIPINFO_FAMILY_MSM8225    = EFICHIPINFO_FAMILY_MSM8625,
  EFICHIPINFO_FAMILY_APQ8064    = 34, /**< APQ8064 family. */
  EFICHIPINFO_FAMILY_MDM9x25    = 35, /**< MDM9x25 family. */
  EFICHIPINFO_FAMILY_MSM8960AB  = 36, /**< MSM8960AB family. */
  EFICHIPINFO_FAMILY_MSM8930AB  = 36, /**< MSM8930AB family. */
  EFICHIPINFO_FAMILY_MSM8x10    = 38, /**< MSM8x10 family. */
  EFICHIPINFO_FAMILY_MPQ8092    = 39, /**< MPQ8092 family. */
  EFICHIPINFO_FAMILY_MSM8x26    = 40, /**< MSM8x26 family. */
  EFICHIPINFO_FAMILY_MSM8225Q   = 41, /**< MSM8225Q family. */
  EFICHIPINFO_FAMILY_MSM8625Q   = 42, /**< MSM8625Q family. */
  EFICHIPINFO_FAMILY_APQ8x94    = 43, /**< APQ8x94 family. */
  EFICHIPINFO_FAMILY_MSM8x32    = 44, /**< MSM8x32 family. */
  EFICHIPINFO_FAMILY_MDM9x35    = 45, /**< MDM9x35 family. */
  EFICHIPINFO_FAMILY_MSM8974_PRO= 46, /**< MSM8974 Pro family. */
  EFICHIPINFO_FAMILY_FSM9900    = 47, /**< FSM9900 family. */
  EFICHIPINFO_FAMILY_MSM8x62    = 48, /**< MSM8x62 family. */
  EFICHIPINFO_FAMILY_MSM8926    = 49, /**< MSM8926 family. */
  EFICHIPINFO_FAMILY_MSM8994    = 50, /**< MSM8994 family. */
  EFICHIPINFO_FAMILY_IPQ8064    = 51, /**< IPQ8064 family. */
  EFICHIPINFO_FAMILY_MSM8916    = 52, /**< MSM8916 family. */
  EFICHIPINFO_FAMILY_MSM8936    = 53,
  EFICHIPINFO_FAMILY_MDM9x45    = 54,
  EFICHIPINFO_FAMILY_MSM8996    = 56,
  EFICHIPINFO_FAMILY_APQ8096    = EFICHIPINFO_FAMILY_MSM8996,
  EFICHIPINFO_FAMILY_MSM8992    = 57,
  EFICHIPINFO_FAMILY_MSM8909    = 58,
  EFICHIPINFO_FAMILY_FSM90xx    = 59,
  EFICHIPINFO_FAMILY_MSM8952    = 60,
  EFICHIPINFO_FAMILY_QDF2432    = 61,
  EFICHIPINFO_FAMILY_MSM8929    = 62,
  EFICHIPINFO_FAMILY_MSM8956    = 63,
  EFICHIPINFO_FAMILY_MSM8976    = EFICHIPINFO_FAMILY_MSM8956,
  EFICHIPINFO_FAMILY_QCA961x    = 64,
  EFICHIPINFO_FAMILY_IPQ40xx    = EFICHIPINFO_FAMILY_QCA961x,
  EFICHIPINFO_FAMILY_MDM9x55    = 65,
  EFICHIPINFO_FAMILY_MDM9x07    = 66,
  EFICHIPINFO_FAMILY_MSM8998    = 67,
  EFICHIPINFO_FAMILY_MSM8953    = 68,
  EFICHIPINFO_FAMILY_MSM8993    = 69,
  EFICHIPINFO_FAMILY_MSM8937    = 70,
  EFICHIPINFO_FAMILY_MSM8917    = 71,
  EFICHIPINFO_FAMILY_MSM8996SG  = 72,
  EFICHIPINFO_FAMILY_MSM8997    = 73,
  EFICHIPINFO_FAMILY_MSM8940    = 74,
  EFICHIPINFO_FAMILY_MDM9x65    = 75,
  EFICHIPINFO_FAMILY_SDM660     = 76,
  EFICHIPINFO_FAMILY_SDM630     = 77,
  EFICHIPINFO_FAMILY_MSM8920    = 78,
  EFICHIPINFO_FAMILY_SDM845     = 79,
  EFICHIPINFO_FAMILY_IPQ807x    = 80,
  EFICHIPINFO_FAMILY_SDM830     = 81,
  EFICHIPINFO_FAMILY_SDX50M     = 82,
  EFICHIPINFO_FAMILY_QCA6290    = 83,
  EFICHIPINFO_FAMILY_SDX24      = 84,
  EFICHIPINFO_FAMILY_SDM670     = 85,
  EFICHIPINFO_FAMILY_SDM855     = 86,
  EFICHIPINFO_FAMILY_SDM900     = 87,
  EFICHIPINFO_FAMILY_SDM1000    = EFICHIPINFO_FAMILY_SDM900,
  EFICHIPINFO_FAMILY_SDX20M     = EFICHIPINFO_FAMILY_MDM9x65,
  EFICHIPINFO_FAMILY_QCS605     = 88,
  EFICHIPINFO_FAMILY_SDM632     = 89,
  EFICHIPINFO_FAMILY_QCS6200    = 90,
  EFICHIPINFO_FAMILY_SDM439     = 91,
  EFICHIPINFO_FAMILY_SDM640     = 92,
  EFICHIPINFO_FAMILY_QCS405     = EFICHIPINFO_FAMILY_QCS6200,
  EFICHIPINFO_FAMILY_SDA865     = 93,
  EFICHIPINFO_FAMILY_SDX55      = 94,
  EFICHIPINFO_FAMILY_QCA639x    = 95,
  EFICHIPINFO_FAMILY_SM8150     = EFICHIPINFO_FAMILY_SDM855,
  EFICHIPINFO_FAMILY_SM8250     = EFICHIPINFO_FAMILY_SDA865,
  EFICHIPINFO_FAMILY_SCX8180    = EFICHIPINFO_FAMILY_SDM1000,
  EFICHIPINFO_FAMILY_SM7150     = 96,
  EFICHIPINFO_FAMILY_SM6150     = EFICHIPINFO_FAMILY_SDM640,
  EFICHIPINFO_FAMILY_SXR1130    = 97,
  EFICHIPINFO_FAMILY_QCN7605    = 98,
  EFICHIPINFO_FAMILY_SC8180X    = EFICHIPINFO_FAMILY_SCX8180,
  EFICHIPINFO_FAMILY_MDM9205    = 99,
  EFICHIPINFO_FAMILY_NICOBAR    = 100,
  EFICHIPINFO_FAMILY_SAIPAN     = 101,
  EFICHIPINFO_FAMILY_IPQ60xx    = 102,
  EFICHIPINFO_FAMILY_RENNELL    = 103,
  EFICHIPINFO_FAMILY_QCA6490    = 104,
  EFICHIPINFO_FAMILY_LAHAINA    = 105,
  EFICHIPINFO_FAMILY_KAMORTA    = 106,
  EFICHIPINFO_FAMILY_QCN90xx    = 107,
  EFICHIPINFO_FAMILY_BITRA      = 108,
  EFICHIPINFO_FAMILY_CHITWAN    = 109,
  EFICHIPINFO_FAMILY_AGATTI     = 110,
  EFICHIPINFO_FAMILY_MOSELLE    = 111,
  EFICHIPINFO_FAMILY_IPQ50xx    = 112,
  EFICHIPINFO_FAMILY_MAKENA     = 113,
  EFICHIPINFO_FAMILY_CEDROS     = 114,
  EFICHIPINFO_FAMILY_MANNAR     = 115,
  EFICHIPINFO_FAMILY_WAIPIO     = 116,
  EFICHIPINFO_FAMILY_OLYMPIC    = 117,

/** @cond */
  EFICHIPINFO_NUM_FAMILIES,
  EFICHIPINFO_FAMILY_32BITS     = 0x7FFFFFF
/** @endcond */
} EFIChipInfoFamilyType;

/**
Chip serial number. Unique within each EfiChipInfoFamily.
*/
typedef UINT32 EFIChipInfoSerialNumType;

/**
Chip serial number as read from QFPROM.
*/
typedef UINT32 EFIChipInfoQFPROMChipIdType;

/**
Chip Foundry type.
*/
typedef enum
{
  EFICHIPINFO_FOUNDRYID_UNKNOWN = 0,
  EFICHIPINFO_FOUNDRYID_TSMC    = 1,
  EFICHIPINFO_FOUNDRYID_GF      = 2,
  EFICHIPINFO_FOUNDRYID_SS      = 3,
  EFICHIPINFO_FOUNDRYID_IBM     = 4,
  EFICHIPINFO_FOUNDRYID_UMC     = 5,
  EFICHIPINFO_FOUNDRYID_SMIC    = 6,

  EFICHIPINFO_NUM_FOUNDRYIDS,
  EFICHIPINFO_FOUNDRYID_32BITS  = 0x7FFFFFF
} EFIChipInfoFoundryIdType;

/**
 * Defective parts supported by the GetDefectivePart API
 */
typedef enum
{
  EFICHIPINFO_PART_UNKNOWN      = 0,
  EFICHIPINFO_PART_GPU          = 1,
  EFICHIPINFO_PART_VIDEO        = 2,
  EFICHIPINFO_PART_CAMERA       = 3,
  EFICHIPINFO_PART_DISPLAY      = 4,
  EFICHIPINFO_PART_AUDIO        = 5,
  EFICHIPINFO_PART_MODEM        = 6,
  EFICHIPINFO_PART_WLAN         = 7,
  EFICHIPINFO_PART_COMP         = 8,
  EFICHIPINFO_PART_SENSORS      = 9,
  EFICHIPINFO_PART_NPU          = 10,
  EFICHIPINFO_PART_SPSS         = 11,
  /*
   * TODO Add Group E parts
   * once they've been finalized
   */

  EFICHIPINFO_NUM_PARTS,
  EFICHIPINFO_PART_32BITS       = 0x7FFFFFFF
} EFIChipInfoPartType;


#endif /* __EFICHIPINFOTYPES_H__ */

