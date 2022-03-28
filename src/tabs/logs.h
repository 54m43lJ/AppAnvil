#ifndef TABS_LOGS_H
#define TABS_LOGS_H

#include "jsoncpp/json/json.h"
#include "status.h"

#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <time.h>
#include <vector>

template<class ColumnRecord> class Logs : public Status
{
public:
  // For production
  Logs();
  // For unit testing
  explicit Logs(std::shared_ptr<ColumnRecord> column_record);

  virtual void add_data_to_record(const std::string &data);
  static void add_row_from_json(const std::shared_ptr<ColumnRecord> &col_record, const Json::Value &entry);
  static std::string format_log_data(std::string &data);
  static std::string format_timestamp(const time_t &timestamp);
  void refresh();

protected:
  struct LogData {
    time_t timestamp;
    std::string type;
    std::string operation;
    std::string name;
    std::string pid;
    std::string status;

    LogData(time_t a, std::string b, std::string c, std::string d, std::string e, std::string f)
        : timestamp{a}, type{b}, operation{c}, name{d}, pid{e}, status{f} { }
  };


private:
  const std::vector<std::string> col_names{"Time", "Type", "Operation", "Name", "Pid", "Status"};
  std::shared_ptr<ColumnRecord> col_record;
};

#include "logs.inl"

#endif // TABS_LOGS_H
