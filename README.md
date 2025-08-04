# C++ Serialization Framework

A high-performance, type-safe C++ serialization framework with comprehensive support for simple types, containers, and validation.

**Header-only:** All source headers are located in the `az/` folder. No source compilation required—just include the headers in your project.

## Features

### ✅ **Complete Type Support**

- **Basic Types**: `int`, `long`, `long long`, `unsigned` variants
- **Fixed-Width Types**: `int8_t`, `int16_t`, `int32_t`, `int64_t`, `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`
- **Floating Point**: `float`, `double`
- **Character & Boolean**: `char`, `bool`, `std::string`, `const char*`
- **Containers**: `std::vector`, `std::map`, `std::set`, `std::list`, `std::array`, `std::unordered_map`, etc.
- **Nested Objects**: Full support for serializable object composition

### 🚀 **Performance Features**

- **Zero Overhead**: Direct type dispatch without `std::any`
- **Template-Based**: Compile-time type resolution
- **Memory Efficient**: Optimized JSON generation with pre-allocation
- **Platform Independent**: Handles type aliasing automatically

### 🛡️ **Type Safety & Validation**

- **Compile-Time Checking**: Template constraints prevent invalid types
- **Comprehensive Validation**: Rule-based validation system with detailed error messages
- **Cross-Platform**: Consistent behavior across different architectures

### 🔍 **Validation System**

- **Type-Specific Rules**: Validate based on the actual data type
- **Property-Specific Rules**: Target validation rules for specific properties
- **General Rules**: Apply validation to all serialized values
- **Flexible Architecture**: Easy to add custom validation logic

## Quick Start

### Basic Usage

```cpp
#include "az/Serializable.h"
#include "az/json/JsonSerializer.h"

class Person : public az::Serializable {
   private:
    std::string name_;
    int age_;
    bool active_;

   public:
    Person(const std::string& name, int age, bool active) : name_(name), age_(age), active_(active) {}

    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("name", name_);
        serializer.serializeProperty("age", age_);
        serializer.serializeProperty("active", active_);
    }
};

int main() {
    Person person("Alice", 30, true);
    az::JsonSerializer serializer;
    person.serialize(serializer);

    std::cout << serializer.toJson() << std::endl;
    // Output: {"name":"Alice","age":30,"active":true}

    return 0;
}
```

### Validation Usage

```cpp
#include "az/Validator.h"

int main() {
    // Create a validator
    az::Validator validator;

    // Add type-specific rules
    validator.addRule<std::string>([](const std::string& prop, const std::string& value, const std::string& serialized) {
        if (value.length() < 3) {
            return az::ValidationResult(false, "String must be at least 3 characters long");
        }
        return az::ValidationResult(true);
    }, "String length >= 3");

    validator.addRule<int>([](const std::string& prop, const int& value, const std::string& serialized) {
        if (value < 0) {
            return az::ValidationResult(false, "Integer must be non-negative");
        }
        return az::ValidationResult(true);
    }, "Integer >= 0");

    // Add property-specific rules
    validator.addPropertyRule("email", [](const std::string& prop, const std::string& serialized) {
        if (serialized.find("@") == std::string::npos) {
            return az::ValidationResult(false, "Email must contain @ symbol");
        }
        return az::ValidationResult(true);
    }, "Email format validation");

    // Validate individual values
    auto result = validator.validate("username", std::string("ab"), "\"ab\"");
    if (!result.is_valid) {
        std::cout << "Validation failed: " << result.error_message << std::endl;
    }

    return 0;
}
```

### Container Support

```cpp
class DataContainer : public az::Serializable {
   private:
    std::vector<int> numbers_;
    std::map<std::string, double> values_;

   public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("numbers", numbers_);
        serializer.serializeProperty("values", values_);
    }
};
```

### Simple Types Example

```cpp
class AllTypes : public az::Serializable {
   private:
    std::int64_t big_number_;
    std::uint32_t count_;
    float precision_;
    char grade_;

   public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("big_number", big_number_);
        serializer.serializeProperty("count", count_);
        serializer.serializeProperty("precision", precision_);
        serializer.serializeProperty("grade", grade_);
    }
};
```

## Building

### Using CMake (Recommended)

```bash
# Create a build directory
mkdir build && cd build

# Configure the project (Linux/macOS/Windows)
cmake ..

# Build all examples
cmake --build .

# Build and copy headers to dist/
cmake --build . --target dist

# Clean build and dist directories
cmake --build . --target clean
```

## Examples

The framework includes several comprehensive examples:

- **`examples/UsageExample.cpp`** - Basic serialization with all supported types
- **`examples/ValidatorExample.cpp`** - Validator integration demonstration

## Architecture

### Core Classes

- **`Serializable`** - Base class for serializable objects
- **`TypedSerializer`** - Abstract serializer interface with type-safe methods
- **`JsonSerializer`** - JSON serialization implementation
- **`Validator`** - Comprehensive validation framework with rule management
- **`ValidationRule`** - Base class for custom validation rules

### Validator Classes

- **`ValidationResult`** - Encapsulates validation outcome and error messages
- **`TypedValidationRule<T>`** - Template-based type-specific validation rules
- **`PropertyValidationRule`** - Property-specific validation rules
- **`ValidationException`** - Exception thrown when validation fails

### Key Features

1. **Template Dispatch**: Automatic type resolution at compile time
2. **Platform Compatibility**: Handles type aliasing (e.g., `int32_t` = `int` on some platforms)
3. **Container Support**: Automatic serialization of STL containers
4. **Validation Framework**: Comprehensive rule-based validation system
5. **Performance Optimized**: Direct method calls, no virtual dispatch overhead

## Validation System

The Validator class provides three types of validation rules:

### Type-Specific Rules

Validate based on the actual C++ type:

