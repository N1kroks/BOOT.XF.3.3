/** @file FwResourceCheck.h
   
  Resource availability checking for data driven firmware update

  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2017/04/22     gparam      Changed the update status from internal type to
                            standard ones defined in EDK headers 
 2012/09/21      mic        Initial version

=============================================================================*/
#include <Uefi.h>
#include "FwResourceCheck.h"
/**
 * This function should check resource availability
 * for firmware update
 *   
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateResourceCheckMain(
  OUT UINT32                   *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  )
{
  return EFI_SUCCESS;
}
