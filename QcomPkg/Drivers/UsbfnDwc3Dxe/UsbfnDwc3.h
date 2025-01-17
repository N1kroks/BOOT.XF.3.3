/**
 * @file UsbfnDwc3.h
 *
 * @brief The header file for the Synopsys core Usbfn driver
 *
 * Copyright (C) 2011,2013-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
 * Portions Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
 * This program and the accompanying materials
 * are licensed and made available under the terms and conditions of the BSD License
 * which accompanies this distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/17/18	gk		Make Superspeed MassStorage as Default setting 
 02/13/17   gk      Port changes from boot.xf.2.0 and boot.xf.2.2
 12/1/17    pm      Added support for device mode on secondary port
 02/23/16   ck      Utilize USB Config Lib for cable detaction
 01/20/15   ck      Remove deprecated functions
 11/20/14   ml      Migrate to official SuperSpeed support
 10/07/14   wufan   Add support of HS Test Mode for compliance testing 
 06/16/14   ck      Workaround issue that FFU allocates more buffer than needed.
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 05/20/14   ar      Add PCD value to override max bus speed 
 04/30/14   ar      Cleanup for NDEBUG build  
 04/25/14   amitg   Charger and Cable Detection Updates for MSM 8994 (Elessar)
 08/23/13   mliss   Cleanup and stabilization
 01/20/11   cching  Initial rework for Synopsys Usbfn driver
 =============================================================================*/

#ifndef _USBFN_DWC3_H_
#define _USBFN_DWC3_H_

#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UsbConfigLib.h>
#include <Library/UsbfnDwc3Lib.h>
#include <Library/UefiLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIPmicSmbchg.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/EFIUsbIo.h>
#include <Protocol/EFIUsbfnIo.h>
#include <Protocol/LoadedImage.h>

#include "UsbError.h"
#include "DwcCommon.h"


#define USBFN_DEV_SIGNATURE   SIGNATURE_32 ('u', 's', 'b', 'f')
#define USBFN_FROM_THIS(x)    CR(x, USBFN_DEV, UsbfnIo, USBFN_DEV_SIGNATURE)

//
// Miscellaneous Definitions
//
#define USB_ENDPOINT_NUMBER_MIN                 0x00
#define USB_ENDPOINT_NUMBER_MAX                 0x0f
#define USB_ENDPOINT_DIR_OUT                    0x00
#define USB_ENDPOINT_DIR_BMSK                   USB_ENDPOINT_DIR_IN

#define USBFN_CTRL_EP                           0
#define USBFN_DEFAULT_CONFIG_INDEX              0

#define USBFN_ENDPOINT_NUMBER_MAX               16
#define USBFN_ENDPOINT_DIR_MAX                  2

#define EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES  72 //  32 alphanumeric characters and four hyphens in WCHAR
#define EMMC_UUID_OCTET_SIZE                    16 //  UUID is a 16-octet number

#define USBFN_PMIC_INDEX                        0
#define USBFN_MESSAGE_NODE_COUNT                5
#define USBFN_MAX_TRANSFER_SIZE                 0xFFFFF8

#define USBFN_MAX_UXSEL_USEC                    125 // uS

#define USBFN_XFER_BUFFER_ALIGNMENT             1024

// For Hijacking code
#define USB_DESC_TYPE_UNDEFINED                 0

// For UsbEpMaxPacketSize
#define USBFN_NUM_EP_TYPES                      4
#define USBFN_NUM_BUS_SPEEDS                    6


typedef struct _PipePolicyInfo {
  BOOLEAN   ZeroLengthTerminationSupport;
  BOOLEAN   ZLT[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];
  UINT32    MaxTransferSize[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];
} PipePolicyInfo;


/**
 * Structure to store Usbfn client messages and their payload in a list.
 */
typedef struct _EFI_USBFN_MESSAGE_NODE {
  LIST_ENTRY Link;
  EFI_USBFN_MESSAGE Message;
  union {
      EFI_USB_DEVICE_REQUEST SetupCbRequest;
      EFI_USBFN_TRANSFER_RESULT XferCbResult;
      EFI_USB_BUS_SPEED  SpeedCbSpeed;
  } Data;
} EFI_USBFN_MESSAGE_NODE;


