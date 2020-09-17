#pragma once
#include "include.h"

class HeirachyPanel :
    public Layer
{
public:
    explicit HeirachyPanel(bool* show);
    ~HeirachyPanel() = default;

    void OnAttach() override;
    void OnFixedUpdate() override;
    void OnImGuiRender() override;

    void SetContext(const Ref<Scene>& scene);
    Entity GetSelectedEntity() { return m_SelectedEntity; }
private:
    void DrawNode(Entity entity);
private:
    bool* m_Show;

    Ref<Scene> m_Scene;

    Entity m_SelectedEntity;
};

