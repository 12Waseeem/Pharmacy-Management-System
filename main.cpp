#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <string>

using namespace std;
using namespace sql;

class PharmacyManagementSystem {
public:
    PharmacyManagementSystem() {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "your_root_password");
        con->setSchema("pharmacy_management");
    }

    ~PharmacyManagementSystem() {
        delete con;
    }

    void addCustomer(const string& name, const string& contact) {
        PreparedStatement *pstmt = con->prepareStatement("INSERT INTO customers (name, contact) VALUES (?, ?)");
        pstmt->setString(1, name);
        pstmt->setString(2, contact);
        pstmt->execute();
        delete pstmt;
        cout << "Customer added successfully.\n";
    }

    void addMedicine(const string& name, double price, int quantity) {
        PreparedStatement *pstmt = con->prepareStatement("INSERT INTO medicines (name, price, quantity) VALUES (?, ?, ?)");
        pstmt->setString(1, name);
        pstmt->setDouble(2, price);
        pstmt->setInt(3, quantity);
        pstmt->execute();
        delete pstmt;
        cout << "Medicine added successfully.\n";
    }

    void makeSale(int customerId, int medicineId, int quantity) {
        PreparedStatement *pstmt = con->prepareStatement("SELECT price, quantity FROM medicines WHERE medicine_id = ?");
        pstmt->setInt(1, medicineId);
        ResultSet *res = pstmt->executeQuery();

        if (res->next()) {
            double price = res->getDouble("price");
            int availableQuantity = res->getInt("quantity");

            if (quantity > availableQuantity) {
                cout << "Not enough stock available.\n";
            } else {
                double totalPrice = price * quantity;
                PreparedStatement *pstmt2 = con->prepareStatement("INSERT INTO sales (customer_id, medicine_id, quantity, total_price) VALUES (?, ?, ?, ?)");
                pstmt2->setInt(1, customerId);
                pstmt2->setInt(2, medicineId);
                pstmt2->setInt(3, quantity);
                pstmt2->setDouble(4, totalPrice);
                pstmt2->execute();

                PreparedStatement *pstmt3 = con->prepareStatement("UPDATE medicines SET quantity = quantity - ? WHERE medicine_id = ?");
                pstmt3->setInt(1, quantity);
                pstmt3->setInt(2, medicineId);
                pstmt3->execute();

                delete pstmt2;
                delete pstmt3;

                cout << "Sale made successfully. Total price: $" << totalPrice << "\n";
            }
        } else {
            cout << "Medicine not found.\n";
        }

        delete res;
        delete pstmt;
    }

    void listCustomers() {
        Statement *stmt = con->createStatement();
        ResultSet *res = stmt->executeQuery("SELECT * FROM customers");

        while (res->next()) {
            cout << "Customer ID: " << res->getInt("customer_id")
                 << ", Name: " << res->getString("name")
                 << ", Contact: " << res->getString("contact") << "\n";
        }

        delete res;
        delete stmt;
    }

    void listMedicines() {
        Statement *stmt = con->createStatement();
        ResultSet *res = stmt->executeQuery("SELECT * FROM medicines");

        while (res->next()) {
            cout << "Medicine ID: " << res->getInt("medicine_id")
                 << ", Name: " << res->getString("name")
                 << ", Price: " << res->getDouble("price")
                 << ", Quantity: " << res->getInt("quantity") << "\n";
        }

        delete res;
        delete stmt;
    }

private:
    sql::Driver *driver;
    sql::Connection *con;
};

int main() {
    PharmacyManagementSystem pms;

    int choice;
    while (true) {
        cout << "\nPharmacy Management System\n";
        cout << "1. Add Customer\n";
        cout << "2. Add Medicine\n";
        cout << "3. Make Sale\n";
        cout << "4. List Customers\n";
        cout << "5. List Medicines\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string name, contact;
            cout << "Enter customer name: ";
            cin >> name;
            cout << "Enter contact: ";
            cin >> contact;
            pms.addCustomer(name, contact);
        } else if (choice == 2) {
            string name;
            double price;
            int quantity;
            cout << "Enter medicine name: ";
            cin >> name;
            cout << "Enter price: ";
            cin >> price;
            cout << "Enter quantity: ";
            cin >> quantity;
            pms.addMedicine(name, price, quantity);
        } else if (choice == 3) {
            int customerId, medicineId, quantity;
            cout << "Enter customer ID: ";
            cin >> customerId;
            cout << "Enter medicine ID: ";
            cin >> medicineId;
            cout << "Enter quantity: ";
            cin >> quantity;
            pms.makeSale(customerId, medicineId, quantity);
        } else if (choice == 4) {
            pms.listCustomers();
        } else if (choice == 5) {
            pms.listMedicines();
        } else if (choice == 6) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
