#ifndef DALTLMM_H
#define DALTLMM_H
/*===========================================================================

                      I N T E R N A L   D A L   T L M M
                            H E A D E R   F I L E

DESCRIPTION
  This file contains definitions for structures used by the DALTLMM 

REFERENCES

===========================================================================
             Copyright (c) 2010 - 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/Library/TLMMHWLib/DALTLMM.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/10   dcf     Restructuring for new targets.
===========================================================================*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/


#include <DALStdErr.h>
#include <DALFramework.h>
#include <DALSys.h>
#include <DALDeviceId.h>
#include <DDITlmm.h>
#include "com_dtypes.h"
#include "GPIOIFace.h"


/*==========================================================================

                            MACRO DEFINITIONS

==========================================================================*/

#define TLMM_MAX_STRING_LENGTH 50


/*==========================================================================

                          EXPORTED STRUCTURES 

==========================================================================*/


typedef struct TLMMDrvCtxt TLMMDrvCtxt;
typedef struct TLMMDevCtxt TLMMDevCtxt;
typedef struct TLMMClientCtxt TLMMClientCtxt;


/*
 * Declaring a private "TLMM" Vtable
 * The following defines the virtual table for use when external modules do a 
 * device attach to the TLMM module for the first time when driver init is called.
 * The driver init and driver de init function pointers reside here.
 */

typedef struct TLMMDALVtbl TLMMDALVtbl;

struct TLMMDALVtbl
{
	int (*TLMM_DriverInit)(TLMMDrvCtxt *);
	int (*TLMM_DriverDeInit)(TLMMDrvCtxt *);
};



/*
 * TLMMDevCtxt
 *
 * Data structure containing all information about an interrupt
 *
 *  dwRefs:               Reserved.
 *  DevId:                DAL tlmm device ID.
 *  dwDevCtxtRefIdx:      Number of clients using this device.
 *  pDrvCtxt:             Pointer to the driver context.
 *  Reserved:             Reserved.

 */
struct TLMMDevCtxt
{
  uint32                                 dwRefs;
  DALDEVICEID                            DevId;
  uint32                                 dwDevCtxtRefIdx; 
  TLMMDrvCtxt                            *pDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32                                 Reserved[16];
};


/*
 * TLMMDrvCtxt
 *
 * Data structure containing all information about an interrupt
 *
 *  TLMMDALVtbl:     Base Members.
 *  dwNumDev:        The number of devices in tlmm this is 1.
 *  dwSizeDevCtxt:   Driver structure size for DALSYS.
 *  bInit:           Flag to indicate the driver has been initialized.
 *  dwRefs:          The number of clients using the driver.
 *  TLMMDevCtxt:     The TLMM Device Context.
 *  hTlmmSync:       TLMM Synchronization object.
 */
struct TLMMDrvCtxt
{
  TLMMDALVtbl                            TLMMDALVtbl;
  uint32                                 dwNumDev;
  uint32                                 dwSizeDevCtxt;
  uint32                                 bInit;
  uint32                                 dwRefs;
  TLMMDevCtxt                            TLMMDevCtxt[1];
  DALSYS_SYNC_OBJECT(hTlmmSyncObj);
  DALSYSSyncHandle                       hTlmmSync;
};


typedef struct
{
  uint32             nClientId;
  uint32             nIdx;
  boolean            bInUse;
}TLMMClientInfoType;

/*
 * TLMMClientCtxt
 *
 * Data structure containing top-level TLMM access structures base members.
 *
 *  dwRefs:          References used internally.
 *  dwAccessMode:    SHARED for TLMM.
 *  pPortCtxt:       Pointer to the remote port context.
 *  pDevCtxt:        Handle to the TLMM device context.
 *  DalTlmmHandle:   Handle to the TLMM configuration structures.
 *  DrvCtxt:         Pointe to a driver context.
 *  ClientInfo:      Client/PD information for PD Restart capabilities.
 */
struct TLMMClientCtxt
{
  uint32             dwRefs;                     
  uint32             dwAccessMode;
  void               *pPortCtxt;
  TLMMDevCtxt        *pDevCtxt;            
  DalTlmmHandle      DalTlmmHandle;
  TLMMDrvCtxt        *pDrvCtxt; 
  TLMMClientInfoType ClientInfo;
};


/*==========================================================================

   MANDATORY DAL DEVICE APIS NOT CURRENTLY IMPLEMENTED ON THIS TARGET

==========================================================================*/

DALResult TLMM_DriverInit(TLMMDrvCtxt *);
DALResult TLMM_DriverDeInit(TLMMDrvCtxt *);
DALResult TLMM_DeviceInit(TLMMClientCtxt *);
DALResult TLMM_DeviceDeInit(TLMMClientCtxt *);
DALResult TLMM_Reset(TLMMClientCtxt *);
DALResult TLMM_PowerEvent(TLMMClientCtxt *, 
                          DalPowerCmd, 
                          DalPowerDomain);
