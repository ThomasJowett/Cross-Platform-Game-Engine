#pragma once
#include "Engine.h"

class HeirachyPanel :
    public Layer
{
public:
    explicit HeirachyPanel(bool* show);
    ~HeirachyPanel() = default;

    void OnAttach() override;
    void OnFixedUpdate() override;
    void OnImGuiRender() override;

    Entity GetSelectedEntity() { return m_SelectedEntity; }
private:
    void DrawNode(Entity entity);
private:
    bool* m_Show;

    Entity m_SelectedEntity;
};