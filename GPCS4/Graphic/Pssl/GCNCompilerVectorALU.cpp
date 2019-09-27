#include "GCNCompiler.h"


namespace pssl
{;

void GCNCompiler::emitVectorALU(GCNInstruction& ins)
{
	Instruction::InstructionClass insClass = ins.instruction->GetInstructionClass();

	switch (insClass)
	{
	case Instruction::VectorRegMov:
		emitVectorRegMov(ins);
		break;
	case Instruction::VectorLane:
		emitVectorLane(ins);
		break;
	case Instruction::VectorBitLogic:
		emitVectorBitLogic(ins);
		break;
	case Instruction::VectorBitField32:
		emitVectorBitField32(ins);
		break;
	case Instruction::VectorThreadMask:
		emitVectorThreadMask(ins);
		break;
	case Instruction::VectorBitField64:
		emitVectorBitField64(ins);
		break;
	case Instruction::VectorFpArith32:
		emitVectorFpArith32(ins);
		break;
	case Instruction::VectorFpRound32:
		emitVectorFpRound32(ins);
		break;
	case Instruction::VectorFpField32:
		emitVectorFpField32(ins);
		break;
	case Instruction::VectorFpTran32:
		emitVectorFpTran32(ins);
		break;
	case Instruction::VectorFpCmp32:
		emitVectorFpCmp32(ins);
		break;
	case Instruction::VectorFpArith64:
		emitVectorFpArith64(ins);
		break;
	case Instruction::VectorFpRound64:
		emitVectorFpRound64(ins);
		break;
	case Instruction::VectorFpField64:
		emitVectorFpField64(ins);
		break;
	case Instruction::VectorFpTran64:
		emitVectorFpTran64(ins);
		break;
	case Instruction::VectorFpCmp64:
		emitVectorFpCmp64(ins);
		break;
	case Instruction::VectorIntArith32:
		emitVectorIntArith32(ins);
		break;
	case Instruction::VectorIntArith64:
		emitVectorIntArith64(ins);
		break;
	case Instruction::VectorIntCmp32:
		emitVectorIntCmp32(ins);
		break;
	case Instruction::VectorIntCmp64:
		emitVectorIntCmp64(ins);
		break;
	case Instruction::VectorConv:
		emitVectorConv(ins);
		break;
	case Instruction::VectorFpGraph32:
		emitVectorFpGraph32(ins);
		break;
	case Instruction::VectorIntGraph:
		emitVectorIntGraph(ins);
		break;
	case Instruction::VectorMisc:
		emitVectorMisc(ins);
		break;
	case Instruction::InstructionClassUnknown:
	case Instruction::InstructionClassCount:
		LOG_FIXME("Instruction class not initialized.");
		break;
	default:
		LOG_ERR("Instruction category is not VectorALU.");
		break;
	}
}


void GCNCompiler::emitVectorRegMov(GCNInstruction& ins)
{
	const auto i = asInstruction<SIVOP1Instruction>(ins);

	switch (i->GetOp())
	{
	case SIVOP1Instruction::V_MOV_B32:
	{
		const auto src0 = loadSSrc(i->GetSRC0(), i->GetSRidx0());
		const auto vDst = loadVgpr(i->GetVDSTRidx());

		m_module.opStore(vDst.varId, src0.varId);
		break;
	}
	default:
		unimplemented();
		break;
	}
}

void GCNCompiler::emitVectorLane(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorBitLogic(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorBitField32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorThreadMask(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorBitField64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpArith32(GCNInstruction& ins)
{
	const auto i = asInstruction<VOPInstruction>(ins);

	SpirvValue src0;
	SpirvValue src1;
	SpirvValue dst;

	s32 op{};

	switch(ins.instruction->GetInstructionFormat()) {
	// case VOPInstruction::Encoding_VOP1: break;
	case Instruction::InstructionSet_VOP2: {
		const auto iVop2 = asInstruction<SIVOP2Instruction>(ins);

		src0 = loadSSrc(iVop2->GetSRC0(), iVop2->GetSRidx0());
		src1 = loadVgpr(iVop2->GetVSRC1Ridx());
		dst = loadVgpr(iVop2->GetVDSTRidx());

		op = iVop2->GetOp();

		break;
	}
	case Instruction::InstructionSet_VOP3: {
		const auto iVop3 = asInstruction<SIVOP3Instruction>(ins);

		src0 = loadSSrc(iVop3->GetSRC0(), 0xFFFFFFFF);
		src1 = loadSSrc(iVop3->GetSRC1(), 0xFFFFFFFF);
		dst = loadVgpr(iVop3->GetVDST());

		op = iVop3->GetOp();

		break;
	}
	// case VOPInstruction::Encoding_VOPC: break;
	// case VOPInstruction::Encoding_VOP3P: break;
	// case VOPInstruction::Encoding_Illegal: break;
	default:
		error("unimplemented instruction format");
		return;
	}

	const auto src0F32 = asFloat(src0.varId);
	const auto vSrc1F32 = asFloat(src1.varId);
	const auto dstF32 = asFloat(dst.varId);

	switch(op) {
	case SIVOP2Instruction::V_MUL_F32:
	case SIVOP3Instruction::V3_MUL_F32: {
		const auto result = asUint32(m_module.opFMul(m_typeF32, src0F32, vSrc1F32));

		m_module.opStore(dst.varId, result);
		break;
	}
	case SIVOP2Instruction::V_MAC_F32:
	case SIVOP3Instruction::V3_MAC_F32: {
		const auto result = asUint32(
			m_module.opFAdd(m_typeF32,
				m_module.opFMul(m_typeF32, src0F32, vSrc1F32),
				dstF32)
		);

		m_module.opStore(dst.varId, result);
		break;
	}
	default:
		error("unimplemented op: {:#x}", op);
		break;
	}
}

void GCNCompiler::emitVectorFpRound32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpField32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpTran32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpCmp32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpArith64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpRound64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpField64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpTran64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpCmp64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorIntArith32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorIntArith64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorIntCmp32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorIntCmp64(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorConv(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorFpGraph32(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorIntGraph(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorMisc(GCNInstruction& ins)
{

}

}  // namespace pssl