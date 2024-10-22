/** @file ResetSystemLib.c
   
  Library implementation to support ResetSystem Runtime call.

  Copyright (c) 2011-2016, Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.           
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/14/11   niting  Initial revision.

=============================================================================*/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>

#include <EfiResetSystemLib.h>


/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
{
  DEBUG((EFI_D_ERROR, "Resetting system\n"));

  CpuDeadLoop();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LibGetResetReason (UINT32 *ResetReason, CHAR16 *ResetReasonStr, UINT32 *ResetReasonStrLen)
{
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LibClearResetReason (VOID)
{
  return EFI_SUCCESS;
}

/**
  Initialize any infrastructure required for LibResetSystem () to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
LibInitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}


