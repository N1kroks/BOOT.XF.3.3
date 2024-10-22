/*==============================================================================
  @file ChipInfoTargetLib.c

  ChipInfo functions for the XBL Loader ChipInfo Driver.

================================================================================
        Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
================================================================================

  $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/ChipInfoTargetLib/ChipInfoTargetLib.c#4 $
  $DateTime: 2019/01/25 13:38:10 $
  $Author: pwbldsvc $

==============================================================================*/

/*==============================================================================
      Include Files
==============================================================================*/
#include "ChipInfoHWIO.h"
#include "ChipInfoLocal.h"
#include "HALhwio.h"

/*==============================================================================
**  Macros
**============================================================================*/

/**
 * Chipset-specific numbers of cores and clusters.
 * CHIPINFO_PARTIALGOODS_NUM_CLUSTERS is different from ChipInfoDefs.h/CHIPINFO_MAX_CPU_CLUSTERS
 * since the latter is a global maximum across all supported chips,
 * while the former only applies to the current chip.
 */
#define CHIPINFO_PARTIALGOODS_NUM_CLUSTERS                    1

/**
 * The PARTIAL_GOOD fuses contain a bitmask of all the defective bins,
 * with bit0 = Bin A, bit1 = Bin B, etc. If Bins A and B are both defective,
 * the fuses would hold 0b00011. Bins A through E are supported.
 * This macro expects uppercase letters only.
 */
#define CHIPINFO_PARTIALGOODS_BIN(x)                          0x1 << ((char)(x) - 'A')

/**
 * Masks needed for reading from the fuses, since partialgoods information isn't
 * the only thing these fuses contain
 */
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PARTIAL_GOOD_BMSK       0x0F800000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PARTIAL_GOOD_SHFT       23

/**
 * Statically allocate the CPU clusters array, so we don't need to do dynamic
 * allocation and have a dependency on heap.
 */
uint32 aCPUClusters[CHIPINFO_PARTIALGOODS_NUM_CLUSTERS] = { 0 };

/*==============================================================================
**  Functions
**============================================================================*/

/*==============================================================================
  FUNCTION      ChipInfo_ChipsetGetPartialGoods

  DESCRIPTION   Read PTE fuses to determine which parts are defective

==============================================================================*/
void ChipInfo_InitChipset(ChipInfoCtxtType *pChipInfoCtxt)
{
  uint32 DefectiveBins;

  if (pChipInfoCtxt == NULL)
  {
    return;
  }

  pChipInfoCtxt->nNumClusters = CHIPINFO_PARTIALGOODS_NUM_CLUSTERS;
  pChipInfoCtxt->aCPUClusters = aCPUClusters;

  /* Check which bins have been marked "defective" */
  DefectiveBins = HWIO_INF(QFPROM_CORR_PTE_ROW1_MSB, PARTIAL_GOOD);

  /* Check the sub-bins of bins marked "defective" for more detailed information */
  if (DefectiveBins & CHIPINFO_PARTIALGOODS_BIN('A') &&
      pChipInfoCtxt->nNumClusters > 0)
  {
    /*
     * One or more of the CPUs is defective. Read the
     * sys_APCCCFGCPUPRESENT_N fuse to see which ones are defective.
     *
     * Sub-binning is not required for Hana; just mark all cores in cluster 0 as defective.
     */
    pChipInfoCtxt->aCPUClusters[0] = 0xF0;
  }

  if (DefectiveBins & CHIPINFO_PARTIALGOODS_BIN('B'))
  {
    /*
     * Sub-binning is not required for Hana; just mark all the Bin-B subsystems as defective
     */
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_GPU] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_VIDEO] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_CAMERA] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_DISPLAY] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_AUDIO] = 1;
  }

  if (DefectiveBins & CHIPINFO_PARTIALGOODS_BIN('C'))
  {
    /*
     * Sub-binning is not required for Hana; just mark all the Bin-C subsystems as defective
     */
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_MODEM] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_WLAN] = 1;
  }

  if (DefectiveBins & CHIPINFO_PARTIALGOODS_BIN('D'))
  {
    /*
     * Sub-binning is not required for Hana; just mark all the Bin-D subsystems as defective
     */
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_COMP] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_SENSORS] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_NPU] = 1;
    pChipInfoCtxt->aDefectiveParts[CHIPINFO_PART_SPSS] = 1;
  }

}
