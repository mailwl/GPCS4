#include "GCNCompiler.h"
#include "GCNParser/Instruction.h"
#include "GCNParser/SOP1Instruction.h"

namespace pssl
{;

void GCNCompiler::emitScalarALU(GCNInstruction& ins)
{
	Instruction::InstructionClass insClass = ins.instruction->GetInstructionClass();

	switch (insClass)
	{
	case Instruction::ScalarArith:
		emitScalarArith(ins);
		break;
	case Instruction::ScalarAbs:
		emitScalarAbs(ins);
		break;
	case Instruction::ScalarMov:
		emitScalarMov(ins);
		break;
	case Instruction::ScalarCmp:
		emitScalarCmp(ins);
		break;
	case Instruction::ScalarSelect:
		emitScalarSelect(ins);
		break;
	case Instruction::ScalarBitLogic:
		emitScalarBitLogic(ins);
		break;
	case Instruction::ScalarBitManip:
		emitScalarBitManip(ins);
		break;
	case Instruction::ScalarBitField:
		emitScalarBitField(ins);
		break;
	case Instruction::ScalarConv:
		emitScalarConv(ins);
		break;
	case Instruction::ScalarExecMask:
		emitScalarExecMask(ins);
		break;
	case Instruction::ScalarQuadMask:
		emitScalarQuadMask(ins);
		break;
	case Instruction::InstructionClassUnknown:
	case Instruction::InstructionClassCount:
		LOG_FIXME("Instruction class not initialized.");
		break;
	default:
		LOG_ERR("Instruction category is not ScalarALU.");
		break;
	}
}


void GCNCompiler::emitScalarArith(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarAbs(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarMov(GCNInstruction& ins)
{
	const auto i = asInstruction<SISOP1Instruction>(ins);
	const auto op = i->GetOp();

	switch(op) {
		case SISOP1Instruction::S_MOV_B32:
		case SISOP1Instruction::S_MOV_B64: {
			const auto size = (op == SISOP1Instruction::S_MOV_B32) ? GprSize::Size32 : GprSize::Size64;

			const auto sSrc0 = loadSSrc(i->GetSSRC0(), i->GetSRidx(), size);
			const auto sDst = loadSDst(i->GetSDST(), i->GetSDSTRidx(), size);

			m_module.opStore(sDst.varId, sSrc0.varId);
			break;
		}
		default:
			error("unimplemented op: {:#x}", (u32)i->GetOp());
			break;
	}
}

void GCNCompiler::emitScalarCmp(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarSelect(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarBitLogic(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarBitManip(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarBitField(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarConv(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarExecMask(GCNInstruction& ins)
{

}

void GCNCompiler::emitScalarQuadMask(GCNInstruction& ins)
{

}

}