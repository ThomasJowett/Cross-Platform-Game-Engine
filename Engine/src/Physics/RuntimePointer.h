#pragma once

// A pointer to a runtime physics object owned by one component
// Copy construction gives null and copy assignment keeps this pointer, even from another RuntimePointer; moves transfer it
template<typename T>
class RuntimePointer
{
public:
	RuntimePointer() = default;
	RuntimePointer(const RuntimePointer&) {}
	RuntimePointer& operator=(const RuntimePointer&) { return *this; }

	RuntimePointer(RuntimePointer&& other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	RuntimePointer& operator=(RuntimePointer&& other) noexcept
	{
		if (this != &other)
		{
			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
		}
		return *this;
	}

	RuntimePointer& operator=(T* ptr)
	{
		m_Ptr = ptr;
		return *this;
	}

	operator T*() const { return m_Ptr; }
	T* operator->() const { return m_Ptr; }

private:
	T* m_Ptr = nullptr;
};
