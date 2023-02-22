#include "stdafx.h"
#include "LayerStack.h"
#include "Logging/Instrumentor.h"

LayerStack::LayerStack()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

LayerStack::~LayerStack()
{
	for(std::vector<Ref<Layer>>::reverse_iterator it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
	{
		(*it)->OnDetach();
		(*it).reset();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::AddLayer(Ref<Layer> layer)
{
	m_WaitingLayers.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::AddOverlay(Ref<Layer> layer)
{
	m_WaitingOverlays.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::RemoveLayer(Ref<Layer> layer)
{
	m_DeadLayers.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::RemoveOverlay(Ref<Layer> layer)
{
	m_DeadOverlays.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::PushPop()
{
	PROFILE_FUNCTION();
	// Push any layers that were created during the update to the stack
	for (Ref<Layer> layer : m_WaitingLayers)
	{
		PushLayer(layer);
	}

	for (Ref<Layer> overlay : m_WaitingOverlays)
	{
		PushOverlay(overlay);
	}

	m_WaitingLayers.clear();
	m_WaitingOverlays.clear();

	// Remove any layers that were deleted during the update
	for (Ref<Layer> layer : m_DeadLayers)
	{
		PopLayer(layer);
	}

	for (Ref<Layer> layer : m_DeadOverlays)
	{
		PopOverlay(layer);
	}

	m_DeadLayers.clear();
	m_DeadOverlays.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::PushLayer(Ref<Layer> layer)
{
	PROFILE_FUNCTION();
	m_Layers.emplace(m_Layers.begin() + m_LayerInsert, layer);
	m_LayerInsert++;
	layer->OnAttach();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void LayerStack::PushOverlay(Ref<Layer> overlay)
{
	PROFILE_FUNCTION();
	m_Layers.emplace_back(overlay);
	overlay->OnAttach();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool LayerStack::PopLayer(Ref<Layer> layer)
{
	PROFILE_FUNCTION();
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		layer->OnDetach();
		m_Layers.erase(it);
		layer.reset();
		m_LayerInsert--;
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool LayerStack::PopOverlay(Ref<Layer> overlay)
{
	PROFILE_FUNCTION();
	auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
		overlay.reset();
		return true;
	}
	return false;
}