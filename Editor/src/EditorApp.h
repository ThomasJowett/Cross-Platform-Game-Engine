#pragma once
#include "Core/EntryPoint.h"

class Editor : public Application
{
public:
	explicit Editor();
	~Editor();
};

/*Entry point*/
Ref<Application> CreateApplication()
{
	auto editor = CreateRef<Editor>();
	if (editor->GetWindow())
		return editor;
	else 
		return nullptr;
}