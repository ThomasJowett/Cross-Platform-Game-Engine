#include "pch.h"
#include "EngineDLL.h"

// This is an example of an exported function.
ENGINEDLL_API int fnEngineDLL(void)
{
    Scene* scene = new Scene("Test Scene");
    
    //FrameBufferSpecification fbSpec = { 600, 600 };
    //fbSpec.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
    //Ref<FrameBuffer> renderTarget = FrameBuffer::Create(fbSpec);
    //scene->Render(nullptr);
    return (int)scene->GetPixelsPerUnit();
}
