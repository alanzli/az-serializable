#include <iostream>
#include <stdexcept>
#include <string>

#include "az/Serializable.h"
#include "az/Validator.h"
#include "az/json/JsonSerializer.h"

// Example class that uses validation
class User : public az::Serializable {
   private:
    std::string username;
    std::string email;
    int age;
    double salary;

   public:
    User(const std::string& username, const std::string& email, int age, double salary)
        : username(username), email(email), age(age), salary(salary) {}

    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("username", username);
        serializer.serializeProperty("email", email);
        serializer.serializeProperty("age", age);
        serializer.serializeProperty("salary", salary);
    }
};

int main() {
    std::cout << "=== Validator Demo ===" << std::endl;

    // Create a validator
    az::Validator validator;

    // Add type-specific rules
    validator.addRule<std::string>(
        [](const std::string& /*prop*/, const std::string& value, const std::string& /*serialized*/) {
            if (value.length() < 3) {
                return az::ValidationResult(false, "String must be at least 3 characters long");
            }
            return az::ValidationResult(true);
        },
        "String length >= 3");

    validator.addRule<int>(
        [](const std::string& /*prop*/, const int& value, const std::string& /*serialized*/) {
            if (value < 0) {
                return az::ValidationResult(false, "Integer must be non-negative");
            }
            return az::ValidationResult(true);
        },
        "Integer >= 0");

    validator.addRule<double>(
        [](const std::string& /*prop*/, const double& value, const std::string& /*serialized*/) {
            if (value < 0.0) {
                return az::ValidationResult(false, "Double must be non-negative");
            }
            return az::ValidationResult(true);
        },
        "Double >= 0.0");

    // Add property-specific rules
    validator.addPropertyRule(
        "email",
        [](const std::string& /*prop*/, const std::string& serialized) {
            // Simple email validation (check for @ symbol in the serialized string)
            if (serialized.find("@") == std::string::npos) {
                return az::ValidationResult(false, "Email must contain @ symbol");
            }
            return az::ValidationResult(true);
        },
        "Email format validation");

    validator.addPropertyRule(
        "age",
        [](const std::string& /*prop*/, const std::string& serialized) {
            try {
                int age = std::stoi(serialized);
                if (age > 150) {
                    return az::ValidationResult(false, "Age cannot exceed 150");
                }
            } catch (...) {
                return az::ValidationResult(false, "Age must be a valid integer");
            }
            return az::ValidationResult(true);
        },
        "Age <= 150");

    // Add general rule for all serialized values
    validator.addGeneralRule(
        [](const std::string& /*prop*/, const std::string& serialized) {
            if (serialized.length() > 1000) {
                return az::ValidationResult(false, "Serialized value too long (>1000 chars)");
            }
            return az::ValidationResult(true);
        },
        "Serialized value length <= 1000");

    std::cout << "\n=== Validation Rules ===" << std::endl;
    auto rules = validator.getRuleDescriptions();
    for (const auto& rule : rules) {
        std::cout << "- " << rule << std::endl;
    }

    // Test with valid data
    std::cout << "\n=== Testing Valid User ===" << std::endl;
    try {
        User valid_user("john_doe", "john@example.com", 25, 50000.0);
        az::UnorderedJsonSerializer serializer;
        serializer.setValidator(&validator);

        valid_user.serialize(serializer);
        std::cout << "Valid user serialized successfully:" << std::endl;
        std::cout << serializer.toJson() << std::endl;
    } catch (const az::ValidationException& e) {
        std::cout << "Validation error: " << e.what() << std::endl;
    }

    // Test with invalid data - short username
    std::cout << "\n=== Testing Invalid User (Short Username) ===" << std::endl;
    try {
        User invalid_user1("jd", "john@example.com", 25, 50000.0);
        az::UnorderedJsonSerializer serializer;
        serializer.setValidator(&validator);

        invalid_user1.serialize(serializer);
        std::cout << "This shouldn't print - validation should fail!" << std::endl;
    } catch (const az::ValidationException& e) {
        std::cout << "Expected validation error: " << e.what() << std::endl;
    }

    // Test with invalid data - invalid email
    std::cout << "\n=== Testing Invalid User (Bad Email) ===" << std::endl;
    try {
        User invalid_user2("john_doe", "invalid-email", 25, 50000.0);
        az::UnorderedJsonSerializer serializer;
        serializer.setValidator(&validator);

        invalid_user2.serialize(serializer);
        std::cout << "This shouldn't print - validation should fail!" << std::endl;
    } catch (const az::ValidationException& e) {
        std::cout << "Expected validation error: " << e.what() << std::endl;
    }

    // Test with invalid data - negative age
    std::cout << "\n=== Testing Invalid User (Negative Age) ===" << std::endl;
    try {
        User invalid_user3("john_doe", "john@example.com", -5, 50000.0);
        az::UnorderedJsonSerializer serializer;
        serializer.setValidator(&validator);

        invalid_user3.serialize(serializer);
        std::cout << "This shouldn't print - validation should fail!" << std::endl;
    } catch (const az::ValidationException& e) {
        std::cout << "Expected validation error: " << e.what() << std::endl;
    }

    // Test with validation disabled
    std::cout << "\n=== Testing With Validation Disabled ===" << std::endl;
    try {
        User invalid_user4("jd", "invalid-email", -5, -1000.0);
        az::UnorderedJsonSerializer serializer;

        invalid_user4.serialize(serializer);
        std::cout << "Invalid user serialized without validation:" << std::endl;
        std::cout << serializer.toJson() << std::endl;
    } catch (const az::ValidationException& e) {
        std::cout << "Unexpected validation error: " << e.what() << std::endl;
    }

    // Test individual property validation
    std::cout << "\n=== Testing Individual Property Validation ===" << std::endl;
    az::UnorderedJsonSerializer test_serializer;
    test_serializer.setValidator(&validator);

    auto result1 = test_serializer.validateProperty("username", std::string("ab"));
    std::cout << "Validate short username: " << (result1.is_valid ? "PASS" : "FAIL") << " - " << result1.error_message
              << std::endl;

    auto result2 = test_serializer.validateProperty("age", 200);
    std::cout << "Validate high age: " << (result2.is_valid ? "PASS" : "FAIL") << " - " << result2.error_message
              << std::endl;

    auto result3 = test_serializer.validateSerializedProperty("email", "\"john@example.com\"");
    std::cout << "Validate good email: " << (result3.is_valid ? "PASS" : "FAIL") << " - " << result3.error_message
              << std::endl;

    auto result4 = test_serializer.validateSerializedProperty("email", "\"invalid-email\"");
    std::cout << "Validate bad email: " << (result4.is_valid ? "PASS" : "FAIL") << " - " << result4.error_message
              << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}
