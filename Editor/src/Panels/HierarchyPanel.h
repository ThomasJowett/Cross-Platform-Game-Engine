#pragma once

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Layer.h"
#include "Scene/Entity.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

struct ImGuiTextFilter;

class HierarchyPanel :
    public Layer, public ICopyable, public IUndoable
{
public:
    explicit HierarchyPanel(bool* show);
    ~HierarchyPanel();

    void OnAttach() override;
    void OnDetach() override;
    void OnFixedUpdate() override;
    void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    Entity GetSelectedEntity();
    void SetSelectedEntity(Entity entity);

    // Inherited via ICopyable
    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;
    virtual void Duplicate() override;
    virtual void Delete() override;
    virtual bool HasSelection() const override;
    virtual void SelectAll() override;
    virtual bool IsReadOnly() const override;

    // Inherited via IUndoable
    virtual void Undo(int asteps) override;
    virtual void Redo(int asteps) override;
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    bool IsFocused() { return m_Focused; }
    void HasFocused() { m_Focused = false; }
private:
    void DrawNode(Entity entity);
    void DragDropTarget(Entity parent);
private:
    bool* m_Show;

    Entity m_SelectedEntity;
    bool m_Focused = false;

    ImGuiTextFilter* m_TextFilter;
};
