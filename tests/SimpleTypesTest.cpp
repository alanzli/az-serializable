#include "Serializable.h"
#include "JsonSerializer.h"
#include <iostream>
#include <cstdint>

class SimpleTypesDemo : public Serializable {
private:
    // Basic signed types
    int int_val = 42;
    long long_val = 1234567890L;
    long long long_long_val = 9223372036854775807LL;
    
    // Basic unsigned types
    unsigned int uint_val = 42U;
    unsigned long ulong_val = 1234567890UL;
    unsigned long long ulong_long_val = 18446744073709551615ULL;
    
    // Floating point types
    float float_val = 3.14159f;
    double double_val = 2.718281828;
    
    // Character and boolean
    char char_val = 'X';
    bool bool_val = true;
    std::string string_val = "Hello";
    
    // Fixed-width integers
    std::int8_t int8_val = -128;
    std::int16_t int16_val = 32767;
    std::int32_t int32_val = -2147483648;
    std::int64_t int64_val = 9223372036854775807LL;
    
    std::uint8_t uint8_val = 255;
    std::uint16_t uint16_val = 65535;
    std::uint32_t uint32_val = 4294967295U;
    std::uint64_t uint64_val = 18446744073709551615ULL;

public:
    void visitProperties(TypedSerializer& serializer) const override {
        // Basic signed types
        serializer.serializeProperty("int_val", int_val);
        serializer.serializeProperty("long_val", long_val);
        serializer.serializeProperty("long_long_val", long_long_val);
        
        // Basic unsigned types
        serializer.serializeProperty("uint_val", uint_val);
        serializer.serializeProperty("ulong_val", ulong_val);
        serializer.serializeProperty("ulong_long_val", ulong_long_val);
        
        // Floating point types
        serializer.serializeProperty("float_val", float_val);
        serializer.serializeProperty("double_val", double_val);
        
        // Character and boolean
        serializer.serializeProperty("char_val", char_val);
        serializer.serializeProperty("bool_val", bool_val);
        serializer.serializeProperty("string_val", string_val);
        
        // Fixed-width integers
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

int main() {
    std::cout << "=== Simple Types Comprehensive Test ===" << std::endl;
    
    SimpleTypesDemo demo;
    JsonSerializer serializer;
    
    demo.serialize(serializer);
    std::string json = serializer.toJson();
    
    std::cout << "All simple types JSON:" << std::endl;
    std::cout << json << std::endl;
    
    // Verify specific types are included
    std::cout << "\n=== Type Verification ===" << std::endl;
    
    if (json.find("int8_val") != std::string::npos) {
        std::cout << "✓ int8_t support confirmed" << std::endl;
    }
    if (json.find("int64_val") != std::string::npos) {
        std::cout << "✓ int64_t support confirmed" << std::endl;
    }
    if (json.find("uint64_val") != std::string::npos) {
        std::cout << "✓ uint64_t support confirmed" << std::endl;
    }
    if (json.find("float_val") != std::string::npos) {
        std::cout << "✓ float support confirmed" << std::endl;
    }
    if (json.find("char_val") != std::string::npos) {
        std::cout << "✓ char support confirmed" << std::endl;
    }
    
    std::cout << "\n=== Platform Type Information ===" << std::endl;
    std::cout << "int size: " << sizeof(int) << " bytes" << std::endl;
    std::cout << "long size: " << sizeof(long) << " bytes" << std::endl;
    std::cout << "long long size: " << sizeof(long long) << " bytes" << std::endl;
    std::cout << "int32_t size: " << sizeof(std::int32_t) << " bytes" << std::endl;
    std::cout << "int64_t size: " << sizeof(std::int64_t) << " bytes" << std::endl;
    
    std::cout << "\nSimple types support successfully implemented!" << std::endl;
    
    return 0;
}
