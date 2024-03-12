#pragma once

class Entity {
public:
  Entity(u64 id) {}

private:
  u64 id;

  friend class Registry;
};