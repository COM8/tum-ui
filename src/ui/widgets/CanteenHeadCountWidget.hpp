#pragma once

#include "backend/eatApi/HeadCount.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <glibmm/dispatcher.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>

namespace ui::widgets {
class CanteenHeadCountWidget : public Gtk::Box {
 private:
    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::mutex headCountMutex{};
    std::shared_ptr<backend::eatApi::HeadCount> headCount{nullptr};

    Gtk::Label infoLbl{};
    Gtk::ProgressBar status{};

 public:
    CanteenHeadCountWidget();
    CanteenHeadCountWidget(CanteenHeadCountWidget&&) = delete;
    CanteenHeadCountWidget(const CanteenHeadCountWidget&) = delete;
    CanteenHeadCountWidget& operator=(CanteenHeadCountWidget&&) = delete;
    CanteenHeadCountWidget& operator=(const CanteenHeadCountWidget&) = delete;
    ~CanteenHeadCountWidget() override;

 private:
    void prep_widget();

    void start_thread();
    void stop_thread();
    void update_head_count_ui();
    void thread_run();
    void update_head_count();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
};
}  // namespace ui::widgets