#pragma once

#include "Engine.h"

class HierarchyPanel :
    public Layer
{
public:
    explicit HierarchyPanel(bool* show);
    ~HierarchyPanel() = default;

    void OnAttach() override;
    void OnFixedUpdate() override;
    void OnImGuiRender() override;

    Entity GetSelectedEntity() { return m_SelectedEntity; }
    void SetSelectedEntity(Entity entity);
private:
    void DrawNode(Entity entity);
private:
    bool* m_Show;

    Entity m_SelectedEntity;
};