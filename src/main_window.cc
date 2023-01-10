#include "main_window.h"

#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/togglebutton.h>
#include <tuple>

MainWindow::MainWindow()
  : database{ new Database() },
    prof_control{ new ProfilesControllerInstance(database) },
    proc_control{ new ProcessesControllerInstance(database) },
    logs_control{ new LogsControllerInstance(database) },
    help{ new Help() },
    console{ new ConsoleThreadInstance(prof_control, proc_control, logs_control) }
{
  // Add tabs to the stack pane
  m_tab_stack.add(*(prof_control->get_tab()), "prof", "Profiles");
  m_tab_stack.add(*(proc_control->get_tab()), "proc", "Processes");
  m_tab_stack.add(*(logs_control->get_tab()), "logs", "Logs");

  // Add a transition to the stack
  m_tab_stack.set_transition_type(Gtk::StackTransitionType::CROSSFADE);
  m_tab_stack.set_transition_duration(DEFAULT_TRANSITION_DURATION);

  // Attach the stack to the stack switcher
  m_switcher.set_stack(m_tab_stack);

  // Connect the stackswitcher to the 'on_switch' method
  auto switcher_controller = Gtk::GestureClick::create();
  auto focus = sigc::mem_fun(*this, &MainWindow::on_switch);
  switcher_controller->signal_pressed().connect(focus, true);
  m_switcher.add_controller(switcher_controller);
  on_switch(0, 0, 0);

  // Connect the profile tab to the `send_status_change` method
  auto change_fun = sigc::mem_fun(*this, &MainWindow::send_status_change);
  prof_control->get_tab()->set_status_change_signal_handler(change_fun);

  // Configure settings related to the 'Help' button
  m_help_button.set_image_from_icon_name("dialog-question");

  auto help_toggle_fun = sigc::mem_fun(*this, &MainWindow::on_help_toggle);
  m_help_button.signal_toggled().connect(help_toggle_fun, true);

  // Configure settings related to 'Search' button
  m_search_button.set_image_from_icon_name("edit-find-symbolic");

  auto search_togggle_fun = sigc::mem_fun(*this, &MainWindow::on_search_toggle);
  m_search_button.signal_toggled().connect(search_togggle_fun, true);

  // Add the main page and the help page to the top stack
  // This stack controls whether the 'Help' page is visible, or the main application
  m_top_stack.add(m_tab_stack, "main_page");
  m_top_stack.add(*help, "help_page");

  m_top_stack.set_transition_duration(DEFAULT_TRANSITION_DURATION);

  // Set some default properties for titlebar
  m_headerbar.set_title_widget(m_switcher);
  m_headerbar.pack_end(m_help_button);
  m_headerbar.pack_end(m_search_button);

  /// m_headerbar.set_title("AppAnvil");
  /// m_headerbar.set_subtitle("GUI for AppArmor");
  m_headerbar.set_hexpand(true);
  m_headerbar.set_show_title_buttons(true);

  // Set the icon
  /// auto builder         = Gtk::Builder::create_from_resource("/resources/icon.ui");
  /// Gtk::Image *icon_ptr = builder->get_widget<Gtk::Image>("icon");
  /// this->set_icon(icon_ptr->get_pixbuf());

  // Set some default settings for the window
  this->set_default_size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  /// this->add_events(Gdk::EventMask::ENTER_NOTIFY_MASK);

  // Add and show all children
  this->set_titlebar(m_headerbar);
  this->set_child(m_top_stack);

  // Hide the side info in the Profiles Tab
  prof_control->get_tab()->hide_profile_info();
}

void MainWindow::send_status_change(const std::string &profile, const std::string &old_status, const std::string &new_status)
{
  console->send_change_profile_status_message(profile, old_status, new_status);
}

void MainWindow::on_help_toggle()
{
  const bool is_active = m_help_button.get_active();

  if (is_active) {
    m_switcher.hide();
    m_top_stack.set_visible_child("help_page", Gtk::StackTransitionType::SLIDE_DOWN);
    m_help_button.set_label("Return to application");
    /// m_help_button.set_always_show_image(false);
  } else {
    m_switcher.show();
    m_top_stack.set_visible_child("main_page", Gtk::StackTransitionType::SLIDE_UP);
    m_help_button.set_label("");
    m_help_button.set_image_from_icon_name("dialog-question");
    /// m_help_button.set_always_show_image(true);
  }

  handle_search_button_visiblity();
}

void MainWindow::on_search_toggle()
{
  const std::string visible_child = m_tab_stack.get_visible_child_name();
  const bool is_active            = m_search_button.get_active();

  if (is_active) {
    // Show the searchbars, and determine which searchbar should be focused
    prof_control->get_tab()->show_searchbar(visible_child == "prof");
    proc_control->get_tab()->show_searchbar(visible_child == "proc");
    logs_control->get_tab()->show_searchbar(visible_child == "logs");
    help->show_searchbar(m_help_button.get_active());
  } else {
    prof_control->get_tab()->hide_searchbar();
    proc_control->get_tab()->hide_searchbar();
    logs_control->get_tab()->hide_searchbar();
    help->hide_searchbar();
  }
}

void MainWindow::on_switch(int, double, double)
{
  const std::string visible_child = m_tab_stack.get_visible_child_name();
  if (visible_child == "prof") {
    console->send_refresh_message(PROFILE);
  } else if (visible_child == "proc") {
    console->send_refresh_message(PROCESS);
  } else if (visible_child == "logs") {
    // Do not refresh logs (until improved)
    console->send_refresh_message(LOGS);
  }

  handle_search_button_visiblity();
}

void MainWindow::handle_search_button_visiblity()
{
  const bool help_is_active       = m_help_button.get_active();
  const std::string visible_child = m_tab_stack.get_visible_child_name();

  if (visible_child == "profile_loader" && !help_is_active) {
    m_search_button.hide();
  } else {
    m_search_button.show();
  }
}
