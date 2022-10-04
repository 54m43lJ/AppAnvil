#ifndef TABS_MODEL_DATABASE_H
#define TABS_MODEL_DATABASE_H

#include <gtkmm/treeiter.h>
#include <map>
#include <memory>
#include <string>

#include "../column_header.h"
#include "../entries.h"
#include "entry_iter.h"
#include "log_adapter.h"
#include "process_adapter.h"
#include "profile_adapter.h"
#include "status_column_record.h"

class Database
{
public:
  // Initializes the database
  Database() = default;

  // The number of processes at this profile
  // TODO(database): Maybe we can store and retrieve this data in the profile table entry instead?
  uint get_number_processes(const std::string &profile);

  // The number of processes at this profile
  // TODO(database): Maybe we can store and retrieve this data in the profile table entry instead?
  uint get_number_logs(const std::string &profile);

private:
  std::map<std::string, EntryIter<ProfileTableEntry>> profile_data;

  // Indexed by profile, returns a map of procesess
  // Each process in the returned map is indexed by the pid it was added
  std::map<std::string, std::map<uint, EntryIter<ProcessTableEntry>>> process_data;

  // Indexed by profile, returns a map of logs
  // Each log in the returned map is indexed by the time it was added
  std::map<std::string, std::map<time_t, EntryIter<LogTableEntry>>> log_data;

  friend class ProfileAdapter<Database>;
  friend class ProcessAdapter<Database, StatusColumnRecord<ProcessTableEntry>>;
  friend class LogAdapter<Database, StatusColumnRecord<LogTableEntry>>;
};

#endif // TABS_MODEL_DATABASE_H
