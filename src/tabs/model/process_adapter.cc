#include "process_adapter.h"
#include "database.h"

#include <memory>
#include <regex>
#include <stdexcept>

template<class Database, class ColumnRecord>
EntryIter<ProcessTableEntry> ProcessAdapter<Database, ColumnRecord>::add_row(const std::string &profile_name,
                                                                             const std::string &process_name,
                                                                             const unsigned int &pid,
                                                                             const unsigned int &ppid,
                                                                             const std::string &user,
                                                                             const std::string &status)
{
  ProcessTableEntry entry(process_name, profile_name, pid);

  std::unique_ptr<EntryIter<ProcessTableEntry>> row;
  if (ppid > 0) {
    auto parent_row = col_record->get_parent_by_pid(ppid);
    row = std::make_unique<EntryIter<ProcessTableEntry>>(col_record->new_child_row(entry, parent_row));
  } else {
    row = std::make_unique<EntryIter<ProcessTableEntry>>(col_record->new_row(entry));
  }

  (*row)->set_value(0, process_name); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  (*row)->set_value(1, user);         // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  (*row)->set_value(2, pid);          // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  (*row)->set_value(3, status);       // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

  return *row;
}

template<class Database, class ColumnRecord>
void
ProcessAdapter<Database, ColumnRecord>::put_data(const std::string &process_name,
                                                 const std::string &profile_name,
                                                 const unsigned int &pid,
                                                 const unsigned int &ppid,
                                                 const std::string &user,
                                                 const std::string &status)
{
  // Attempt to find an map with this profile name
  auto map_pair = db->process_data.find(profile_name);

  // The map (indexed by pid) that we will add to
  std::map<uint, EntryIter<ProcessTableEntry>> pid_map;

  // Check that we actually found the map
  if (map_pair == db->process_data.end()) {
    // Create new map if no previous one was found
    pid_map = std::map<uint, EntryIter<ProcessTableEntry>>();
  } else {
    pid_map = map_pair->second;
  }

  // Attempt to find an entry with this profile
  auto entry_pair = pid_map.find(pid);

  // Check that we actually found the entry
  if (entry_pair != pid_map.end()) {
    // A pre-existing entry was found, so we should modify it
    EntryIter<ProcessTableEntry> iter = entry_pair->second;
    iter.get_entry().process_name     = process_name;
    iter.get_entry().profile_name     = profile_name;

    iter->set_value(1, user);   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    iter->set_value(3, status); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  } else {
    // If not entry was found, we should create one
    auto row = ProcessAdapter<Database, ColumnRecord>::add_row(profile_name, process_name, pid, ppid, user, status);

    // Add the entry to the map
    pid_map.insert({ pid, row });
  }

  // A weird way of updating our profile in the map (because insert_or_assign does not exist with C++11)
  db->process_data.erase(profile_name);
  db->process_data.insert({ profile_name, pid_map });
}

template<class Database, class ColumnRecord>
std::pair<ProcessTableEntry, bool>
ProcessAdapter<Database, ColumnRecord>::get_data(std::string profile_name, const unsigned int &pid)
{
  auto pid_map_iter = db->process_data.find(profile_name);
  if (pid_map_iter != db->process_data.end()) {
    auto pid_map = pid_map_iter->second;
    auto iter    = pid_map.find(pid);
    if (iter != pid_map.end()) {
      // We actually found some data, so return the found data
      return std::pair<ProcessTableEntry, bool>(iter->second.get_entry(), true);
    }
  }

  // If the entry is not found
  return std::pair<ProcessTableEntry, bool>(ProcessTableEntry(), false);
}

template<class Database, class ColumnRecord>
std::shared_ptr<ColumnRecord>
ProcessAdapter<Database, ColumnRecord>::get_col_record()
{
  return col_record;
}

template<class Database, class ColumnRecord>
ProcessAdapter<Database, ColumnRecord>::ProcessAdapter(std::shared_ptr<Database> db,
                                                       const std::shared_ptr<Gtk::TreeView> &view,
                                                       const std::shared_ptr<Gtk::ScrolledWindow> &win)
  : db{ db },
    col_record{ ColumnRecord::create(view, win, col_names) }
{
}

template<class Database, class ColumnRecord>
ProcessAdapter<Database, ColumnRecord>::ProcessAdapter(std::shared_ptr<Database> db, std::shared_ptr<ColumnRecord> mock)
  : db{ db },
    col_record{ mock }
{
}

template class ProcessAdapter<Database, StatusColumnRecord<ProcessTableEntry>>;