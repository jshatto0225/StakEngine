#include <Runtime/Runtime.h>
#include <Runtime/Core/EntryPoint.h>

#include "EditorLayer.h"

/**
 * Editor Application
 */
class Editor : public Application {
public:
  Editor(const ApplcationSpec &spec) 
    : Application(spec) {
    this->addLayer<EditorLayer>(spec.window_width, spec.window_height);
  }
};

Application *GetApp() {
  ApplcationSpec s;
  s.window_width = 1280;
  s.window_height = 720;
  s.window_x = 0;
  s.window_y = 0;
  s.window_title = "Stak Editor";
  return new Editor(s);
}