/*! \file pm_vib_driver.c
 *  \n
 *  \n &copy; Copyright 2014 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/drivers/vib/src/pm_vib_driver.c#2 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       	who     	 what, where, why
--------   	---    		 ---------------------------------------------------------- 
11/12/17         aab               Moved common Register address defination to driver file  
09/08/14         al                Comm lib update
08/29/14         al                KW fixes
05/20/14         al                Architecture update
05/09/14         va                Using common debug and assert Marco

========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vib_driver.h"
#include "CoreVerify.h"
#include "pm_utils.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the VIB driver data */
static pm_vib_data_type *pm_vib_data_arr[PM_MAX_NUM_PMICS];
static pm_vib_register_info_type vib_reg[1] = 
{
  {0xC000, 0x100, 0x046, 0x051, 0x070}
};
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_vib_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_vib_data_type *vib_ptr = NULL;

    vib_ptr = pm_vib_data_arr[pmic_index];
    
    if (vib_ptr == NULL)
    {
        pm_malloc( sizeof(pm_vib_data_type), (void**)&vib_ptr);
                                                    
        /* Assign Comm ptr */
        vib_ptr->comm_ptr = comm_ptr;

        /* VIB Register Info - Obtaining Data through dal config */
        vib_ptr->vib_reg_table = vib_reg;
        CORE_VERIFY_PTR(vib_ptr->vib_reg_table);

        vib_ptr->vib_data = (pm_vib_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_VIB_DATA);
        CORE_VERIFY_PTR(vib_ptr->vib_data);

        //vib_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_VIB;
        vib_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_VIB_NUM, pmic_index);
        CORE_VERIFY(vib_ptr->num_of_peripherals  != 0);

        pm_vib_data_arr[pmic_index] = vib_ptr;
    }
}

pm_vib_data_type* pm_vib_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_vib_data_arr[pmic_index];
  }

    return NULL;
}

uint8 pm_vib_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
     (pm_vib_data_arr[pmic_index] !=NULL))
  {
      return pm_vib_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}


