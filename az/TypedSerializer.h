#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "az/Validator.h"

namespace az {
// Forward declarations
class Serializable;

// Type traits for container detection
template <typename T>
struct is_container : std::false_type {};

template <typename T, typename Alloc>
struct is_container<std::vector<T, Alloc>> : std::true_type {};

template <typename T, typename Alloc>
struct is_container<std::list<T, Alloc>> : std::true_type {};

template <typename T, typename Alloc>
struct is_container<std::deque<T, Alloc>> : std::true_type {};

template <typename T, std::size_t N>
struct is_container<std::array<T, N>> : std::true_type {};

template <typename T, typename Compare, typename Alloc>
struct is_container<std::set<T, Compare, Alloc>> : std::true_type {};

template <typename T, typename Hash, typename Equal, typename Alloc>
struct is_container<std::unordered_set<T, Hash, Equal, Alloc>> : std::true_type {};

// Type traits for associative container detection
template <typename T>
struct is_associative_container : std::false_type {};

template <typename K, typename V, typename Compare, typename Alloc>
struct is_associative_container<std::map<K, V, Compare, Alloc>> : std::true_type {};

template <typename K, typename V, typename Hash, typename Equal, typename Alloc>
struct is_associative_container<std::unordered_map<K, V, Hash, Equal, Alloc>> : std::true_type {};

// Helper to detect if type has begin/end (for range-based for)
template <typename T>
using has_begin_end =
    std::conjunction<std::is_same<decltype(std::begin(std::declval<T &>())), decltype(std::begin(std::declval<T &>()))>,
                     std::is_same<decltype(std::end(std::declval<T &>())), decltype(std::end(std::declval<T &>()))>>;

// Type-safe serializer interface
class TypedSerializer {
   public:
    TypedSerializer() : validator_(nullptr) {}
    virtual ~TypedSerializer() = default;

    template <typename T>
    void serializeProperty(const std::string &name, const T &value) {
        std::string serialized_value = serializeElementToString(value);
        if (validator_) {
            ValidationResult result = validator_->validate(name, value, serialized_value);
            if (!result.is_valid) {
                throw ValidationException("Validation failed for property '" + name + "': " + result.error_message);
            }
        }
        processProperty(name, serialized_value);
    }

    void setValidator(const Validator *validator) { validator_ = validator; }

    // Validate a serialized property without processing it
    template <typename T>
    ValidationResult validateProperty(const std::string &name, const T &value) const {
        if (!validator_) {
            return ValidationResult(true, "No validator set");
        }

        std::string serialized_value = serializeElementToString(value);
        return validator_->validate(name, value, serialized_value);
    }

    // Validate just the serialized value
    ValidationResult validateSerializedProperty(const std::string &name, const std::string &serialized_value) const {
        if (!validator_) {
            return ValidationResult(true, "No validator set");
        }

        return validator_->validateSerialized(name, serialized_value);
    }

   protected:
    virtual void processProperty(const std::string &name, const std::string &value) = 0;
    virtual std::string serializeToString(bool value) const = 0;
    virtual std::string serializeToString(char value) const = 0;
    virtual std::string serializeToString(const std::string &value) const = 0;
    virtual std::string serializeToString(const Serializable &obj) const = 0;
    virtual std::string serializeToString(const std::vector<std::string> &elements) const = 0;
    virtual std::string serializeToString(const std::vector<std::pair<std::string, std::string>> &pairs) const = 0;

    virtual std::string serializeToString(std::int64_t value) const { return std::to_string(value); }
    virtual std::string serializeToString(std::uint64_t value) const { return std::to_string(value); }
    virtual std::string serializeToString(double value) const { return std::to_string(value); }

    virtual std::string serializeUnsupportedType() const { return "\"[unsupported type]\""; }

    virtual std::string serializeToString(float value) const { return serializeToString(static_cast<double>(value)); }

    virtual std::string serializeToString(const char *value) const { return serializeToString(std::string(value)); }

    virtual std::string escapeString(const std::string &input) const { return input; }

    template <typename T>
    std::string serializeElementToString(const T &value) const {
        if constexpr (std::is_same_v<T, int>) {
            return serializeToString(static_cast<std::int64_t>(value));
        } else if constexpr (std::is_same_v<T, long>) {
            return serializeToString(static_cast<std::int64_t>(value));
        } else if constexpr (std::is_same_v<T, long long>) {
            return serializeToString(static_cast<std::int64_t>(value));
        } else if constexpr (std::is_same_v<T, unsigned int>) {
            return serializeToString(static_cast<std::uint64_t>(value));
        } else if constexpr (std::is_same_v<T, unsigned long>) {
            return serializeToString(static_cast<std::uint64_t>(value));
        } else if constexpr (std::is_same_v<T, unsigned long long>) {
            return serializeToString(static_cast<std::uint64_t>(value));
        } else if constexpr (std::is_same_v<T, float>) {
            return serializeToString(static_cast<double>(value));
        } else if constexpr (std::is_same_v<T, double>) {
            return serializeToString(static_cast<double>(value));
        } else if constexpr (std::is_same_v<T, bool>) {
            return serializeToString(static_cast<bool>(value));
        } else if constexpr (std::is_same_v<T, char>) {
            return serializeToString(static_cast<char>(value));
        } else if constexpr (std::is_same_v<T, std::string>) {
            return serializeToString(static_cast<const std::string &>(value));
        } else if constexpr (std::is_same_v<T, const char *>) {
            return serializeToString(value);
        } else if constexpr (std::is_same_v<T, std::int8_t>) {
            return serializeToString(static_cast<std::int64_t>(value));
        } else if constexpr (std::is_same_v<T, std::int16_t>) {
            return serializeToString(static_cast<std::int64_t>(value));
        } else if constexpr (std::is_same_v<T, std::uint8_t>) {
            return serializeToString(static_cast<std::uint64_t>(value));
        } else if constexpr (std::is_same_v<T, std::uint16_t>) {
            return serializeToString(static_cast<std::uint64_t>(value));
        } else if constexpr (std::is_base_of_v<Serializable, T>) {
            return serializeToString(static_cast<const Serializable &>(value));
        } else if constexpr (is_associative_container<T>::value) {
            return serializeAssociativeContainer(value);
        } else if constexpr (is_container<T>::value || has_begin_end<T>::value) {
            return serializeSequenceContainer(value);
        } else {
            return serializeUnsupportedType();
        }
    }

    template <typename Container>
    std::string serializeSequenceContainer(const Container &container) const {
        std::vector<std::string> elements;
        elements.reserve(container.size());
        for (const auto &element : container) {
            elements.push_back(serializeElementToString(element));
        }
        return serializeToString(elements);
    }

    template <typename Container>
    std::string serializeAssociativeContainer(const Container &container) const {
        std::vector<std::pair<std::string, std::string>> pairs;
        pairs.reserve(container.size());
        for (const auto &[key, value] : container) {
            std::string key_str = serializeElementToString(key);
            std::string value_str = serializeElementToString(value);
            pairs.emplace_back(std::move(key_str), std::move(value_str));
        }
        return serializeToString(pairs);
    }

   private:
    const Validator *validator_;
};
}  // namespace az
