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
    virtual ~TypedSerializer() = default;

    template <typename T>
    void serializeProperty(const std::string &name, const T &value) {
        if constexpr (std::is_same_v<T, int>) {
            serializePropertyImpl(name, static_cast<int>(value));
        } else if constexpr (std::is_same_v<T, long>) {
            serializePropertyImpl(name, static_cast<long>(value));
        } else if constexpr (std::is_same_v<T, long long>) {
            serializePropertyImpl(name, static_cast<long long>(value));
        } else if constexpr (std::is_same_v<T, unsigned int>) {
            serializePropertyImpl(name, static_cast<unsigned int>(value));
        } else if constexpr (std::is_same_v<T, unsigned long>) {
            serializePropertyImpl(name, static_cast<unsigned long>(value));
        } else if constexpr (std::is_same_v<T, unsigned long long>) {
            serializePropertyImpl(name, static_cast<unsigned long long>(value));
        } else if constexpr (std::is_same_v<T, float>) {
            serializePropertyImpl(name, static_cast<float>(value));
        } else if constexpr (std::is_same_v<T, double>) {
            serializePropertyImpl(name, static_cast<double>(value));
        } else if constexpr (std::is_same_v<T, bool>) {
            serializePropertyImpl(name, static_cast<bool>(value));
        } else if constexpr (std::is_same_v<T, char>) {
            serializePropertyImpl(name, static_cast<char>(value));
        } else if constexpr (std::is_same_v<T, std::string>) {
            serializePropertyImpl(name, static_cast<const std::string &>(value));
        } else if constexpr (std::is_same_v<T, std::int8_t>) {
            serializePropertyImpl(name, static_cast<std::int8_t>(value));
        } else if constexpr (std::is_same_v<T, std::int16_t>) {
            serializePropertyImpl(name, static_cast<std::int16_t>(value));
        } else if constexpr (std::is_same_v<T, std::uint8_t>) {
            serializePropertyImpl(name, static_cast<std::uint8_t>(value));
        } else if constexpr (std::is_same_v<T, std::uint16_t>) {
            serializePropertyImpl(name, static_cast<std::uint16_t>(value));
        } else if constexpr (std::is_same_v<T, std::int32_t>) {
            if constexpr (std::is_same_v<std::int32_t, int>) {
                serializePropertyImpl(name, static_cast<int>(value));
            } else if constexpr (std::is_same_v<std::int32_t, long>) {
                serializePropertyImpl(name, static_cast<long>(value));
            } else {
                serializeInt32(name, value);
            }
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            if constexpr (std::is_same_v<std::int64_t, long>) {
                serializePropertyImpl(name, static_cast<long>(value));
            } else if constexpr (std::is_same_v<std::int64_t, long long>) {
                serializePropertyImpl(name, static_cast<long long>(value));
            } else {
                serializeInt64(name, value);
            }
        } else if constexpr (std::is_same_v<T, std::uint32_t>) {
            if constexpr (std::is_same_v<std::uint32_t, unsigned int>) {
                serializePropertyImpl(name, static_cast<unsigned int>(value));
            } else if constexpr (std::is_same_v<std::uint32_t, unsigned long>) {
                serializePropertyImpl(name, static_cast<unsigned long>(value));
            } else {
                serializeUint32(name, value);
            }
        } else if constexpr (std::is_same_v<T, std::uint64_t>) {
            if constexpr (std::is_same_v<std::uint64_t, unsigned long>) {
                serializePropertyImpl(name, static_cast<unsigned long>(value));
            } else if constexpr (std::is_same_v<std::uint64_t, unsigned long long>) {
                serializePropertyImpl(name, static_cast<unsigned long long>(value));
            } else {
                serializeUint64(name, value);
            }
        } else if constexpr (std::is_base_of_v<Serializable, T>) {
            serializeNestedObject(name, static_cast<const Serializable &>(value));
        } else if constexpr (is_associative_container<T>::value) {
            serializeAssociativeContainer(name, value);
        } else if constexpr (is_container<T>::value || has_begin_end<T>::value) {
            serializeSequenceContainer(name, value);
        } else {
            serializeUnsupportedType(name);
        }
    }

   protected:
    virtual void serializePropertyImpl(const std::string &name, long long value) = 0;
    virtual void serializePropertyImpl(const std::string &name, unsigned long long value) = 0;
    virtual void serializePropertyImpl(const std::string &name, double value) = 0;
    virtual void serializePropertyImpl(const std::string &name, bool value) = 0;
    virtual void serializePropertyImpl(const std::string &name, char value) = 0;
    virtual void serializePropertyImpl(const std::string &name, const std::string &value) = 0;

    virtual void serializeNestedObject(const std::string &name, const Serializable &value) = 0;
    virtual void serializeArray(const std::string &name, const std::vector<std::string> &elements) = 0;
    virtual void serializeObject(const std::string &name,
                                 const std::vector<std::pair<std::string, std::string>> &pairs) = 0;

    virtual std::string serializeNestedToString(const Serializable &obj) const = 0;
    virtual std::string createArray(const std::vector<std::string> &elements) const = 0;
    virtual std::string createObject(const std::vector<std::pair<std::string, std::string>> &pairs) const = 0;
    virtual void serializeUnsupportedType(const std::string &name) = 0;

    // Overloaded methods for different types
    virtual void serializePropertyImpl(const std::string &name, int value) {
        serializePropertyImpl(name, static_cast<long long>(value));
    }
    virtual void serializePropertyImpl(const std::string &name, long value) {
        serializePropertyImpl(name, static_cast<long long>(value));
    }

    virtual void serializePropertyImpl(const std::string &name, unsigned int value) {
        serializePropertyImpl(name, static_cast<unsigned long long>(value));
    }
    virtual void serializePropertyImpl(const std::string &name, unsigned long value) {
        serializePropertyImpl(name, static_cast<unsigned long long>(value));
    }

    virtual void serializePropertyImpl(const std::string &name, float value) {
        serializePropertyImpl(name, static_cast<double>(value));
    }

    virtual void serializePropertyImpl(const std::string &name, const char *value) {
        serializePropertyImpl(name, std::string(value));
    }
    virtual void serializePropertyImpl(const std::string &name, std::int8_t value) {
        serializePropertyImpl(name, static_cast<int>(value));
    }
    virtual void serializePropertyImpl(const std::string &name, std::int16_t value) {
        serializePropertyImpl(name, static_cast<int>(value));
    }

    template <typename T = std::int32_t>
    typename std::enable_if_t<!std::is_same_v<T, int> && !std::is_same_v<T, long> && !std::is_same_v<T, long long>>
    serializePropertyImpl(const std::string &name, std::int32_t value) {
        serializeInt32(name, value);
    }
    template <typename T = std::int64_t>
    typename std::enable_if_t<!std::is_same_v<T, int> && !std::is_same_v<T, long> && !std::is_same_v<T, long long>>
    serializePropertyImpl(const std::string &name, std::int64_t value) {
        serializeInt64(name, value);
    }
    virtual void serializePropertyImpl(const std::string &name, std::uint8_t value) {
        serializePropertyImpl(name, static_cast<unsigned int>(value));
    }
    virtual void serializePropertyImpl(const std::string &name, std::uint16_t value) {
        serializePropertyImpl(name, static_cast<unsigned int>(value));
    }

    template <typename T = std::uint32_t>
    typename std::enable_if_t<!std::is_same_v<T, unsigned int> && !std::is_same_v<T, unsigned long> &&
                              !std::is_same_v<T, unsigned long long>>
    serializePropertyImpl(const std::string &name, std::uint32_t value) {
        serializeUint32(name, value);
    }
    template <typename T = std::uint64_t>
    typename std::enable_if_t<!std::is_same_v<T, unsigned int> && !std::is_same_v<T, unsigned long> &&
                              !std::is_same_v<T, unsigned long long>>
    serializePropertyImpl(const std::string &name, std::uint64_t value) {
        serializeUint64(name, value);
    }

    virtual void serializeInt32(const std::string &name, std::int32_t value) {
        serializePropertyImpl(name, static_cast<long long>(value));
    }
    virtual void serializeInt64(const std::string &name, std::int64_t value) {
        serializePropertyImpl(name, static_cast<long long>(value));
    }
    virtual void serializeUint32(const std::string &name, std::uint32_t value) {
        serializePropertyImpl(name, static_cast<unsigned long long>(value));
    }
    virtual void serializeUint64(const std::string &name, std::uint64_t value) {
        serializePropertyImpl(name, static_cast<unsigned long long>(value));
    }

    virtual std::string escapeString(const std::string &input) const { return input; }

   private:
    template <typename Container>
    void serializeSequenceContainer(const std::string &name, const Container &container) {
        std::vector<std::string> elements;
        elements.reserve(container.size());
        for (const auto &element : container) {
            elements.push_back(serializeElementToString(element));
        }
        serializeArray(name, elements);
    }
    template <typename Container>
    void serializeAssociativeContainer(const std::string &name, const Container &container) {
        std::vector<std::pair<std::string, std::string>> pairs;
        pairs.reserve(container.size());
        for (const auto &[key, value] : container) {
            std::string key_str = serializeElementToString(key);
            std::string value_str = serializeElementToString(value);
            pairs.emplace_back(std::move(key_str), std::move(value_str));
        }
        serializeObject(name, pairs);
    }
    template <typename T>
    std::string serializeElementToString(const T &element) {
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, long>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, long long>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, unsigned int>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, unsigned long>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, unsigned long long>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, bool>) {
            return element ? "true" : "false";
        } else if constexpr (std::is_same_v<T, char>) {
            return "\"" + std::string(1, element) + "\"";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + escapeString(element) + "\"";
        } else if constexpr (std::is_same_v<T, std::int8_t>) {
            return std::to_string(static_cast<int>(element));
        } else if constexpr (std::is_same_v<T, std::int16_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, std::int32_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, std::uint8_t>) {
            return std::to_string(static_cast<unsigned int>(element));
        } else if constexpr (std::is_same_v<T, std::uint16_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, std::uint32_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_same_v<T, std::uint64_t>) {
            return std::to_string(element);
        } else if constexpr (std::is_base_of_v<Serializable, T>) {
            return serializeNestedToString(element);
        } else if constexpr (is_associative_container<T>::value) {
            std::vector<std::pair<std::string, std::string>> pairs;
            for (const auto &[key, value] : element) {
                pairs.emplace_back(serializeElementToString(key), serializeElementToString(value));
            }
            return createObject(pairs);
        } else if constexpr (is_container<T>::value || has_begin_end<T>::value) {
            std::vector<std::string> elements;
            for (const auto &nested_element : element) {
                elements.push_back(serializeElementToString(nested_element));
            }
            return createArray(elements);
        } else {
            return "\"[unsupported type]\"";
        }
    }
};

}  // namespace az
