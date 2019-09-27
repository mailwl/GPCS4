#pragma once

#include "base.h"
// #include "GPCS4Common.h"
#include "PsslShaderStructure.h"
#include "PsslEnums.h"
#include "PsslKey.h"

struct gcn_analyzer_t;

namespace pssl
{;

struct GCNInstruction;

class PsslProgramInfo
{
public:
	PsslProgramInfo(const uint8_t* code);
	~PsslProgramInfo();

	uint32_t getCodeSizeBytes() const;

	uint32_t getCodeSizeDwords() const;

	bool hasFetchShader();

	PsslProgramType getShaderType() const;

	PsslKey getKey() const;

	uint32_t getInputUsageSlotCount() const;

	const InputUsageSlot* getInputUsageSlot(uint32_t idx) const;

	std::vector<InputUsageSlot> getInputUsageSlots() const;

	void analyzeInstruction(const GCNInstruction& instruction);

	gcn_analyzer_t* m_orbitalAnalyzer;

private:
	bool initBinaryInfo(const uint8_t* code);

private:
	ShaderBinaryInfo m_shaderBinaryInfo;
	std::vector<InputUsageSlot> m_inputUsageSlots;

	const uint32_t* m_code;
};

// return UINT_MAX means no fetch shader
uint32_t getFetchShaderStartRegister(const PsslProgramInfo& progInfo);

uint32_t getFetchShaderStartRegister(const uint8_t* code);
}
