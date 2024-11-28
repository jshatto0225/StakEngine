#pragma once

#include "Types.h"

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHI.h"

struct FRHICommand {
  FRHICommand *Next = nullptr;

  virtual void Execute(FRHICommandList &CommandList) = 0;
};

struct FRHICommandNOP : public FRHICommand {
  void Execute(FRHICommandList &CommandList) override {}
};

struct FRHICommandBeginDrawing : public FRHICommand {
  FRHICommandBeginDrawing()
  {}

  void Execute(FRHICommandList &CommandList) override;
};

struct FRHICommandEndDrawing : public FRHICommand {
  FRHICommandEndDrawing()
  {}

  void Execute(FRHICommandList &CommandList) override;
};

struct FRHICommandResourceTransition : public FRHICommand {
  IRHITransition *Transition;

  FRHICommandResourceTransition(IRHITransition *Transition)
    : Transition(Transition) 
  {}

  void Execute(FRHICommandList &CommandList) override;
};

struct FRHICommandRenderImGuiDrawData : public FRHICommand {
  ImDrawData *Data;

  FRHICommandRenderImGuiDrawData(ImDrawData *Data)
    : Data(Data)
  {}

  void Execute(FRHICommandList &CommandList) override;
};

class FRHICommandList {
public:
  inline FRHICommandList() {
    mGraphicsContext = FRHI::Get().GetCommandContext();
  }

  inline void BeginDrawing() {
    AddCommand(new FRHICommandBeginDrawing());
  }

  inline void EndDrawing() {
    AddCommand(new FRHICommandEndDrawing());
  }

  inline void ResourceTransition(IRHITransition *Transition) {
    AddCommand(new FRHICommandResourceTransition(Transition));
  }

  inline void RenderImGuiDrawData(ImDrawData *DrawData) {
    AddCommand(new FRHICommandRenderImGuiDrawData(DrawData));
  }

  inline IRHICommandContext &GetContext() {
    return *mGraphicsContext;
  }

private:
  inline void AddCommand(FRHICommand *Command) {
    mEnd->Next = Command;
    mEnd = mEnd->Next;
  }

  inline void Execute() {
    FRHICommand *Curr = mBase->Next;
    FRHICommand *Prev = Curr;

    while (Curr != nullptr) {
      Curr->Execute(*this);
      Prev = Curr;
      Curr = Curr->Next;
      
      delete Prev;
    }

    mEnd = mBase;
  }

private:
  FRHICommand *mBase = new FRHICommandNOP();
  FRHICommand *mEnd = mBase;
  IRHICommandContext *mGraphicsContext = nullptr;

private:
  friend class FRHI;
};

// Command Executes

inline void FRHICommandBeginDrawing::Execute(FRHICommandList &CommandList) {
  CommandList.GetContext().BeginDrawing();
}

inline void FRHICommandEndDrawing::Execute(FRHICommandList &CommandList) {
  CommandList.GetContext().EndDrawing();
}

inline void FRHICommandResourceTransition::Execute(FRHICommandList &CommandList) {
  CommandList.GetContext().ResourceTransition(Transition);
}

inline void FRHICommandRenderImGuiDrawData::Execute(FRHICommandList &CommandList) {
  CommandList.GetContext().RenderImGuiDrawData(Data);
}