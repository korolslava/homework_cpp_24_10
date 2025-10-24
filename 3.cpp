#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
using namespace std;
using json = nlohmann::json;

struct Course {
    string title;
    int credits;
    Course() = default;
    Course(const string& t, int c) : title(t), credits(c) {}
};

struct Student {
    string name;
    int year;
    vector<Course> courses;
    Student() = default;
    Student(const string& n, int y) : name(n), year(y) {}
};

struct University {
    string name;
    map<string, Student> students;
    University() = default;
    University(const string& n) : name(n) {}
};

void to_json(json& j, const Course& c) {
    j = json{{"title", c.title}, {"credits", c.credits}};
}

void from_json(const json& j, Course& c) {
    c.title = j.at("title").get<string>();
    c.credits = j.at("credits").get<int>();
}


void to_json(json& j, const Student& s) {
    j = json{{"year", s.year}, {"courses", s.courses}};
}

void from_json(const json& j, Student& s) {
    s.year = j.at("year").get<int>();
    s.courses = j.at("courses").get<vector<Course>>();
}

int total_credits(const Student& s) {
    int sum = 0;
    for (const auto& c : s.courses) sum += c.credits;
    return sum;
}

bool save_university_json(const University& uni, const string& filename) {
    json j;
    j["name"] = uni.name;
    j["students"] = json::object();
    for (const auto& kv : uni.students) {
        const string& studentName = kv.first;
        const Student& s = kv.second;
        json sj;
        sj["year"] = s.year;
        sj["courses"] = s.courses;
        j["students"][studentName] = sj;
    }
    ofstream out(filename);
    if (!out.is_open()) return false;
    out << j.dump(4);
    out.close();
    return true;
}

int main()
{
    University uni("Lviv Polytechnic");

    Student andriy("Andriy", 2);
    andriy.courses.push_back(Course("Algorithms", 5));
    andriy.courses.push_back(Course("Databases", 4));

    Student ira("Ira", 1);
    ira.courses.push_back(Course("Math", 5));

    uni.students[andriy.name] = andriy;
    uni.students[ira.name] = ira;

    cout << "Students and their total credits:\n";
    for (const auto& kv : uni.students) {
        const string& name = kv.first;
        const Student& s = kv.second;
        cout << name << ": " << total_credits(s) << " credits\n";
    }

    uni.students["Ira"].courses.push_back(Course("Programming", 3));

    if (save_university_json(uni, "file.json")) {
        cout << "Saved updated university to file.json" << endl;
    } else {
        cerr << "Failed to save file.json" << endl;
    }

    return 0;
}