/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include "base/AFPlatform.hpp"

// this file is based on https://github.com/Bosma/Scheduler.
// under the MIT license.

namespace ark {

class BadCronExpression : public std::exception
{
public:
    explicit BadCronExpression(const std::string&& msg)
        : msg_(std::move(msg))
    {
    }

    const char* what() const noexcept override
    {
        return (msg_.c_str());
    }

private:
    std::string msg_;
};

inline void add(std::tm& tm, std::chrono::system_clock::duration time)
{
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    auto tp_adjusted = tp + time;
    auto tm_adjusted = std::chrono::system_clock::to_time_t(tp_adjusted);
    tm = *std::localtime(&tm_adjusted);
}

class AFCron
{
public:
    using Clock = std::chrono::system_clock;

    explicit AFCron(const std::string& expression)
    {
        std::istringstream iss(expression);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

        if (tokens.size() != 5)
            throw BadCronExpression("malformed cron string (must be 5 fields): " + expression);

        verify_and_set(tokens[0], expression, minute, 0, 59);
        verify_and_set(tokens[1], expression, hour, 0, 23);
        verify_and_set(tokens[2], expression, day, 1, 31);
        verify_and_set(tokens[3], expression, month, 1, 12, true);
        verify_and_set(tokens[4], expression, day_of_week, 0, 6);
    }

    // http://stackoverflow.com/a/322058/1284550
    Clock::time_point cron_to_next(const Clock::time_point from = Clock::now()) const
    {
        // get current time as a tm object
        auto now = Clock::to_time_t(from);
        std::tm next(*std::localtime(&now));
        // it will always at least run the next minute
        next.tm_sec = 0;
        add(next, std::chrono::minutes(1));
        while (true)
        {
            if (month != -1 && next.tm_mon != month)
            {
                // add a month
                // if this will bring us over a year, increment the year instead and reset the month
                if (next.tm_mon + 1 > 11)
                {
                    next.tm_mon = 0;
                    next.tm_year++;
                }
                else
                {
                    next.tm_mon++;
                }

                next.tm_mday = 1;
                next.tm_hour = 0;
                next.tm_min = 0;
                continue;
            }

            if (day != -1 && next.tm_mday != day)
            {
                add(next, std::chrono::hours(24));
                next.tm_hour = 0;
                next.tm_min = 0;
                continue;
            }

            if (day_of_week != -1 && next.tm_wday != day_of_week)
            {
                add(next, std::chrono::hours(24));
                next.tm_hour = 0;
                next.tm_min = 0;
                continue;
            }

            if (hour != -1 && next.tm_hour != hour)
            {
                add(next, std::chrono::hours(1));
                next.tm_min = 0;
                continue;
            }

            if (minute != -1 && next.tm_min != minute)
            {
                add(next, std::chrono::minutes(1));
                continue;
            }

            break;
        }

        // telling mktime to figure out dst
        next.tm_isdst = -1;
        return Clock::from_time_t(std::mktime(&next));
    }

    int minute, hour, day, month, day_of_week;

private:
    inline void verify_and_set(const std::string& token, const std::string& expression, int& field,
        const int lower_bound, const int upper_bound, const bool adjust = false)
    {
        if (token == "*")
        {
            field = -1;
        }
        else
        {
            try
            {
                field = std::stoi(token);
            }
            catch (const std::invalid_argument&)
            {
                throw BadCronExpression("malformed cron string (`" + token + "` not an integer or *): " + expression);
            }
            catch (const std::out_of_range&)
            {
                throw BadCronExpression(
                    "malformed cron string (`" + token + "` not convertable to int): " + expression);
            }

            if (field < lower_bound || field > upper_bound)
            {
                std::ostringstream oss;
                oss << "malformed cron string ('" << token << "' must be <= " << upper_bound
                    << " and >= " << lower_bound << "): " << expression;
                throw BadCronExpression(oss.str());
            }

            if (adjust)
            {
                field--;
            }
        }
    }
};
} // namespace ark
