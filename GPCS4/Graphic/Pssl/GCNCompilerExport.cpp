#include "GCNCompiler.h"

namespace pssl
{;


void GCNCompiler::emitExport(GCNInstruction& ins)
{
	emitExp(ins);
}

void GCNCompiler::emitExp(GCNInstruction& ins)
{
	const auto i = asInstruction<EXPInstruction>(ins);

	// assert(!i->GetCOMPR()); // TODO: f16 export

	switch(m_programInfo.getShaderType()) {
	// case PixelShader: break;
	case VertexShader: {
		std::vector<spv::Id> valueArray;
		for (u32 vSrcIndex = 0; vSrcIndex < 4; ++vSrcIndex) {
			const auto vSrc = i->GetVSRC(vSrcIndex);
			const auto vgprF32 = asFloat(loadVgpr(vSrc).varId);
			valueArray.push_back(vgprF32);
		}
		const auto varSrc = m_module.opCompositeConstruct(m_typeVec4, 4, valueArray.data());

		spv::Id varDst{ spvIdInvalid };
		const auto tgt = i->GetTGT();

		if (tgt == EXPInstruction::TGTExpPosMin)
			varDst = m_vsOutputPos.varId;
		else if (EXPInstruction::TGTExpParamMin <= tgt && tgt <= EXPInstruction::TGTExpParamMax) {
			const auto paramIndex = (u8)tgt - (u8)EXPInstruction::TGTExpParamMin;
			varDst = m_vsOutputParams[paramIndex].varId;
		}

		m_module.opStore(varDst, varSrc);
		break;
	}
	case PixelShader: {
		// TODO:
		warn("PS export unimplemented");
		break;
	}
	default:
		error("unimplemented shader type");
		assert(0);
	}
}

}  // namespace pssl