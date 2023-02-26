#pragma once

#include "ViewerManager.h"

#include <filesystem>

class Font;
class FrameBuffer;
class OrthographicCamera;

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

    Ref<Font> m_Font;

    Ref<FrameBuffer> m_Framebuffer;

    Ref<OrthographicCamera> m_Camera;

    std::string m_String;
};
