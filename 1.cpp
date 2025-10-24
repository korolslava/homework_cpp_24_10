#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class Book {
	public:
		string title;
		string author;
		int year;
		Book() = default;
		Book(const string& t, const string& a, int y) : title(t), author(a), year(y) {}
};

void to_json(json& j, const Book& b) {
    j = json{{"title", b.title}, {"author", b.author}, {"year", b.year}};
}

void from_json(const json& j, Book& b) {
    b.title = j.at("title").get<string>();
    b.author = j.at("author").get<string>();
    b.year = j.at("year").get<int>();
}

int main()
{
	vector<Book> library = {
		Book("1984", "George Orwell", 1949),
		Book("To Kill a Mockingbird", "Harper Lee", 1960),
		Book("The Great Gatsby", "F. Scott Fitzgerald", 1925)
	};
	ofstream outFile("file.json");
	json jLibrary = json::array();
	for (const auto& book : library) {
		json jBook;
		jBook["title"] = book.title;
		jBook["author"] = book.author;
		jBook["year"] = book.year;
		jLibrary.push_back(jBook);
	}
	outFile << jLibrary.dump(4);
	outFile.close();

	ifstream inFile("file.json");
	if (!inFile.is_open()) {
		cerr << "Error opening file!" << endl;
		return 1;
	}
	json jLoaded;
	inFile >> jLoaded;

	vector<Book> loadedLibrary = jLoaded.get<vector<Book>>();

	cout << "Loaded Books:" << endl;
	for (const auto& book : loadedLibrary) {
		cout << "Title: " << book.title << ", Author: " << book.author << ", Year: " << book.year << endl;
	}
	inFile.close();
	return 0;
}