/**
 * USB SET_SEL Request data
 */
#pragma pack(1)
typedef struct _USB_SET_SEL_DATA {
  UINT8                         U1SEL;
  UINT8                         U1PEL;
  UINT16                        U2SEL;
  UINT16                        U2PEL;
} USB_SET_SEL_DATA;
#pragma pack()


/**
 * Data to manage request hijacking
 */
typedef struct USBFN_REQUEST_HIJACK_DATA {
  BOOLEAN                       RequestHijackingEnabled;
  BOOLEAN                       HijackThisRequest;
  UINT8                         HijackDescType;
  VOID                          *HijackedBuffer;
  UINTN                         HijackedBufferLength;
  BOOLEAN                       HijackNextRxEvent;
  BOOLEAN                       HijackNextTxEvent;
  VOID                          *HijackDataBuffer;
  VOID                          *HijackStatusBuffer;
} USBFN_REQUEST_HIJACK_DATA;


/**
 * Usbfn software device structure.
 */
typedef struct _USBFN_DEV {
  UINTN                         Signature;
  EFI_USBFN_IO_PROTOCOL         UsbfnIo;

  // UEFI protocols
  QCOM_USB_CONFIG_PROTOCOL      *UsbConfigProtocol;
  QCOM_USB_CONFIG_CORE_TYPE     CoreType;

  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatType;
  EFI_EVENT                     ExitBootServiceEvent;
  EFI_UNICODE_STRING_TABLE      *ControllerNameTable;

  PipePolicyInfo                PipePolicy;

  // Driver state
  QCOM_USB_CORE_NUM             CoreNum;
  BOOLEAN                       Enabled;
  BOOLEAN                       IsAttached;
  BOOLEAN                       IsSuspended;
  EFI_USB_BUS_SPEED             Speed;
  UINT8                         MaxSpeed;
  UINT8                         Address;
  UINT8                         ConfigValue;
  UINTN                         MaxCurrent; // in mA
  dci_dwc_chg_port_type         ChgPortType;
  BOOLEAN                       Disconnecting;
  BOOLEAN                       DisconnectEvtPosted;

  BOOLEAN                       StoreSELData;
  USB_SET_SEL_DATA              ExitLatency;

  // USB descriptor sets
  EFI_USB_DEVICE_INFO             *DeviceInfo;
  EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo;

  // Client messages
  LIST_ENTRY                    UsbfnMessages;
  LIST_ENTRY                    UsbfnMessagePool;

  // Hardware base addresses
  UINTN                         Usb30WrapperBase;
  UINTN                         DwcUsb3RegBase;
  UINTN                         Usb30Qscratch;

  // Common layer data
  DWC_DEVICE                    DwcDevice;

  // Use single event buffer for all events
  VOID*                         EvtBuffer;
  UINT32                        EvtBufferSize;
  UINT8                         EvtBuffIndex;

  // EP FIFO mapping
  UINT8                         FifoNumMapping[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];

  // TRB buffers for each EP - these need to be 16 bytes aligned
  VOID*                         TrbBuf[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];
  UINT32                        TrbNum[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];

  LIST_ENTRY                    ActiveTransfers[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];

  LIST_ENTRY                    UrbPool;
  UINT32                        UrbPoolSize;

  // This is used to tracked last configuration of ep
  DWC_EP_CMD                    EpConfig[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];

  // The list is to store the data or zero length pkt sent by the client
  LIST_ENTRY                    PendingXfer[USBFN_ENDPOINT_NUMBER_MAX][USBFN_ENDPOINT_DIR_MAX];

  // Buffer for setup packet
  VOID                          *SetupPkt;

  // Data to manage the hijacking of client requests
  USBFN_REQUEST_HIJACK_DATA     HijackData;
  
  // If set, bypass the status stage in the controller transfer
  BOOLEAN                       ByPassStatusStage;

  // If different from USB_TEST_DISABLE, enable the HS test mode after the SET_FEATURE request is acknowledged
  USB_TEST_MODE                 HsTestMode;

  // LPM Start event triggered during wall charger connection for dead battery by Charger app
  EFI_EVENT                     EnterLPMEvent;
} USBFN_DEV;


