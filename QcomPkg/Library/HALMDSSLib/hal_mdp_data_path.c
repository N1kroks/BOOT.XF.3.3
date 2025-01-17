/*! \file */

/*
===========================================================================

FILE:          hal_mdp_data_path.h

DESCRIPTION: Connects the blocks of the MDP (Source, Mixer and Display Interface) 

===========================================================================
  Copyright (c) 2012-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_DataPath_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the data path
*
* \param [in] eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_DataPath_BaseOffset(HAL_MDP_DataPathId  eDataPathId)
{
   uintPtr   uRegOffset  = 0x0;

   if((HAL_MDP_DATA_PATH_NONE         == eDataPathId) ||
      (HAL_MDP_GET_DATA_PATHS_COUNT() <  (uint32)eDataPathId) )
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPDataPathRegBaseOffset[eDataPathId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_SurfaceAttrib_Config()
*/
/*!
* \brief
*     Set up surface attribute configurations for the given data path
*
* \param [in]   eDataPathId          - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   eDataPathMode        - Data path mode. Block mode or line mode
* \param [in]   eChromaDownModuleId  - Chroma down module 
* \param [in]   psSurfaceConfig      - Surface Configuration information
* \param [in]   uFlags                - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_SurfaceAttrib_Config(HAL_MDP_DataPathId          eDataPathId,
                                                  HAL_MDP_DataPath_ModeType   eDataPathMode,
                                                  HAL_MDP_ChromaDownModuleId  eChromaDownModuleId,
                                                  HAL_MDP_SurfaceAttrType    *psSurfaceConfig,
                                                  uint32                      uFlags)
{
   uintPtr                    uRegOffset              = 0;              // Register offset
   uint32                     uPlane0Address          = 0;              // Plane 0 address (Color#0 - G/Luma)
   uint32                     uPlane1Address          = 0;              // Plane 1 address (Color#1 - R/Cr)
   uint32                     uPlane2Address          = 0;              // Plane 2 address (Color#2 - B/Cb)
   uint32                     uPlane3Address          = 0;              // Plane 3 address (Color#3 - Alpha)
   uint32                     uPlane01StrideInBytes   = 0;              // Plane 0 and 1 stride
   uint32                     uPlane23StrideInBytes   = 0;              // Plane 2 and 3 stride
   uint32                     uDstFormatInfo          = 0;              // Destination format information (#of planes, #colors, #bits/each color)
   uint32                     uDstPackInfo            = 0;              // Packing information of the destination buffer 
   uint32                     uDstFormatFields        = 0;              // Cache of the current dest format fields that should not be overwritten
   uint32                     uDstOpMode              = 0;              // OP MODE register
   uint32                     uImageSize              = 0;              // Image size register
//   uint32                     uDitherDepth            = 0;
//   uint32                     uTempRegInfo            = 0;
   uint32                     uDstWriteConfig         = 0;
   HAL_MDP_InterfaceId        eInterfaceId            = HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE;

   HAL_MDSS_ErrorType         eStatus                 = HAL_MDSS_STATUS_SUCCESS;

   // Extract the base address to program the HW registers
   uRegOffset   = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);
   
   

   uDstOpMode = in_dword(HWIO_MDP_WB_2_DST_OP_MODE_ADDR + uRegOffset);

   // Keep the DS_ALPHA_X configuration from being overwritten
   uDstFormatFields = in_dword(HWIO_MDP_WB_2_DST_FORMAT_ADDR + uRegOffset) & 
                      (HWIO_OUT_FLD(0, MDP_WB_2_DST_FORMAT, DST_ALPHA_X, 1));

   if(0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurfaceConfig->uFlags))
   {
     if(HAL_MDP_COMPRESSION_UBWC == pgsMdpHwInfo->eBWC)
     {
       uPlane0Address = psSurfaceConfig->uPlanes.sCompressedPlane.sCompressedDataPlane0DeviceAddress.u.uLowPart;
       uPlane1Address = psSurfaceConfig->uPlanes.sCompressedPlane.sCompressedDataPlane1DeviceAddress.u.uLowPart;
       uPlane2Address = psSurfaceConfig->uPlanes.sCompressedPlane.sMetadataPlane0DeviceAddress.u.uLowPart;
       uPlane3Address = psSurfaceConfig->uPlanes.sCompressedPlane.sMetadataPlane1DeviceAddress.u.uLowPart;
       
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE0,
                                              DST0_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uCompressedDataPlane0StrideInBytes);
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE0,
                                              DST1_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uCompressedDataPlane1StrideInBytes);
       uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE1,
                                              DST2_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uMetadataPlane0StrideInBytes);
       uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE1,
                                              DST3_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uMetadataPlane1StrideInBytes);
     }
     else
     {
       // SSPP_SRCx_ADDR
       uPlane0Address          = psSurfaceConfig->uPlanes.sCompressedPlane.sCompressedDataPlane0DeviceAddress.u.uLowPart;
       uPlane1Address          = psSurfaceConfig->uPlanes.sCompressedPlane.sMetadataPlane0DeviceAddress.u.uLowPart; 

       // SSPP_SRC_YSTRIDEx
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE0,
                                              DST0_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uCompressedDataPlane0StrideInBytes);
       uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                              MDP_WB_2_DST_YSTRIDE0,
                                              DST1_YSTRIDE,
                                              psSurfaceConfig->uPlanes.sCompressedPlane.uMetadataPlane0StrideInBytes);
     }
   }
   else if(TRUE == HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sRgb.sDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sRgb.uStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.sChromaDeviceAddress.u.uLowPart;


      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.uLumaStrideInBytes);
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE0,
                                             DST1_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPsuedoPlanar.uChromaStrideInBytes);
   }
   else if(TRUE == HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(psSurfaceConfig->ePixelFormat))
   {
      // SSPP_SRCx_ADDR
      uPlane0Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sLumaDeviceAddress.u.uLowPart;
      uPlane1Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sChromaBlueDeviceAddress.u.uLowPart;
      uPlane2Address          = psSurfaceConfig->uPlanes.sYuvPlanar.sChromaRedDeviceAddress.u.uLowPart;

      // SSPP_SRC_YSTRIDEx
      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE0,
                                             DST0_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uLumaStrideInBytes);

      uPlane01StrideInBytes   = HWIO_OUT_FLD(uPlane01StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE0,
                                             DST1_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uChromaBlueStrideInBytes);

      uPlane23StrideInBytes   = HWIO_OUT_FLD(uPlane23StrideInBytes,
                                             MDP_WB_2_DST_YSTRIDE1,
                                             DST2_YSTRIDE,
                                             psSurfaceConfig->uPlanes.sYuvPlanar.uChromaRedStrideInBytes);
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT;
   }

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
     if (0x00 != (HAL_MDP_FLAGS_SURFACE_COMPRESSED & psSurfaceConfig->uFlags))
     {
       uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_2_DST_OP_MODE, UBWC_ENC_EN, 1);

       // Enable partial writes for UBWC formats
       uDstWriteConfig = HWIO_OUT_FLD(uDstWriteConfig,
                                      MDP_WB_2_DST_WRITE_CONFIG,
                                      ADDR_GEN_MODE,
                                      1);
     }
     else
     {

       uDstOpMode = HWIO_OUT_FLD(uDstOpMode, MDP_WB_2_DST_OP_MODE, UBWC_ENC_EN, 0);
     }
      // Writeback Destination OP Mode
      if((HAL_MDP_DATA_PATH_LINE_MODE == eDataPathMode)||(HAL_MDP_DATA_PATH_LINEAR_MODE == eDataPathMode))
      {
         // Enable CSC only if destination format is YUV mode 
         if (TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurfaceConfig->ePixelFormat))
         {
           // Program the Chroma down block
           if (HAL_MDP_CDM_NONE != eChromaDownModuleId)
           {
             HAL_MDP_CDM_Output_Format_Config(eChromaDownModuleId, eInterfaceId, psSurfaceConfig->ePixelFormat, 0x00); 
           }
           //#TODO#IMPLEMENT - obsolete registers - CSC removed from WB
         }
         else
         {
        
           if (HAL_MDP_CDM_NONE != eChromaDownModuleId)
           {
             // Disable CSC in chroma down block
             HAL_MDP_CDM_Output_Format_Config(eChromaDownModuleId, eInterfaceId, psSurfaceConfig->ePixelFormat, 0x00); 
           }           
           //#TODO#IMPLEMENT - obsolete registers : dither removed from WB
         }
      }
      else if(HAL_MDP_DATA_PATH_BLOCK_MODE == eDataPathMode)
      {
        //#TODO#IMPLEMENT - obsolete fields in register - CSC_EN and CHROMA_DWN_SAMPLE_EN
      }
      else // Linear mode
      {
        // how to handle this scenario ?
        // In this case, the dither is in DSPP block. 
      }

      // DSPP_SRC_FORMAT
      uDstFormatInfo    = HAL_MDP_GetDestinationFormatInfo(psSurfaceConfig);
      uDstFormatInfo   |= uDstFormatFields; // Keep dithering & const alpha settings
      if(TRUE == HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(psSurfaceConfig->ePixelFormat))
      {
        uDstFormatInfo |= HWIO_MDP_WB_2_DST_FORMAT_DST_COLOR_SPACE_BMSK;
      }
      else
      {
        uDstFormatInfo &=~(HWIO_MDP_WB_2_DST_FORMAT_DST_COLOR_SPACE_BMSK);
      }
      // Pack pattern 
      uDstPackInfo = HAL_PIXELFORMAT_TO_SOURCE_PACK_MAP(psSurfaceConfig->ePixelFormat);

      // Because UBWC only takes one RGB order which is RGBA, so the color components (red and blue)
      // need to be swapped before it goes to UBWC encoder. This is required for A5x macro-tile, no matter
      // UBWC is enabled or not.
      if((HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE & psSurfaceConfig->uFlags) &&
         (NULL != pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent))
      {
        uDstPackInfo = pgsMdpHwInfo->sIpFxnTbl.SwapMacroTileColorComponent(psSurfaceConfig->ePixelFormat, uDstPackInfo);
      }
      
      /* WB Burst size configuartio deprecated */
      
      uImageSize   = HWIO_OUT_FLD(uImageSize, MDP_WB_2_OUT_IMG_SIZE, IMG_H, psSurfaceConfig->uHeightInPixel);
      uImageSize   = HWIO_OUT_FLD(uImageSize, MDP_WB_2_OUT_IMG_SIZE, IMG_W, psSurfaceConfig->uWidthInPixel);

      out_dword(HWIO_MDP_WB_2_DST0_ADDR_ADDR + uRegOffset, uPlane0Address);           // Plane-0 address
      out_dword(HWIO_MDP_WB_2_DST1_ADDR_ADDR + uRegOffset, uPlane1Address);           // Plane-1 address
      out_dword(HWIO_MDP_WB_2_DST2_ADDR_ADDR + uRegOffset, uPlane2Address);           // Plane-2 address
      out_dword(HWIO_MDP_WB_2_DST3_ADDR_ADDR + uRegOffset, uPlane3Address);           // Plane-3 address
      out_dword(HWIO_MDP_WB_2_DST_YSTRIDE0_ADDR + uRegOffset, uPlane01StrideInBytes); // Plane-0 and 1 stride
      out_dword(HWIO_MDP_WB_2_DST_YSTRIDE1_ADDR + uRegOffset, uPlane23StrideInBytes); // Plane-2 and 3 stride
      out_dword(HWIO_MDP_WB_2_DST_FORMAT_ADDR + uRegOffset, uDstFormatInfo);          // Dst format information
      out_dword(HWIO_MDP_WB_2_DST_PACK_PATTERN_ADDR + uRegOffset, uDstPackInfo);      // Dst unpack information
      out_dword(HWIO_MDP_WB_2_DST_OP_MODE_ADDR + uRegOffset, uDstOpMode);             // Op-mode
      out_dword(HWIO_MDP_WB_2_DST_WRITE_CONFIG_ADDR + uRegOffset, uDstWriteConfig);   // Write config

      out_dword(HWIO_MDP_WB_2_OUT_IMG_SIZE_ADDR + uRegOffset, uImageSize);

      if(HAL_MDP_DATA_PATH_LINEAR_MODE != eDataPathMode)
      {
//         out_dword(HWIO_MDP_WB_2_DST_DITHER_BITDEPTH_ADDR + uRegOffset, uDitherDepth);   // Dither bit-depth
      }
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_ConstAlpha_Config()
*/
/*!
* \brief
*     Set up configurations for constant alpha values in data path
*
* \param [in]   eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psConstAlphaConfig  - Constant Alpha configuration
* \param [in]   uFlags              - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_ConstAlpha_Config(HAL_MDP_DataPathId             eDataPathId,
                                               HAL_MDP_ConstAlphaConfigType   *psConstAlphaConfig,
                                               uint32                         uFlags)
{
   uintPtr   uRegOffset      = 0;              // Register offset
   uint32    uDstFormatInfo;

   // Extract the base address to program the HW registers
   uRegOffset     = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uDstFormatInfo = in_dword(HWIO_MDP_WB_2_DST_FORMAT_ADDR + uRegOffset);          // Dst format information

   if (psConstAlphaConfig->bConstAlpha)
   {
       uDstFormatInfo = HWIO_OUT_FLD(uDstFormatInfo, MDP_WB_2_DST_FORMAT, DST_ALPHA_X, 1);

       out_dword(HWIO_MDP_WB_2_DST_ALPHA_X_VALUE_ADDR + uRegOffset, psConstAlphaConfig->uConstAlphaValue);
   }
   else
   {
       // Disable const alpha
       uDstFormatInfo = HWIO_OUT_FLD(uDstFormatInfo, MDP_WB_2_DST_FORMAT, DST_ALPHA_X, 0);
   }

   out_dword(HWIO_MDP_WB_2_DST_FORMAT_ADDR + uRegOffset, uDstFormatInfo);          // Dst format information
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_OutRect_Config()
*/
/*!
* \brief
*     Configures output rectangle for given data path
*
* \param [in]   eDataPathId          - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   eChromaDownModuleId  - Chroma down module 
* \param [in]   psRectConfig         - Output rectangle information
* \param [in]   uFlags               - Reserved.
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DataPath_OutRect_Config(HAL_MDP_DataPathId          eDataPathId,
                                            HAL_MDP_ChromaDownModuleId  eChromaDownModuleId,
                                            HAL_MDP_RectType            *psRectConfig,
                                            uint32                      uFlags)
{
   uintPtr        uRegOffset         = 0;              // Register offset
   uint32         uOutSize           = 0;
   uint32         uOutXY             = 0;

   // Extract the base address to program the HW registers
   uRegOffset   = HAL_MDP_Get_DataPath_BaseOffset(eDataPathId);

   uOutSize   = HWIO_OUT_FLD(uOutSize, MDP_WB_2_OUT_SIZE, DST_H, psRectConfig->uHeightInPixels);
   uOutSize   = HWIO_OUT_FLD(uOutSize, MDP_WB_2_OUT_SIZE, DST_W, psRectConfig->uWidthInPixels);
   uOutXY     = HWIO_OUT_FLD(uOutXY, MDP_WB_2_OUT_XY, DST_X, psRectConfig->uPosX);
   uOutXY     = HWIO_OUT_FLD(uOutXY, MDP_WB_2_OUT_XY, DST_Y, psRectConfig->uPosY);

   // program Chroma down source image width and height
   if (HAL_MDP_CDM_NONE != eChromaDownModuleId)
   {
     HAL_MDP_CDM_OutRect_Config(eChromaDownModuleId, psRectConfig,0x00);
   }

   out_dword(HWIO_MDP_WB_2_OUT_SIZE_ADDR + uRegOffset, uOutSize);
   out_dword(HWIO_MDP_WB_2_OUT_XY_ADDR + uRegOffset, uOutXY);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Scalar_GetDownscaleFactor()
*/
/*!
* \brief
*     Calculates horizontal  and vertical downscale factors
*
* \param [in]   psScalarConfig      - pointer to src/dst cropping parameters
* \param [out]  pHDownscaleFactor   - pointer to horizontal  downscale factor
* \param [out]  pVDownscaleFactor   - pointer to vertical downscale factor
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_DataPath_Scalar_GetDownscaleFactor(HAL_MDP_ScalarConfigType *psScalarConfig,
                                                                     uint32                   *pHDownscaleFactor,
                                                                     uint32                   *pVDownscaleFactor)
{
  HAL_MDSS_ErrorType              eStatusRet              = HAL_MDSS_STATUS_SUCCESS;

  if((NULL != psScalarConfig) && (NULL != pHDownscaleFactor) && (NULL != pVDownscaleFactor))
  {
    /* Calculate Horizontal downscaler Ratio*/
    if(   (0 != psScalarConfig->sRectInfo.sSrcRectConfig.uPosX)
        ||(0 != psScalarConfig->sRectInfo.sSrcRectConfig.uPosY)
        ||(0 != psScalarConfig->sRectInfo.sDstRectConfig.uPosX)
        ||(0 != psScalarConfig->sRectInfo.sDstRectConfig.uPosY))
    {
      /* Currently rotator doesn`t support X/Y shift*/
      eStatusRet = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
    }
    if(HAL_MDSS_STATUS_SUCCESS == eStatusRet)
    {
      
      if(psScalarConfig->sRectInfo.sSrcRectConfig.uWidthInPixels > psScalarConfig->sRectInfo.sDstRectConfig.uWidthInPixels)
      {
        *pHDownscaleFactor = psScalarConfig->sRectInfo.sSrcRectConfig.uWidthInPixels/psScalarConfig->sRectInfo.sDstRectConfig.uWidthInPixels;
      }
      else
      {
        *pHDownscaleFactor = 0;
      }
      if(psScalarConfig->sRectInfo.sSrcRectConfig.uHeightInPixels> psScalarConfig->sRectInfo.sDstRectConfig.uHeightInPixels)
      {
        *pVDownscaleFactor = psScalarConfig->sRectInfo.sSrcRectConfig.uHeightInPixels / psScalarConfig->sRectInfo.sDstRectConfig.uHeightInPixels;
      }
      else
      {
        *pVDownscaleFactor = 0;
      }
    }
  }
  else
  {
    eStatusRet = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
  }
  return eStatusRet;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Scalar_Config()
