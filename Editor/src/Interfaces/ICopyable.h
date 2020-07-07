#pragma once

class ICopyable
{
public:
	virtual void Copy() = 0;
	virtual void Cut() = 0;
	virtual void Paste() = 0;
	virtual void Duplicate() = 0;
	virtual void Delete() = 0;
};
