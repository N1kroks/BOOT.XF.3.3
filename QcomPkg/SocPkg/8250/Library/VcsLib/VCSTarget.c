/*
===========================================================================
*/
/**
  @file VCSTarget.c

*/
/*
  ====================================================================

  Copyright (c) 2017- 2019 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/VcsLib/VCSTarget.c#1 $
  $DateTime: 2019/03/27 17:31:08 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSImage.h"

/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Extern Definitions
==========================================================================*/


/*=========================================================================
      Function prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/* =========================================================================
      Prototypes
==========================================================================*/


/* =========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_InitTarget
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_InitTarget (VCSDrvCtxt *pDrvCtxt)
{
  DALResult eResult;

  eResult = VCS_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END VCS_InitTarget */

/* =========================================================================
**  Function : VCSStub_InitTarget
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCSStub_InitTarget (VCSDrvCtxt *pDrvCtxt)
{
  return DAL_SUCCESS;

} /* END VCSStub_InitTarget */