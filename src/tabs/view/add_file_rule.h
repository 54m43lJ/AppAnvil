#ifndef TABS_VIEW_ADD_FILE_RULE_H
#define TABS_VIEW_ADD_FILE_RULE_H

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/combobox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/stack.h>
#include <gtkmm/textview.h>
#include <libappanvil/apparmor_parser.hh>
#include <memory>

class AddFileRule
{
public:
  typedef std::pair<int, const AppArmor::FileRule> show_dialog_response;

  /**
   * @brief Builds and shows the "Add FileRule" dialog.
   *
   * @returns std::pair<int, AppArmor::Tree::FileRuleRule> The Gtk::ResponseType from the Dialog and the rule to add (if it exists)
   */
  static show_dialog_response show_dialog();

protected:
  AddFileRule();

  // Signal Handlers
  void handle_file_button();
  void handle_entry_changed();
  void handle_button_cancel();
  void handle_button_next();
  void handle_button_back();
  void handle_button_accept();

private:
  // GUI Builder to parse UI from xml file
  Glib::RefPtr<Gtk::Builder> builder;

  // Widgets
  std::shared_ptr<Gtk::Dialog> dialog;
  std::unique_ptr<Gtk::HeaderBar> headerbar;
  std::unique_ptr<Gtk::Stack> stack_1;
  std::unique_ptr<Gtk::Stack> stack_2;

  std::unique_ptr<Gtk::Button> button_cancel;
  std::unique_ptr<Gtk::Button> button_next;
  std::unique_ptr<Gtk::Button> button_back;
  std::unique_ptr<Gtk::Button> button_accept;

  std::unique_ptr<Gtk::Entry> fr_entry;
  std::unique_ptr<Gtk::Button> fr_entry_button;
  std::unique_ptr<Gtk::TextView> fr_preview;

  std::unique_ptr<Gtk::CheckButton> cb_read;
  std::unique_ptr<Gtk::CheckButton> cb_write;
  std::unique_ptr<Gtk::CheckButton> cb_exec;
  std::unique_ptr<Gtk::CheckButton> cb_link;
  std::unique_ptr<Gtk::CheckButton> cb_lock;

  std::unique_ptr<Gtk::CheckButton> cb_deny;
  std::unique_ptr<Gtk::CheckButton> cb_owner;
  std::unique_ptr<Gtk::CheckButton> cb_audit;

  // Currently selected rule (if there is one)
  AppArmor::Tree::FileRule prospective_rule;
};

#endif // TABS_VIEW_ADD_FILE_RULE_H