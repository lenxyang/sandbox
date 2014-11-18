#pragma once

#include "base/basictypes.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/label.h"
#include "ui/views/background.h"

class ExamplesWindow : public views::WidgetDelegateView {
 public:
  ExamplesWindow() {
    // view_ = new View;
    status_label_ = new views::Label;
    status_label_->SetText(::base::UTF8ToUTF16("status label"));

    set_background(views::Background::CreateStandardPanelBackground());
    views::GridLayout* layout = new views::GridLayout(this);
    SetLayoutManager(layout);
    views::ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 1,
                          views::GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 5);
    layout->AddPaddingRow(0, 5);
    layout->StartRow(0, 0);
    layout->AddView(status_label_);
    layout->AddPaddingRow(0, 5);
  }
 private:
  bool CanResize() const override { return true; }
  bool CanMaximize() const override { return true; }
  bool CanMinimize() const override { return true; }
  base::string16 GetWindowTitle() const override {
    return base::ASCIIToUTF16("Views Examples");
  }
  View* GetContentsView() override { return this; }
  void WindowClosing() override {
    base::MessageLoopForUI::current()->Quit();
  }

  views::View* view_;
  views::Label* status_label_;
  DISALLOW_COPY_AND_ASSIGN(ExamplesWindow);
};