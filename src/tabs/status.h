#ifndef TABS_STATUS_H
#define TABS_STATUS_H

#include "jsoncpp/json/json.h"
#include "status_column_record.h"

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/combobox.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/spinner.h>
#include <gtkmm/treeview.h>

#include "gtest/gtest.h"

constexpr auto UNKNOWN_STATUS = "unknown";

class Status : public Gtk::ScrolledWindow
{
  public:
    Status();
    
  protected:
    /**
     * @brief Decide whether a string should be added to the table
     * 
     * @details
     * Takes a string as input and decides whether it should be added the table.
     * Uses the text in the searchbar and the state of the checkboxes to determine
     * whether the string can be added.
     *  
     * @returns `true` if the string should be added, `false` if it should not
     */

    FRIEND_TEST(STATUS, FILTER_FFF); // Gtest get protected methods added by Zixin Yang
    FRIEND_TEST(STATUS, FILTER_TFF);
    FRIEND_TEST(STATUS, FILTER_FTF);
    FRIEND_TEST(STATUS, FILTER_FFT);
    FRIEND_TEST(STATUS, FILTER_TTF);
    FRIEND_TEST(STATUS, FILTER_TFT);
    FRIEND_TEST(STATUS, FILTER_FTT);
    FRIEND_TEST(STATUS, FILTER_TTT);
    FRIEND_TEST(STATUS, FILTER_REGEX_TOLOWER);


    bool filter(const Gtk::TreeModel::iterator& node);

    /**
     * @brief Change the text in the label directly above the searchbar.
     */
    void set_status_label_text(const std::string& str);

    /**
     * @brief Set the method to be called every time the search area updated.
     * 
     * @details
     * Sets the signal handler method to be called every time the search area is updated. This
     * happens when the user clicks a checkbutton, or types into the searchbar.
     * 
     * The signal handler should have the following protoype: `void method_name()`
     */
    void set_refresh_signal_handler(const Glib::SignalProxyProperty::SlotType& func);

    /**
     * @brief Set the method to be called every time the apply button is clicked.
     * 
     * @details
     * Sets the signal handler method to be called every time the apply button is clicked. This
     * happens when the user clicks the 'Apply' button above the search bar.
     * 
     * The signal handler should have the following protoype: `void method_name()`
     */
    void set_apply_signal_handler(const Glib::SignalProxyProperty::SlotType& func);

    /**
     * @brief Return the TreeView associated with this class.
     * 
     * @returns The TreeView data member used by this class.
     */
    std::shared_ptr<Gtk::TreeView> get_view();

    /**
     * @brief Make widgets related to changing profile status invisible to the user.
     * 
     * @details
     * Removes the widgets associated with changing a profiles status. This cannot be undone.
     * Widgets that are children of `s_selection_box` will be permanently invisible to the user
     * for this instance of Status.
     */
    void remove_status_selection();

    // Should probably add a description!
    static Json::Value parse_JSON(const std::string& raw_json);

  private:
    // GUI Builder to parse UI from xml file
    Glib::RefPtr<Gtk::Builder> builder;

    // Container Widgets
    std::shared_ptr<Gtk::TreeView> s_view;
    std::shared_ptr<Gtk::TreeSelection> s_row;
    std::unique_ptr<Gtk::ScrolledWindow> s_win;
    std::unique_ptr<Gtk::Box> s_box;

    // Widgets related to searchbar
    std::unique_ptr<Gtk::SearchEntry> s_search;
    std::unique_ptr<Gtk::CheckButton> s_use_regex;
    std::unique_ptr<Gtk::CheckButton> s_match_case;
    std::unique_ptr<Gtk::CheckButton> s_whole_word;
    std::unique_ptr<Gtk::Label>       s_found_label;

    // Widgets related to changing profile status (above search)
    std::unique_ptr<Gtk::Box>         s_selection_box;
    std::unique_ptr<Gtk::Button>      s_apply_button;
    std::unique_ptr<Gtk::Spinner>     s_spinner;
    std::unique_ptr<Gtk::Label>       s_apply_info_text;
    std::unique_ptr<Gtk::ComboBox>    s_status_selection;

    template <typename T_Widget>
    static std::unique_ptr<T_Widget> get_widget(Glib::ustring name, const Glib::RefPtr<Gtk::Builder>& builder);    
    static bool filter(const std::string& str, const std::string& rule, const bool& use_regex, const bool& match_case, const bool& whole_word);
};

#endif // TABS_STATUS_H
