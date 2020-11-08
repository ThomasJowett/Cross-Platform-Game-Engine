#include "stdafx.h"
#include "DirectX11VertexArray.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

#include "DirectX11Shader.h"

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

	std::string hlslCode;

	hlslCode += "struct VS_INPUT \n{\n";

	D3D11_INPUT_ELEMENT_DESC* elementDescriptions = new D3D11_INPUT_ELEMENT_DESC[numElements];
	for (const BufferElement& element : layout)
	{
		elementDescriptions[index].Format = ShaderDataTypeToDXGIBaseTypes(element.Type);
		elementDescriptions[index].InputSlot = 0;
		elementDescriptions[index].AlignedByteOffset = index == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		elementDescriptions[index].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		switch (element.Type)
		{
		case ShaderDataType::Float:
		{
			static int semanticIndexFloat = 0;
			elementDescriptions[index].SemanticName = "float";
			elementDescriptions[index].SemanticIndex = semanticIndexFloat;
			semanticIndexFloat++;
			break;
		}
		case ShaderDataType::Float2:
		{
			static int semanticIndexFloat2 = 0;
			elementDescriptions[index].SemanticName = "float2";
			elementDescriptions[index].SemanticIndex = semanticIndexFloat2;
			semanticIndexFloat2++;
			break;
		}
		case ShaderDataType::Float3:
		{
			static int semanticIndexFloat3 = 0;
			elementDescriptions[index].SemanticName = "float3";
			elementDescriptions[index].SemanticIndex = semanticIndexFloat3;
			semanticIndexFloat3++;
			break;
		}
		case ShaderDataType::Float4:
		{
			static int semanticIndexFloat4 = 0;
			elementDescriptions[index].SemanticName = "float4";
			semanticIndexFloat4++;
			break;
		}
		case ShaderDataType::Int:
		{
			static int semanticIndexInt = 0;
			elementDescriptions[index].SemanticName = "int";
			elementDescriptions[index].SemanticIndex = semanticIndexInt;
			semanticIndexInt++;
			break;
		}
		case ShaderDataType::Int2:
		{
			static int semanticIndexInt2 = 0;
			elementDescriptions[index].SemanticName = "int2";
			elementDescriptions[index].SemanticIndex = semanticIndexInt2;
			semanticIndexInt2++;
			break;
		}
		case ShaderDataType::Int3:
		{
			static int semanticIndexInt3 = 0;
			elementDescriptions[index].SemanticName = "int3";
			elementDescriptions[index].SemanticIndex = semanticIndexInt3;
			semanticIndexInt3++;
			break;
		}
		case ShaderDataType::Int4:
		{
			static int semanticIndexInt4 = 0;
			elementDescriptions[index].SemanticName = "int4";
			elementDescriptions[index].SemanticIndex = semanticIndexInt4;
			semanticIndexInt4++;
			break;
		}
		case ShaderDataType::Bool:
		{
			static int semanticIndexBool = 0;
			elementDescriptions[index].SemanticName = "int4";
			elementDescriptions[index].SemanticIndex = semanticIndexBool;
			semanticIndexBool++;
			break;
		}
		default:
			CORE_ASSERT(false, "Unknown ShaderDataType");
		}

		index++;
	}

	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	hlslCode += 
R"(};
struct VS_OUTPUT
{
	float4 sv_Position : SV_POSITION;
};

VS_OUTPUT vertexShader(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.sv_Position = float4(0.0,0.0,0.0,0.0);
	return output;
}
)";

	//TODO: write a dummy shader that has the input layout as the layout created

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION; // | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	HRESULT hr = D3DCompile(hlslCode.c_str(), hlslCode.size(), nullptr, nullptr, nullptr, "vertexShader", ("vs" + DirectX11Shader::GetShaderVersion()).c_str(), flags, 0, &pVSBlob, &pErrorBlob);

	if (pErrorBlob != nullptr)
	{
		ENGINE_ERROR((char*)pErrorBlob->GetBufferPointer());
	}

	if (FAILED(hr))
	{
		ENGINE_ERROR("Could not compile dummy shader");
	}

	hr = g_D3dDevice->CreateInputLayout(elementDescriptions, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);

	if (FAILED(hr))
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