/**
* @file:  cpr_smem_internal.h
* @brief:
*
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* QUALCOMM Proprietary and Confidential.
*
* $DateTime: 2018/07/26 18:39:07 $
* $Header: //components/rel/boot.xf/3.2/QcomPkg/Library/CPRLib/common/inc/cpr_smem_internal.h#1 $
* $Change: 16743401 $
*/
#ifndef CPR_SMEM_INTERNAL_H
#define CPR_SMEM_INTERNAL_H

#include "cpr_cfg.h"
#include "cpr_data.h"
#include "cpr_smem.h"

void cpr_smem_write_rail_settings(cpr_smem_hdr* hdr,
        cpr_smem_rail_hdr* railHdr, uint32 *offset,
        cpr_rail_state* state);
void cpr_smem_read_rail_settings(cpr_smem_hdr* hdr,
        cpr_smem_rail_hdr* railHdr, uint32 *offset,
        cpr_settings *settings, uint8 idx);

#endif
