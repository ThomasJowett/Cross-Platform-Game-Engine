#include "pch.h"
#include "EngineDLL.h"

// This is an example of an exported function.
ENGINEDLL_API int fnEngineDLL(void)
{
    Scene* scene = new Scene(std::string("Test"));

    FrameBufferSpecification fbSpec = { 600, 600 };
    fbSpec.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
    Ref<FrameBuffer> renderTarget = FrameBuffer::Create(fbSpec);
    scene->Render(renderTarget);
    return (int)scene->GetSceneName().size();
}
