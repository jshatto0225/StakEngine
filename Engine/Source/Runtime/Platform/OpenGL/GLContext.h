#pragma once

class GLContext {
public:
  GLContext() = default;
  virtual ~GLContext() {}

  virtual void make_current() = 0;
};

