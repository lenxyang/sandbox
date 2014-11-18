
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "base/i18n/icu_util.h"
#include "base/run_loop.h"
#include "ui/base/ime/input_method_initializer.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/compositor/test/in_process_context_factory.h"
#include "ui/gfx/screen.h"
#include "ui/gl/gl_surface.h"
#include "ui/aura/env.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#include "ui/wm/core/wm_state.h"
#include "ui/base/win/scoped_ole_initializer.h"
#include "ui/views/test/desktop_test_views_delegate.h"

#include "azer/sandbox/ui/aura/window.h"

int main(int argc, char* argv[]) {
  ui::ScopedOleInitializer ole_initializer_;
  CommandLine::Init(argc, argv);
  base::AtExitManager at_exit;
  gfx::GLSurface::InitializeOneOff();
  scoped_ptr<ui::InProcessContextFactory> context_factory(
      new ui::InProcessContextFactory);
  ::base::MessageLoopForUI message_loop;

  base::i18n::InitializeICU();
  ui::RegisterPathProvider();

  base::FilePath ui_test_pak_path;
  DCHECK(PathService::Get(ui::UI_TEST_PAK, &ui_test_pak_path));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(ui_test_pak_path); 

  aura::Env::CreateInstance(true);
  aura::Env::GetInstance()->set_context_factory(context_factory.get());
  ui::InitializeInputMethodForTesting();
  views::DesktopTestViewsDelegate views_delegate;
  wm::WMState wm_state;
  scoped_ptr<gfx::Screen> desktop_screen(views::CreateDesktopScreen());
  gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE,
                                 desktop_screen.get());
  

  views::Widget* widget = new views::Widget;
  views::Widget::InitParams params;
  params.delegate = new ExamplesWindow;
  params.context = NULL;
  params.bounds = gfx::Rect(0, 0, 800, 600);
  widget->Init(params);
  widget->Show();

  base::RunLoop().Run();

  ui::ShutdownInputMethod();
  aura::Env::DeleteInstance();
  return 0;
}
