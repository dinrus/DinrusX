// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/D3DRender/Vulkan/VKBase.hpp>
//#include <spirv_cross.hpp>

class CDrxVKShaderReflection;
class CDrxVKShaderReflectionVariable;
class CDrxVKShaderReflectionConstantBuffer;
class CDrxVKShaderReflectionType;
class CDrxVKBlob;

typedef CDrxVKBlob ID3DBlob;

//Global functions
HRESULT D3DReflect(const void* pShaderBytecode, size_t BytecodeLength, UINT pInterface, void** ppReflector);
HRESULT D3D10CreateBlob(size_t NumBytes, ID3DBlob** ppBuffer);
HRESULT D3DDisassemble(const void* pShader, size_t BytecodeLength, uint32 nFlags, ID3DBlob** ppComments, ID3DBlob** ppDisassembly);
HRESULT D3DCompile(_In_reads_bytes_(SrcDataSize) LPCVOID pSrcData, _In_ SIZE_T SrcDataSize, _In_opt_ LPCSTR pSourceName, CONST D3D_SHADER_MACRO* pDefines,
                   _In_opt_ ID3DInclude* pInclude, _In_opt_ LPCSTR pEntrypoint, _In_ LPCSTR pTarget, _In_ UINT Flags1, _In_ UINT Flags2, _Out_ ID3DBlob** ppCode,
                   _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorMsgs);

class CDrxVKBlob : public NDrxVulkan::CRefCounted
{
public:
	CDrxVKBlob(size_t numBytes);
	virtual ~CDrxVKBlob();

	virtual LPVOID STDMETHODCALLTYPE GetBufferPointer() { return m_pData; }
	virtual SIZE_T STDMETHODCALLTYPE GetBufferSize()    { return m_size; }

private:
	uint8* m_pData = nullptr;
	size_t m_size = 0;
};

class CDrxVKShaderReflectionType // Not ref-counted
{
public:
	CDrxVKShaderReflectionType(CDrxVKShaderReflection* pShaderReflection, uint32_t typeId);
	~CDrxVKShaderReflectionType() {};

	HRESULT STDMETHODCALLTYPE GetDesc(D3D11_SHADER_TYPE_DESC* pDesc);

private:
	D3D11_SHADER_TYPE_DESC m_Desc;
};

class CDrxVKShaderReflectionVariable // Not ref-counted
{
public:
	CDrxVKShaderReflectionVariable(CDrxVKShaderReflection* pShaderReflection, const spirv_cross::Resource& constantBuffer, uint32 memberIndex, bool bInUse);
	~CDrxVKShaderReflectionVariable() {};

	HRESULT STDMETHODCALLTYPE                               GetDesc(D3D11_SHADER_VARIABLE_DESC* pDesc);
	ID3D11ShaderReflectionType* STDMETHODCALLTYPE           GetType();
	ID3D11ShaderReflectionConstantBuffer* STDMETHODCALLTYPE GetBuffer();
	UINT STDMETHODCALLTYPE                                  GetInterfaceSlot(UINT uArrayIndex);

private:
	CDrxVKShaderReflection* const               m_pShaderReflection;
	const spirv_cross::Resource&                m_constantBuffer;
	uint32                                      m_memberIndex;
	char                                        m_name[128];
	bool                                        m_bInUse;
	std::unique_ptr<CDrxVKShaderReflectionType> m_pType;
};

class CDrxVKShaderReflectionConstantBuffer // Not ref-counted
{
public:
	CDrxVKShaderReflectionConstantBuffer(CDrxVKShaderReflection* pShaderReflection, const spirv_cross::Resource& resource);
	~CDrxVKShaderReflectionConstantBuffer() {};

	HRESULT STDMETHODCALLTYPE                         GetDesc(D3D11_SHADER_BUFFER_DESC* pDesc);
	ID3D11ShaderReflectionVariable* STDMETHODCALLTYPE GetVariableByIndex(UINT Index);
	ID3D11ShaderReflectionVariable* STDMETHODCALLTYPE GetVariableByName(LPCSTR Name);

private:
	char                                                         m_name[64];
	CDrxVKShaderReflection* const                                m_pShaderReflection;
	const spirv_cross::Resource&                                 m_resource;
	std::vector<spirv_cross::BufferRange>                        m_usedVariables;
	std::vector<std::unique_ptr<CDrxVKShaderReflectionVariable>> m_variables;
};

class CDrxVKShaderReflection : public NDrxVulkan::CRefCounted
{
	friend class CDrxVKShaderReflectionVariable;
	friend class CDrxVKShaderReflectionType;
	friend class CDrxVKShaderReflectionConstantBuffer;

public:
	CDrxVKShaderReflection(const void* pShaderBytecode, size_t BytecodeLength);
	virtual ~CDrxVKShaderReflection() {}

	HRESULT STDMETHODCALLTYPE                               GetDesc(D3D11_SHADER_DESC* pDesc);
	ID3D11ShaderReflectionConstantBuffer* STDMETHODCALLTYPE GetConstantBufferByIndex(UINT Index);
	ID3D11ShaderReflectionConstantBuffer* STDMETHODCALLTYPE GetConstantBufferByName(LPCSTR Name);
	HRESULT STDMETHODCALLTYPE                               GetResourceBindingDesc(UINT ResourceIndex, D3D11_SHADER_INPUT_BIND_DESC* pDesc);
	HRESULT STDMETHODCALLTYPE                               GetInputParameterDesc(UINT ParameterIndex, D3D11_SIGNATURE_PARAMETER_DESC* pDesc);
	HRESULT STDMETHODCALLTYPE                               GetOutputParameterDesc(UINT ParameterIndex, D3D11_SIGNATURE_PARAMETER_DESC* pDesc);
	HRESULT STDMETHODCALLTYPE                               GetPatchConstantParameterDesc(UINT ParameterIndex, D3D11_SIGNATURE_PARAMETER_DESC* pDesc);
	ID3D11ShaderReflectionVariable* STDMETHODCALLTYPE       GetVariableByName(LPCSTR Name);
	HRESULT STDMETHODCALLTYPE                               GetResourceBindingDescByName(LPCSTR Name, D3D11_SHADER_INPUT_BIND_DESC* pDesc);
	UINT STDMETHODCALLTYPE                                  GetMovInstructionCount();
	UINT STDMETHODCALLTYPE                                  GetMovcInstructionCount();
	UINT STDMETHODCALLTYPE                                  GetConversionInstructionCount();
	UINT STDMETHODCALLTYPE                                  GetBitwiseInstructionCount();
	D3D_PRIMITIVE STDMETHODCALLTYPE                         GetGSInputPrimitive();
	BOOL STDMETHODCALLTYPE                                  IsSampleFrequencyShader();
	UINT STDMETHODCALLTYPE                                  GetNumInterfaceSlots();
	HRESULT STDMETHODCALLTYPE                               GetMinFeatureLevel(D3D_FEATURE_LEVEL* pLevel);

private:
	struct SInputParameter
	{
		char semanticName[64];
		int  semanticIndex;
		int  attributeLocation;
	};

	struct SResourceBinding
	{
		char semanticName[128];
		int  semanticType;
		int  bindPoint;
	};

	std::unique_ptr<spirv_cross::Compiler>                             m_pCompiler;
	spirv_cross::ShaderResources                                       m_shaderResources;
	std::vector<SInputParameter>                                       m_shaderInputs;
	std::vector<SResourceBinding>                                      m_shaderBindings;
	std::vector<std::unique_ptr<CDrxVKShaderReflectionConstantBuffer>> m_constantBuffers;
};

