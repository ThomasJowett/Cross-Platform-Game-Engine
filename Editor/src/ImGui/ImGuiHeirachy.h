#pragma once
#include "include.h"

class ImGuiHeirachy :
    public Layer
{
public:
    explicit ImGuiHeirachy(bool* show);
    ~ImGuiHeirachy() = default;

    void OnAttach() override;
    void OnFixedUpdate() override;
    void OnImGuiRender() override;
private:
    bool* m_Show;

    Scene* m_Scene;
};

