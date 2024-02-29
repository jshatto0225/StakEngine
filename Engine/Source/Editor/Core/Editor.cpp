#include <Runtime/Runtime.h>
#include <Runtime/Core/EntryPoint.h>

#include "EditorLayer.h"

class Editor : public Application {
public:
  Editor() {
    this->addLayer<EditorLayer>();
  }
};

Application *GetApp() {
  return new Editor;
}