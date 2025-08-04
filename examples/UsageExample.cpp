#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "az/Serializable.h"
#include "az/json/JsonSerializer.h"

// --- Simple Types Demo ---
class SimpleTypesDemo : public az::Serializable {
   private:
    int int_val = 42;
    long long_val = 1234567890L;
    long long long_long_val = 9223372036854775807LL;
    unsigned int uint_val = 42U;
    unsigned long ulong_val = 1234567890UL;
    unsigned long long ulong_long_val = 18446744073709551615ULL;
    float float_val = 3.14159f;
    double double_val = 2.718281828;
    char char_val = 'X';
    bool bool_val = true;
    std::string string_val = "Hello";
    std::int8_t int8_val = -128;
    std::int16_t int16_val = 32767;
    std::int32_t int32_val = -214748364;
    std::int64_t int64_val = 9223372036854775807LL;
    std::uint8_t uint8_val = 255;
    std::uint16_t uint16_val = 65535;
    std::uint32_t uint32_val = 4294967295U;
    std::uint64_t uint64_val = 18446744073709551615ULL;

   public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("int_val", int_val);
        serializer.serializeProperty("long_val", long_val);
        serializer.serializeProperty("long_long_val", long_long_val);
        serializer.serializeProperty("uint_val", uint_val);
        serializer.serializeProperty("ulong_val", ulong_val);
        serializer.serializeProperty("ulong_long_val", ulong_long_val);
        serializer.serializeProperty("float_val", float_val);
        serializer.serializeProperty("double_val", double_val);
        serializer.serializeProperty("char_val", char_val);
        serializer.serializeProperty("bool_val", bool_val);
        serializer.serializeProperty("string_val", string_val);
        serializer.serializeProperty("int8_val", int8_val);
        serializer.serializeProperty("int16_val", int16_val);
        serializer.serializeProperty("int32_val", int32_val);
        serializer.serializeProperty("int64_val", int64_val);
        serializer.serializeProperty("uint8_val", uint8_val);
        serializer.serializeProperty("uint16_val", uint16_val);
        serializer.serializeProperty("uint32_val", uint32_val);
        serializer.serializeProperty("uint64_val", uint64_val);
    }
};

// --- Container Demo ---
class ContainerData : public az::Serializable {
   private:
    std::vector<int> numbers_ = {1, 2, 3, 4, 5};
    std::list<std::string> names_ = {"Alice", "Bob", "Charlie"};
    std::set<double> unique_values_ = {1.1, 2.2, 3.3};
    std::map<std::string, int> string_to_int_ = {{"one", 1}, {"two", 2}, {"three", 3}};
    std::unordered_map<int, std::string> int_to_string_ = {{1, "first"}, {2, "second"}, {3, "third"}};
    std::array<bool, 3> flags_ = {true, false, true};
    std::vector<std::vector<int>> matrix_ = {{1, 2}, {3, 4}, {5, 6}};

   public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("numbers", numbers_);
        serializer.serializeProperty("names", names_);
        serializer.serializeProperty("unique_values", unique_values_);
        serializer.serializeProperty("string_to_int", string_to_int_);
        serializer.serializeProperty("int_to_string", int_to_string_);
        serializer.serializeProperty("flags", flags_);
        serializer.serializeProperty("matrix", matrix_);
    }
};

// --- Nested Container Demo ---
class NestedItem : public az::Serializable {
   private:
    int id_;
    std::string name_;

   public:
    NestedItem(int id, const std::string& name) : id_(id), name_(name) {}
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("id", id_);
        serializer.serializeProperty("name", name_);
    }
};

class NestedContainerData : public az::Serializable {
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
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("items", items_);
        serializer.serializeProperty("item_map", item_map_);
    }
};

// --- Numeric and Type Safety Demo ---
class TypeSafetyDemo : public az::Serializable {
   private:
    std::int8_t small_int_;
    std::uint64_t large_uint_;

   public:
    TypeSafetyDemo(std::int8_t small, std::uint64_t large) : small_int_(small), large_uint_(large) {}
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("small_int", small_int_);
        serializer.serializeProperty("large_uint", large_uint_);
    }
};

class NumericContainerDemo : public az::Serializable {
   private:
    std::vector<std::int64_t> int64_vector_ = {-9223372036854775807LL, 0LL, 9223372036854775807LL};
    std::vector<float> float_vector_ = {-3.14f, 0.0f, 2.718f, 1.414f};
    std::map<std::uint32_t, std::string> uint_to_string_map_ = {
        {1U, "one"}, {1000000U, "million"}, {4294967295U, "max_uint32"}};

