#include "RHIPipeline.h"

FRHIPipelineLayout::FRHIPipelineLayout(TRef<IRHIPipelineLayout> Layout) {
    Impl = Layout;
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