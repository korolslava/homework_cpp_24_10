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

class Product {
public:
    int id;
    string name;
    double price;
    set<string> categories;
    Product(int id, string name, double price, set<string> categories)
        : id(id), name(name), price(price), categories(categories) {}

    json to_json() const {
        return {
            {"id", id},
            {"name", name},
            {"price", price},
            {"categories", categories}
        };
    }

    static Product from_json(const json& j) {
        return Product(
            j["id"].get<int>(),
            j["name"].get<string>(),
            j["price"].get<double>(),
            j["categories"].get<set<string>>()
        );
    }
};

class Store {
private:
    vector<Product> products;
    map<int, int> stock;

public:
    Store() = default;
    Store(vector<Product> products, map<int, int> stock)
        : products(products), stock(stock) {}

    void addProduct(const Product& p, int quantity) {
        for (const auto& prod : products) {
            if (prod.id == p.id) {
                stock[p.id] += quantity;
                return;
            }
        }
        products.push_back(p);
        stock[p.id] = quantity;
    }

    vector<Product> findByCategory(const string& cat) const {
        vector<Product> result;
        for (const auto& product : products) {
            if (product.categories.find(cat) != product.categories.end()) {
                result.push_back(product);
            }
        }
        return result;
    }

    bool removeProduct(int id) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if (it->id == id) {
                products.erase(it);
                stock.erase(id);
                return true;
            }
        }
        return false;
    }

    void saveToJson(const string& filename) const {
        json j;
        j["products"] = json::array();
        for (const auto& product : products) {
            j["products"].push_back(product.to_json());
        }
        j["stock"] = stock;

        ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
        } else {
            cerr << "Unable to open file for writing: " << filename << endl;
        }
    }

    bool loadFromJson(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file for reading: " << filename << endl;
            return false;
        }

        try {
            json j;
            file >> j;
            
            products.clear();
            stock.clear();

            if (!j.contains("products") || !j.contains("stock")) {
                cerr << "Invalid JSON format: missing required fields" << endl;
                return false;
            }

            for (const auto& product_json : j["products"]) {
                products.push_back(Product::from_json(product_json));
            }
            stock = j["stock"].get<map<int, int>>();

            return true;
        }
        catch (const json::exception& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
            return false;
        }
    }

    void displayProducts(const vector<Product>& prods) const {
        if (prods.empty()) {
            cout << "No products found." << endl;
            return;
        }
        for (const auto& p : prods) {
            cout << "ID: " << p.id << endl
                 << "Name: " << p.name << endl
                 << "Price: " << p.price << endl
                 << "Categories: ";
            for (const auto& cat : p.categories) {
                cout << cat << " ";
            }
            cout << "\nIn stock: " << stock.at(p.id) << endl
                 << "------------------------" << endl;
        }
    }

    const vector<Product>& getAllProducts() const {
        return products;
    }

    size_t getProductCount() const {
        return products.size();
    }
};

int main() {
    Store store;
    string filename = "file.json";

    while (true) {
        cout << "\n=== Store Management System ===\n"
             << "1. Load data from JSON\n"
             << "2. Add new product\n"
             << "3. Find by category\n"
             << "4. Remove product by ID\n"
             << "5. Save to JSON\n"
             << "0. Exit\n"
             << "Choose option: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            string dummy;
            getline(cin, dummy);
            cout << "Invalid input.\n";
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1: {
                if (store.loadFromJson(filename)) {
                    cout << "Data loaded successfully from " << filename << endl;
                    cout << "Loaded " << store.getProductCount() << " products." << endl;
                    cout << "\nCurrent inventory:" << endl;
                    store.displayProducts(store.getAllProducts());
                } else {
                    cout << "Failed to load data from " << filename << endl;
                    cout << "Make sure the file exists and has valid JSON format." << endl;
                }
                break;
            }
            case 2: {
                int id;
                string name;
                double price;
                int quantity;
                set<string> categories;

                cout << "Enter product ID: ";
                cin >> id;
                cin.ignore();

                cout << "Enter product name: ";
                getline(cin, name);

                cout << "Enter price: ";
                cin >> price;
                cin.ignore();

                cout << "Enter quantity: ";
                cin >> quantity;
                cin.ignore();

                cout << "Enter categories (empty line to finish):\n";
                string category;
                while (true) {
                    getline(cin, category);
                    if (category.empty()) break;
                    categories.insert(category);
                }

                Product newProduct(id, name, price, categories);
                store.addProduct(newProduct, quantity);
                cout << "Product added successfully.\n";
                break;
            }
            case 3: {
                cout << "Enter category to search: ";
                string category;
                getline(cin, category);
                auto found = store.findByCategory(category);
                cout << "\nProducts in category '" << category << "':\n";
                store.displayProducts(found);
                break;
            }
            case 4: {
                cout << "Enter product ID to remove: ";
                int id;
                cin >> id;
                cin.ignore();
                if (store.removeProduct(id)) {
                    cout << "Product removed successfully.\n";
                } else {
                    cout << "Product not found.\n";
                }
                break;
            }
            case 5: {
                store.saveToJson(filename);
                cout << "Data saved to " << filename << endl;
                break;
            }
            case 0:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid option.\n";
        }
    }
    return 0;
}