/*
 * Endpoint maximum packet size array
 */
extern CONST UINTN UsbEpMaxPacketSize[USBFN_NUM_EP_TYPES][USBFN_NUM_BUS_SPEEDS];



/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has proper protocol installed will
  be supported.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          This driver supports this device.
  @return EFI_UNSUPPORTED      This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

/**
  Starting the Usb UsbFnI Driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          supports this device.
  @return EFI_UNSUPPORTED      do not support this device.
  @return EFI_DEVICE_ERROR     cannot be started due to device Error.
  @return EFI_OUT_OF_RESOURCES cannot allocate resources.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

/**
  Stop this driver on ControllerHandle. Support stopping any child handles
  created by this driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to stop driver on.
  @param  NumberOfChildren     Number of Children in the ChildHandleBuffer.
  @param  ChildHandleBuffer    List of handles for the children we need to stop.

  @return EFI_SUCCESS          Success.
  @return EFI_DEVICE_ERROR     Fail.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

  
  
/*
 * Client event generation callbacks, invoked from Ch9.c
 */

/**
 * A callback function invoked when a setup pkt whose recipient is usb device is
 * received.
 *
 * See UpdateDeviceInfo for an overall description of request hijacking.
 *
 * @param  [in]  Usbfn          Usbfn device instance pointer
 * @param  [in]  Req            A received setup packet
 *
 * @retval EFI_SUCCESS
 */
EFI_STATUS
UsbfnSetupDevCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  );

/**
 * A callback function invoked when a setup pkt whose recipient is usb interface is
 * received.
 *
 * @param  [in]  Usbfn          Usbfn device instance pointer
 * @param  [in]  Req            A received setup packet
 *
 * @retval EFI_SUCCESS
 */
EFI_STATUS
UsbfnSetupIfcCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  );

/**
 * A callback function invoked when a USB cable is attached/detached
 *
 * @param  [in]  Usbfn          Usbfn device instance pointer
 * @param  [in]  IsAttached     Whether the USB cable is attached or detached
 */
VOID
UsbfnXtachCb (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsAttached
  );

/**
 * A callback function invoked when a Reset, Suspend or Resume event is detected
 *
 * @param  [in]  Usbfn          Usbfn device instance pointer
 * @param  [in]  Message        Which message to notify client of
 */
VOID
UsbfnBusEventCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USBFN_MESSAGE          Message
  );

/**
 * A callback function invoked when a USB operation speed is detected
 *
 * @param  [in]  Usbfn          Usbfn device instance pointer
 * @param  [in]  Speed          USB operation speed
 */
VOID
UsbfnSpeedCb (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_BUS_SPEED          Speed
  );

/*
 * Protocol functions
 */

/**
 * @brief Returns the detected port type.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [out] PortType        USB port type
 *
 * @retval EFI_SUCCESS          Port detected successfully
 * @retval Others               Failed to detect port
 */
EFI_STATUS
EFIAPI
UsbfnDetectPort (
  IN  EFI_USBFN_IO_PROTOCOL     *This,
  OUT EFI_USBFN_PORT_TYPE       *PortType
  );

/**
 * @brief Configure the endpoints and activate the port.
 *
 * This function assumes that the hardware has been initialized and uses the
 * supplied DevceInfo to configure the endpoints, activate the port and start
 * receiving USB events.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  DeviceInfo      Device descriptor information
 *
 * @retval EFI_SUCCESS          The port is ready
 * @retval Others               Endpoint configuration failed
 */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpoints (
  IN EFI_USBFN_IO_PROTOCOL      *This,
  IN EFI_USB_DEVICE_INFO        *DeviceInfo
  );

