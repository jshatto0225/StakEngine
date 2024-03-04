#pragma once

#include <Runtime/Runtime.h>

class UIRenderer {
public:
  static bool CheckBox(const std::string &label);
  static std::string TextBox();
  //static void Image(const std::shared_ptr<Texture> &image);
  static bool TextButton(const std::string &text, const std::shared_ptr<Font> &font);
  static bool ImageButton(const std::shared_ptr<Texture> &image);
  static void Text(const std::string &text, const std::shared_ptr<Font> &font);

private:
};