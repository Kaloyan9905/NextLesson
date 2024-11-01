#include <iostream>
#include <map>
#include <chrono>
#include <ctime>
#include <tuple>
#include <string>
#include <unordered_map>

using namespace std;
using namespace chrono;

map<string, vector<pair<string, pair<string, string> > > > universitySchedule() {
    map<string, vector<pair<string, pair<string, string> > > > schedule;

    schedule["Monday"] = {};

    schedule["Tuesday"] = {
        {"CS Basics", {"13:00", "14:30"}},
        {"Lab Exercise", {"14:40", "16:10"}}
    };

    schedule["Wednesday"] = {
        {"The Bulgarian National Revival", {"13:00", "14:30"}},
        {"Chess", {"14:40", "16:10"}},
        {"English", {"19:40", "21:10"}}
    };

    schedule["Thursday"] = {
        {"C++ Exercise", {"08:00", "09:30"}}
    };

    schedule["Friday"] = {
        {"Linear Algebra", {"09:40", "11:10"}},
        {"Discrete Mathematics", {"11:20", "12:50"}},
        {"Bulgarian - Academic speaking and writing", {"13:00", "14:30"}},
        {"C++ Lab", {"14:40", "16:10"}},
        {"English", {"18:00", "21:10"}}
    };

    schedule["Saturday"] = {};
    schedule["Sunday"] = {};

    return schedule;
}


string getDayOfWeek(int wday) {
    string daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    return daysOfWeek[wday];
}

string getNextDay(const string &currentDay) {
    string daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    pmr::unordered_map<string, int> dayIndex = {
        {"Sunday", 0},
        {"Monday", 1},
        {"Tuesday", 2},
        {"Wednesday", 3},
        {"Thursday", 4},
        {"Friday", 5},
        {"Saturday", 6}
    };

    const int index = (dayIndex[currentDay] + 1) % 7;

    return daysOfWeek[index];
}

int timeToMinutes(const string &time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));

    return hours * 60 + minutes;
}

tuple<string, int, int> getDayHourMinute() {
    const auto now = system_clock::now();
    const time_t currentTime = system_clock::to_time_t(now);
    const tm *localTime = localtime(&currentTime);

    string dayOfWeek = getDayOfWeek(localTime->tm_wday);

    int hours = localTime->tm_hour;
    int minutes = localTime->tm_min;

    return make_tuple(dayOfWeek, hours, minutes);
}

string nextLesson() {
    auto [day, hour, minute] = getDayHourMinute();
    // string day = "Friday";
    // string time = "11:11";

    const string time = format("{:02}:{}", hour, minute);

    auto schedule = universitySchedule();

    for (const auto &[subject, times]: schedule[day]) {
        auto [startTime, endTime] = times;

        const int startTimeMinutes = timeToMinutes(startTime);
        const int endTimeMinutes = timeToMinutes(endTime);
        const int currentTimeMinutes = timeToMinutes(time);

        if (currentTimeMinutes >= startTimeMinutes && currentTimeMinutes <= endTimeMinutes) {
            return "There is currently ongoing lesson - " + subject;
        }

        if (currentTimeMinutes < startTimeMinutes) {
            return subject + " - starts on " += day;
        }
    }

    do {
        day = getNextDay(day);
    } while (schedule[day].empty());

    const auto object = schedule[day].begin();
    const string subject = object->first;

    cout << subject << endl;

    return subject + " - starts on " += day;
}
