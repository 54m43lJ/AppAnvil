#include "main_window.h"
#include "console_thread.h"

#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/togglebutton.h>
#include <tuple>

MainWindow::MainWindow()
  : database{ std::make_shared<Database>() },
    prof_control{ std::make_shared<ProfilesControllerInstance>(database) },
    proc_control{ std::make_shared<ProcessesControllerInstance>(database) },
    logs_control{ std::make_shared<LogsControllerInstance>(database) },
    console{ std::make_shared<ConsoleThreadInstance>(prof_control, proc_control, logs_control) }
{
  // Add tabs to the stack pane
  m_tab_stack.add(*(prof_control->get_tab()), "prof", "Profiles");
  m_tab_stack.add(*(proc_control->get_tab()), "proc", "Processes");
  m_tab_stack.add(*(logs_control->get_tab()), "logs", "Logs");

  // Add a transition to the stack
  m_tab_stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_CROSSFADE);
  m_tab_stack.set_transition_duration(DEFAULT_TRANSITION_DURATION);

  // Attach the stack to the stack switcher
  m_switcher.set_stack(m_tab_stack);

  // Connect the stackswitcher to the 'on_switch' method
  auto focus = sigc::mem_fun(*this, &MainWindow::on_switch);
  m_switcher.signal_event().connect(focus, true);
  on_switch(NULL);

  // Connect the profile tab to the `send_status_change` method
  auto change_fun = sigc::mem_fun(*console, &ConsoleThreadInstance::send_change_profile_status_message);
  prof_control->set_status_change_signal_handler(change_fun);

  // Configure settings related to 'Search' button
  m_search_button.set_image_from_icon_name("edit-find-symbolic");

  auto search_togggle_fun = sigc::mem_fun(*this, &MainWindow::on_search_toggle);
  m_search_button.signal_toggled().connect(search_togggle_fun, true);

  // Connect the reauthenticate button
  auto enable_auth_fun = sigc::mem_fun(*console, &ConsoleThreadInstance::reenable_authentication_for_refresh);
  prof_control->get_tab()->connect_reauthenticate_button(enable_auth_fun);
  proc_control->get_tab()->connect_reauthenticate_button(enable_auth_fun);
  logs_control->get_tab()->connect_reauthenticate_button(enable_auth_fun);

  // Set some default properties for titlebar
  m_headerbar.set_custom_title(m_switcher);
  m_headerbar.pack_end(help_toggle);
  m_headerbar.pack_end(m_search_button);

  m_headerbar.set_title("AppAnvil");
  m_headerbar.set_subtitle("GUI for AppArmor");
  m_headerbar.set_hexpand(true);
  m_headerbar.set_show_close_button(true);

  // Set the icon
  auto builder         = Gtk::Builder::create_from_resource("/misc/icon.glade");
  Gtk::Image *icon_ptr = nullptr;
  builder->get_widget<Gtk::Image>("icon", icon_ptr);
  this->set_icon(icon_ptr->get_pixbuf());

  // Set some default settings for the window
  this->set_default_size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  this->add_events(Gdk::EventMask::ENTER_NOTIFY_MASK);

  // Add and show all children
  this->set_titlebar(m_headerbar);
  this->add(m_tab_stack);
  this->show_all();

  // Hide the side info in the Profiles Tab
  prof_control->get_tab()->hide_profile_info();
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
  } else {
    prof_control->get_tab()->hide_searchbar();
    proc_control->get_tab()->hide_searchbar();
    logs_control->get_tab()->hide_searchbar();
  }
}

bool MainWindow::on_switch(GdkEvent *event)
{
  std::ignore = event;

  const std::string visible_child = m_tab_stack.get_visible_child_name();
  if (visible_child == "prof") {
    console->send_refresh_message(PROFILE);
    help_toggle.set_help_type(Help::Type::PROFILE);
  } else if (visible_child == "proc") {
    console->send_refresh_message(PROCESS);
    help_toggle.set_help_type(Help::Type::PROCESS);
  } else if (visible_child == "logs") {
    console->send_refresh_message(LOGS);
    help_toggle.set_help_type(Help::Type::LOGS);
  }

  return false;
}
