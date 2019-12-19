#pragma once

#include "Core/core.h"

enum class ShaderDataType
{
	None = 0,
	Float, Float2, Float3, Float4,
	Mat3, Mat4,
	Int, Int2, Int3, Int4,
	Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:		return 4;
	case ShaderDataType::Float2:	return 4 * 2;
	case ShaderDataType::Float3:	return 4 * 3;
	case ShaderDataType::Float4:	return 4 * 4;
	case ShaderDataType::Mat3:		return 4 * 3 * 3;
	case ShaderDataType::Mat4:		return 4 * 4 * 4;
	case ShaderDataType::Int:		return 4;
	case ShaderDataType::Int2:		return 4 * 2;
	case ShaderDataType::Int3:		return 4 * 3;
	case ShaderDataType::Int4:		return 4 * 4;
	case ShaderDataType::Bool:		return 1;
	default:
		break;
	}

	CORE_ASSERT(false, "Shader Data type not recognised");
	return 0;
}
struct BufferElement
{
	std::string Name;
	ShaderDataType Type;
	size_t Offset;
	uint32_t Size;
	bool Normalized;

	BufferElement()
		:Name(""), Type(ShaderDataType::None), Normalized(false), Size(0), Offset(0) {}

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		:Name(name), Type(type), Normalized(normalized), Size(ShaderDataTypeSize(type)), Offset(0) {}

	uint32_t Count() const
	{
		switch (Type)
		{
		case ShaderDataType::Float:		return 1;
		case ShaderDataType::Float2:	return 2;
		case ShaderDataType::Float3:	return 3;
		case ShaderDataType::Float4:	return 4;
		case ShaderDataType::Mat3:		return 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4;
		case ShaderDataType::Int:		return 1;
		case ShaderDataType::Int2:		return 2;
		case ShaderDataType::Int3:		return 3;
		case ShaderDataType::Int4:		return 4;
		case ShaderDataType::Bool:		return 1;
		default:
			break;
		}

		return 0;
	}
};
class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(const std::initializer_list<BufferElement>& elements)
		:m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
	inline const uint32_t GetStride() const { return m_Stride; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }	
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		size_t offset = 0; 
		m_Stride = 0;
		for (BufferElement& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

class VertexBuffer
{
public:
	virtual ~VertexBuffer() = default;

	virtual void SetLayout(const BufferLayout& layout) = 0;
	virtual const BufferLayout& GetLayout() = 0;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
};

class IndexBuffer
{
public:
	virtual ~IndexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	virtual uint32_t GetCount() const = 0;

	static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
};