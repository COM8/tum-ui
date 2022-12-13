#include "CreditsWidget.hpp"
#include <gtkmm/enums.h>

namespace ui::widgets {
CreditsWidget::CreditsWidget() { prep_widget(); }

void CreditsWidget::prep_widget() {
    set_orientation(Gtk::Orientation::HORIZONTAL);

    label.set_label("Developed with ❤️ at TUM (CM)");
    append(label);

    link.set_tooltip_text("https://github.com/com8/tum-ui");
    link.set_uri("https://github.com/com8/tum-ui");
    link.set_label("GitHub");
    link.set_margin_start(5);
    append(link);
}
}  // namespace ui::widgets