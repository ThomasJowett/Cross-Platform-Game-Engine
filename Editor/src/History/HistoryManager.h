#pragma once

#include "Core/core.h"

class HistoryRecord
{
public:
	virtual void Undo() = 0;
	virtual void Redo() = 0;
	virtual void End() = 0;
};

class HistoryManager
{
public:
	static void AddHistoryRecord(Ref<HistoryRecord> undoRecord);
	static void Undo(int steps);
	static void Redo(int steps);

	static bool CanUndo();
	static bool CanRedo();

	static void Reset();
};