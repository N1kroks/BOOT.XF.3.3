/** @file AslrLib.h
   
  Library routines for ASLR functionality

  Copyright (c) 2016,2019 Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/04/19   md      Added function to check if using constant RNG value.
 12/08/16   bh      Initial version

=============================================================================*/
#ifndef __ASLR_LIB_H__
#define __ASLR_LIB_H__

#include <Uefi.h>
#include <UefiInfoBlk.h>
#include <Library/BaseLib.h>

#define ASLR_HEAP_RNG_BITS   0x4
#define ASLR_STACK_RNG_BITS  0x4
#define ASLR_STACK_ALIGN     0x40
#define ASLR_HEAP_ALIGN      EFI_PAGE_SIZE

UINTN GetRNumOfBits(UINTN Shift);

UINTN RandomizeStack(UINTN StackSize);

BOOLEAN IsStaticRNGUsed();


#endif /* __ASLR_LIB_H__ */
