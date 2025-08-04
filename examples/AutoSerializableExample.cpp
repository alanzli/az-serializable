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
class Person : public az::Serializable {
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
    AZ_SERIALIZE(AZ_MEMBER(name_), AZ_MEMBER(age_), AZ_MEMBER(height_), AZ_MEMBER(is_student_))

    // Optional: provide getters for external access
    const std::string& name() const { return name_; }
    int age() const { return age_; }
};

// Example 2: Partial serialization - only serialize some members
class Account : public az::Serializable {
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
    AZ_SERIALIZE(AZ_MEMBER(username_), AZ_MEMBER(email_), AZ_MEMBER(account_id_), AZ_MEMBER(balance_)
                 // Note: password_ is intentionally omitted
    )
};

// Example 3: Complex nested structure with containers
class Department : public az::Serializable {
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

    AZ_SERIALIZE(AZ_MEMBER(dept_name_), AZ_MEMBER(manager_), AZ_MEMBER(employees_), AZ_MEMBER(project_budgets_),
                 AZ_MEMBER(quarterly_revenue_))
};

// Example 4: Multiple inheritance levels
class Vehicle : public az::Serializable {
   protected:
    AZ_SERIALIZE(AZ_MEMBER(make_), AZ_MEMBER(model_), AZ_MEMBER(year_))
   protected:
    std::string make_;
    std::string model_;
    int year_;

   public:
    Vehicle(const std::string& make, const std::string& model, int year) : make_(make), model_(model), year_(year) {}
};

class Car : public Vehicle {
   protected:
    // Extend Vehicle serialization with car-specific properties
    AZ_EXTEND(Vehicle, AZ_MEMBER(doors_), AZ_MEMBER(fuel_type_), AZ_MEMBER(has_sunroof_))
   private:
    int doors_;
    std::string fuel_type_;
    bool has_sunroof_;

   public:
    Car(const std::string& make, const std::string& model, int year, int doors, const std::string& fuel)
        : Vehicle(make, model, year), doors_(doors), fuel_type_(fuel), has_sunroof_(false) {}

    void setSunroof(bool has_sunroof) { has_sunroof_ = has_sunroof; }
};

class SportsCar : public Car {
   protected:
    AZ_EXTEND(Car,                                   // Extend Car serialization with sports car specific properties
              AZ_MEMBER_X(horsepower_, horsepower),  // rename horsepower_ to horsepower in serialization
              AZ_MEMBER_X(top_speed_, TOP_SPEED),    // rename top_speed_ to TOP_SPEED in serialization
              AZ_MEMBER(has_turbo_), AZ_MEMBER(racing_stripe_color_))
   private:
    int horsepower_;
    double top_speed_;
    bool has_turbo_;
    std::string racing_stripe_color_;

   public:
    SportsCar(const std::string& make, const std::string& model, int year, int doors, const std::string& fuel, int hp,
              double speed)
        : Car(make, model, year, doors, fuel),
          horsepower_(hp),
          top_speed_(speed),
          has_turbo_(false),
          racing_stripe_color_("none") {}

    void enableTurbo() { has_turbo_ = true; }
    void setRacingStripe(const std::string& color) { racing_stripe_color_ = color; }
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
    printSeparator("4. SportsCar (Vehicle -> Car -> SportsCar)");
    SportsCar ferrari("Ferrari", "488 GTB", 2023, 2, "Gasoline", 661, 330.0);
    ferrari.enableTurbo();
    ferrari.setRacingStripe("Red");
    ferrari.setSunroof(false);

    ferrari.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    json_serializer.clear();
    printSeparator("5. Regular Car (Vehicle -> Car)");
    Car sedan("Toyota", "Camry", 2023, 4, "Hybrid");
    sedan.setSunroof(true);

    sedan.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    json_serializer.clear();
    printSeparator("6. Base Vehicle");
    Vehicle motorcycle("Harley-Davidson", "Street 750", 2023);
    motorcycle.serialize(json_serializer);
    std::cout << json_serializer.toJson() << "\n";

    return 0;
}
