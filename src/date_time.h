#include <boost/date_time/local_time/tz_database.hpp>
#include <optional>
#include <string_view>

namespace DateTime {

std::optional<std::string> GetDateTimeWithTz(const boost::local_time::tz_database &tz_db, std::string_view tz_name);

}