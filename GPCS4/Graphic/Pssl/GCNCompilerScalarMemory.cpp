#include "GCNCompiler.h"

#include <util/log.h>

namespace pssl
{;

void GCNCompiler::emitScalarMemory(GCNInstruction& ins)
{
	Instruction::InstructionClass insClass = ins.instruction->GetInstructionClass();

	switch (insClass)
	{
	case Instruction::ScalarMemRd:
		emitScalarMemRd(ins);
		break;
	case Instruction::ScalarMemUt:
		emitScalarMemUt(ins);
		break;
	case Instruction::InstructionClassUnknown:
	case Instruction::InstructionClassCount:
		warn("Instruction class not initialized.");
		break;
	default:
		error("Instruction category is not ScalarMemory.");
		break;
	}
}

void GCNCompiler::emitScalarMemRd(GCNInstruction& ins)
{
	const auto i = asInstruction<SISMRDInstruction>(ins);

	switch (i->GetOp())
	{
	case SISMRDInstruction::S_BUFFER_LOAD_DWORD:
	case SISMRDInstruction::S_BUFFER_LOAD_DWORDX2:
	case SISMRDInstruction::S_BUFFER_LOAD_DWORDX4:
	case SISMRDInstruction::S_BUFFER_LOAD_DWORDX8:
	case SISMRDInstruction::S_BUFFER_LOAD_DWORDX16:
	{
		const u32 lanes = 2 << (i->GetOp() - 0xA + 1);

		assert(i->GetImm()); // TODO: support dynamic offset

		const auto offset = i->GetOffset();
		const auto vertexBuffer = m_vertexResources[m_vertexResourceIndex++];

		for (u32 lane = 0; lane < lanes; ++lane) {
			const spv::Id constOffset = m_module.constu32(offset + lane);

			const spv::Id indexArray[] = {	m_module.constu32(0), constOffset };
			const spv::Id dstPtr = m_module.opAccessChain(m_typeVertexBuffer, vertexBuffer.varId, 2, indexArray);
			const spv::Id dst = m_module.opLoad(m_typeGpr, dstPtr);

			storeSgpr(static_cast<u32>(i->GetSRidx() + lane), dst);
		}
		break;
	}
	default:
		error("unimplemented op: {:#x}", (u32)i->GetOp());
		break;
	}

}

void GCNCompiler::emitScalarMemUt(GCNInstruction& ins)
{

}

}  // namespace pssl