/**
 * @brief Configure the endpoints and activate the port.
 *
 * This function assumes that the hardware has been initialized and uses the
 * supplied DevceInfo to configure the endpoints, activate the port and start
 * receiving USB events. This function accepts DeviceInfo and SSDeviceInfo
 * objects and configures the endpoints with the information from the object
 * that supports the highest speed allowed by the unerlying hardware. The
 * high speed and super speed DeviceInfo objects passed in must have the same
 * DeviceClass in the EFI_USB_DEVICE_DESCRIPTOR, otherwise this function will
 * return EFI_UNSUPPORTED.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  DeviceInfo      Device descriptor information
 * @param [in]  SSDeviceInfo    SuperSpeed device descriptor information
 *
 * @retval EFI_SUCCESS          The port is ready
 * @retval Others               Endpoint configuration failed
 */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpointsEx (
  IN EFI_USBFN_IO_PROTOCOL           *This,
  IN EFI_USB_DEVICE_INFO             *DeviceInfo,
  IN EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo
  );

/**
 * @brief Return the max packet size for the specified endpoint type
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointType    Bulk, interrupt, etc.
 * @param [in]  BusSpeed        USB bus speed
 * @param [out] MaxPacketSize   Maximum packet size, in bytes
 *
 * @retval EFI_SUCCESS          MaxPacketSize is valid
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointMaxPacketSize (
  IN  EFI_USBFN_IO_PROTOCOL     *This,
  IN  EFI_USB_ENDPOINT_TYPE     EndpointType,
  IN  EFI_USB_BUS_SPEED         BusSpeed,
  OUT UINT16                    *MaxPacketSize
  );

/**
 * @brief Return a unicode string describing various device specific info
 *
 * This function does NOT pertain to the EFI_USB_DEVICE_INFO structure but
 * returns information such as the device's serial number.
 *
 * @param [in]     This         UsbfnIo protocol instance
 * @param [in]     Id           ID of requested information
 * @param [in,out] BufferSize   On input, size of buffer. On output, length
 *                              of data returned in buffer.
 * @param [out]    Buffer       Buffer to return requested information in
 *
 * @retval EFI_SUCCESS          Requested info returned in Buffer
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnGetDeviceInfo (
  IN     EFI_USBFN_IO_PROTOCOL     *This,
  IN     EFI_USBFN_DEVICE_INFO_ID  Id,
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer
  );

/**
 * @brief Return the device's Vendor ID (VID) and Product ID (PID)
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [out] Vid             Vendor ID
 * @param [out] Pid             Product ID
 *
 * @retval EFI_SUCCESS          VID and PID are valid
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnGetVendorIdProductId (
  IN  EFI_USBFN_IO_PROTOCOL     *This,
  OUT UINT16                    *Vid,
  OUT UINT16                    *Pid
  );

/**
 * @brief Abort a transfer on the specified endpoint
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointIndex   Endpoint of ongoing transfer
 * @param [in]  Direction       Direction of ongoing transfer
 *
 * @retval EFI_SUCCESS          Transfer aborted successfully
 * @retval Others               No transfer to cancel or error
 */
EFI_STATUS
EFIAPI
UsbfnAbortTransfer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction
  );

/**
 * @brief Returns whether an endpoint is currently stalled or not
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointIndex   Endpoint to check
 * @param [in]  Direction       Direction of endpoint
 * @param [out] State           Current stall state (TRUE or FALSE)
 *
 * @retval EFI_SUCCESS          State checked successfully
 * @retval Others               Failed to fetch stall state
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointStallState (
  IN  EFI_USBFN_IO_PROTOCOL         *This,
  IN  UINT8                         EndpointIndex,
  IN  EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  OUT BOOLEAN                       *State
  );

/**
 * @brief Sets or clears stall on an endpoint
 *
 * In this implementation, it is not acceptable to clear stall on the
 * control endpoint. It is automatically cleared by the hardware.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointIndex   Endpoint to change stall state on
 * @param [in]  Direction       Direction of endpoint
 * @param [in]  State           Stall state to set
 *
 * @retval EFI_SUCCESS          Stall state set successfully
 * @retval Others               Failed to set stall state
 */
