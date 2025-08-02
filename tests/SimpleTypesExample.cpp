// SimpleTypesExample.cpp
#include "Serializable.h"
#include "JsonSerializer.h"
#include "SerializationErrors.h"
#include <iostream>
#include <cstdint>
#include <limits>
#include <chrono>

// Example class demonstrating all supported simple types
class AllTypesData : public Serializable {
private:
    // Standard integer types
    int int_val_;
    long long_val_;
    long long long_long_val_;
    unsigned int uint_val_;
    unsigned long ulong_val_;
    unsigned long long ulong_long_val_;
    
    // Floating point types
    float float_val_;
    double double_val_;
    
    // Character and boolean types
    bool bool_val_;
    char char_val_;
    
    // String types
    std::string string_val_;
    
    // Fixed-width integer types (C++11)
    std::int8_t int8_val_;
    std::int16_t int16_val_;
    std::int32_t int32_val_;
    std::int64_t int64_val_;
    std::uint8_t uint8_val_;
    std::uint16_t uint16_val_;
    std::uint32_t uint32_val_;
    std::uint64_t uint64_val_;
    
public:
    AllTypesData() {
        // Initialize with diverse values to show range
        int_val_ = -42;
        long_val_ = -1234567890L;
        long_long_val_ = -9223372036854775807LL; // Close to LLONG_MIN
        uint_val_ = 42U;
        ulong_val_ = 1234567890UL;
        ulong_long_val_ = 18446744073709551614ULL; // Close to ULLONG_MAX
        
        float_val_ = 3.14159f;
        double_val_ = 2.718281828459045;
        
        bool_val_ = true;
        char_val_ = 'A';
        
        string_val_ = "Hello, 世界! 🌍";
        
        // Fixed-width types with edge values
        int8_val_ = -128;  // INT8_MIN
        int16_val_ = 32767; // INT16_MAX
        int32_val_ = -2147483648; // INT32_MIN
        int64_val_ = 9223372036854775807LL; // INT64_MAX
        uint8_val_ = 255; // UINT8_MAX
        uint16_val_ = 65535; // UINT16_MAX
        uint32_val_ = 4294967295U; // UINT32_MAX
        uint64_val_ = 18446744073709551615ULL; // UINT64_MAX
    }
    
    void visitProperties(TypedSerializer& serializer) const override {
        // Standard integer types
        serializer.serializeProperty("int_val", int_val_);
        serializer.serializeProperty("long_val", long_val_);
        serializer.serializeProperty("long_long_val", long_long_val_);
        serializer.serializeProperty("uint_val", uint_val_);
        serializer.serializeProperty("ulong_val", ulong_val_);
        serializer.serializeProperty("ulong_long_val", ulong_long_val_);
        
        // Floating point types
        serializer.serializeProperty("float_val", float_val_);
        serializer.serializeProperty("double_val", double_val_);
        
        // Character and boolean types
        serializer.serializeProperty("bool_val", bool_val_);
        serializer.serializeProperty("char_val", char_val_);
        
        // String types
        serializer.serializeProperty("string_val", string_val_);
        
        // Fixed-width integer types
        serializer.serializeProperty("int8_val", int8_val_);
        serializer.serializeProperty("int16_val", int16_val_);
        serializer.serializeProperty("int32_val", int32_val_);
        serializer.serializeProperty("int64_val", int64_val_);
        serializer.serializeProperty("uint8_val", uint8_val_);
        serializer.serializeProperty("uint16_val", uint16_val_);
        serializer.serializeProperty("uint32_val", uint32_val_);
        serializer.serializeProperty("uint64_val", uint64_val_);
    }
    
    // Getters for demonstration
    std::int64_t getInt64() const { return int64_val_; }
    std::uint64_t getUint64() const { return uint64_val_; }
    float getFloat() const { return float_val_; }
    char getChar() const { return char_val_; }
};

