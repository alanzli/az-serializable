// ContainerExample.cpp
#include "Serializable.h"
#include "JsonSerializer.h"
#include "SerializationErrors.h"
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <array>
#include <cmath>

// Example class that uses various containers
class ContainerData : public Serializable {
private:
    std::vector<int> numbers_;
    std::list<std::string> names_;
    std::set<double> unique_values_;
    std::map<std::string, int> string_to_int_;
    std::unordered_map<int, std::string> int_to_string_;
    std::array<bool, 3> flags_;
    std::vector<std::vector<int>> matrix_;
    
public:
    ContainerData() {
        // Initialize with sample data
        numbers_ = {1, 2, 3, 4, 5};
        names_ = {"Alice", "Bob", "Charlie"};
        unique_values_ = {1.1, 2.2, 3.3};
        string_to_int_ = {{"one", 1}, {"two", 2}, {"three", 3}};
        int_to_string_ = {{1, "first"}, {2, "second"}, {3, "third"}};
        flags_ = {true, false, true};
        matrix_ = {{1, 2}, {3, 4}, {5, 6}};
    }
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("numbers", numbers_);
        serializer.serializeProperty("names", names_);
        serializer.serializeProperty("unique_values", unique_values_);
        serializer.serializeProperty("string_to_int", string_to_int_);
        serializer.serializeProperty("int_to_string", int_to_string_);
        serializer.serializeProperty("flags", flags_);
        serializer.serializeProperty("matrix", matrix_);
    }
};

// Example with nested serializable objects in containers
class NestedItem : public Serializable {
private:
    int id_;
    std::string name_;
    
public:
    NestedItem(int id, const std::string& name) : id_(id), name_(name) {}
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("id", id_);
        serializer.serializeProperty("name", name_);
    }
};

class NestedContainerData : public Serializable {
private:
    std::vector<NestedItem> items_;
    std::map<std::string, NestedItem> item_map_;
    
public:
    NestedContainerData() {
        items_.emplace_back(1, "Item One");
        items_.emplace_back(2, "Item Two");
        items_.emplace_back(3, "Item Three");
        
        item_map_.emplace("first", NestedItem(10, "First Item"));
        item_map_.emplace("second", NestedItem(20, "Second Item"));
    }
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("items", items_);
        serializer.serializeProperty("item_map", item_map_);
    }
};

// Enhanced validator with container validation
class ContainerValidatedSerializer : public ValidatedSerializer {
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
        if (value.length() > 100) {
            addError("String too long (max 100 characters)", "string");
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
        if (json_elements.size() > 50) {
            addError("Array too large (max 50 elements)", "array");
        } else {
            json_serializer_.serializeArray(name, json_elements);
        }
        popPath();
    }
    
    void serializeObject(const std::string& name, const std::vector<std::pair<std::string, std::string>>& json_pairs) override {
        pushPath(name);
        if (json_pairs.size() > 20) {
            addError("Object too large (max 20 properties)", "object");
        } else {
            json_serializer_.serializeObject(name, json_pairs);
        }
        popPath();
    }
    
    void serializeUnsupportedType(const std::string& name) override {
        pushPath(name);
        addError("Unsupported type encountered", "unknown");
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
    
private:
    JsonSerializer json_serializer_;
};

int main() {
    std::cout << "=== Container Serialization Demo ===\n\n";
    
    // Basic container serialization
    ContainerData container_data;
    JsonSerializer serializer;
    container_data.serialize(serializer);
    
    std::cout << "Container serialization:\n";
    std::cout << serializer.toJson() << "\n\n";
    
    // Nested object containers
    NestedContainerData nested_data;
    JsonSerializer nested_serializer;
    nested_data.serialize(nested_serializer);
    
    std::cout << "Nested container serialization:\n";
    std::cout << nested_serializer.toJson() << "\n\n";
    
    // Validation demo
    std::cout << "=== Container Validation Demo ===\n";
    ContainerValidatedSerializer validator;
    
    try {
        container_data.serialize(validator);
        
        if (validator.hasErrors()) {
            std::cout << "Validation errors found:\n";
            std::cout << validator.getErrorCollector().getFormattedErrors();
        } else {
            std::cout << "Container validation passed!\n";
            std::cout << "Validated JSON: " << validator.toJson() << "\n";
        }
    } catch (const SerializationException& e) {
        std::cout << "Serialization exception: " << e.what() << "\n";
    }
    
    // Test with large containers that should trigger validation errors
    std::cout << "\n=== Large Container Validation Test ===\n";
    std::vector<int> large_vector(100, 42); // 100 elements - should trigger validation error
    
    class LargeContainerTest : public Serializable {
    private:
        std::vector<int> large_data_;
    public:
        LargeContainerTest(const std::vector<int>& data) : large_data_(data) {}
        
        void visitProperties(TypedSerializer& serializer) const override {
            serializer.serializeProperty("large_data", large_data_);
        }
    };
    
    LargeContainerTest large_test(large_vector);
    ContainerValidatedSerializer large_validator;
    
    try {
        large_test.serialize(large_validator);
        
        if (large_validator.hasErrors()) {
            std::cout << "Large container validation errors (as expected):\n";
            std::cout << large_validator.getErrorCollector().getFormattedErrors();
        } else {
            std::cout << "Unexpected: Large container validation passed\n";
        }
    } catch (const SerializationException& e) {
        std::cout << "Large container exception: " << e.what() << "\n";
    }
    
    return 0;
}
