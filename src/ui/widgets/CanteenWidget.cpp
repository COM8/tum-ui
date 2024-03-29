#include "CanteenWidget.hpp"
#include "backend/eatApi/Canteen.hpp"
#include "backend/eatApi/Dish.hpp"
#include "backend/eatApi/EatApiHelper.hpp"
#include "backend/eatApi/date.hpp"
#include "backend/eatApi/iso_week.hpp"
#include "backend/storage/Serializer.hpp"
#include "logger/Logger.hpp"
#include "ui/widgets/DishWidget.hpp"
#include <backend/storage/Settings.hpp>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <gtkmm/checkbutton.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/widget.h>
#include <re2/re2.h>
#include <spdlog/spdlog.h>

namespace ui::widgets {
CanteenWidget::CanteenWidget() {
    prep_widget();
    disp.connect(sigc::mem_fun(*this, &CanteenWidget::on_notification_from_update_thread));
    start_thread();
}

CanteenWidget::~CanteenWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void CanteenWidget::prep_widget() {
    set_orientation(Gtk::Orientation::VERTICAL);

    canteenNameLbl.set_margin_top(5);
    append(canteenNameLbl);
    canteenOpeningHoursLbl.set_margin_bottom(5);
    append(canteenOpeningHoursLbl);
    canteenHeadCount.set_margin_bottom(5);
    canteenHeadCount.set_margin_start(10);
    canteenHeadCount.set_margin_end(10);
    append(canteenHeadCount);

    Gtk::ScrolledWindow* scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroll->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    dishesListBox.set_selection_mode(Gtk::SelectionMode::NONE);
    dishesListBox.add_css_class("boxed-list");
    dishesListBox.set_margin_start(10);
    dishesListBox.set_margin_end(10);

    // Transparent background:
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    dishesListBox.get_style_context()->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    dishesListBox.add_css_class("transparent-background");

    scroll->set_child(dishesListBox);
    scroll->set_vexpand(true);
    scroll->set_hexpand(true);
    append(*scroll);
}

void CanteenWidget::update_dishes_ui() {
    // Clear existing items:
    for (DishWidget& widget : dishWidgets) {
        dishesListBox.remove(*static_cast<Gtk::Widget*>(&widget));
    }
    dishWidgets.clear();

    // Add new items:
    dishesMutex.lock();
    if (menu) {
        for (const std::shared_ptr<backend::eatApi::Dish>& dish : menu->dishes) {
            dishWidgets.emplace_back(dish, &labels);
            DishWidget* dishW = &dishWidgets.back();
            dishesListBox.append(*dishW);
        }
    }

    std::chrono::sys_days sysDays(menu->date);
    std::chrono::system_clock::time_point tp(sysDays);
    std::string date = menu ? (std::to_string(static_cast<unsigned>(menu->date.day())) + "." + std::to_string(static_cast<unsigned>(menu->date.month())) + "." + std::to_string(static_cast<int>(menu->date.year()))) : "";
    canteenNameLbl.set_markup(canteen ? "<span font_weight='bold'>" + canteen->name + " - " + date + "</span>" : "Canteen not found!");
    const std::optional<backend::eatApi::OpeningHours> openingHours = canteen ? canteen->get_current_opening_hours() : std::nullopt;
    if (openingHours) {
        canteenOpeningHoursLbl.set_text(openingHours->start + " - " + openingHours->end);
        canteenOpeningHoursLbl.set_visible(true);
    } else {
        canteenOpeningHoursLbl.set_visible(false);
    }

    dishesMutex.unlock();
}

void CanteenWidget::update_dishes() {
    SPDLOG_INFO("Updating labels...");
    std::unordered_map<std::string, std::shared_ptr<backend::eatApi::Label>> labels;
    try {
        labels = backend::eatApi::request_labels();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update labels with: {}", e.what());
        return;
    }
    if (labels.empty()) {
        SPDLOG_ERROR("Updating labels failed - none found.");
        return;
    }
    SPDLOG_INFO("Labels updated successfully.");

    SPDLOG_INFO("Updating canteens...");
    std::vector<std::shared_ptr<backend::eatApi::Canteen>> canteens;
    try {
        canteens = backend::eatApi::request_canteens();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update canteens with: {}", e.what());
        return;
    }
    if (canteens.empty()) {
        SPDLOG_ERROR("Updating canteens failed - none found.");
        return;
    }

    backend::storage::Settings* settings = backend::storage::get_settings_instance();
    std::shared_ptr<backend::eatApi::Canteen> canteen;
    for (const std::shared_ptr<backend::eatApi::Canteen>& c : canteens) {
        if (c->id == settings->data.canteenId) {
            canteen = c;
            break;
        }
    }

    if (!canteen) {
        SPDLOG_ERROR("Requested canteen id '{}' not found. Updating failed", settings->data.canteenId);
        return;
    }

    SPDLOG_INFO("Canteens updated successfully.");

    SPDLOG_INFO("Updating dishes...");
    std::shared_ptr<backend::eatApi::Menu> curMenu;
    try {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        // Show the dishes for tomorrow in case it is past 4pm:
        std::chrono::hh_mm_ss time{std::chrono::floor<std::chrono::milliseconds>(now - std::chrono::floor<date::days>(now))};
        if (time.hours() >= std::chrono::hours(16)) {
            now += std::chrono::days(1);
        }

        std::chrono::year_month_day ymd{std::chrono::floor<date::days>(now)};

        curMenu = request_menu(settings->data.canteenId, ymd);
        // Retry for the next week in case there are no dishes for this week:
        if (!curMenu) {
            std::chrono::sys_days sysDays(ymd);
            sysDays += std::chrono::days(1);
            ymd = std::chrono::year_month_day(sysDays);
            curMenu = request_menu(settings->data.canteenId, ymd);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to update dishes with: {}", e.what());
        return;
    }
    if (canteens.empty()) {
        SPDLOG_ERROR("Updating dishes failed - no canteen found.");
        return;
    }
    if (!curMenu) {
        SPDLOG_ERROR("Updating dishes failed - no next dishes found.");
        return;
    }
    SPDLOG_INFO("Dishes updated successfully.");

    dishesMutex.lock();
    this->labels = labels;
    this->canteen = canteen;
    this->menu = curMenu;
    dishesMutex.unlock();
    disp.emit();
}

std::shared_ptr<backend::eatApi::Menu> CanteenWidget::request_menu(const std::string& canteenId, std::chrono::year_month_day date) {
    iso_week::year_weeknum_weekday yww(std::chrono::sys_days{date});
    std::shared_ptr<backend::eatApi::Menu> curMenu;
    std::vector<std::shared_ptr<backend::eatApi::Menu>> menus = backend::eatApi::request_dishes(canteenId, yww.year(), yww.weeknum());
    for (const std::shared_ptr<backend::eatApi::Menu>& menu : menus) {
        if (menu->date >= date && (!curMenu || curMenu->date > menu->date)) {
            curMenu = menu;
        }
    }
    return curMenu;
}

void CanteenWidget::thread_run() {
    SPDLOG_INFO("Departure thread started.");
    while (shouldRun) {
        update_dishes();
        std::this_thread::sleep_for(std::chrono::minutes(30));
    }
    SPDLOG_INFO("Departure thread stoped.");
}

void CanteenWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&CanteenWidget::thread_run, this);
}

void CanteenWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void CanteenWidget::on_notification_from_update_thread() { update_dishes_ui(); }

}  // namespace ui::widgets