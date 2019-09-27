#include "GCNCompiler.h"

namespace pssl
{;


void GCNCompiler::emitVectorInterpolation(GCNInstruction& ins)
{
	emitVectorInterpFpCache(ins);
}

void GCNCompiler::emitVectorInterpFpCache(GCNInstruction& ins)
{
	const auto i = asInstruction<SIVINTRPInstruction>(ins);

	switch(i->GetOp()) {

	case SIVINTRPInstruction::V_INTERP_P1_F32:
		// Interpolation is performed implicitly on the host GPU
		break;
	case SIVINTRPInstruction::V_INTERP_P2_F32: {
		// Just do the move here

		const auto channel = static_cast<u32>(i->GetATTRCHAN());
		assert(channel < 4);

		const auto varAttr = loadAttr(i->GetATTR());

		const u32 indexArray[] = { channel };
		const spv::Id varAttrElem = m_module.opCompositeExtract(m_typeF32, varAttr.varId, 1, indexArray);

		const auto vDst = loadVgpr(i->GetVDST());

		m_module.opStore(vDst.varId, asUint32(varAttrElem));

		break;
	}
	// case SIVINTRPInstruction::V_INTERP_MOV_F32: break;
	// case SIVINTRPInstruction::V_INTERP_RESERVED: break;
	default:
		error("unimplemented op: {:#x}", (u32)i->GetOp());
		break;
	}
}


}
