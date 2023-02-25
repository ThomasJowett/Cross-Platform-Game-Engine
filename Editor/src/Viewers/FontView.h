#pragma once

#include "ViewerManager.h"

#include "Renderer/FrameBuffer.h"

#include <filesystem>

class FontView :
    public View
{
public:
    FontView(bool* show, std::filesystem::path filepath);
    ~FontView() = default;

    virtual void OnAttach() override;
    virtual void OnImGuiRender() override;

private:
    bool* m_Show;

    std::filesystem::path m_Filepath;

    Ref<FrameBuffer> m_Framebuffer;
};
