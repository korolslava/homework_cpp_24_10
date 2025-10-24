#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

struct Employee {
    string name;
    int id;
	double salary;
};

class Company {
    public:
    string company_name;
    vector<Employee> employees;
	Company(const string& name) : company_name(name) {}
};

void to_json(json& j, const Employee& e) {
    j = json{{"name", e.name}, {"id", e.id}, {"salary", e.salary}};
}

void from_json(const json& j, Employee& e) {
    j.at("name").get_to(e.name);
    j.at("id").get_to(e.id);
    j.at("salary").get_to(e.salary);
}

void calculateAvgSalary(const Company& company) {
    double totalSalary = 0.0;
    for (const auto& emp : company.employees) {
        totalSalary += emp.salary;
    }
    double avgSalary = totalSalary / company.employees.size();
    cout << "Average Salary in " << company.company_name << ": " << avgSalary << endl;
}

int main()
{
	vector<Employee> employees = {
        {"Alice", 1, 70000.0},
        {"Bob", 2, 80000.0},
        {"Charlie", 3, 90000.0}
	};
    Company myCompany("TechCorp");
    myCompany.employees = employees;

    json j;
    j["company_name"] = myCompany.company_name;
    j["employees"] = myCompany.employees;

    ofstream outFile("file.json");
    outFile << j.dump(4);
    outFile.close();

    ifstream inFile("file.json");
    json j_in;
    inFile >> j_in;
    inFile.close();

    Company loadedCompany(j_in["company_name"].get<string>());
    loadedCompany.employees = j_in["employees"].get<vector<Employee>>();

    cout << "Company Name: " << loadedCompany.company_name << endl;
    for (const auto& emp : loadedCompany.employees) {
        cout << "Employee Name: " << emp.name << ", ID: " << emp.id << ", Salary: " << emp.salary << endl;
	}
	calculateAvgSalary(loadedCompany);
	return 0;
}