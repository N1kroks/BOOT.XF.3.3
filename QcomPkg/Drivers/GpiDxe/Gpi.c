/** @file Gpi.c
   
  Contains Gpi driver entry point.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/21/18   ts      Added new protocol interfaces
 01/31/17   ts      Initial checkin for sdm845

=============================================================================*/
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include "Protocol/EFIGpiProtocol.h"

/*===========================================================================*/
/*                        FUNCTIONS PROTOTYPES                               */
/*===========================================================================*/
EFI_STATUS gpi_dxe_iface_reg(gpi_iface_params *params);
EFI_STATUS gpi_dxe_iface_dereg(GPI_CLIENT_HANDLE handle);
EFI_STATUS gpi_dxe_query_chan_status(GPI_CLIENT_HANDLE handle,
                                 GPI_CHAN_TYPE chan,
                                 chan_status_info * chan_status);
EFI_STATUS gpi_dxe_iface_poll(GPI_CLIENT_HANDLE handle);
EFI_STATUS gpi_dxe_issue_cmd(GPI_CLIENT_HANDLE handle,
                         GPI_CHAN_TYPE chan,
                         GPI_CHAN_CMD cmd_type,
                         UINT32 user_cmd,
                         VOID *user_data);
EFI_STATUS gpi_dxe_process_tre(gpi_tre_req *tre_request);
EFI_STATUS gpi_dxe_iface_active(GPI_CLIENT_HANDLE handle, boolean active);
EFI_STATUS gpi_dxe_reg_save(GPI_CLIENT_HANDLE handle, gpi_debug_regs *regs);

/*===========================================================================*/
/*                           EXTERN FUNCTIONS                                */
/*===========================================================================*/
// GPI Driver Lib init function to init GSI hardware
extern void gpi_init(void);

// Pcie protocol implementation
QCOM_GPI_PROTOCOL QcomGpiInit = 
{
  .Revision           =  EFI_GPI_PROTOCOL_REVISION,
  .iface_reg          =  gpi_dxe_iface_reg,
  .iface_dereg        =  gpi_dxe_iface_dereg,
  .query_chan_status  =  gpi_dxe_query_chan_status,
  .iface_poll         =  gpi_dxe_iface_poll,
  .issue_cmd          =  gpi_dxe_issue_cmd,
  .process_tre        =  gpi_dxe_process_tre,
  .iface_active       =  gpi_dxe_iface_active,
  .reg_save           =  gpi_dxe_reg_save
};

/**
  The Entry Point for GPI module. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurred when executing this entry point.

**/
EFI_STATUS
EFIAPI
GpiEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->InstallProtocolInterface (
                &ImageHandle,
                &gQcomGpiInitProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &QcomGpiInit
                );
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "GpiEntryPoint: Installing Gpi protocol interface failed, Status = (0x%p)\r\n", Status));
    goto Exit;
  }

  /*
   * Initialize GSI hardware and prepare GPI
   */
  gpi_init();

Exit:
  return Status;
}

/**
 * Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface registration parameters
 *
 * @return       EFI_STATUS            Return status.
 */
EFI_STATUS gpi_dxe_iface_reg (
    gpi_iface_params *params
    )
{
  return gpi_iface_reg(params);
}

/**
 * De-Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface handle
 *
 * @return       EFI_STATUS            Return status.
 */
EFI_STATUS gpi_dxe_iface_dereg (
    GPI_CLIENT_HANDLE handle
    )
{
  return gpi_iface_dereg(handle);
}

/**
 * Queries the channel current status.
 *
 * @param[in]         handle           Handle to the GPI Interface
 * @param[in]         chan             Virtual Channel ID
 * @param[in, out]    chan_status      Information about channel current status
 *
 * @return            EFI_STATUS       Return status.
 */
EFI_STATUS gpi_dxe_query_chan_status (
    GPI_CLIENT_HANDLE handle,
    GPI_CHAN_TYPE chan,
    chan_status_info * chan_status
    )
{
  return gpi_query_chan_status(handle, chan, chan_status);
}

/**
 * Polls a GPI interface.
 * The corresponding callback will be invoked with any available results.
 *
 * @param[in]    handle               GPI client handle
 *
 * @return       EFI_STATUS           Return status.
 */
EFI_STATUS gpi_dxe_iface_poll (
    GPI_CLIENT_HANDLE handle
    )
{
  return gpi_iface_poll(handle);
}

/**
 * Issues a channel command.
 *
 * @param[in]      handle              Handle to GPI Interface
 * @param[in]      chan                Virtual Channel ID
 * @param[in]      cmd_type            Command type
 * @param[in]      user_data           Client context data for this command
 *
 * @return         EFI_STATUS          Return status.
 */
EFI_STATUS gpi_dxe_issue_cmd (
    GPI_CLIENT_HANDLE handle,
    GPI_CHAN_TYPE chan,    
    GPI_CHAN_CMD cmd_type,
    UINT32 user_cmd,
    VOID *user_data
    )
{
  return gpi_issue_cmd(handle, chan, cmd_type, user_cmd, user_data);
}

/**
 * Process TREs request.
 *
 * @param[in]    tre_request           Request details
 *
 * @return       EFI_STATUS            Return status.
 */
EFI_STATUS gpi_dxe_process_tre (
    gpi_tre_req *tre_request
    )
{
  return gpi_process_tre(tre_request);
}

/**
 * Informs GPI of the interface active status
 *
 * @param[in]    params                GPI interface registration parameters
 * @param[in]    active                TRUE if interface is active, FALSE otherwise
 *
 * @return       EFI_STATUS            Return status.
 */
EFI_STATUS gpi_dxe_iface_active (
    GPI_CLIENT_HANDLE handle,
    boolean active
    )
{
  return gpi_iface_active(handle, active);
}

/**
 * @brief A GPI client may call this function to save the QUP registers of a GPII that it owns.
 *        Clients must make sure to turn the specific QUP clocks on similar to what's done during transfer requests.
 *
 * @param[in]        handle              Handle to GPI Interface
 * @param[in, out]   regs                Register values saved
 *
 * @return         GPI_RETURN_STATUS     Return status.
 *
 */
EFI_STATUS gpi_dxe_reg_save (
    GPI_CLIENT_HANDLE handle,
	gpi_debug_regs *regs
    )
{
  return gpi_reg_save(handle, regs);
}
