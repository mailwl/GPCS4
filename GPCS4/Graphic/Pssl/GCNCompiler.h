#pragma once

// #include "GPCS4Common.h"
#include "PsslProgramInfo.h"
#include "PsslFetchShader.h"
#include "GCNInstruction.h"

#include "GCNParser/SMRDInstruction.h"
#include "GCNParser/SOPPInstruction.h"
#include "GCNParser/SOPCInstruction.h"
#include "GCNParser/SOP1Instruction.h"
#include "GCNParser/SOPKInstruction.h"
#include "GCNParser/SOP2Instruction.h"
#include "GCNParser/VINTRPInstruction.h"
#include "GCNParser/DSInstruction.h"
#include "GCNParser/MUBUFInstruction.h"
#include "GCNParser/MTBUFInstruction.h"
#include "GCNParser/MIMGInstruction.h"
#include "GCNParser/EXPInstruction.h"
#include "GCNParser/VOPInstruction.h"

// #include "../Gve/GveShader.h"
#include "../SpirV/SpirvModule.h"

#include <optional>
#include <map>

#define ARRAYCOUNT(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

namespace pssl
{

constexpr size_t MAX_COUNT_VGPR = 256;
constexpr size_t MAX_COUNT_SGPR = 103;
constexpr size_t MAX_COUNT_ATTR = 32;


constexpr spv::Id spvIdInvalid = 0; // TODO: Is this right?

struct SpirvValue {
	constexpr SpirvValue() : typeId(spvIdInvalid), varId(spvIdInvalid) {}
	SpirvValue(spv::Id typeId, spv::Id varId) : typeId(typeId), varId(varId) {
		assert(isValid());
	}

	bool isValid() const { return typeId != spvIdInvalid && varId != spvIdInvalid; }

	spv::Id typeId{ spvIdInvalid };
	spv::Id varId{ spvIdInvalid };
};

constexpr SpirvValue spvValueInvalid = SpirvValue();

enum class GprType {
	Scalar,
	Vector
};

enum class GprSize {
	Size32,
	Size64
};

class GCNCompiler
{
public:
	GCNCompiler(const PsslProgramInfo& progInfo);
	GCNCompiler(const PsslProgramInfo& programInfo, const std::vector<VertexInputSemantic>& inputSemantic);
	~GCNCompiler() = default;
	void initVgprs();

	void processInstruction(GCNInstruction& ins);

	SpirvCodeBuffer finalize();

private:
	using GprId = u8;
	using AttrId = u8;
	using ParamId = u8;
	using VertexSemantic = u8;

private:
	SpirvModule m_module;

	// Basic SPIR-V types
	const spv::Id m_typeU32;
	const spv::Id m_typeU64;
	const spv::Id m_typeF32;
	const spv::Id m_typeVec2;
	const spv::Id m_typeVec3;
	const spv::Id m_typeVec4;

	const spv::Id m_typeGpr;
	const spv::Id m_typePtrGpr;

	const spv::Id m_typeVertexBuffer;
	const spv::Id m_typePtrVertexBuffer;

	GCNInstruction* m_currentInstruction{};
	SpirvCodeBuffer m_compiledCode;
	PsslProgramInfo m_programInfo;

	// Shader resources
	std::vector<SpirvValue> m_vertexResources;
	std::vector<SpirvValue> m_textureResources;
	std::vector<SpirvValue> m_samplerResources;
	u32 m_vertexResourceIndex{};
	u32 m_textureResourceIndex{};
	u32 m_samplerResourceIndex{};

	// General purpose registers
	std::map<GprId, SpirvValue> m_vgprs;
	std::map<GprId, SpirvValue> m_sgprs;

	// Status/mask registers
	SpirvValue m_vcc{};
	SpirvValue m_vccLo{}; // TODO: should be part of vcc
	SpirvValue m_vccHi{}; // TODO: should be part of vcc
	SpirvValue m_m0{};
	SpirvValue m_exec{};

	// Entry point description
	std::vector<spv::Id> m_entryPointInterfaces;
	spv::Id m_entryPointId = 0;

	// Vertex input and outputs
	std::map<VertexSemantic, SpirvValue> m_vsInputs;
	std::map<std::string, SpirvValue> m_vsBuiltin;
	SpirvValue m_vsOutputPos{};
	std::map<ParamId, SpirvValue> m_vsOutputParams;
	std::vector<VertexInputSemantic> m_vsInputSemantics;