EFI_STATUS
EFIAPI
UsbfnSetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       State
  );

/**
 * @brief Poll for transfer and bus events
 *
 * This function must be repeatedly called by the client to receive updates on
 * the state of the USB bus, setup packet arrivals on control endpoint 0 and
 * status changes for submitted transfers on all endpoints. 
 *
 * @param [in]     This         UsbfnIo protocol instance
 * @param [out]    Message      Indicates the type of event
 * @param [in,out] PayloadSize  On input, size of buffer in Payload. On output,
 *                              size of data returned in payload.
 * @param [out]    Payload      The message specific payload
 *
 * @retval EFI_SUCCESS          Successfully polled for events
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnEventHandler (
  IN      EFI_USBFN_IO_PROTOCOL      *This,
  OUT     EFI_USBFN_MESSAGE          *Message,
  IN OUT  UINTN                      *PayloadSize,
  OUT     EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  );

/**
 * @brief Transfer data to or from the host on the specified endpoint
 *
 * See UpdateDeviceInfo for an overall description of request hijacking.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointIndex   Endpoint to submit transfer on
 * @param [in]  Direction       Direction of endpoint
 * @param [in]  BufferSize      Size of buffer
 * @param [in]  Buffer          Bufer to read from/write to
 *
 * @retval EFI_SUCCESS          Successfully submitted transfer
 * @retval Others               Failed to submit transfer
 */
EFI_STATUS
EFIAPI
UsbfnTransfer (
  IN  EFI_USBFN_IO_PROTOCOL         *This,
  IN  UINT8                         EndpointIndex,
  IN  EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN  UINTN                         *BufferSize,
  IN  VOID                          *Buffer
  );

/**
 * @brief Returns the maximum number of bytes that a single transfer
 *        can accomodate
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [out] MaxTransferSize Maximum supported transfer size
 *
 * @retval EFI_SUCCESS          MaxTransferSize is valid
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnGetMaxTransferSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINTN                  *MaxTransferSize
  );


/**
 * @brief Allocate a buffer to satisfy any requirements of the controller
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  Size            Number of bytes to allocate
 * @param [out] Buffer          Returned buffer pointer on success
 *
 * @retval EFI_SUCCESS          Buffer was allocated successfully
 * @retval Others               Error
 */
EFI_STATUS
EFIAPI
UsbfnAllocateTransferBuffer (
  IN   EFI_USBFN_IO_PROTOCOL  *This,
  IN   UINTN                  Size,
  OUT  VOID                   **Buffer
  );

/**
 * @brief Free a buffer allocated with UsbfnAllocateTransferBuffer
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  Buffer          Buffer to free
 *
 * @retval EFI_SUCCESS          Buffer freed
 * @retval Others               Invalid buffer pointer
 */
EFI_STATUS
EFIAPI
UsbfnFreeTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  IN VOID                   *Buffer
  );

/**
 * @brief Initialize the hardware, but must not activate the port
 *
 * @param [in]  This            UsbfnIo protocol instance
 *
 * @retval EFI_SUCCESS          Successfully initialized controller
 * @retval Others               Failed to initialize controller
 */
EFI_STATUS
EFIAPI
UsbfnStartController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  );

/**
 * @brief Disable the hardware
 *
 * @param [in]  This            UsbfnIo protocol instance
 *
 * @retval EFI_SUCCESS          Successfully disabled the controller
 * @retval Others               Failed to disable the controller
 */
EFI_STATUS
EFIAPI
UsbfnStopController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  );

/**
 * @brief Sets policies supported by the USB endpoint
 *
 * This function must be called before StartController or after StopController.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [in]  EndpointIndex   Endpoint to set policy on
 * @param [in]  Direction       Direction of endpoint
 * @param [in]  PolicyType      Policy to be applied
 * @param [in]  BufferSize      Length of policy buffer
 * @param [in]  Buffer          Buffer holding policy value
 *
 * @retval EFI_SUCCESS          Policy successfully applied
 * @retval Others               Failed to apply policy
 */
