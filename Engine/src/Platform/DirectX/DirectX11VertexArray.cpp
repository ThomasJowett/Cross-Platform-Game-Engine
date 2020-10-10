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
	m_VertexLayout = nullptr;
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
	// TODO: add vertex buffer directX11
	vertexBuffer->Bind();

	CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout has no elements");

	uint32_t index = 0;
	const BufferLayout& layout = vertexBuffer->GetLayout();

	UINT numElements = (UINT)layout.GetElements().size();

	D3D11_INPUT_ELEMENT_DESC* ed =  new D3D11_INPUT_ELEMENT_DESC[numElements];
	for (const BufferElement& element : layout)
	{
		ed[index].Format = ShaderDataTypeToDXGIBaseTypes(element.Type);
		ed[index].InputSlot = 0;
		ed[index].AlignedByteOffset = index == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		ed[index].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		switch (element.Type)
		{
		case ShaderDataType::Float:
		{
			static int semanticIndexFloat = 0;
			ed[index].SemanticName = "float";
			ed[index].SemanticIndex = semanticIndexFloat;
			semanticIndexFloat++;
			break;
		}
		case ShaderDataType::Float2:
		{
			static int semanticIndexFloat2 = 0;
			ed[index].SemanticName = "float2";
			ed[index].SemanticIndex = semanticIndexFloat2;
			semanticIndexFloat2++;
			break;
		}
		case ShaderDataType::Float3:
		{
			static int semanticIndexFloat3 = 0;
			ed[index].SemanticName = "float3";
			ed[index].SemanticIndex = semanticIndexFloat3;
			semanticIndexFloat3++;
			break;
		}
		case ShaderDataType::Float4:
		{
			static int semanticIndexFloat4 = 0;
			ed[index].SemanticName = "float4";
			semanticIndexFloat4++;
			break;
		}
		case ShaderDataType::Int:
		{
			static int semanticIndexInt = 0;
			ed[index].SemanticName = "int";
			ed[index].SemanticIndex = semanticIndexInt;
			semanticIndexInt++;
			break;
		}
		case ShaderDataType::Int2:
		{
			static int semanticIndexInt2 = 0;
			ed[index].SemanticName = "int2";
			ed[index].SemanticIndex = semanticIndexInt2;
			semanticIndexInt2++;
			break;
		}
		case ShaderDataType::Int3:
		{
			static int semanticIndexInt3 = 0;
			ed[index].SemanticName = "int3";
			ed[index].SemanticIndex = semanticIndexInt3;
			semanticIndexInt3++;
			break;
		}
		case ShaderDataType::Int4:
		{
			static int semanticIndexInt4 = 0;
			ed[index].SemanticName = "int4";
			ed[index].SemanticIndex = semanticIndexInt4;
			semanticIndexInt4++;
			break;
		}
		case ShaderDataType::Bool:
		{
			static int semanticIndexInt4 = 0;
			ed[index].SemanticName = "int4";
			ed[index].SemanticIndex = semanticIndexInt4;
			semanticIndexInt4++;
			break;
		}
		default:
			CORE_ASSERT(false, "Unkown ShaderDataType");
		}

		index++;
	}

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