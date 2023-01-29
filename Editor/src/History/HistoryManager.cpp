#include "HistoryManager.h"
#include "Scene/SceneManager.h"

#include <vector>

static std::vector<Ref<HistoryRecord>> s_UndoBuffer;
static int s_UndoIndex = 0;

void HistoryManager::AddHistoryRecord(Ref<HistoryRecord> undoRecord)
{
	undoRecord->End();
	if (s_UndoIndex != s_UndoBuffer.size())
		s_UndoBuffer.erase(s_UndoBuffer.begin() + s_UndoIndex, s_UndoBuffer.end());
	s_UndoBuffer.push_back(undoRecord);
	++s_UndoIndex;
}

void HistoryManager::Undo(int steps)
{
	while (s_UndoIndex > 0 && steps-- > 0)
		s_UndoBuffer[--s_UndoIndex]->Undo();

	if (s_UndoIndex > 0)
		SceneManager::CurrentScene()->MakeDirty();
}

void HistoryManager::Redo(int steps)
{
	while (s_UndoIndex < (int)s_UndoBuffer.size() && steps-- > 0)
		s_UndoBuffer[s_UndoIndex++]->Redo();
	if (s_UndoIndex > 0)
		SceneManager::CurrentScene()->MakeDirty();
}

bool HistoryManager::CanUndo()
{
	return s_UndoIndex > 0;
}

bool HistoryManager::CanRedo()
{
	return s_UndoIndex < (int)s_UndoBuffer.size();
}

void HistoryManager::Reset()
{
	s_UndoBuffer.clear();
	s_UndoIndex = 0;
}
