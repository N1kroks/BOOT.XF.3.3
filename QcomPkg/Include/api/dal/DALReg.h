#ifndef DALREG_H
#define DALREG_H
/*======================================================
FILE: DALReg.h

SERVICES:
  DAL Driver Registry

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:
  None.

   Copyright � 2007, 2014 QUALCOMM Technologies, Inc.
      All Rights Reserved.
   Qualcomm Confidential and Proprietary 
=====================================================*/
#include "DalDevice.h"

//Avoid missing braces warning from driver initializations
#pragma GCC diagnostic ignored "-Wmissing-braces"

//Following structure represents the Driver information
typedef struct DALREG_DriverInfo DALREG_DriverInfo;
struct DALREG_DriverInfo
{
	union
	{
	        DALResult (*pfnDALNewFunc)(const char * , DALDEVICEID, DalDeviceHandle**);
                DALResult (*pfnDALNewFuncEx)(const char * , DALDEVICEID, DalDeviceHandle**, int);
	};
	//DALResult (*pfnDALNewFunc)(const char * , DALDEVICEID, DalDeviceHandle**, ... );
	uint32 dwNumDevices;
	DALDEVICEID *pDeviceId;
};

//Following structure represents info regarding all drivers for a process
typedef struct DALREG_DriverInfoList DALREG_DriverInfoList;
struct DALREG_DriverInfoList
{
	uint32 dwLen;
	DALREG_DriverInfo ** pDriverInfo;
};

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================
TODO
=======================================================================*/
#endif /* DALREG_H */