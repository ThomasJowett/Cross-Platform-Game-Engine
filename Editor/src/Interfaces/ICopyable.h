#pragma once

class ICopyable
{
public:
	virtual void Copy() = 0;
	virtual void Cut() = 0;
	virtual void Paste() = 0;
	virtual void Duplicate() = 0;
	virtual void Delete() = 0;
	virtual bool HasSelection() const = 0;
	virtual void SelectAll() = 0;

	virtual bool IsReadOnly() const = 0;
};
