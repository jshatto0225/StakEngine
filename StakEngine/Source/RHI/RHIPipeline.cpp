#include "RHIPipeline.h"

FRHIPipelineLayout::FRHIPipelineLayout(TRef<IRHIPipelineLayout> Layout) {
	Impl = Layout;
}

TRef<IRHIPipelineLayout> FRHIPipelineLayout::GetImpl() {
	return Impl;
}

void FRHIPipelineLayout::Shutdown() {
	Impl->Shutdown();
}

FRHIPipeline::FRHIPipeline(TRef<IRHIPipeline> Pipeline) {
	Impl = Pipeline;
}

void FRHIPipeline::Shutdown() {
	Impl->Shutdown();
}