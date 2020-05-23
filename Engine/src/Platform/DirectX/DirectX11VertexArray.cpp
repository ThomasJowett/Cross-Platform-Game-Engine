#include "stdafx.h"
#include "DirectX11VertexArray.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

static DXGI_FORMAT ShaderDataTypeToDXGIBaseTypes(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:		return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Float2:	return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Float3:	return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Float4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Mat3:		return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Mat4:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Int:		return DXGI_FORMAT_R32_SINT;
	case ShaderDataType::Int2:		return DXGI_FORMAT_R32G32_SINT;
	case ShaderDataType::Int3:		return DXGI_FORMAT_R32G32B32_SINT;
	case ShaderDataType::Int4:		return DXGI_FORMAT_R32G32B32A32_SINT;
	case ShaderDataType::Bool:		return DXGI_FORMAT_R1_UNORM;
	default:
		break;
	}

	CORE_ASSERT(false, "Shader Data type not recognised");
	return DXGI_FORMAT_UNKNOWN;
}

DirectX11VertexArray::DirectX11VertexArray()
{
}

DirectX11VertexArray::~DirectX11VertexArray()
{
}

void DirectX11VertexArray::Bind() const
{
}

void DirectX11VertexArray::UnBind() const
{
}

void DirectX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	// TODO: add index buffer directX11
	vertexBuffer->Bind();

	CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout has no elements");

	uint32_t index = 0;
	const BufferLayout& layout = vertexBuffer->GetLayout();

	D3D11_INPUT_ELEMENT_DESC ed[4];
	for (const BufferElement& element : layout)
	{
		switch (element.Type)
		{
		case ShaderDataType::Float:
		{
			//ed[index].SemanticName = 
		}
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			//ed[index].SemanticName = 
			break;
		}
		default:
			CORE_ASSERT(false, "Unkown ShaderDataType");
		}
	}

	UINT numElements = ARRAYSIZE(ed);
	ID3DBlob* pVSBlob = nullptr;

	//HRESULT hr = g_D3dDevice->CreateInputLayout(ed, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);

	//if (FAILED(hr))
	{
		ENGINE_ERROR("Could not create input layout");
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void DirectX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	// TODO: set index buffer directX11
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

const std::vector<Ref<VertexBuffer>>& DirectX11VertexArray::GetVertexBuffers() const
{
	return m_VertexBuffers;
}

const Ref<IndexBuffer>& DirectX11VertexArray::GetIndexBuffer() const
{
	return m_IndexBuffer;
}