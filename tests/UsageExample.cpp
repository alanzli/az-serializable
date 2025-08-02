// ExampleUsage.cpp
#include "Serializable.h"
#include "JsonSerializer.h"
#include "SerializationErrors.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

// Example class using serialization
class MyData : public Serializable {
private:
    int a_;
    int b_;
    std::string name_;
    
public:
    MyData(int a, int b, const std::string& name) 
        : a_(a), b_(b), name_(name) {}
    
    // Type-safe property visitation - no std::any overhead
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("a", a_);
        serializer.serializeProperty("b", b_);
        serializer.serializeProperty("name", name_);
    }
    
    // Getters for validation/access
    int getA() const { return a_; }
    int getB() const { return b_; }
    const std::string& getName() const { return name_; }
};

class NestedData : public Serializable {
private:
    MyData data_;
    std::vector<int> numbers_;
    bool is_valid_;
    
public:
    NestedData(const MyData& data, const std::vector<int>& numbers, bool valid)
        : data_(data), numbers_(numbers), is_valid_(valid) {}
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeNestedObject("data", data_);
        serializer.serializeProperty("is_valid", is_valid_);
        
        // Note: std::vector requires special handling
        // This is a limitation that would need additional template specialization
        // For now, we'll serialize vector size as an example
        serializer.serializeProperty("numbers_size", static_cast<int>(numbers_.size()));
    }
};

// Performance testing function
void performanceComparison() {
    const int iterations = 10000;
    JsonSerializer serializer;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        MyData data(i, i * 2, "test" + std::to_string(i));
        
        data.serialize(serializer);
        auto json = serializer.toJson();
        // Clear for reuse (performance optimization)
        serializer.clear();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Implementation: " << duration.count() << " microseconds for " 
              << iterations << " iterations" << std::endl;
}

// Example with error handling
class ValidatedJsonSerializer : public ValidatedSerializer {
public:
    void serializeProperty(const std::string& name, int value) override {
        pushPath(name);
        if (value < 0) {
            addError("Negative integer values not allowed", "int");
        } else {
            json_serializer_.serializeProperty(name, value);
        }
        popPath();
    }
    
    void serializeProperty(const std::string& name, double value) override {
        pushPath(name);
        if (std::isnan(value) || std::isinf(value)) {
            addError("NaN or infinite values not allowed", "double");
        } else {
            json_serializer_.serializeProperty(name, value);
        }
        popPath();
    }
    
    void serializeProperty(const std::string& name, bool value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, const std::string& value) override {
        pushPath(name);
        if (value.length() > 1000) {
            addError("String too long (max 1000 characters)", "string");
        } else {
            json_serializer_.serializeProperty(name, value);
        }
        popPath();
    }
    
    void serializeProperty(const std::string& name, const char* value) override {
        serializeProperty(name, std::string(value));
    }
    
    // Add implementations for all missing simple types
    void serializeProperty(const std::string& name, long value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, long long value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, unsigned int value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, unsigned long value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, unsigned long long value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, float value) override {
        pushPath(name);
        if (std::isnan(value) || std::isinf(value)) {
            addError("NaN or infinite values not allowed", "float");
        } else {
            json_serializer_.serializeProperty(name, value);
        }
        popPath();
    }
    
    void serializeProperty(const std::string& name, char value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, std::int8_t value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, std::int16_t value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, std::uint8_t value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    void serializeProperty(const std::string& name, std::uint16_t value) override {
        pushPath(name);
        json_serializer_.serializeProperty(name, value);
        popPath();
    }
    
    // Helper methods for conditional fixed-width types
    void serializeInt32(const std::string& name, std::int32_t value) override {
        pushPath(name);
        json_serializer_.serializeInt32(name, value);
        popPath();
    }
    
    void serializeInt64(const std::string& name, std::int64_t value) override {
        pushPath(name);
        json_serializer_.serializeInt64(name, value);
        popPath();
    }
    
    void serializeUint32(const std::string& name, std::uint32_t value) override {
        pushPath(name);
        json_serializer_.serializeUint32(name, value);
        popPath();
    }
    
    void serializeUint64(const std::string& name, std::uint64_t value) override {
        pushPath(name);
        json_serializer_.serializeUint64(name, value);
        popPath();
    }
    
    void serializeNestedObject(const std::string& name, const Serializable& value) override {
        pushPath(name);
        json_serializer_.serializeNestedObject(name, value);
        popPath();
    }
    
    void serializeArray(const std::string& name, const std::vector<std::string>& json_elements) override {
        pushPath(name);
        json_serializer_.serializeArray(name, json_elements);
        popPath();
    }
    
    void serializeObject(const std::string& name, const std::vector<std::pair<std::string, std::string>>& json_pairs) override {
        pushPath(name);
        json_serializer_.serializeObject(name, json_pairs);
        popPath();
    }
    
    std::string toJson() const {
        return json_serializer_.toJson();
    }
    
    // Helper methods required by base class
protected:
    std::string escapeJsonString(const std::string& input) const override {
        return json_serializer_.escapeJsonString(input);
    }
    
    std::string serializeNestedToString(const Serializable& obj) const override {
        return json_serializer_.serializeNestedToString(obj);
    }
    
    std::string createJsonArray(const std::vector<std::string>& elements) const override {
        return json_serializer_.createJsonArray(elements);
    }
    
    std::string createJsonObject(const std::vector<std::pair<std::string, std::string>>& pairs) const override {
        return json_serializer_.createJsonObject(pairs);
    }
    
protected:
    void serializeUnsupportedType(const std::string& /* name */) override {
        addError("Unsupported type encountered", "unknown");
    }
    
private:
    JsonSerializer json_serializer_;
};

int main() {
    std::cout << "=== Serialization Framework Demo ===\n\n";
    
    // Basic usage
    MyData data(42, 99, "Hello World");
    JsonSerializer serializer;
    data.serialize(serializer);
    std::cout << "Basic serialization: " << serializer.toJson() << "\n\n";
    
    // Nested object
    NestedData nested(data, {1, 2, 3, 4, 5}, true);
    JsonSerializer nested_serializer;
    nested.serialize(nested_serializer);
    std::cout << "Nested serialization: " << nested_serializer.toJson() << "\n\n";
    
    // Error handling demo
    std::cout << "=== Error Handling Demo ===\n";
    MyData invalid_data(-1, 99, std::string(1500, 'x')); // Negative int, long string
    ValidatedJsonSerializer validator;
    
    try {
        invalid_data.serialize(validator);
        if (validator.hasErrors()) {
            std::cout << "Validation errors found:\n";
            std::cout << validator.getErrorCollector().getFormattedErrors();
        } else {
            std::cout << "Validation passed: " << validator.toJson() << "\n";
        }
    } catch (const SerializationException& e) {
        std::cout << "Serialization exception: " << e.what() << "\n";
    }
    
    // Performance test
    std::cout << "\n=== Performance Test ===\n";
    performanceComparison();
    
    return 0;
}
