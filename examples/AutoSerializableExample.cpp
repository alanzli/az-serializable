#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "az/AutoSerializable.h"
#include "az/json/JsonSerializer.h"

// Example 1: Simple class using AutoSerializable
class Person : public az::AutoSerializable<Person> {
   private:
    std::string name_;
    int age_;
    double height_;
    bool is_student_;

   public:
    Person(const std::string& name, int age, double height, bool is_student)
        : name_(name), age_(age), height_(height), is_student_(is_student) {}

    // Default constructor for container usage
    Person() : name_("Unknown"), age_(0), height_(0.0), is_student_(false) {}

    // Automatically serialize these members
    AZ_ENABLE_AUTO_SERIALIZATION(AZ_MEMBER(name_), AZ_MEMBER(age_), AZ_MEMBER(height_), AZ_MEMBER(is_student_))

    // Optional: provide getters for external access
    const std::string& name() const { return name_; }
    int age() const { return age_; }
};

// Example 2: Partial serialization - only serialize some members
class Account : public az::AutoSerializable<Account> {
   private:
    std::string username_;
    std::string password_;  // This will NOT be serialized
    std::string email_;
    int account_id_;
    double balance_;

   public:
    Account(const std::string& username, const std::string& password, const std::string& email, int id, double balance)
        : username_(username), password_(password), email_(email), account_id_(id), balance_(balance) {}

    // Only serialize public information, exclude sensitive data
    AZ_ENABLE_AUTO_SERIALIZATION(AZ_MEMBER(username_), AZ_MEMBER(email_), AZ_MEMBER(account_id_), AZ_MEMBER(balance_)
                                 // Note: password_ is intentionally omitted
    )
};

// Example 3: Complex nested structure with containers
class Department : public az::AutoSerializable<Department> {
   private:
    std::string dept_name_;
    std::string manager_;
    std::vector<Person> employees_;
    std::map<std::string, int> project_budgets_;
    std::array<double, 4> quarterly_revenue_;

   public:
    Department(const std::string& name, const std::string& manager)
        : dept_name_(name), manager_(manager), quarterly_revenue_{0.0, 0.0, 0.0, 0.0} {}

    void addEmployee(const Person& person) { employees_.push_back(person); }

    void addProject(const std::string& project, int budget) { project_budgets_[project] = budget; }

    void setQuarterlyRevenue(int quarter, double revenue) {
        if (quarter >= 0 && quarter < 4) {
            quarterly_revenue_[quarter] = revenue;
        }
    }

    AZ_ENABLE_AUTO_SERIALIZATION(AZ_MEMBER(dept_name_), AZ_MEMBER(manager_), AZ_MEMBER(employees_),
                                 AZ_MEMBER(project_budgets_), AZ_MEMBER(quarterly_revenue_))
};

// Example 4: Multiple inheritance levels
class Vehicle : public az::AutoSerializable<Vehicle> {
   private:
    std::string make_;
    std::string model_;
    int year_;

   public:
    Vehicle(const std::string& make, const std::string& model, int year) : make_(make), model_(model), year_(year) {}

    AZ_ENABLE_AUTO_SERIALIZATION(AZ_MEMBER(make_), AZ_MEMBER(model_), AZ_MEMBER(year_))
};

class Car : public az::AutoSerializable<Car> {
   private:
    Vehicle base_vehicle_;
    int doors_;
    std::string fuel_type_;

   public:
    Car(const std::string& make, const std::string& model, int year, int doors, const std::string& fuel)
        : base_vehicle_(make, model, year), doors_(doors), fuel_type_(fuel) {}

    AZ_ENABLE_AUTO_SERIALIZATION(AZ_MEMBER(base_vehicle_), AZ_MEMBER(doors_), AZ_MEMBER(fuel_type_))
};

// Example 5: Traditional approach for comparison
class TraditionalPerson : public az::Serializable {
   private:
    std::string name_;
    int age_;

   public:
    TraditionalPerson(const std::string& name, int age) : name_(name), age_(age) {}

    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("name_", name_);
        serializer.serializeProperty("age_", age_);
    }
};

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(50, '=') << "\n";
}

int main() {
    std::cout << "AutoSerializable Comprehensive Demo\n";

    // Create JSON serializer
    az::FIFOJsonSerializer json_serializer;

    // Example 1: Simple Person
    printSeparator("1. Simple Person (AutoSerializable)");
    Person person("Alice Johnson", 28, 165.5, false);
    person.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    // Example 2: Partial serialization
    json_serializer.clear();
    printSeparator("2. Account (Partial Serialization - No Password)");
    Account account("alice123", "secret_password", "alice@example.com", 1001, 2500.75);
    account.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    // Example 3: Complex nested structure
    json_serializer.clear();
    printSeparator("3. Department (Complex Nested Structure)");
    Department engineering("Engineering", "Bob Wilson");
    engineering.addEmployee(Person("Charlie Brown", 32, 175.0, false));
    engineering.addEmployee(Person("Diana Prince", 29, 168.0, true));
    engineering.addProject("Project Alpha", 100000);
    engineering.addProject("Project Beta", 150000);
    engineering.setQuarterlyRevenue(0, 250000.0);
    engineering.setQuarterlyRevenue(1, 275000.0);
    engineering.setQuarterlyRevenue(2, 300000.0);
    engineering.setQuarterlyRevenue(3, 325000.0);

    engineering.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    // Example 4: Nested AutoSerializable objects
    json_serializer.clear();
    printSeparator("4. Car (Nested AutoSerializable Objects)");
    Car car("Toyota", "Camry", 2023, 4, "Hybrid");
    car.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    // Example 5: Traditional approach for comparison
    json_serializer.clear();
    printSeparator("5. Traditional Approach (Manual Implementation)");
    TraditionalPerson traditional("John Doe", 35);
    traditional.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    return 0;
}
