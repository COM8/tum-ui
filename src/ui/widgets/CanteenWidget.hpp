#pragma once

#include "CanteenHeadCountWidget.hpp"
#include "DishWidget.hpp"
#include "backend/eatApi/Canteen.hpp"
#include "backend/eatApi/Dish.hpp"
#include "backend/eatApi/Label.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <glibmm/dispatcher.h>
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class CanteenWidget : public Gtk::Box {
 private:
    Gtk::ListBox dishesListBox{};
    Gtk::Box loadingBox{Gtk::Orientation::VERTICAL};
    Gtk::Label canteenNameLbl{};
    Gtk::Label canteenOpeningHoursLbl{};
    CanteenHeadCountWidget canteenHeadCount{};

    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::unordered_map<std::string, std::shared_ptr<backend::eatApi::Label>> labels;
    std::shared_ptr<backend::eatApi::Canteen> canteen{};
    std::shared_ptr<backend::eatApi::Menu> menu{};
    std::vector<DishWidget> dishWidgets{};
    std::mutex dishesMutex{};

 public:
    CanteenWidget();
    CanteenWidget(CanteenWidget&&) = delete;
    CanteenWidget(const CanteenWidget&) = delete;
    CanteenWidget& operator=(CanteenWidget&&) = delete;
    CanteenWidget& operator=(const CanteenWidget&) = delete;
    ~CanteenWidget() override;

 private:
    void prep_widget();
    void start_thread();
    void stop_thread();

    void update_dishes();
    static std::shared_ptr<backend::eatApi::Menu> request_menu(const std::string& canteenId, std::chrono::year_month_day date);
    void update_dishes_ui();
    void thread_run();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
};
}  // namespace ui::widgets