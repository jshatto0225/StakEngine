#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>

#include "Component.h"

struct Archetype {

}

class Registry {
public:
  Registry();
  ~Registry();

  Entity createEntity();

private:
  
};