	// Fragment inputs and outputs
	std::map<AttrId, SpirvValue> m_psInputAttrs;

	bool m_insideFunction{};

private:
	void emitInit();

	void emitVsInit();
	void emitHsInit();
	void emitDsInit();
	void emitGsInit();
	void emitPsInit();
	void emitCsInit();

	// Category handlers
	void emitScalarALU(GCNInstruction& ins);
	void emitScalarMemory(GCNInstruction& ins);
	void emitVectorALU(GCNInstruction& ins);
	void emitVectorMemory(GCNInstruction& ins);
	void emitFlowControl(GCNInstruction& ins);
	void emitDataShare(GCNInstruction& ins);
	void emitVectorInterpolation(GCNInstruction& ins);
	void emitExport(GCNInstruction& ins);
	void emitDebugProfile(GCNInstruction& ins);

	// ScalarALU
	void emitScalarArith(GCNInstruction& ins);
	void emitScalarAbs(GCNInstruction& ins);
	void emitScalarMov(GCNInstruction& ins);
	void emitScalarCmp(GCNInstruction& ins);
	void emitScalarSelect(GCNInstruction& ins);
	void emitScalarBitLogic(GCNInstruction& ins);
	void emitScalarBitManip(GCNInstruction& ins);
	void emitScalarBitField(GCNInstruction& ins);
	void emitScalarConv(GCNInstruction& ins);
	void emitScalarExecMask(GCNInstruction& ins);
	void emitScalarQuadMask(GCNInstruction& ins);

	// VectorALU
	void emitVectorRegMov(GCNInstruction& ins);
	void emitVectorLane(GCNInstruction& ins);
	void emitVectorBitLogic(GCNInstruction& ins);
	void emitVectorBitField32(GCNInstruction& ins);
	void emitVectorThreadMask(GCNInstruction& ins);
	void emitVectorBitField64(GCNInstruction& ins);
	void emitVectorFpArith32(GCNInstruction& ins);
	void emitVectorFpRound32(GCNInstruction& ins);
	void emitVectorFpField32(GCNInstruction& ins);
	void emitVectorFpTran32(GCNInstruction& ins);
	void emitVectorFpCmp32(GCNInstruction& ins);
	void emitVectorFpArith64(GCNInstruction& ins);
	void emitVectorFpRound64(GCNInstruction& ins);
	void emitVectorFpField64(GCNInstruction& ins);
	void emitVectorFpTran64(GCNInstruction& ins);
	void emitVectorFpCmp64(GCNInstruction& ins);
	void emitVectorIntArith32(GCNInstruction& ins);
	void emitVectorIntArith64(GCNInstruction& ins);
	void emitVectorIntCmp32(GCNInstruction& ins);
	void emitVectorIntCmp64(GCNInstruction& ins);
	void emitVectorConv(GCNInstruction& ins);
	void emitVectorFpGraph32(GCNInstruction& ins);
	void emitVectorIntGraph(GCNInstruction& ins);
	void emitVectorMisc(GCNInstruction& ins);

	// FlowControl
	void emitScalarProgFlow(GCNInstruction& ins);
	void emitScalarSync(GCNInstruction& ins);
	void emitScalarWait(GCNInstruction& ins);
	void emitScalarCache(GCNInstruction& ins);
	void emitScalarPrior(GCNInstruction& ins);
	void emitScalarRegAccess(GCNInstruction& ins);
	void emitScalarMsg(GCNInstruction& ins);

	// ScalarMemory
	void emitScalarMemRd(GCNInstruction& ins);
	void emitScalarMemUt(GCNInstruction& ins);

	// VectorMemory
	void emitVectorMemBufNoFmt(GCNInstruction& ins);
	void emitVectorMemBufFmt(GCNInstruction& ins);
	void emitVectorMemImgNoSmp(GCNInstruction& ins);
	void emitVectorMemImgSmp(GCNInstruction& ins);
	void emitVectorMemImgUt(GCNInstruction& ins);
	void emitVectorMemL1Cache(GCNInstruction& ins);

