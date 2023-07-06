//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/core_functions/scalar/date_functions.hpp
//
//
//===----------------------------------------------------------------------===//
// This file is generated by scripts/generate_functions.py

#pragma once

#include "duckdb/function/function_set.hpp"

namespace duckdb {

struct AgeFun {
	static constexpr const char *Name = "age";
	static constexpr const char *Parameters = "timestamp,timestamp";
	static constexpr const char *Description = "Subtract arguments, resulting in the time difference between the two timestamps";
	static constexpr const char *Example = "age(TIMESTAMP '2001-04-10', TIMESTAMP '1992-09-20')";

	static ScalarFunctionSet GetFunctions();
};

struct CenturyFun {
	static constexpr const char *Name = "century";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the century component from a date or timestamp";
	static constexpr const char *Example = "century(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct CurrentDateFun {
	static constexpr const char *Name = "current_date";
	static constexpr const char *Parameters = "";
	static constexpr const char *Description = "Returns the current date";
	static constexpr const char *Example = "current_date()";

	static ScalarFunction GetFunction();
};

struct TodayFun {
	using ALIAS = CurrentDateFun;

	static constexpr const char *Name = "today";
};

struct DateDiffFun {
	static constexpr const char *Name = "date_diff";
	static constexpr const char *Parameters = "part,startdate,enddate";
	static constexpr const char *Description = "The number of partition boundaries between the timestamps";
	static constexpr const char *Example = "date_diff('hour', TIMESTAMPTZ '1992-09-30 23:59:59', TIMESTAMPTZ '1992-10-01 01:58:00')";

	static ScalarFunctionSet GetFunctions();
};

struct DatediffFun {
	using ALIAS = DateDiffFun;

	static constexpr const char *Name = "datediff";
};

struct DatePartFun {
	static constexpr const char *Name = "date_part";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Get subfield (equivalent to extract)";
	static constexpr const char *Example = "date_part('minute', TIMESTAMP '1992-09-20 20:38:40')";

	static ScalarFunctionSet GetFunctions();
};

struct DatepartFun {
	using ALIAS = DatePartFun;

	static constexpr const char *Name = "datepart";
};

struct DateSubFun {
	static constexpr const char *Name = "date_sub";
	static constexpr const char *Parameters = "part,startdate,enddate";
	static constexpr const char *Description = "The number of complete partitions between the timestamps";
	static constexpr const char *Example = "date_sub('hour', TIMESTAMPTZ '1992-09-30 23:59:59', TIMESTAMPTZ '1992-10-01 01:58:00')";

	static ScalarFunctionSet GetFunctions();
};

struct DatesubFun {
	using ALIAS = DateSubFun;

	static constexpr const char *Name = "datesub";
};

struct DateTruncFun {
	static constexpr const char *Name = "date_trunc";
	static constexpr const char *Parameters = "part,timestamp";
	static constexpr const char *Description = "Truncate to specified precision";
	static constexpr const char *Example = "date_trunc('hour', TIMESTAMPTZ '1992-09-20 20:38:40')";

	static ScalarFunctionSet GetFunctions();
};

struct DatetruncFun {
	using ALIAS = DateTruncFun;

	static constexpr const char *Name = "datetrunc";
};

struct DayFun {
	static constexpr const char *Name = "day";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the day component from a date or timestamp";
	static constexpr const char *Example = "day(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct DayNameFun {
	static constexpr const char *Name = "dayname";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "The (English) name of the weekday.";
	static constexpr const char *Example = "dayname(TIMESTAMP '1992-03-22')";

	static ScalarFunctionSet GetFunctions();
};

struct DayOfMonthFun {
	static constexpr const char *Name = "dayofmonth";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the dayofmonth component from a date or timestamp";
	static constexpr const char *Example = "dayofmonth(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct DayOfWeekFun {
	static constexpr const char *Name = "dayofweek";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the dayofweek component from a date or timestamp";
	static constexpr const char *Example = "dayofweek(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct DayOfYearFun {
	static constexpr const char *Name = "dayofyear";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the dayofyear component from a date or timestamp";
	static constexpr const char *Example = "dayofyear(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct DecadeFun {
	static constexpr const char *Name = "decade";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the decade component from a date or timestamp";
	static constexpr const char *Example = "decade(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct EpochFun {
	static constexpr const char *Name = "epoch";
	static constexpr const char *Parameters = "temporal";
	static constexpr const char *Description = "Extract the epoch component from a temporal type";
	static constexpr const char *Example = "epoch(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct EpochMsFun {
	static constexpr const char *Name = "epoch_ms";
	static constexpr const char *Parameters = "temporal";
	static constexpr const char *Description = "Extract the epoch component in milliseconds from a temporal type";
	static constexpr const char *Example = "epoch_ms(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct EpochUsFun {
	static constexpr const char *Name = "epoch_us";
	static constexpr const char *Parameters = "temporal";
	static constexpr const char *Description = "Extract the epoch component in microseconds from a temporal type";
	static constexpr const char *Example = "epoch_us(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct EpochNsFun {
	static constexpr const char *Name = "epoch_ns";
	static constexpr const char *Parameters = "temporal";
	static constexpr const char *Description = "Extract the epoch component in nanoseconds from a temporal type";
	static constexpr const char *Example = "epoch_ns(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct EraFun {
	static constexpr const char *Name = "era";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the era component from a date or timestamp";
	static constexpr const char *Example = "era(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct CurrentTimeFun {
	static constexpr const char *Name = "get_current_time";
	static constexpr const char *Parameters = "";
	static constexpr const char *Description = "Returns the current time";
	static constexpr const char *Example = "get_current_time()";