EFI_STATUS
EFIAPI
UsbfnSetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN      UINTN                         BufferSize,
  IN      VOID                          *Buffer
  );

/**
 * @brief Gets policies supported by the endpoint
 *
 * @param [in]     This          UsbfnIo protocol instance
 * @param [in]     EndpointIndex Endpoint to get policy on
 * @param [in]     Direction     Direction of endpoint
 * @param [in]     PolicyType    Policy to return
 * @param [in,out] BufferSize    On input, length of buffer, on output,
 *                               length of data returned in Buffer
 * @param [out]    Buffer        Buffer to hold policy value
 *
 * @retval EFI_SUCCESS           Policy successfully returned
 * @retval Others                Failed to return policy
 */
EFI_STATUS
EFIAPI
UsbfnGetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN OUT  UINTN                         *BufferSize,
  OUT     VOID                          *Buffer
  );
  
/*
* @brief Free the descriptor buffers within Descriptors
*
* The function frees the descriptors and the array of pointers to the descriptors
* allocated with Allocate*Pool method.
*
*
* @param[in]  DescriptorCount   Number pointers in the Descriptors
* @param[in]  Descriptors       Array of pointers to the buffers to be freed
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*/
STATIC
EFI_STATUS
UsbFreeDescriptorArray(
  IN    UINT8               DescriptorCount,
  IN    VOID              **Descriptors
);


/*
* @brief Create a local copy of the usb configurations
*
* The function creates a local copy of the USB configurations passed in by the
* client. 
*
*
* @param[in]  ConfigurationCount     Number of configurations in CConfigurations
* @param[in]  CConfigurations        Client USB configuration(s)
* @param[out] LConfigurations        Local copy of USB configuration(s) to be crated
*
* @retval     EFI_SUCCESS            Operation succeeded.
* @retval     Others                 Operation failed
*/
STATIC
EFI_STATUS
UsbUpdateConfigurations(
  IN    UINT8                               ConfigurationCount,
  IN    EFI_USB_SUPERSPEED_CONFIG_INFO       **CConfigurations,
  OUT   VOID                                ***LConfigurations);


/**
 * @brief Update the Super speed descriptor of the device.
 *
 * @param [in]  This            UsbfnIo protocol instance
 * @param [Out] DescriptorSet   Device Descriptor instance
 
 * @retval EFI_SUCCESS          Successfully Updated the Super speed descriptor
 * @retval Others               Failed to Update the Super speed descriptor
 */
EFI_STATUS
EFIAPI
UsbUpdateSSDescriptor (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  OUT USB_DESCRIPTOR_SET    *DescriptorSet
);


/**
 * This is a driver Exit point function that is called when
 * gBS->ExitBootServices() is called.
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event hanlder private data
 */
VOID
EFIAPI
UsbfnExitBootService (
  EFI_EVENT                      Event,
  VOID                           *Context
  );

extern EFI_DRIVER_BINDING_PROTOCOL   gUsbfnDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL   gUsbfnComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL  gUsbfnComponentName2;

//
// Helper Macro
//

#define ALLOC(Ptr, Size) \
  do { \
    Ptr = AllocatePool (Size); \
    if (!(Ptr)) { \
      Status = EFI_OUT_OF_RESOURCES; \
      ERR_CHK ("failed to allocate %d bytes", Size); \
    } \
  } while (0)

#define FREE(Ptr) \
  do { \
    FreePool (Ptr); \
    Ptr = NULL; \
  } while (0)


// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Direction) \
  ( \
    (EndpointIndex) \
    | ( \
        (Direction) == EfiUsbEndpointDirectionHostOut \
                     ? USB_ENDPOINT_DIR_OUT \
                     : USB_ENDPOINT_DIR_IN \
        ) \
  )

#endif /* _USBFN_DWC3_H_ */
