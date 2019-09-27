#include "GCNCompiler.h"

namespace pssl
{;

void GCNCompiler::emitVectorMemory(GCNInstruction& ins)
{
	Instruction::InstructionClass insClass = ins.instruction->GetInstructionClass();

	switch (insClass)
	{
	case Instruction::VectorMemBufNoFmt:
		emitVectorMemBufNoFmt(ins);
		break;
	case Instruction::VectorMemBufFmt:
		emitVectorMemBufFmt(ins);
		break;
	case Instruction::VectorMemImgNoSmp:
		emitVectorMemImgNoSmp(ins);
		break;
	case Instruction::VectorMemImgSmp:
		emitVectorMemImgSmp(ins);
		break;
	case Instruction::VectorMemImgUt:
		emitVectorMemImgUt(ins);
		break;
	case Instruction::VectorMemL1Cache:
		emitVectorMemL1Cache(ins);
		break;
	case Instruction::InstructionClassUnknown:
	case Instruction::InstructionClassCount:
		LOG_FIXME("Instruction class not initialized.");
		break;
	default:
		LOG_ERR("Instruction category is not VectorMemory.");
		break;
	}
}


void GCNCompiler::emitVectorMemBufNoFmt(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorMemBufFmt(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorMemImgNoSmp(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorMemImgSmp(GCNInstruction& ins)
{
	const auto i = asInstruction<SIMIMGInstruction>(ins);

	switch(i->GetOp()) {
	case SIMIMGInstruction::IMAGE_SAMPLE_L:
	{
		const auto vaddr = i->GetVADDR();

		const auto varCx = asFloat(loadVgpr(vaddr)).varId;
		const auto varCy = asFloat(loadVgpr(vaddr + 1)).varId;
		const spv::Id valueArray[] = { varCx, varCy };
		const auto coords = m_module.opCompositeConstruct(m_typeVec2, 2, valueArray);

		const auto textureResource = m_textureResources[m_textureResourceIndex++];
		const auto res_th = loadValue(textureResource);

		const auto samplerResource = m_samplerResources[m_samplerResourceIndex++];
		const auto res_sh = loadValue(samplerResource);

		// Create sampled image
		const auto typeSampledImage = m_module.defSampledImageType(res_th.typeId);
		const auto varSampler = m_module.opSampledImage(typeSampledImage, res_th.varId, res_sh.varId);

		// Sample
		const auto sampledVec = m_module.opImageSampleImplicitLod(m_typeVec4, varSampler, coords, SpirvImageOperands());

		const auto vdata = i->GetVDATA();
		const auto dmask = i->GetDMASK();

		// Store results
		for (u8 index = 0; index < 4; index++)
		{
			if ((dmask >> index) & 1)
			{
				const u32 indexArray[] = { index };
				const auto elem = m_module.opCompositeExtract(m_typeF32, sampledVec, 1, indexArray);

				m_module.opStore(m_vgprs[vdata + index].varId, asUint32(elem));
			}
		}

		break;
	}
	default:
		error("unimplemented op: {:#x}", (u32)i->GetOp());
		break;
	}
}

void GCNCompiler::emitVectorMemImgUt(GCNInstruction& ins)
{

}

void GCNCompiler::emitVectorMemL1Cache(GCNInstruction& ins)
{

}

}  // namespace pssl