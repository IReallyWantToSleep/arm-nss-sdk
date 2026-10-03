// Copyright  © 2023 Advanced Micro Devices, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
// SPDX-License-Identifier: MIT

/// @defgroup VKBackend Vulkan Backend
/// FidelityFX SDK native backend implementation for Vulkan.
///
/// @ingroup Backends

#pragma once

#include <vulkan/vulkan.h>

#include "../../ffx_interface.h"

#if defined(__cplusplus)
extern "C" {
#endif  // #if defined(__cplusplus)

typedef enum FfxGpuVendorId
{
    UnknownVendor = 0xffffffff,
    NotQueried    = 0,

    Amd         = 0x1002,
    ImgTec      = 0x1010,
    Nvidia      = 0x10DE,
    Arm         = 0x13B5,
    Broadcom    = 0x14E4,
    Qualcomm    = 0x5143,
    Intel       = 0x8086,
    Apple       = 0x106B,
    Vivante     = 0x7a05,
    VeriSilicon = 0x1EB1,
    SamsungAMD  = 0x144D,
    Microsoft   = 0x1414,

    Kazan    = 0x10003,  // VkVendorId
    Codeplay = 0x10004,  // VkVendorId
    Mesa     = 0x10005,  // VkVendorId
} FfxGpuVendorId;

/// Convenience structure to hold all VK-related device information
typedef struct VkDeviceContext
{
    VkDevice                  vkDevice;               /// The Vulkan device
    VkPhysicalDevice          vkPhysicalDevice;       /// The Vulkan physical device
    PFN_vkGetDeviceProcAddr   vkDeviceProcAddr;       /// The device's function address table
    VkInstance                vkInstance;             /// The Vulkan instance
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;  /// The instance's function address table
    VkQueue                   vkQueue;                /// Optional queue used by portable NSS DP4A.
    uint32_t                  queueFamilyIndex;       /// Queue family index for vkQueue.
} VkDeviceContext;

/// Query how much memory is required for the Vulkan backend's scratch buffer.
///
/// @param [in] physicalDevice              A pointer to the VkPhysicalDevice device.
/// @param [in] maxContexts                 The maximum number of simultaneous effect contexts that will share the backend.
///                                         (Note that some effects contain internal contexts which count towards this maximum)
///
/// @returns
/// The size (in bytes) of the required scratch memory buffer for the VK backend.
///
/// @ingroup VKBackend
FFX_API size_t ffxGetScratchMemorySizeVK(VkDeviceContext& deviceContext, size_t maxContexts);

/// Create a <c><i>FfxDevice</i></c> from a <c><i>VkDevice</i></c>.
///
/// @param [in] vkDeviceContext             A pointer to a VKDeviceContext that holds all needed information
///
/// @returns
/// An abstract FidelityFX device.
///
/// @ingroup VKBackend
FFX_API FfxDevice ffxGetDeviceVK(VkDeviceContext* vkDeviceContext);

/// Populate an interface with pointers for the VK backend.
///
/// @param [out] backendInterface           A pointer to a <c><i>FfxInterface</i></c> structure to populate with pointers.
/// @param [in] device                      A pointer to the VkDevice device.
/// @param [in] scratchBuffer               A pointer to a buffer of memory which can be used by the DirectX(R)12 backend.
/// @param [in] scratchBufferSize           The size (in bytes) of the buffer pointed to by <c><i>scratchBuffer</i></c>.
/// @param [in] maxContexts                 The maximum number of simultaneous effect contexts that will share the backend.
///                                         (Note that some effects contain internal contexts which count towards this maximum)
///
/// @retval
/// FFX_OK                                  The operation completed successfully.
/// @retval
/// FFX_ERROR_CODE_INVALID_POINTER          The <c><i>interface</i></c> pointer was <c><i>NULL</i></c>.
///
/// @ingroup VKBackend
FFX_API FfxErrorCode ffxGetInterfaceVK(FfxInterface* backendInterface, FfxDevice device, void* scratchBuffer, size_t scratchBufferSize, size_t maxContexts);

/// Create a <c><i>FfxCommandList</i></c> from a <c><i>VkCommandBuffer</i></c>.
///
/// @param [in] cmdBuf                      A pointer to the Vulkan command buffer.
///
/// @returns
/// An abstract FidelityFX command list.
///
/// @ingroup VKBackend
FFX_API FfxCommandList ffxGetCommandListVK(VkCommandBuffer cmdBuf);

/// Create a <c><i>FfxPipeline</i></c> from a <c><i>VkPipeline</i></c>.
///
/// @param [in] pipeline                    A pointer to the Vulkan pipeline.
///
/// @returns
/// An abstract FidelityFX pipeline.
///
/// @ingroup VKBackend
FFX_API FfxPipeline ffxGetPipelineVK(VkPipeline pipeline);

/// Fetch a <c><i>FfxResource</i></c> from a <c><i>GPUResource</i></c>.
///
/// @param [in] vkResource                  A pointer to the (agnostic) VK resource.
/// @param [in] ffxResDescription           An <c><i>FfxResourceDescription</i></c> for the resource representation.
/// @param [in] ffxResName                  (optional) A name string to identify the resource in debug mode.
/// @param [in] state                       The state the resource is currently in.
///
/// @returns
/// An abstract FidelityFX resources.
///
/// @ingroup VKBackend
FFX_API FfxResource ffxGetResourceVK(void*                  vkResource,
                                     FfxResourceDescription ffxResDescription,
                                     const char*            ffxResName,
                                     FfxResourceStates      state = FFX_RESOURCE_STATE_COMPUTE_READ);

/// Fetch a <c><i>FfxSurfaceFormat</i></c> from a VkFormat.
///
/// @param [in] format              The VkFormat to convert to <c><i>FfxSurfaceFormat</i></c>.
///
/// @returns
/// An <c><i>FfxSurfaceFormat</i></c>.
///
/// @ingroup VKBackend
FFX_API FfxSurfaceFormat ffxGetSurfaceFormatVK(VkFormat format);

/// Fetch a <c><i>FfxResourceDescription</i></c> from an existing VkBuffer.
///
/// @param [in] buffer              The VkBuffer resource to create a <c><i>FfxResourceDescription</i></c> for.
/// @param [in] createInfo          The VkBufferCreateInfo of the buffer
/// @param [in] additionalUsages    Optional <c><i>FfxResourceUsage</i></c> flags needed for select resource mapping.
///
/// @returns
/// An <c><i>FfxResourceDescription</i></c>.
///
/// @ingroup VKBackend
FFX_API FfxResourceDescription ffxGetBufferResourceDescriptionVK(const VkBuffer           buffer,
                                                                 const VkBufferCreateInfo createInfo,
                                                                 FfxResourceUsage         additionalUsages = FFX_RESOURCE_USAGE_READ_ONLY);

/// Fetch a <c><i>FfxResourceDescription</i></c> from an existing VkImage.
///
/// @param [in] image               The VkImage resource to create a <c><i>FfxResourceDescription</i></c> for.
/// @param [in] createInfo          The VkImageCreateInfo of the buffer
/// @param [in] additionalUsages    Optional <c><i>FfxResourceUsage</i></c> flags needed for select resource mapping.
///
/// @returns
/// An <c><i>FfxResourceDescription</i></c>.
///
/// @ingroup VKBackend
FFX_API FfxResourceDescription ffxGetImageResourceDescriptionVK(const VkImage           image,
                                                                const VkImageCreateInfo createInfo,
                                                                FfxResourceUsage        additionalUsages = FFX_RESOURCE_USAGE_READ_ONLY);

/// Fetch a <c><i>FfxCommandQueue</i></c> from an existing VkQueue.
///
/// @param [in] commandQueue       The VkQueue to create a <c><i>FfxCommandQueue</i></c> from.
///
/// @returns
/// An <c><i>FfxCommandQueue</i></c>.
///
/// @ingroup VKBackend
FFX_API FfxCommandQueue ffxGetCommandQueueVK(VkQueue commandQueue);

#if defined(__cplusplus)
}
#endif  // #if defined(__cplusplus)
