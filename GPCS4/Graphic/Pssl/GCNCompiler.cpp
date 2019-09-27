#include "GCNCompiler.h"

#include <util/log.h>
#include "rend/starsha/orbital/gca/gcn_analyzer.h"

#include <array>
#include <string>
#include <optional>
#include <utility>

namespace pssl
{

GCNCompiler::GCNCompiler(const PsslProgramInfo& progInfo):
	GCNCompiler(progInfo, {})
{

}

GCNCompiler::GCNCompiler(const PsslProgramInfo& programInfo, const std::vector<VertexInputSemantic>& inputSemantic) :
	m_typeU32(m_module.defIntType(32, false)),
	m_typeU64(m_module.defIntType(64, false)),
	m_typeF32(m_module.defFloatType(32)),
	m_typeVec2(m_module.defVectorType(m_typeF32, 2)),
	m_typeVec3(m_module.defVectorType(m_typeF32, 3)),
	m_typeVec4(m_module.defVectorType(m_typeF32, 4)),
	m_typeGpr(m_typeU32),
	m_typePtrGpr(m_module.defPointerType(m_typeGpr, spv::StorageClassFunction)),
	m_typeVertexBuffer(m_typeU32),
	m_typePtrVertexBuffer(m_module.defPointerType(m_typeVertexBuffer, spv::StorageClassUniform)),
	m_programInfo(programInfo),
	m_vsInputSemantics(inputSemantic)
{
	// Declare an entry point ID. We'll need it during the
	// initialization phase where the execution mode is set.
	m_entryPointId = m_module.allocateId();

	// Set the shader name so that we recognize it in renderdoc
	m_module.setDebugSource(
		spv::SourceLanguageUnknown, 0,
		m_module.addDebugString(programInfo.getKey().toString().c_str()),
		nullptr);

	//// Set the memory model. This is the same for all shaders.
	m_module.setMemoryModel(
		spv::AddressingModelLogical,
		spv::MemoryModelGLSL450);

	// Make sure our interface registers are clear
	//for (uint32_t i = 0; i < DxbcMaxInterfaceRegs; i++) {
	//	m_vRegs.at(i) = DxbcRegisterPointer{ };
	//	m_oRegs.at(i) = DxbcRegisterPointer{ };
	//}

	this->emitInit();
}

void GCNCompiler::emitInit()
{
	// Set up common capabilities for all shaders
	m_module.enableCapability(spv::CapabilityShader);
	m_module.enableCapability(spv::CapabilityImageQuery);

	// Initialize the shader module with capabilities
	// etc. Each shader type has its own peculiarities.
	switch (m_programInfo.getShaderType())
	{
	case VertexShader:   emitVsInit(); break;
	case HullShader:     emitHsInit(); break;
	case DomainShader:   emitDsInit(); break;
	case GeometryShader: emitGsInit(); break;
	case PixelShader:    emitPsInit(); break;
	case ComputeShader:  emitCsInit(); break;
	}

	const auto analyzer = m_programInfo.m_orbitalAnalyzer;

	// Declare resources

	// Vertex buffer resources (V#)
	u32 binding = 0;
	for (u32 i = 0; i < analyzer->res_vh_count; i++) {
		// Create data type
		const spv::Id typeRuntimeUintArray = m_module.defRuntimeArrayType(m_typeU32);
		m_module.decorateArrayStride(typeRuntimeUintArray, 4);

		// Create V# struct type
		const spv::Id member[] = { typeRuntimeUintArray };
		const spv::Id typeStructVertexBufferResource = m_module.defStructType(1, member);
		m_module.setDebugName(typeStructVertexBufferResource, "resource_vh");
		m_module.decorate(typeStructVertexBufferResource, spv::DecorationBufferBlock);
		m_module.setDebugMemberName(typeStructVertexBufferResource, 0, "data");
		m_module.memberDecorateOffset(typeStructVertexBufferResource, 0, 0);

		const spv::Id typeStructVertexBufferResourcePtr = m_module.defPointerType(typeStructVertexBufferResource, spv::StorageClassUniform);
		const auto varVh = m_module.newVar(typeStructVertexBufferResourcePtr, spv::StorageClassUniform);
		const auto vhName = fmt::format("vh{}", i);
		m_module.setDebugName(varVh, vhName.c_str());
		m_module.decorateBinding(varVh, binding);
		m_module.decorate(varVh, spv::DecorationNonWritable);

		m_vertexResources.emplace_back(typeStructVertexBufferResource, varVh);
		++binding;
	}

	// Texture resources (T#)
	for (u32 i = 0; i < analyzer->res_th_count; i++) {
		// TODO: args here are hardcoded
		const spv::Id typeTextureResource = m_module.defImageType(m_typeF32,spv::Dim::Dim2D, false, false, false, 1, spv::ImageFormatUnknown);
		m_module.setDebugName(typeTextureResource, "resource_th");
		const auto varTh = m_module.newVar(typeTextureResource, spv::StorageClassUniformConstant);
		const auto thName = fmt::format("th{}", i);
		m_module.setDebugName(varTh, thName.c_str());
		m_module.decorateBinding(varTh, binding);

		m_textureResources.emplace_back(typeTextureResource, varTh);
		++binding;
	}

	// Sampler resources (S#)
	for (u32 i = 0; i < analyzer->res_sh_count; i++) {
		const auto typeSampler = m_module.defSamplerType();
		const auto typeSamplerPtr = m_module.defPointerType(typeSampler, spv::StorageClassUniformConstant);

		const auto varSh = m_module.newVar(typeSamplerPtr, spv::StorageClassUniformConstant);
		const auto shName = fmt::format("sh{}", i);
		m_module.setDebugName(varSh, shName.c_str());
		m_module.decorateBinding(varSh, binding);

		m_samplerResources.emplace_back(typeSampler, varSh);
		++binding;
	}

	// Declare SGPRs
	for (u32 i = 0; i < MAX_COUNT_SGPR; i++) {
		if (analyzer->used_sgpr[i] > 0) {
			m_sgprs[i] = createUint32(fmt::format("s{}", i));;
		}
	}

	// Declare status/mask registers
	m_vcc = createUint64("vcc");
	m_vccLo = createUint32("vcc_lo");
	m_vccHi = createUint32("vcc_hi");
	m_m0 = createUint32("m0");
	m_exec = createUint64("exec");
}

void GCNCompiler::emitVsInit()
{
	m_module.enableCapability(spv::CapabilityDrawParameters);
	m_module.enableExtension("SPV_KHR_shader_draw_parameters");

	// Declare the vertex attributes (inputs)
	for (const auto &vsInput : m_vsInputSemantics)
	{
		m_vsInputs[vsInput.semantic] = createIntVector(fmt::format("in_attr{}", vsInput.semantic), vsInput.sizeInElements, 32, false, spv::StorageClassInput);
		m_entryPointInterfaces.push_back(m_vsInputs[vsInput.semantic].varId);
	}

	const auto analyzer = m_programInfo.m_orbitalAnalyzer;

	// Declare the vertex outputs
	if (analyzer->used_exp_pos[0]) {
		m_vsOutputPos = createFloatVector("out_position", 4, 32, spv::StorageClassOutput);
		m_entryPointInterfaces.push_back(m_vsOutputPos.varId);
	}

	for (u32 i = 0; i < ARRAYCOUNT(analyzer->used_exp_param); i++) {
		if (analyzer->used_exp_param[i] > 0) {
			m_vsOutputParams[i] = createIntVector(fmt::format("out_param{}", i), 4, 32, false, spv::StorageClassOutput);
			m_entryPointInterfaces.push_back(m_vsOutputParams[i].varId);
		}
	}

	this->emitMainFunctionBegin();
	// We're emitting code in the main function now

	// Create built-ins
	m_vsBuiltin["gl_VertexID"] = createIntScalar("gl_VertexID", 32, true, spv::StorageClassInput);
	m_module.decorateBuiltIn(m_vsBuiltin["gl_VertexID"].varId, spv::BuiltInVertexIndex);

	// Determine initial VGPR values based on fetch shader info

	std::array<std::optional<spv::Id>, MAX_COUNT_SGPR> vgprInitValues{};

	for (const auto& inputSemantic : m_vsInputSemantics) {
		for (u32 i = 0; i < inputSemantic.sizeInElements; i++) {
			const u32 vgprIndex = inputSemantic.vgpr + i;
			const spv::Id accessIndexArray[] = { m_module.constu32(i) };

			const auto inputVarId = m_vsInputs[inputSemantic.semantic].varId;

			const auto& inputElementAccess =
				m_module.opAccessChain(
					m_typePtrGpr,
					inputVarId,
					1, accessIndexArray);

			vgprInitValues[vgprIndex] = m_module.opLoad(m_typeGpr, inputElementAccess);
		}
	}

	// Declare & initialize VGPRs
	// TODO: wrap in a vs_fetch func
	for (u32 i = 0; i < MAX_COUNT_VGPR; i++) {
		if (analyzer->used_vgpr[i] > 0) {
			m_vgprs[i] = createUint32(fmt::format("v{}", i));

			const auto initValue = vgprInitValues[i];

			if (initValue.has_value())
				m_module.opStore(m_vgprs[i].varId, initValue.value());
		}
	}

	// v0 = vertexIndex
	m_module.opStore(m_vgprs[0].varId, asUint32(m_vsBuiltin["gl_VertexID"].varId));
}

void GCNCompiler::emitHsInit()
{

}

void GCNCompiler::emitDsInit()
{

}

void GCNCompiler::emitGsInit()
{

}

void GCNCompiler::emitPsInit()
{
	const auto analyzer = m_programInfo.m_orbitalAnalyzer;

	for (u32 i = 0; i < MAX_COUNT_ATTR; i++) {
		if (analyzer->used_attr[i] > 0) {
			// TODO: Remove hardcoded size (3)
			m_psInputAttrs[i] = createFloatVector(fmt::format("in_attr{}", i), 3, 32, spv::StorageClassInput);
		}
	}

	this->emitMainFunctionBegin();
	// We're emitting code in the main function now

	// Declare VGPRs
	for (u32 i = 0; i < MAX_COUNT_VGPR; i++) {
		if (analyzer->used_vgpr[i] > 0) {
			m_vgprs[i] = createUint32(fmt::format("v{}", i));
		}
	}
}

void GCNCompiler::emitCsInit()
{

}

SpirvValue GCNCompiler::createUint32(const std::string& name)
{
	return createIntScalar(name, 32, false);
}

SpirvValue GCNCompiler::createUint64(const std::string& name)
{
	return createIntScalar(name, 64, false);
}

SpirvValue GCNCompiler::createIntScalar(const std::string& name, u32 width, bool isSigned, spv::StorageClass storageClass, std::optional<u32> initConstValue)
{
	const auto typeId = m_module.defPointerType(m_module.defIntType(width, isSigned), storageClass);
	spv::Id var;

	if (initConstValue.has_value())
		var = m_module.newVarInit(typeId, storageClass, initConstValue.value());
	else
		var = m_module.newVar(typeId, storageClass);

	if (!name.empty())
		m_module.setDebugName(var, name.c_str());
	return { typeId, var };
}

SpirvValue GCNCompiler::createFloatVector(const std::string& name, u32 size, u32 width, spv::StorageClass storageClass)
{
	const uint32_t typeFloat = m_module.defFloatType(width);
	const uint32_t typeFloatVector = m_module.defVectorType(typeFloat, size);

	const auto typeId = m_module.defPointerType(typeFloatVector, storageClass);
	const auto varId = m_module.newVar(typeId, storageClass);

	if (!name.empty())
		m_module.setDebugName(varId, name.c_str());
	return SpirvValue(typeId, varId);
}

SpirvValue GCNCompiler::loadGprValue(const SpirvValue& valGpr)
{
	assert(valGpr.isValid());

	const auto typeId = m_typeGpr;
	const auto varId = m_module.opLoad(typeId, valGpr.varId);

	assert(varId);

	return SpirvValue(typeId, varId);
}

SpirvValue GCNCompiler::loadVgpr(u32 regIndex)
{
	return loadGprValue(m_vgprs[regIndex]);
}
SpirvValue GCNCompiler::loadSgpr(u32 regIndex)
{
	return loadGprValue(m_sgprs[regIndex]);
}

SpirvValue GCNCompiler::loadAttr(AttrId attrIndex)
{
	const auto attr = m_psInputAttrs[attrIndex];
	const auto typeId = m_typeVec3;
	const auto varId = m_module.opLoad(typeId, attr.varId);

	assert(attr.isValid());
	assert(varId != spvIdInvalid);

	return SpirvValue(typeId, varId);
}

SpirvValue GCNCompiler::createIntVector(const std::string& name, u32 size, u32 width, bool isSigned, spv::StorageClass storageClass)
{
	uint32_t typeInt = m_module.defIntType(width, isSigned);
	uint32_t typeFloatVector = m_module.defVectorType(typeInt, size);

	const auto typeId = m_module.defPointerType(typeFloatVector, storageClass);
	const auto varId = m_module.newVar(typeId, storageClass);

	if (!name.empty())
		m_module.setDebugName(varId, name.c_str());
	return { typeId, varId };
}

SpirvValue GCNCompiler::loadOperand(int sOperand, u32 regIndex, GprSize size)
{
	SpirvValue operand;

	// HACK: use VOPInstruction::SRC enums arbitrarily, they all have the same value anyway

	const auto sOperandTyped = static_cast<VOPInstruction::SRC>(sOperand);
	switch (sOperandTyped) {
	case VOPInstruction::SRCVccLo:
		if (size == GprSize::Size32)
			operand = loadValue(m_vccLo); // TODO: see comment in declaration
		else if (size == GprSize::Size64)
			operand = loadValue(m_vcc);
		break;
	case VOPInstruction::SRCVccHi:
		operand = loadValue(m_vccHi);
		break;
	case VOPInstruction::SRCLiteralConst:
		assert(m_currentInstruction->hasLiteral);
		operand ={
			m_typeU32,
			m_module.constu32(m_currentInstruction->literalConst) };
		break;
	case VOPInstruction::SRCConstZero:
		operand = SpirvValue(m_typeGpr, m_module.constu32(0));
		break;
	case VOPInstruction::SRCScalarGPRMin ... VOPInstruction::SRCScalarGPRMax: {
		u32 index;

		if (regIndex == 0xFFFFFFFF)
			index = (u32)sOperandTyped - (u32)VOPInstruction::SRCScalarGPRMin;
		else
			index = regIndex;

		operand = loadSgpr(index);
		break;
	}
	case VOPInstruction::SRCVectorGPRMin ... VOPInstruction::SRCVectorGPRMax: {
		u32 index;

		if (regIndex == 0xFFFFFFFF)
			index = (u32)sOperandTyped - (u32)VOPInstruction::SRCVectorGPRMin;
		else
			index = regIndex;

		operand = loadVgpr(index);
		break;
	}
	case VOPInstruction::SRCM0:
		operand = loadValue(m_m0);
		break;
	case VOPInstruction::SRCExecLo:
		assert(size == GprSize::Size64);
		operand = loadValue(m_exec);
		break;
	default:
		warn("sOperand: {}", sOperand);
		operand = spvValueInvalid;
	}

	assert(operand.isValid());
	return operand;
}

SpirvValue GCNCompiler::loadSSrc(int sSrc, u32 regIndex, GprSize size)
{
	return loadOperand(sSrc, regIndex, size);
}

SpirvValue GCNCompiler::loadSDst(int sDst, u32 regIndex, GprSize size)
{
	return loadOperand(sDst, regIndex, size);
}

SpirvValue GCNCompiler::loadVSrc(int vSrc, u32 regIndex, GprSize size)
{
	return loadOperand(vSrc, regIndex, size);
}

SpirvValue GCNCompiler::loadVDst(int vDst, u32 regIndex, GprSize size)
{
	return loadOperand(vDst, regIndex, size);
}

// TODO: store helper for VGPR

void GCNCompiler::storeSgpr(u32 index, spv::Id value)
{
	assert(value);

	// TODO: handle other types (need to bitcast)

	const spv::Id varSgpr = m_sgprs[index].varId;
	assert(varSgpr);

	m_module.opStore(m_sgprs[index].varId, value);
}

void GCNCompiler::processInstruction(GCNInstruction& ins)
{
	m_currentInstruction = &ins;

	Instruction::InstructionCategory insCategory = ins.instruction->GetInstructionCategory();
	switch (insCategory)
	{
	case Instruction::ScalarALU:
		emitScalarALU(ins);
		break;
	case Instruction::ScalarMemory:
		emitScalarMemory(ins);
		break;
	case Instruction::VectorALU:
		emitVectorALU(ins);
		break;
	case Instruction::VectorMemory:
		emitVectorMemory(ins);
		break;
	case Instruction::FlowControl:
		emitFlowControl(ins);
		break;
	case Instruction::DataShare:
		emitDataShare(ins);
		break;
	case Instruction::VectorInterpolation:
		emitVectorInterpolation(ins);
		break;
	case Instruction::Export:
		emitExport(ins);
		break;
	case Instruction::DebugProfile:
		emitDebugProfile(ins);
		break;
	case Instruction::CategoryUnknown:
	case Instruction::InstructionsCategoriesCount:
		LOG_FIXME("Instruction category not initialized. Encoding %d", ins.instruction->GetInstructionFormat());
		break;
	default:
		break;
	}
}

void GCNCompiler::emitFunctionLabel()
{
	m_module.opLabel(m_module.allocateId());
}

void GCNCompiler::emitFunctionBegin(u32 entryPoint, u32 returnType, u32 funcType)
{
	emitFunctionEnd();

	m_module.functionBegin(
		returnType, entryPoint, funcType,
		spv::FunctionControlMaskNone);

	m_insideFunction = true;
}

void GCNCompiler::emitMainFunctionBegin()
{
	emitFunctionBegin(m_entryPointId, m_module.defVoidType(), m_module.defFunctionType( m_module.defVoidType(), 0, nullptr));
	emitFunctionLabel();
}

void GCNCompiler::emitFunctionEnd() {
	if (m_insideFunction) {
		m_module.opReturn();
		m_module.functionEnd();
	}

	m_insideFunction = false;
}

void GCNCompiler::emitVsFinalize()
{
	emitFunctionEnd();
}

void GCNCompiler::emitPsFinalize()
{
	emitFunctionEnd();
}

SpirvCodeBuffer GCNCompiler::finalize()
{
	// Depending on the shader type, this will prepare
	// input registers, call various shader functions
	// and write back the output registers.
	spv::ExecutionModel executionModel = spv::ExecutionModelMax;
	switch (m_programInfo.getShaderType()) {
	case VertexShader:
		emitVsFinalize();
		executionModel = spv::ExecutionModelVertex;
		break;
	// case HullShader:     emitHsFinalize(); break;
	// case DomainShader:   emitDsFinalize(); break;
	// case GeometryShader: emitGsFinalize(); break;
	case PixelShader:
		emitPsFinalize();
		executionModel = spv::ExecutionModelFragment;
		break;
	// case ComputeShader:  emitCsFinalize(); break;
	default: assert(0);
	}

	// Declare the entry point, we now have all the
	// information we need, including the interfaces
	m_module.addEntryPoint(m_entryPointId,
		executionModel, "main",
		m_entryPointInterfaces.size(),
		m_entryPointInterfaces.data());
	m_module.setDebugName(m_entryPointId, "main");

	m_compiledCode = std::move(m_module.compile());

	return std::move(m_compiledCode);
}

// spv::Id GCNCompiler::floatToUint(spv::Id var)
// {
// 	assert(var);
// 	return m_module.opConvertFtoU(m_typeU32, var);
// }

SpirvValue GCNCompiler::loadValue(const SpirvValue val) {
	const auto varLoaded = m_module.opLoad(val.typeId, val.varId);
	return SpirvValue(val.typeId, varLoaded);
}

SpirvValue GCNCompiler::asUint32(SpirvValue val)
{
	return SpirvValue(m_typeU32, m_module.opBitcast(m_typeU32, val.varId));
}

spv::Id GCNCompiler::asUint32(spv::Id var)
{
	return asUint32(SpirvValue(m_typeU32, var)).varId;
}

SpirvValue GCNCompiler::loadAsUint32(SpirvValue val)
{
	return loadValue(asUint32(val));
}

SpirvValue GCNCompiler::asUint64(SpirvValue val)
{
	return SpirvValue(m_typeU64, m_module.opBitcast(m_typeU64, val.varId));
}

spv::Id GCNCompiler::asUint64(spv::Id var)
{
	return asUint64(SpirvValue(m_typeU64, var)).varId;
}

SpirvValue GCNCompiler::loadAsUint64(SpirvValue val)
{
	return loadValue(asUint64(val));
}

SpirvValue GCNCompiler::asFloat(SpirvValue val)
{
	return SpirvValue(m_typeF32, m_module.opBitcast(m_typeF32, val.varId));
}

spv::Id GCNCompiler::asFloat(spv::Id var)
{
	return asFloat(SpirvValue(m_typeF32, var)).varId;
}

SpirvValue GCNCompiler::loadAsFloat(SpirvValue val)
{
	return loadValue(asFloat(val));
}

} // namespace pssl