*/
/*!
* \brief
*     Configures scalar for given data path
*
* \param [in]   eDataPathId      - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psScalarConfig   - Scalar information
* \param [in]   uFlags           - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_DataPath_Scalar_Config(HAL_MDP_DataPathId        eDataPathId,
                                                         HAL_MDP_ScalarConfigType *psScalarConfig,
                                                         uint32                    uFlags)
{
  //#TODO#IMPLEMENT - obsolete register - ROTATOR_PIPE_DOWNSCALER
  return HAL_MDSS_STATUS_FAILED_NOT_IMPLEMENTED;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Surface_Config()
*/
/*!
* \brief
*     Set up surface and output rectangle configurations for given data path.
*     If valid Chroma down ID is passed configures the CSC and down sampler based on 
*     pixel format
*
* \param [in]   eDataPathId            - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   eDataPathMode          - Data path mode. Block mode or line mode.
* \param [in]   eChromaDownModuleId    - Chroma down module ID
* \param [in]   psSurfaceConfig        - Surface and Rectangle Configuration information
* \param [in]   uFlags                 - Reserved.
*
* \retval NONE
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_DataPath_Surface_Config(HAL_MDP_DataPathId                   eDataPathId,
                                                          HAL_MDP_DataPath_ModeType            eDataPathMode,
                                                          HAL_MDP_ChromaDownModuleId           eChromaDownModuleId,
                                                          HAL_MDP_DataPath_SurfaceConfigType  *psSurfaceConfig,
                                                          uint32                              uFlags)
{
   HAL_MDSS_ErrorType  eStatusRet            = HAL_MDSS_STATUS_SUCCESS;
   if(NULL != psSurfaceConfig->psOutSurfaceConfig)
   {
      HAL_MDP_DataPath_SurfaceAttrib_Config(eDataPathId, eDataPathMode, eChromaDownModuleId, psSurfaceConfig->psOutSurfaceConfig, 0x00);
   }

   if(NULL != psSurfaceConfig->psOutputRectConfig)
   {
      HAL_MDP_DataPath_OutRect_Config(eDataPathId, eChromaDownModuleId, psSurfaceConfig->psOutputRectConfig, 0x00);
   }

   if(NULL != psSurfaceConfig->psConstAlphaConfig)
   {
      HAL_MDP_DataPath_ConstAlpha_Config(eDataPathId, psSurfaceConfig->psConstAlphaConfig, 0x00);
   }
   
   if(NULL != psSurfaceConfig->psScalarConfig)
   {
      eStatusRet = HAL_MDP_DataPath_Scalar_Config(eDataPathId, psSurfaceConfig->psScalarConfig, 0x00);
   }
   return eStatusRet;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Setup()
*/
/*!
* \brief
*     Configures the data path
*
*
* \param [in]   eDataPathId         - DATA PATH ID (0, 1, 2, 3, and 4)
* \param [in]   psDataPathConfig    - Data path configuration data
* \param [in]   uFlags              - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DataPath_Setup(HAL_MDP_DataPathId          eDataPathId,
                                          HAL_MDP_DataPathConfigType  *psDataPathConfig,
                                          uint32                       uFlags)
{
   HAL_MDSS_ErrorType            eStatus     = HAL_MDSS_STATUS_SUCCESS;

   // Call for HAL_MDP_DataPath_Rotator_Config() is intentionally removed for 3xx, because We don't need to disable rotator while blt operation, if platform has independent Rotator.

   if(NULL != psDataPathConfig->pSurfaceConfig)
   {
      eStatus = HAL_MDP_DataPath_Surface_Config(eDataPathId,  psDataPathConfig->eDataPathMode, psDataPathConfig->eChromaDownModuleId, psDataPathConfig->pSurfaceConfig, 0x00);
   }

   /* Chroma module configuration */
   if (NULL != psDataPathConfig->psChromaDownConfig)
   {
     eStatus = HAL_MDP_CDM_Setup(psDataPathConfig->eChromaDownModuleId, psDataPathConfig->psChromaDownConfig, 0x00);
   }


   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Init()
*/
/*!
* \brief
*     Initializes the Data Path block
*
* \param [in] eDataPathId           - Data Path ID
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DataPath_Init(HAL_MDP_DataPathId      eDataPathId,
                           uint32                  uInitFlags,
                           uint32                  uFlags)
{
  /* Nothing to do */
}


#ifdef __cplusplus
}
#endif
