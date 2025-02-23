#pragma once

#include "Types.h"

#include <imgui.h>

#include "RHICommandContext.h"
#include "RHI.h"

struct Rhi_Command {
    Rhi_Command *next = nullptr;

    virtual void execute(Rhi_Command_List &command_list) = 0;
};

struct Rhi_Command_Nop : public Rhi_Command {
    void execute(Rhi_Command_List &command_list) override {}
};

struct Rhi_Command_Begin_Drawing : public Rhi_Command {
    Rhi_Command_Begin_Drawing() {}

    void execute(Rhi_Command_List & command_list) override;
};

struct Rhi_Command_End_DRawing : public Rhi_Command {
    Rhi_Command_End_DRawing() {}

    void execute(Rhi_Command_List & command_list) override;
};

struct Rhi_Command_Resource_Transition : public Rhi_Command {
    Rhi_Transition *transition;

    Rhi_Command_Resource_Transition(Rhi_Transition *Transition) : transition(Transition) {}

    void execute(Rhi_Command_List & command_list) override;
};

struct Rhi_Command_Render_Imgui_Draw_Data : public Rhi_Command {
    ImDrawData *data;

    Rhi_Command_Render_Imgui_Draw_Data(ImDrawData *data) : data(data) {}

    void execute(Rhi_Command_List & command_list) override;
};

class Rhi_Command_List {
public:
    inline Rhi_Command_List() {
        graphics_context = Rhi::get().get_command_context();
    }

    inline void begin_drawing() {
        add_command(new Rhi_Command_Begin_Drawing());
    }

    inline void end_drawing() {
        add_command(new Rhi_Command_End_DRawing());
    }

    inline void resource_transition(Rhi_Transition *Transition) {
        add_command(new Rhi_Command_Resource_Transition(Transition));
    }

    inline void render_imgui_draw_data(ImDrawData *DrawData) {
        add_command(new Rhi_Command_Render_Imgui_Draw_Data(DrawData));
    }

    inline Rhi_Command_Context &get_graphics_context() {
        return *graphics_context;
    }

private:
    inline void add_command(Rhi_Command *command) {
        end->next = command;
        end = end->next;
    }

    inline void Execute() {
        Rhi_Command *curr = base->next;
        Rhi_Command *prev = curr;
        
        while (curr != nullptr) {
            curr->execute(*this);
            prev = curr;
            curr = curr->next;
            
            delete prev;
        }
        
        end = base;
    }
    
private:
    Rhi_Command *base = new Rhi_Command_Nop();
    Rhi_Command *end = base;
    Rhi_Command_Context *graphics_context = nullptr;

private:
    friend class Rhi;
};

// Command Executes

inline void Rhi_Command_Begin_Drawing::execute(Rhi_Command_List &command_list) {
    command_list.get_graphics_context().begin_drawing();
}

inline void Rhi_Command_End_DRawing::execute(Rhi_Command_List &command_list) {
    command_list.get_graphics_context().end_drawing();
}

inline void Rhi_Command_Resource_Transition::execute(Rhi_Command_List &command_list) {
    command_list.get_graphics_context().resource_transition(transition);
}

inline void Rhi_Command_Render_Imgui_Draw_Data::execute(Rhi_Command_List &command_list) {
    command_list.get_graphics_context().render_ingui_draw_data(data);
}
