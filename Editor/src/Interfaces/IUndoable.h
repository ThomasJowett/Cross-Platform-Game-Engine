#pragma once
class IUndoable
{
public:
	virtual void Undo(int asteps = 1) = 0;
	virtual void Redo(int asteps = 1) = 0;
};