	static ScalarFunction GetFunction();
};

struct GetCurrentTimestampFun {
	static constexpr const char *Name = "get_current_timestamp";
	static constexpr const char *Parameters = "";
	static constexpr const char *Description = "Returns the current timestamp";
	static constexpr const char *Example = "get_current_timestamp()";

	static ScalarFunction GetFunction();
};

struct NowFun {
	using ALIAS = GetCurrentTimestampFun;

	static constexpr const char *Name = "now";
};

struct TransactionTimestampFun {
	using ALIAS = GetCurrentTimestampFun;

	static constexpr const char *Name = "transaction_timestamp";
};

struct HoursFun {
	static constexpr const char *Name = "hour";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the hour component from a date or timestamp";
	static constexpr const char *Example = "hour(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct ISODayOfWeekFun {
	static constexpr const char *Name = "isodow";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the isodow component from a date or timestamp";
	static constexpr const char *Example = "isodow(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct ISOYearFun {
	static constexpr const char *Name = "isoyear";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the isoyear component from a date or timestamp";
	static constexpr const char *Example = "isoyear(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct LastDayFun {
	static constexpr const char *Name = "last_day";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Returns the last day of the month";
	static constexpr const char *Example = "last_day(TIMESTAMP '1992-03-22 01:02:03.1234')";

	static ScalarFunctionSet GetFunctions();
};

struct MakeDateFun {
	static constexpr const char *Name = "make_date";
	static constexpr const char *Parameters = "year,month,day";
	static constexpr const char *Description = "The date for the given parts";
	static constexpr const char *Example = "make_date(1992, 9, 20)";

	static ScalarFunctionSet GetFunctions();
};

struct MakeTimeFun {
	static constexpr const char *Name = "make_time";
	static constexpr const char *Parameters = "hour,minute,seconds";
	static constexpr const char *Description = "The time for the given parts";
	static constexpr const char *Example = "make_time(13, 34, 27.123456)";

	static ScalarFunction GetFunction();
};

struct MakeTimestampFun {
	static constexpr const char *Name = "make_timestamp";
	static constexpr const char *Parameters = "year,month,day,hour,minute,seconds";
	static constexpr const char *Description = "The timestamp for the given parts";
	static constexpr const char *Example = "make_timestamp(1992, 9, 20, 13, 34, 27.123456)";

	static ScalarFunctionSet GetFunctions();
};

struct MicrosecondsFun {
	static constexpr const char *Name = "microsecond";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the microsecond component from a date or timestamp";
	static constexpr const char *Example = "microsecond(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct MillenniumFun {
	static constexpr const char *Name = "millennium";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the millennium component from a date or timestamp";
	static constexpr const char *Example = "millennium(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct MillisecondsFun {
	static constexpr const char *Name = "millisecond";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the millisecond component from a date or timestamp";
	static constexpr const char *Example = "millisecond(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct MinutesFun {
	static constexpr const char *Name = "minute";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the minute component from a date or timestamp";
	static constexpr const char *Example = "minute(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct MonthFun {
	static constexpr const char *Name = "month";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the month component from a date or timestamp";
	static constexpr const char *Example = "month(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct MonthNameFun {
	static constexpr const char *Name = "monthname";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "The (English) name of the month.";
	static constexpr const char *Example = "monthname(TIMESTAMP '1992-09-20')";

	static ScalarFunctionSet GetFunctions();
};

struct QuarterFun {
	static constexpr const char *Name = "quarter";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the quarter component from a date or timestamp";
	static constexpr const char *Example = "quarter(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct SecondsFun {
	static constexpr const char *Name = "second";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the second component from a date or timestamp";
	static constexpr const char *Example = "second(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct StrfTimeFun {
	static constexpr const char *Name = "strftime";
	static constexpr const char *Parameters = "text,format";
	static constexpr const char *Description = "Converts timestamp to string according to the format string";
	static constexpr const char *Example = "strftime(timestamp '1992-01-01 20:38:40', '%a, %-d %B %Y - %I:%M:%S %p')";

	static ScalarFunctionSet GetFunctions();
};

struct StrpTimeFun {
	static constexpr const char *Name = "strptime";
	static constexpr const char *Parameters = "text,format";
	static constexpr const char *Description = "Converts string to timestamp with time zone according to the format string if %Z is specified.";
	static constexpr const char *Example = "strptime('Wed, 1 January 1992 - 08:38:40 PST', '%a, %-d %B %Y - %H:%M:%S %Z')";

