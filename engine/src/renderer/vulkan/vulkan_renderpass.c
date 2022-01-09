//
// Created by 12132 on 2022/1/9.
//

#include "vulkan_renderpass.h"
#include "core.h"

void vulkan_renderpass_create(
    vulkan_context* context,
    vulkan_renderpass* out_renderpass,
    f32 x,f32 y ,f32 w,f32 h,
    f32 r, f32 g, f32 b, int_fast32_t a,
    f32 depth,u32 stencil)
{

    /// main subpass
    VkSubpassDescription subpass={};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    /// todo configurable
    u32 attachment_desc_count = 2;
    VkAttachmentDescription attachment_desc[attachment_desc_count];
    VkAttachmentDescription color_attachment={};
    color_attachment.format = context->swapchain.image_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags = 0;

    attachment_desc[0] = color_attachment;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;      /// Attachment desc index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount =1;
    subpass.pColorAttachments = &color_attachment_reference;

    VkAttachmentDescription depth_attachment={};
    depth_attachment.format = context->device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment.flags = 0;

    attachment_desc[1]=depth_attachment;

    /// depth attachment reference
    VkAttachmentReference depth_attachment_reference={};
    depth_attachment_reference.attachment =1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    /// depth attachment data
    subpass.pDepthStencilAttachment= &depth_attachment_reference;

    /// todo other attachment type (input ,resolve , preserve)

    /// input from a shader
    subpass.inputAttachmentCount= 0;
    subpass.pInputAttachments = 0;

    /// attachments used for multisampling colour attachment
    subpass.pResolveAttachments =0;

    ///attachment not in  this subpass but must be preserved for next
    subpass.pPreserveAttachments=0;
    subpass.preserveAttachmentCount= 0;

    /// render pass dependency TODO configurable
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;


    // render pass create info
    VkRenderPassCreateInfo renderpassCI ={};
    renderpassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassCI.attachmentCount =attachment_desc_count;
    renderpassCI.pAttachments = attachment_desc;
    renderpassCI.subpassCount =1;
    renderpassCI.pSubpasses = &subpass;
    renderpassCI.dependencyCount = 1;
    renderpassCI.pDependencies = &dependency;
    renderpassCI.pNext = 0;
    renderpassCI.flags =0;
    VK_CHECK(vkCreateRenderPass(context->device.logical_device,
                                &renderpassCI,context->allocator,&out_renderpass->handle));
    KDEBUG("Create render pass success");

}

void vulkan_renderpass_destroy(vulkan_context* context,
                               vulkan_renderpass* renderpass)
{
    KINFO("Destroy Render pass");
    if (renderpass->handle){
        vkDestroyRenderPass(context->device.logical_device,
                            renderpass->handle,context->allocator);
        renderpass->handle = 0;

    }
}

void vulkan_renderpass_begin (
    vulkan_command_buffer* command_buffer,
    vulkan_renderpass* renderpass,
    VkFramebuffer frame_buffer)
{
    VkRenderPassBeginInfo beginInfo ={};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext =0;
    beginInfo.renderPass = renderpass->handle;
    beginInfo.framebuffer= frame_buffer;
    beginInfo.renderArea.offset.x  = renderpass->x;
    beginInfo.renderArea.offset.y  = renderpass->y;
    beginInfo.renderArea.extent.width  = renderpass->w;
    beginInfo.renderArea.extent.height = renderpass->h;

    VkClearValue clear_values[2];
    kzero_memory(clear_values, sizeof(VkClearValue)*2);
    clear_values[0].color.float32[0] = renderpass->r;
    clear_values[0].color.float32[1] = renderpass->g;
    clear_values[0].color.float32[2] = renderpass->b;
    clear_values[0].color.float32[3] = renderpass->a;

    clear_values[1].depthStencil.depth = renderpass->depth;
    clear_values[1].depthStencil.stencil = renderpass->stencil;
    beginInfo.clearValueCount=2;
    beginInfo.pClearValues = clear_values;

    vkCmdBeginRenderPass(command_buffer->handle,&beginInfo,VK_SUBPASS_CONTENTS_INLINE);
    command_buffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void vulkan_renderpass_end(
    vulkan_command_buffer* command_buffer,
    vulkan_renderpass* renderpass)
{
    vkCmdEndRenderPass(command_buffer->handle);
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;

}

