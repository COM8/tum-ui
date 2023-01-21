#include "CanteenHeadCountWidget.hpp"
#include "backend/eatApi/EatApiHelper.hpp"
#include "backend/eatApi/HeadCount.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <memory>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>

namespace ui::widgets {
CanteenHeadCountWidget::CanteenHeadCountWidget() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    prep_widget();

    disp.connect(sigc::mem_fun(*this, &CanteenHeadCountWidget::on_notification_from_update_thread));
    start_thread();
}

CanteenHeadCountWidget::~CanteenHeadCountWidget() {
    if (shouldRun) {
        stop_thread();
    }
}

void CanteenHeadCountWidget::prep_widget() {
    set_visible(false);

    append(infoLbl);
    status.set_hexpand_set(true);
    append(status);
}

void CanteenHeadCountWidget::update_head_count_ui() {
    headCountMutex.lock();
    if (!headCount) {
        headCountMutex.unlock();
        SPDLOG_INFO("No head count data found.");
        set_visible(false);
        return;
    }

    assert(headCount->percent >= 0);
    assert(headCount->percent <= 100);
    SPDLOG_INFO("Head Count: {} -> {}%", headCount->count, std::round(headCount->percent));
    status.set_fraction(headCount->percent / 100);
    status.set_tooltip_text(fmt::format("{}%", std::round(headCount->percent)));
    infoLbl.set_text(fmt::format("Head Count: {} -> {}%", headCount->count, std::round(headCount->percent)));
    headCountMutex.unlock();
    set_visible(true);
}

void CanteenHeadCountWidget::update_head_count() {
    backend::storage::Settings* settings = backend::storage::get_settings_instance();
    std::shared_ptr<backend::eatApi::HeadCount> headCount = backend::eatApi::request_head_count(settings->data.canteenId);

    headCountMutex.lock();
    this->headCount = std::move(headCount);
    headCountMutex.unlock();
    disp.emit();
}

void CanteenHeadCountWidget::thread_run() {
    SPDLOG_INFO("Canteen head count thread started.");
    while (shouldRun) {
        update_head_count();
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
    SPDLOG_INFO("Canteen head count thread stoped.");
}

void CanteenHeadCountWidget::start_thread() {
    assert(!updateThread);
    assert(!shouldRun);
    shouldRun = true;
    updateThread = std::make_unique<std::thread>(&CanteenHeadCountWidget::thread_run, this);
}

void CanteenHeadCountWidget::stop_thread() {
    assert(updateThread);
    assert(shouldRun);
    shouldRun = false;
    updateThread->join();
    updateThread = nullptr;
}

//-----------------------------Events:-----------------------------
void CanteenHeadCountWidget::on_notification_from_update_thread() {
    update_head_count_ui();
}
}  // namespace ui::widgets