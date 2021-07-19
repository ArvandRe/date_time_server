#include <boost/date_time/local_time/local_time.hpp>
#include <sstream>
#include "date_time.h"

namespace DateTime {

std::optional<boost::posix_time::time_duration> GetOffset(const boost::local_time::tz_database &tz_db, std::string_view tz_name) {
	for (const auto& region : tz_db.region_list()) {
		const auto tz = tz_db.time_zone_from_region(region);	
		if (tz_name == tz->std_zone_abbrev()) {
			return tz->base_utc_offset();
		}
	}
	return std::nullopt;
}

std::optional<std::string> GetDateTimeWithTz(const boost::local_time::tz_database &tz_db, std::string_view tz_name) {
	const auto offset = GetOffset(tz_db, tz_name);
	if (!offset) {
		return std::nullopt;
	}
	const auto datetime = boost::posix_time::second_clock::universal_time();
	const auto date = datetime.date();

	std::stringstream datetime_tz;
	datetime_tz << date.day_of_week() << ' ' << date.month() << ' ' << date.day() << ' '
				<< datetime.time_of_day() + offset.value() << ' ' << tz_name << ' ' << date.year();
	return datetime_tz.str();
}

}
