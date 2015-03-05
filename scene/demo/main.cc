#include <memory>

#include "azer/render/render.h"
#include "azer/render/ui/views.h"
#include "azer/uisbox/azerui/controls/azer.h"

using base::ASCIIToUTF16;

class MainWindowContent : public views::WidgetDelegateView {
 public:
  MainWindowContent(azer::UIEnvironment* env)
      : env_(env)
      , fontlist_("Arial, Helvetica, Bold 24px") {
    set_background(views::Background::CreateStandardPanelBackground());
  }

 private:
  bool CanResize() const override { return true; }
  bool CanMaximize() const override { return false; }
  bool CanMinimize() const override { return false; }
  base::string16 GetWindowTitle() const override {
    return base::ASCIIToUTF16("Hello Views");
  }
  View* GetContentsView() override { return this; }
  void WindowClosing() override {
    base::MessageLoopForUI::current()->Quit();
    env_->GetRenderLoop()->Quit();
  }
  
  
 private:
  azer::UIEnvironment* env_;
  DISALLOW_COPY_AND_ASSIGN(MainWindowContent);
};

int main(int argc, char** argv) {
  azer::UIEnvironment env;
  CHECK(env.Init(argc, argv));

  MainDelegate azer_content;
  azer::UIEnvironment::Params params;
  params.width = 800;
  params.height = 600;
  params.view_delegate = new MainWindowContent(&env);
  params.render_delegate = &azer_content;
  CHECK(env.MainLoop(params));
  
  return 0;
}

