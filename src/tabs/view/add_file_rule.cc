#include <fstream>
#include <gtkmm/dialog.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treemodelsort.h>
#include <libappanvil/tree/FileRule.hh>
#include <tree/FileMode.hh>
#include <tree/PrefixNode.hh>
#include <utility>

#include "add_file_rule.h"
#include "../../threads/command_caller.h"
#include "common.h"

AddFileRule::AddFileRule()
  : builder{ Gtk::Builder::create_from_resource("/modal/rule/file_rule.glade") },
    dialog{ Common::get_widget_shared<Gtk::Dialog>("dialog", builder) },
    headerbar{ Common::get_widget<Gtk::HeaderBar>("headerbar", builder) },
    stack_1{ Common::get_widget<Gtk::Stack>("stack_1", builder)},
    stack_2{ Common::get_widget<Gtk::Stack>("stack_2", builder)},
    button_cancel{ Common::get_widget<Gtk::Button>("button_cancel", builder)},
    button_next{ Common::get_widget<Gtk::Button>("button_next", builder)},
    button_back{ Common::get_widget<Gtk::Button>("button_back", builder)},
    button_accept{ Common::get_widget<Gtk::Button>("button_accept", builder)},
    fr_entry{ Common::get_widget<Gtk::Entry>("fr_entry", builder) },
    fr_entry_button{ Common::get_widget<Gtk::Button>("fr_entry_button", builder) },
    fr_preview{ Common::get_widget<Gtk::TextView>("fr_preview", builder) },
    cb_read{ Common::get_widget<Gtk::CheckButton>("cb_read", builder) },
    cb_write{ Common::get_widget<Gtk::CheckButton>("cb_write", builder) },
    cb_exec{ Common::get_widget<Gtk::CheckButton>("cb_exec", builder) },
    cb_link{ Common::get_widget<Gtk::CheckButton>("cb_link", builder) },
    cb_lock{ Common::get_widget<Gtk::CheckButton>("cb_lock", builder) }, 
    cb_deny{ Common::get_widget<Gtk::CheckButton>("cb_deny", builder) },
    cb_owner{ Common::get_widget<Gtk::CheckButton>("cb_owner", builder) },
    cb_audit{ Common::get_widget<Gtk::CheckButton>("cb_audit", builder) }
{
    // Connect the buttons
    auto handle_button_cancel = sigc::mem_fun(*this, &AddFileRule::handle_button_cancel);
    auto handle_button_next   = sigc::mem_fun(*this, &AddFileRule::handle_button_next);
    auto handle_button_back   = sigc::mem_fun(*this, &AddFileRule::handle_button_back);
    auto handle_button_accept = sigc::mem_fun(*this, &AddFileRule::handle_button_accept);
    button_cancel->signal_clicked().connect(handle_button_cancel);
    button_next->signal_clicked().connect(handle_button_next);
    button_back->signal_clicked().connect(handle_button_back);
    button_accept->signal_clicked().connect(handle_button_accept);

    // Connect the file button
    auto handle_file_button = sigc::mem_fun(*this, &AddFileRule::handle_file_button);
    fr_entry_button->signal_clicked().connect(handle_file_button);

    // If the entry or any of the checkboxes change, call `handle_entry_chancged()`
    auto handle_entry = sigc::mem_fun(*this, &AddFileRule::handle_entry_changed);
    fr_entry->signal_changed().connect(handle_entry);
    cb_read->signal_clicked().connect(handle_entry);
    cb_write->signal_clicked().connect(handle_entry);
    cb_exec->signal_clicked().connect(handle_entry);
    cb_link->signal_clicked().connect(handle_entry);
    cb_lock->signal_clicked().connect(handle_entry);
    cb_deny->signal_clicked().connect(handle_entry);
    cb_owner->signal_clicked().connect(handle_entry);
    cb_audit->signal_clicked().connect(handle_entry);

    handle_entry_changed();

    // Show the dialog
    dialog->show_all();
}

void AddFileRule::handle_button_cancel()
{
    dialog->response(Gtk::RESPONSE_CANCEL);
}

void AddFileRule::handle_button_next()
{
    stack_1->set_visible_child("page1");
    stack_2->set_visible_child("page1");
}

void AddFileRule::handle_button_back()
{
    stack_1->set_visible_child("page0");
    stack_2->set_visible_child("page0");
}

void AddFileRule::handle_button_accept()
{
    dialog->response(Gtk::RESPONSE_ACCEPT);
}

void AddFileRule::handle_file_button()
{
    Gtk::FileChooserDialog dialog("~/");
    dialog.set_create_folders(false);

    auto status = dialog.run();
    if(status == 0)
    {
        auto file = dialog.get_file();
        auto filepath = file->get_uri();
        fr_entry->set_text(filepath);
    }
}

void AddFileRule::handle_entry_changed()
{
    bool audit = cb_audit->get_active();
    bool deny  = cb_deny->get_active();
    bool owner = cb_owner->get_active();
    AppArmor::Tree::PrefixNode prefix(audit, deny, owner);

    bool read  = cb_read->get_active();
    bool write = cb_write->get_active();
    bool append = false;
    bool memory_map = false;
    bool exec  = cb_exec->get_active();
    bool link  = cb_link->get_active();
    bool lock  = cb_lock->get_active();
    std::string exec_mode = exec? (deny? "x" : "cix") : "";
    AppArmor::Tree::FileMode mode(read, write, append, memory_map, link, lock, exec_mode);

    auto text = fr_entry->get_text();
    prospective_rule = AppArmor::Tree::FileRule(text, mode);
    prospective_rule.setPrefix(prefix);

    if(text.empty())
    {
        button_next->set_sensitive(false);
    } else {
        button_next->set_sensitive(true);

        // Populate the "FileRule Preview" TextView
        auto preview_text = prospective_rule.operator std::string();
        fr_preview->get_buffer()->set_text(preview_text);
    }
}

AddFileRule::show_dialog_response AddFileRule::show_dialog()
{
    AddFileRule add_file_rule;
    int response = add_file_rule.dialog->run();
    return show_dialog_response(response, add_file_rule.prospective_rule);
}