	static ScalarFunctionSet GetFunctions();
};

struct TimeBucketFun {
	static constexpr const char *Name = "time_bucket";
	static constexpr const char *Parameters = "bucket_width,timestamp,origin";
	static constexpr const char *Description = "Truncate timestamptz by the specified interval bucket_width. Buckets are aligned relative to origin timestamptz. origin defaults to 2000-01-03 00:00:00+00 for buckets that don’t include a month or year interval, and to 2000-01-01 00:00:00+00 for month and year buckets.";
	static constexpr const char *Example = "time_bucket(INTERVAL '2 weeks', TIMESTAMP '1992-04-20 15:26:00-07', TIMESTAMP '1992-04-01 00:00:00-07')";

	static ScalarFunctionSet GetFunctions();
};

struct TimezoneFun {
	static constexpr const char *Name = "timezone";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the timezone component from a date or timestamp";
	static constexpr const char *Example = "timezone(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct TimezoneHourFun {
	static constexpr const char *Name = "timezone_hour";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the timezone_hour component from a date or timestamp";
	static constexpr const char *Example = "timezone_hour(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct TimezoneMinuteFun {
	static constexpr const char *Name = "timezone_minute";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the timezone_minute component from a date or timestamp";
	static constexpr const char *Example = "timezone_minute(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct ToDaysFun {
	static constexpr const char *Name = "to_days";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a day interval";
	static constexpr const char *Example = "to_days(5)";

	static ScalarFunction GetFunction();
};

struct ToHoursFun {
	static constexpr const char *Name = "to_hours";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a hour interval";
	static constexpr const char *Example = "to_hours(5)";

	static ScalarFunction GetFunction();
};

struct ToMicrosecondsFun {
	static constexpr const char *Name = "to_microseconds";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a microsecond interval";
	static constexpr const char *Example = "to_microseconds(5)";

	static ScalarFunction GetFunction();
};

struct ToMillisecondsFun {
	static constexpr const char *Name = "to_milliseconds";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a millisecond interval";
	static constexpr const char *Example = "to_milliseconds(5)";

	static ScalarFunction GetFunction();
};

struct ToMinutesFun {
	static constexpr const char *Name = "to_minutes";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a minute interval";
	static constexpr const char *Example = "to_minutes(5)";

	static ScalarFunction GetFunction();
};

struct ToMonthsFun {
	static constexpr const char *Name = "to_months";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a month interval";
	static constexpr const char *Example = "to_months(5)";

	static ScalarFunction GetFunction();
};

struct ToSecondsFun {
	static constexpr const char *Name = "to_seconds";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a second interval";
	static constexpr const char *Example = "to_seconds(5)";

	static ScalarFunction GetFunction();
};

struct ToTimestampFun {
	static constexpr const char *Name = "to_timestamp";
	static constexpr const char *Parameters = "sec";
	static constexpr const char *Description = "Converts secs since epoch to a timestamp with time zone";
	static constexpr const char *Example = "to_timestamp(1284352323.5)";

	static ScalarFunction GetFunction();
};

struct ToYearsFun {
	static constexpr const char *Name = "to_years";
	static constexpr const char *Parameters = "integer";
	static constexpr const char *Description = "Construct a year interval";
	static constexpr const char *Example = "to_years(5)";

	static ScalarFunction GetFunction();
};

struct TryStrpTimeFun {
	static constexpr const char *Name = "try_strptime";
	static constexpr const char *Parameters = "text,format";
	static constexpr const char *Description = "Converts string to timestamp using the format string (timestamp with time zone if %Z is specified). Returns NULL on failure.";
	static constexpr const char *Example = "try_strptime('Wed, 1 January 1992 - 08:38:40 PM', '%a, %-d %B %Y - %I:%M:%S %p')";

	static ScalarFunctionSet GetFunctions();
};

struct WeekFun {
	static constexpr const char *Name = "week";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the week component from a date or timestamp";
	static constexpr const char *Example = "week(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct WeekDayFun {
	static constexpr const char *Name = "weekday";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the weekday component from a date or timestamp";
	static constexpr const char *Example = "weekday(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct WeekOfYearFun {
	static constexpr const char *Name = "weekofyear";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the weekofyear component from a date or timestamp";
	static constexpr const char *Example = "weekofyear(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct YearFun {
	static constexpr const char *Name = "year";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the year component from a date or timestamp";
	static constexpr const char *Example = "year(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

struct YearWeekFun {
	static constexpr const char *Name = "yearweek";
	static constexpr const char *Parameters = "ts";
	static constexpr const char *Description = "Extract the yearweek component from a date or timestamp";
	static constexpr const char *Example = "yearweek(timestamp '2021-08-03 11:59:44.123456')";

	static ScalarFunctionSet GetFunctions();
};

} // namespace duckdb