   public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("int64_vector", int64_vector_);
        serializer.serializeProperty("float_vector", float_vector_);
        serializer.serializeProperty("uint_to_string_map", uint_to_string_map_);
    }
};

void demonstrateTypeLimits() {
    std::cout << "=== Type Limits Demonstration ===\n";
    std::cout << "int8_t range: " << static_cast<int>(std::numeric_limits<std::int8_t>::min()) << " to "
              << static_cast<int>(std::numeric_limits<std::int8_t>::max()) << "\n";
    std::cout << "uint64_t max: " << std::numeric_limits<std::uint64_t>::max() << "\n";
    std::cout << "float precision: " << std::numeric_limits<float>::digits10 << " decimal digits\n";
    std::cout << "double precision: " << std::numeric_limits<double>::digits10 << " decimal digits\n\n";
}

int main() {
    std::cout << "=== Serialization Framework Demo ===\n\n";
    demonstrateTypeLimits();

    std::string result;

    // Simple types
    SimpleTypesDemo simple_demo;
    az::JsonSerializer simple_serializer;
    simple_demo.serialize(simple_serializer);
    result = simple_serializer.toJson();
    std::cout << "Simple types JSON:\n" << result << "\n";
    std::cout << "isLikelyJsonCheck: " << (az::isLikelyValidJson(result) ? "true" : "false") << "\n\n";

    // Container types
    ContainerData container_demo;
    az::JsonSerializer container_serializer;
    container_demo.serialize(container_serializer);
    result = container_serializer.toJson();
    std::cout << "Container types JSON:\n" << result << "\n";
    std::cout << "isLikelyJsonCheck: " << (az::isLikelyValidJson(result) ? "true" : "false") << "\n\n";

    // Nested containers
    NestedContainerData nested_demo;
    az::JsonSerializer nested_serializer;
    nested_demo.serialize(nested_serializer);
    result = nested_serializer.toJson();
    std::cout << "Nested container JSON:\n" << result << "\n";
    std::cout << "isLikelyJsonCheck: " << (az::isLikelyValidJson(result) ? "true" : "false") << "\n\n";

    // Type safety
    TypeSafetyDemo type_demo(127, 18446744073709551615ULL);
    az::JsonSerializer type_serializer;
    type_demo.serialize(type_serializer);
    result = type_serializer.toJson();
    std::cout << "Type safety JSON:\n" << result << "\n";
    std::cout << "isLikelyJsonCheck: " << (az::isLikelyValidJson(result) ? "true" : "false") << "\n\n";

    // Numeric containers
    NumericContainerDemo numeric_demo;
    az::JsonSerializer numeric_serializer;
    numeric_demo.serialize(numeric_serializer);
    result = numeric_serializer.toJson();
    std::cout << "Numeric containers JSON:\n" << result << "\n";
    std::cout << "isLikelyJsonCheck: " << (az::isLikelyValidJson(result) ? "true" : "false") << "\n\n";

    // Performance test
    std::cout << "=== Performance Test (int64_t and int) ===\n\n";
    const int iterations = 10000;
    std::vector<az::JsonSerializer> serializers;
    serializers.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        serializers.emplace_back();
    }
    std::vector<TypeSafetyDemo> perf_demos;
    perf_demos.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        perf_demos.emplace_back(static_cast<std::int8_t>(i % 128), static_cast<std::uint64_t>(i) * 1000000ULL);
    }

    std::string results;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        perf_demos[i].serialize(serializers[i]);
        results = serializers[i].toJson();  // Force serialization to JSON
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "resulting JSON:\n";
    std::cout << results << "\n\n";

    // clear all serializers for next test
    for (auto& serializer : serializers) {
        serializer.clear();
    }

    std::cout << "Performance: " << duration.count() << " microseconds for " << iterations << " iterations\n\n";

    // Performance test for nested objects
    std::cout << "=== Performance Test (Nested Objects) ===\n\n";
    std::vector<NestedContainerData> nested_perf_demos;
    nested_perf_demos.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        nested_perf_demos.emplace_back();
    }

    auto nested_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        nested_perf_demos[i].serialize(serializers[i]);
        results = serializers[i].toJson();  // Force serialization to JSON
    }
    auto nested_end = std::chrono::high_resolution_clock::now();
    auto nested_duration = std::chrono::duration_cast<std::chrono::microseconds>(nested_end - nested_start);

    std::cout << "resulting JSON:\n" << results << "\n\n";

    std::cout << "Nested Performance: " << nested_duration.count() << " microseconds for " << iterations
              << " iterations\n";

    // clear all serializers for next test
    for (auto& serializer : serializers) {
        serializer.clear();
    }

    std::cout << "\n=== All tests and examples ran successfully! ===\n";
    return 0;
}
