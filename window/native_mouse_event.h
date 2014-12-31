#pragma once

#include "azer/base/export.h"
#include "azer/ui/window/native_message.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/point.h"

namespace azer {
namespace window {
class NativeMouseEvent : public NativeEvent {
 public:
 protected:
};

class NativeMouseMoveEvent : public NativeMouseEvent {
public:
};

class NativeMouseKeydownEvent : public NativeMouseEvent {
};

class NativeMouseKeyupEvent : public NativeMouseEvent {
};

class NativeMouseClickEvent : public NativeMouseEvent {
 public:
 private:
};
}  // namespace window
}  // namespace azer
