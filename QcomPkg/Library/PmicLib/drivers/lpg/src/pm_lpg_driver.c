/*! \file pm_lpg_driver.c 
*  \n
*  \brief This file contains LPG peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/drivers/lpg/src/pm_lpg_driver.c#2 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/17   aab     Moved common Register address defination to driver file  
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lpg_driver.h"
#include "CoreVerify.h"
#include "pm_comm.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the LPG driver data */
static pm_lpg_data_type *pm_lpg_data_arr[PM_MAX_NUM_PMICS];

static pm_lpg_register_info_type lpg_reg[1] = 
{
    {0xB000, 0x100, 0x005, 0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x050, 0x051, 0x052, 0x054, 0x056, 0x057, 0x040, 0x041, 0x0C8, 0x0e2, 0x0e3, 0x0e4, 0x0e5}
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_lpg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_lpg_data_type *lpg_ptr = NULL;

    lpg_ptr = pm_lpg_data_arr[pmic_index];

    if (lpg_ptr == NULL)
    {
        pm_malloc( sizeof(pm_lpg_data_type), (void**)&lpg_ptr);
                                                    
        /* Assign Comm ptr */
        lpg_ptr->comm_ptr = comm_ptr;

        /* LPG Register Info - Obtaining Data through dal config */
        lpg_ptr->lpg_reg_table = lpg_reg;
        CORE_VERIFY_PTR(lpg_ptr->lpg_reg_table);

	    lpg_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LPG_NUM, pmic_index);
        CORE_VERIFY(lpg_ptr->num_of_peripherals  != 0);

        lpg_ptr->lpg_specific = (pm_lpg_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_LPG_SPECIFIC_DATA);
		CORE_VERIFY_PTR(lpg_ptr->lpg_specific);

		pm_lpg_data_arr[pmic_index] = lpg_ptr;

    }
}

pm_lpg_data_type* pm_lpg_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_lpg_data_arr[pmic_index];
  }

  return NULL;
}


uint8 pm_lpg_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
     (pm_lpg_data_arr[pmic_index] !=NULL))
  {
      return pm_lpg_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}