	// DataShare
	void emitDsIdxRd(GCNInstruction& ins);
	void emitDsIdxWr(GCNInstruction& ins);
	void emitDsIdxWrXchg(GCNInstruction& ins);
	void emitDsIdxCondXchg(GCNInstruction& ins);
	void emitDsIdxWrap(GCNInstruction& ins);
	void emitDsAtomicArith32(GCNInstruction& ins);
	void emitDsAtomicArith64(GCNInstruction& ins);
	void emitDsAtomicMinMax32(GCNInstruction& ins);
	void emitDsAtomicMinMax64(GCNInstruction& ins);
	void emitDsAtomicCmpSt32(GCNInstruction& ins);
	void emitDsAtomicCmpSt64(GCNInstruction& ins);
	void emitDsAtomicLogic32(GCNInstruction& ins);
	void emitDsAtomicLogic64(GCNInstruction& ins);
	void emitDsAppendCon(GCNInstruction& ins);
	void emitDsDataShareUt(GCNInstruction& ins);
	void emitDsDataShareMisc(GCNInstruction& ins);
	void emitGdsSync(GCNInstruction& ins);
	void emitGdsOrdCnt(GCNInstruction& ins);

	// VectorInterpolation
	void emitVectorInterpFpCache(GCNInstruction& ins);

	// Export
	void emitExp(GCNInstruction& ins);

	// DebugProfile
	void emitDbgProf(GCNInstruction& ins);

	//////////////
	// Misc stuff
	void emitDclInputArray(
		uint32_t          vertexCount);

	void emitDclInputPerVertex(
		uint32_t          vertexCount,
		const char*             varName);

	uint32_t emitDclClipCullDistanceArray(
		uint32_t          length,
		spv::BuiltIn      builtIn,
		spv::StorageClass storageClass);

	void emitFunctionLabel();
	void emitFunctionBegin(u32 entryPoint, u32 returnType, u32 funcType);
	void emitMainFunctionBegin();
	void emitFunctionEnd();

	void emitVsFinalize();
	void emitPsFinalize();

	SpirvValue createUint32(const std::string& name);
	SpirvValue createUint64(const std::string& name);
	SpirvValue createIntScalar(const std::string& name, u32 width, bool isSigned, spv::StorageClass storageClass = spv::StorageClassFunction, std::optional<u32> initConstValue = std::nullopt);
	SpirvValue createIntVector(const std::string& name, u32 size, u32 width, bool isSigned, spv::StorageClass storageClass = spv::StorageClassFunction);
	SpirvValue createFloatVector(const std::string& name, u32 size, u32 width, spv::StorageClass storageClass = spv::StorageClassFunction);

	SpirvValue loadGprValue(const SpirvValue &valGpr);
	SpirvValue loadVgpr(u32 regIndex);
	SpirvValue loadSgpr(u32 regIndex);
	SpirvValue loadAttr(AttrId attrIndex);
	SpirvValue loadOperand(int sOperand, u32 regIndex, GprType gprType, GprSize size = GprSize::Size32);
	SpirvValue loadSSrc(int sSrc, u32 regIndex, GprSize size = GprSize::Size32);
	SpirvValue loadSDst(int sDst, u32 regIndex, GprSize size = GprSize::Size32);
	SpirvValue loadVSrc(int vSrc, u32 regIndex, GprSize size = GprSize::Size32);
	SpirvValue loadVDst(int vDst, u32 regIndex, GprSize size = GprSize::Size32);

	void storeSgpr(u32 index, spv::Id value);

	// spv::Id floatToUint(spv::Id var);

	SpirvValue loadValue(SpirvValue val);

	SpirvValue asUint32(SpirvValue val);
	spv::Id asUint32(spv::Id var);
	SpirvValue loadAsUint32(SpirvValue val);
	SpirvValue asUint64(SpirvValue val);
	spv::Id asUint64(spv::Id var);
	SpirvValue loadAsUint64(SpirvValue val);
	SpirvValue asFloat(SpirvValue val);
	spv::Id asFloat(spv::Id var);
	SpirvValue loadAsFloat(SpirvValue val);

	// Convenience function to dynamic cast instruction types
	template<typename I>
	typename std::enable_if<std::is_base_of<Instruction, I>::value, const I*>::type
	asInstruction(const GCNInstruction& ins)
	{
		const I* i = dynamic_cast<const I*>(ins.instruction.get());
		assert(i);
		return i;
	}
};



} // namespace pssl