// Example showing type safety and overflow handling
class TypeSafetyDemo : public Serializable {
private:
    std::int8_t small_int_;
    std::uint64_t large_uint_;
    
public:
    TypeSafetyDemo(std::int8_t small, std::uint64_t large) 
        : small_int_(small), large_uint_(large) {}
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("small_int", small_int_);
        serializer.serializeProperty("large_uint", large_uint_);
    }
};

// Container with various numeric types
class NumericContainerDemo : public Serializable {
private:
    std::vector<std::int64_t> int64_vector_;
    std::vector<float> float_vector_;
    std::map<std::uint32_t, std::string> uint_to_string_map_;
    
public:
    NumericContainerDemo() {
        int64_vector_ = {-9223372036854775807LL, 0LL, 9223372036854775807LL};
        float_vector_ = {-3.14f, 0.0f, 2.718f, 1.414f};
        uint_to_string_map_ = {
            {1U, "one"},
            {1000000U, "million"},
            {4294967295U, "max_uint32"}
        };
    }
    
    void visitProperties(TypedSerializer& serializer) const override {
        serializer.serializeProperty("int64_vector", int64_vector_);
        serializer.serializeProperty("float_vector", float_vector_);
        serializer.serializeProperty("uint_to_string_map", uint_to_string_map_);
    }
};

void demonstrateTypeLimits() {
    std::cout << "=== Type Limits Demonstration ===\n";
    std::cout << "int8_t range: " << static_cast<int>(std::numeric_limits<std::int8_t>::min()) 
              << " to " << static_cast<int>(std::numeric_limits<std::int8_t>::max()) << "\n";
    std::cout << "uint64_t max: " << std::numeric_limits<std::uint64_t>::max() << "\n";
    std::cout << "float precision: " << std::numeric_limits<float>::digits10 << " decimal digits\n";
    std::cout << "double precision: " << std::numeric_limits<double>::digits10 << " decimal digits\n\n";
}

int main() {
    std::cout << "=== Enhanced Simple Types Support Demo ===\n\n";
    
    demonstrateTypeLimits();
    
    // Comprehensive type demonstration
    std::cout << "=== All Supported Types Serialization ===\n";
    AllTypesData all_types;
    JsonSerializer serializer;
    all_types.serialize(serializer);
    
    std::cout << "All types JSON:\n";
    std::cout << serializer.toJson() << "\n\n";
    
    // Type safety demonstration
    std::cout << "=== Type Safety Demo ===\n";
    TypeSafetyDemo type_demo(127, 18446744073709551615ULL); // Max values
    JsonSerializer type_serializer;
    type_demo.serialize(type_serializer);
    
    std::cout << "Type safety JSON:\n";
    std::cout << type_serializer.toJson() << "\n\n";
    
    // Container with numeric types
    std::cout << "=== Numeric Containers Demo ===\n";
    NumericContainerDemo container_demo;
    JsonSerializer container_serializer;
    container_demo.serialize(container_serializer);
    
    std::cout << "Numeric containers JSON:\n";
    std::cout << container_serializer.toJson() << "\n\n";
    
    // Performance test with different numeric types
    std::cout << "=== Performance Test (int64_t vs int) ===\n";
    const int iterations = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        TypeSafetyDemo demo(static_cast<std::int8_t>(i % 128), 
                           static_cast<std::uint64_t>(i) * 1000000ULL);
        JsonSerializer perf_serializer;
        demo.serialize(perf_serializer);
        perf_serializer.clear();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Enhanced types performance: " << duration.count() 
              << " microseconds for " << iterations << " iterations\n";
    
    // Demonstrate specific type advantages
    std::cout << "\n=== Type-Specific Advantages ===\n";
    std::cout << "✓ int64_t: Platform-independent 64-bit integers\n";
    std::cout << "✓ uint64_t: Full 64-bit unsigned range (0 to " << std::numeric_limits<std::uint64_t>::max() << ")\n";
    std::cout << "✓ float: 32-bit precision for memory efficiency\n";
    std::cout << "✓ char: Single character serialization\n";
    std::cout << "✓ Fixed-width types: Guaranteed size across platforms\n";
    std::cout << "✓ Type safety: No implicit conversions, exact type matching\n";
    
    return 0;
}
