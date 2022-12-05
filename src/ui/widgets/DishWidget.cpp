#include "DishWidget.hpp"
#include <string>

namespace ui::widgets {
DishWidget::DishWidget(std::shared_ptr<backend::eatApi::Dish> dish, std::unordered_map<std::string, std::shared_ptr<backend::eatApi::Label>>* labels) : dish(std::move(dish)),
                                                                                                                                                        labels(labels) {
    prep_widget();
}

void DishWidget::prep_widget() {
    Gtk::Box* mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    mainBox->set_margin_start(10);
    mainBox->set_margin_end(10);
    set_child(*mainBox);

    nameLbl.set_wrap(true);
    nameLbl.set_text(dish->name);
    mainBox->append(nameLbl);

    labelsLbl.set_wrap(true);
    labelsLbl.set_visible(!dish->labels.empty());
    labelsLbl.set_text(gen_labels_str());
    labelsLbl.set_tooltip_text(gen_labels_tooltip());
    mainBox->append(labelsLbl);

    priceLbl.set_text(dish->price);
    priceLbl.set_visible(!dish->price.empty());
    mainBox->append(priceLbl);
}

std::string DishWidget::gen_labels_str() {
    std::string result;
    for (const std::string& label : dish->labels) {
        std::string part;
        if (labels->contains(label)) {
            part = (*labels)[label]->abbreviation;
        } else {
            part = label;
        }

        if (!result.empty()) {
            result += " ";
        }
        result += part;
    }
    return result;
}

std::string DishWidget::gen_labels_tooltip() {
    std::string result;
    for (const std::string& label : dish->labels) {
        std::string part;
        if (labels->contains(label)) {
            part = (*labels)[label]->abbreviation + " - " + (*labels)[label]->text;
        } else {
            part = label;
        }

        if (!result.empty()) {
            result += "\n";
        }
        result += part;
    }
    return result;
}
}  // namespace ui::widgets