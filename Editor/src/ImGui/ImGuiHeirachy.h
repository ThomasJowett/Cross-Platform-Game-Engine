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

    void SetContext(const Ref<Scene>& scene);
private:
    void DrawNode(Entity entity);
private:
    bool* m_Show;

    Ref<Scene> m_Scene;

    Entity m_SelectedEntity;
};

