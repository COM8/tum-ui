#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/linkbutton.h>

namespace ui::widgets {
class CreditsWidget : public Gtk::Box {
 private:
    Gtk::Label label;
    Gtk::LinkButton link;

 public:
    CreditsWidget();
    CreditsWidget(CreditsWidget&&) = default;
    CreditsWidget(const CreditsWidget&) = delete;
    CreditsWidget& operator=(CreditsWidget&&) = default;
    CreditsWidget& operator=(const CreditsWidget&) = delete;
    ~CreditsWidget() override = default;

 private:
    void prep_widget();
};
}  // namespace ui::widgets