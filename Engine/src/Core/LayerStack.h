#pragma once

#include <vector>

#include "Core/core.h"
#include "Layer.h"

class LayerStack
{
public:
	LayerStack();
	~LayerStack();

	// Add a layer to the layer stack
	void AddLayer(Ref<Layer> layer);

	// Add an overlay to the  layer stack
	void AddOverlay(Ref<Layer> layer);

	// Removes a layer from the layer stack
	void RemoveLayer(Ref<Layer> layer);

	// Removes an overlay from the layer stack
	void RemoveOverlay(Ref<Layer> layer);

	void PushLayer(Ref<Layer> layer);
	void PushOverlay(Ref<Layer> overlay);
	bool PopLayer(Ref<Layer> layer);
	bool PopOverlay(Ref<Layer> overlay);

	void PushPop();

	std::vector<Ref<Layer>>::iterator begin() { return m_Layers.begin(); }
	std::vector<Ref<Layer>>::iterator end() { return m_Layers.end(); }
	std::vector<Ref<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
	std::vector<Ref<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }

	std::vector<Ref<Layer>>::const_iterator begin() const { return m_Layers.begin(); }
	std::vector<Ref<Layer>>::const_iterator end() const { return m_Layers.end(); }
	std::vector<Ref<Layer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
	std::vector<Ref<Layer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
	std::vector<Ref<Layer>> m_Layers;
	uint32_t m_LayerInsert = 0;

	std::vector<Ref<Layer>> m_WaitingLayers;
	std::vector<Ref<Layer>> m_WaitingOverlays;

	std::vector<Ref<Layer>> m_DeadLayers;
	std::vector<Ref<Layer>> m_DeadOverlays;
};

