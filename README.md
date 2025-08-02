# C++ Serialization Framework

A high-performance, type-safe C++ serialization framework with comprehensive support for simple types, containers, and validation.

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
- **Error Handling**: Comprehensive validation framework with detailed error messages
- **Cross-Platform**: Consistent behavior across different architectures

## Quick Start

### Basic Usage

```cpp
#include "Serializable.h"
#include "JsonSerializer.h"

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

```bash
# Build all examples
make all

# Run all tests
make test

# Build specific examples
make out/example           # Basic serialization example
```

## Examples

The framework includes several comprehensive examples:

- **`examples/UsageExample.cpp`** - Basic serialization with validation

## Architecture

### Core Classes

- **`Serializable`** - Base class for serializable objects
- **`TypedSerializer`** - Abstract serializer interface with type-safe methods
- **`JsonSerializer`** - JSON serialization implementation
- **`ValidatedSerializer`** - Base class for validation support

### Key Features

1. **Template Dispatch**: Automatic type resolution at compile time
2. **Platform Compatibility**: Handles type aliasing (e.g., `int32_t` = `int` on some platforms)
3. **Container Support**: Automatic serialization of STL containers
4. **Validation Framework**: Error collection and reporting
5. **Performance Optimized**: Direct method calls, no virtual dispatch overhead

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

- **C++17** or later required
- **Cross-Platform**: Works on Linux, Windows, macOS
- **Architecture Independent**: Handles 32-bit and 64-bit systems
- **Compiler Support**: GCC, Clang, MSVC

## Future Enhancements

- XML serialization support
- Binary serialization formats
- Deserialization framework
- Schema validation
- Custom type registration

## License

This project is available for educational and commercial use. See the source code for implementation details.
