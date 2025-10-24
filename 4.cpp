#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
#include <set>
#include <list>
#include <string>
using namespace std;
using json = nlohmann::json;

struct Task {
    int id;
    string title;
    bool completed;
    Task(int i, const string& t, bool c) : id(i), title(t), completed(c) {}
};

class TodoList {
private:
    list<Task> tasks;
    set<int> usedIds;
public:
    int AddTask(const string& title) {
        int newId = 1;
        while (usedIds.count(newId)) {
            newId++;
        }
        tasks.emplace_back(newId, title, false);
        usedIds.insert(newId);
        return newId;
    }
    bool completeTask(int id) {
        for (auto& task : tasks) {
            if (task.id == id) {
                task.completed = true;
                return true;
            }
        }
        return false;
    }

    bool removeTask(int id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == id) {
                tasks.erase(it);
                usedIds.erase(id);
                return true;
            }
        }
        return false;
    }

    void saveToJson(const string& path) const {
        json j = json::array();
        for (const auto& task : tasks) {
            j.push_back({
                {"id", task.id},
                {"title", task.title},
                {"completed", task.completed}
            });
        }

        ofstream ofs(path);
        if (!ofs) {
            cerr << "Failed to open file for writing: " << path << '\n';
            return;
        }
        ofs << j.dump(4);
    }

    void loadFromJson(const string& path) {
        ifstream ifs(path);
        if (!ifs) {
            return;
        }

        json j;
        try {
            ifs >> j;
        } catch (const json::parse_error& e) {
            cerr << "JSON parse error: " << e.what() << '\n';
            return;
        }

        if (!j.is_array()) {
            cerr << "JSON root is not an array in file: " << path << '\n';
            return;
        }

        tasks.clear();
        usedIds.clear();
        for (const auto& item : j) {
            if (!item.contains("id") || !item.contains("title") || !item.contains("completed")) {
                continue;
            }
            int id = item["id"].get<int>();
            string title = item["title"].get<string>();
            bool completed = item["completed"].get<bool>();
            tasks.emplace_back(id, title, completed);
            usedIds.insert(id);
        }
    }

    void ShowAll() const {
        if (tasks.empty()) {
            cout << "No tasks." << endl;
            return;
        }
        cout << "Tasks:" << endl;
        for (const auto& task : tasks) {
            cout << "[" << task.id << "] " << task.title << " - " << (task.completed ? "Done" : "Pending") << '\n';
        }
    }
};

int main()
{
    TodoList todo;
    const string dataFile = "file.json";
    todo.loadFromJson(dataFile);

    while (true) {
        cout << "--- Todo Menu ---" << endl;
        cout << "1) Add task" << endl;
        cout << "2) Complete task" << endl;
        cout << "3) Delete" << endl;
        cout << "4) Show all" << endl;
        cout << "0) Exit" << endl;
        cout << "Choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            string dummy;
            getline(cin, dummy);
            cout << "Invalid input." << endl;
            continue;
        }
        string dummy;
        getline(cin, dummy);

        if (choice == 0) {
            break;
        }
        switch (choice) {
        case 1: {
            cout << "Enter title: ";
            string title;
            getline(cin, title);
            if (title.empty()) {
                cout << "Empty title. Cancelled." << endl;
                break;
            }
            int id = todo.AddTask(title);
            todo.saveToJson(dataFile);
            cout << "Added task with id=" << id << endl;
            break;
        }
        case 2: {
            cout << "Enter task id to mark completed: ";
            int id;
            if (!(cin >> id)) {
                cin.clear();
                string dummy2;
                getline(cin, dummy2);
                cout << "Invalid id." << endl;
                break;
            }
            if (todo.completeTask(id)) {
                todo.saveToJson(dataFile);
                cout << "Task " << id << " marked as completed." << endl;
            }
            else {
                cout << "Task with id=" << id << " not found." << endl;
            }
            string dummy3;
            getline(cin, dummy3);
            break;
        }
        case 3: {
            cout << "Enter task id to delete: ";
            int id;
            if (!(cin >> id)) {
                cin.clear();
                string dummy2;
                getline(cin, dummy2);
                cout << "Invalid id." << endl;
                break;
            }
            if (todo.removeTask(id)) {
                todo.saveToJson(dataFile);
                cout << "Task " << id << " deleted." << endl;
            }
            else {
                cout << "Task with id=" << id << " not found." << endl;
            }
            string dummy3;
            getline(cin, dummy3);
            break;
        }
        case 4: {
            todo.ShowAll();
            break;
        }
        }
    }

    cout << "Exit." << endl;
    return 0;
}