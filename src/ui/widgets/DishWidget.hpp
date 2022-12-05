#pragma once

#include "backend/eatApi/Dish.hpp"
#include "backend/eatApi/Label.hpp"
#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/listboxrow.h>

namespace ui::widgets {
class DishWidget : public Gtk::ListBoxRow {
 private:
    std::shared_ptr<backend::eatApi::Dish> dish;
    std::unordered_map<std::string, std::shared_ptr<backend::eatApi::Label>>* labels;

    Gtk::Label nameLbl{};
    Gtk::Label labelsLbl{};
    Gtk::Label priceLbl{};

 public:
    DishWidget(std::shared_ptr<backend::eatApi::Dish> dish, std::unordered_map<std::string, std::shared_ptr<backend::eatApi::Label>>* labels);
    DishWidget(DishWidget&&) = default;
    DishWidget(const DishWidget&) = delete;
    DishWidget& operator=(DishWidget&&) = default;
    DishWidget& operator=(const DishWidget&) = delete;
    ~DishWidget() override = default;

 private:
    void prep_widget();
    std::string gen_labels_str();
    std::string gen_labels_tooltip();
};
}  // namespace ui::widgets