DALResult TLMM_Open(TLMMClientCtxt *, 
                    uint32);
DALResult TLMM_Close(TLMMClientCtxt *);
DALResult TLMM_Info(TLMMClientCtxt *,
                    DalDeviceInfo *,
                    uint32);
DALResult TLMM_InheritObjects(TLMMClientCtxt *,
                              DALInheritSrcPram *,
                              DALInheritDestPram *);


/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/

/*==========================================================================

  FUNCTION      TLMM_ConfigGpio

  DESCRIPTION   Configure a GPIO to active or low power state based on the
                enable parameter.

  PARAMETERS    pCtxt      - Pointer to the client context.
                eGpioConfig - The packed configuration to program.
                eEnable     - Specifies whether the GPIO should be configured
                              to active or low power.  Passing in 
                              DAL_TLMM_GPIO_ENABLE uses the 'eGpioConfig' 
                              configuration while passing in 
                              DAL_TLMM_GPIO_DISABLE uses low power 
                              configuration.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_ConfigGpio
( 
  TLMMClientCtxt*   pCtxt,  
  DALGpioSignalType eGpioConfig,  
  DALGpioEnableType eEnable
);


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioGroup

  DESCRIPTION   Configures a group of GPIO.

  PARAMETERS    pCtxt         - Pointer to the client context.
                eEnable       - See TLMM_ConfigGpio above. 
                eGpioConfigs  - Pointer to an array of GPIO configurations.
                nSize         - The size in bytes of the gpio_group array.
                                (each configuration is 4 bytes).

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_ConfigGpioGroup
( 
  TLMMClientCtxt*     pCtxt,
  DALGpioEnableType  eEnable,
  DALGpioSignalType* eGpioConfigs,
  uint32             nSize
);


/*==========================================================================

  FUNCTION      TLMM_GetGpioNumber

  DESCRIPTION   Retrieves a GPIO number for the specified configuration.

  PARAMETERS    pCtxt        - Pointer to the client context.
                eGpioConfig  - A packed GPIO configuration.
                pnGpioNumber - The number to return to the caller.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GetGpioNumber
( 
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType eGpioConfig,
  uint32             *pnGpioNumber
);


/*==========================================================================

  FUNCTION      TLMM_GpioIn

  DESCRIPTION   Initialized the trampoline services.

  PARAMETERS    pCtxt        - Pointer to the client context.
                eGpioConfig  - The configuration to read.
                peValue      - Pointer to a return value.

  DEPENDENCIES  Expects the GPIO to be configured as input.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GpioIn
(
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType eGpioConfig,
  DALGpioValueType* peValue
);


/*==========================================================================

  FUNCTION      TLMM_GpioOut

  DESCRIPTION   Reads the input value (high or low) of a GPIO.

  PARAMETERS    pCtxt       - Pointer to the client context.
                eGpioConfig  - The configuration to write to.
                eValue       - The value to write to this configuration.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GpioOut
(
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType eGpioConfig,
  DALGpioValueType  eValue
);


/*==========================================================================

  FUNCTION      TLMM_GpioOutGroup

  DESCRIPTION   Writes output to a group of GPIO.

  PARAMETERS    pCtxt        - Pointer to the client context.
                *eGpioConfigs  - Pointer to an array of GPIO to write output.
                nSize         - Number of elements in the array.
                eValue        - Value to write (high or low).

  DEPENDENCIES  Expects that all the GPIO in gpio_group array are already
                configured as output.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GpioOutGroup
(
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType* eGpioConfigs,
  uint32             nSize,
  DALGpioValueType   eValue
);


/*==========================================================================

  FUNCTION      TLMM_SetInactiveConfig

  DESCRIPTION   See DDITlmm.h

==========================================================================*/

DALResult TLMM_SetInactiveConfig
(
  TLMMClientCtxt*   pCtxt,
  uint32            nGpioNumber,
  DALGpioSignalType nInactiveConfig
);


/*==========================================================================

  FUNCTION      TLMM_GetOutput

  DESCRIPTION   Retrieves the output value for a GPIO which is programmed as
                a generic output.

  PARAMETERS    pCtxt        - Pointer to the client context.
                nGpioNumber  - The GPIO to retrieve output for.
                peValue      - The DALGpioValueType parameter:
                                DAL_GPIO_HIGH_VALUE
                                DAL_GPOI_LOW_VALUE

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GetOutput
(
  TLMMClientCtxt* pCtxt,
  uint32 nGpioNumber,
  DALGpioValueType* peValue
);


#endif /*DALTLMM_H*/

