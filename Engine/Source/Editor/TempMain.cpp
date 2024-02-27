#include "Runtime/Runtime.h"

class TempLayer : public sk::ApplicationLayer {

};

class TempApp : public sk::Application {
public:
  TempApp() {
    add_layer<TempLayer>();
  }
};

sk::Application *get_app() {
  return new TempApp();
}