```cpp
validator.addRule<std::string>([](const std::string& prop, const std::string& value, const std::string& serialized) {
    if (value.length() < 3) {
        return az::ValidationResult(false, "String too short");
    }
    return az::ValidationResult(true);
});
```

### Property-Specific Rules

Target specific property names:

```cpp
validator.addPropertyRule("email", [](const std::string& prop, const std::string& serialized) {
    if (serialized.find("@") == std::string::npos) {
        return az::ValidationResult(false, "Invalid email format");
    }
    return az::ValidationResult(true);
});
```

### General Rules

Apply to all serialized values:

```cpp
validator.addGeneralRule([](const std::string& prop, const std::string& serialized) {
    if (serialized.length() > 1000) {
        return az::ValidationResult(false, "Value too long");
    }
    return az::ValidationResult(true);
});
```

## Type Safety

The framework provides strong type safety through:

- Compile-time type checking with `constexpr if`
- Template constraints to prevent invalid types
- Platform-independent type handling
- No implicit conversions between numeric types

## Performance

- **Direct Type Dispatch**: Eliminates `std::any` overhead
- **Template Optimization**: Compile-time method resolution
- **Memory Efficiency**: Pre-allocated containers and optimized JSON generation
- **Zero Copy**: Direct serialization without intermediate representations

## Platform Support

**C++17** or later required
**Cross-Platform**: Works on Linux, Windows, macOS
**Architecture Independent**: Handles 32-bit and 64-bit systems
**Compiler Support**: GCC, Clang, MSVC
**Build System**: CMake (recommended for all platforms)

# AutoSerializable - Simplified Automatic Serialization

This extension to the az-serializable library provides automatic serialization without needing to manually implement the `visitProperties` method.

## Key Features

- **Automatic member serialization** using C++ templates and macros
- **Zero runtime overhead** - generates the same code as manual implementation
- **Full backward compatibility** with existing manual implementations
- **Type-safe** - all existing type safety and validation features preserved
- **Easy to use** - just use the `AZ_SERIALIZE` macro

## Basic Usage

### Before (Manual Implementation)

```cpp
class Person : public az::Serializable {
private:
    std::string name_;
    int age_;
    double height_;

public:
    void visitProperties(az::TypedSerializer& serializer) const override {
        serializer.serializeProperty("name_", name_);
        serializer.serializeProperty("age_", age_);
        serializer.serializeProperty("height_", height_);
    }
};
```

### After (Automatic Implementation)

```cpp
class Person : public az::Serializable {
private:
    std::string name_;
    int age_;
    double height_;

public:
    AZ_SERIALIZE(
        AZ_MEMBER(name_),
        AZ_MEMBER(age_),
        AZ_MEMBER(height_)
    )
};
```

## Complete Example

```cpp
#include "az/AutoSerializable.h"
#include "az/json/JsonSerializer.h"

class Employee : public az::Serializable {
private:
    std::string name_;
    int employee_id_;
    double salary_;
    std::vector<std::string> skills_;

public:
    Employee(const std::string& name, int id, double salary)
        : name_(name), employee_id_(id), salary_(salary) {}

    void addSkill(const std::string& skill) {
        skills_.push_back(skill);
    }

    // Automatically serialize these members
    AZ_SERIALIZE(
        AZ_MEMBER(name_),
        AZ_MEMBER(employee_id_),
        AZ_MEMBER(salary_),
        AZ_MEMBER(skills_)
    )
};

int main() {
    Employee emp("Alice Johnson", 1001, 75000.0);
    emp.addSkill("C++");
    emp.addSkill("Python");

    az::JsonSerializer serializer;
    emp.serialize(serializer);
    std::cout << serializer.toJson() << std::endl;
    // Output: {"name_":"Alice Johnson","employee_id_":1001,"salary_":75000.000000,"skills_":["C++","Python"]}

    return 0;
}
```

## Partial Serialization

You can control which members are serialized by only including them in the macro:

```cpp
class User : public az::Serializable {
private:
    std::string username_;
    std::string password_;  // Sensitive data
    std::string email_;

public:
    // Only serialize public information
    AZ_SERIALIZE(
        AZ_MEMBER(username_),
        AZ_MEMBER(email_)
        // password_ intentionally omitted
    )
};
```

## Supported Types

All types supported by the original `TypedSerializer`:

- Primitive types: `int`, `float`, `double`, `bool`, `char`, `std::string`
- Standard containers: `std::vector`, `std::map`, `std::set`, etc.
- Nested serializable objects
- Custom types with proper serialization support

## Requirements

- **C++17 or later** (uses fold expressions and other modern features)
- All members must be accessible where the macro is used (typically in public/protected sections)

## Performance

The AutoSerializable approach has:

- **Zero runtime overhead** - same performance as manual implementation
- **Minimal compile-time overhead** - template instantiation cost
- **Same memory usage** - no additional data structures

## Limitations

- Members must be named at compile-time (no dynamic member lists)
- Only works with direct member variables (not getter/setter methods)
- Requires C++17 for fold expressions
- Member names in JSON output include any naming conventions (like trailing underscores)

## Troubleshooting

**Compile Error**: "no matching function for call to 'serialize_auto'"

- Make sure you've added the `AZ_SERIALIZE` macro

**Compile Error**: "fold expression contains unexpanded parameter pack"

- Ensure you're compiling with C++17 or later (`-std=c++17` or `set(CMAKE_CXX_STANDARD 17)`)

**Runtime Error**: Member not found

- Check that all members listed in `AZ_MEMBER()` actually exist in the class

## Future Enhancements

- XML serialization support
- Binary serialization formats
- Deserialization framework
- Schema validation
- Custom type registration

## License

This project is available for educational and commercial use. See the source code for implementation details.
