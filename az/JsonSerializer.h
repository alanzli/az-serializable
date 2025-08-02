// JsonSerializer.h
#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "az/Serializable.h"
#include "az/Serializer.h"

namespace az {
class JsonSerializer : public TypedSerializer {
   public:
    // Reserve initial capacity for better performance
    JsonSerializer() {
        properties_.reserve(16);  // Assume typical object has ~16 properties
        json_parts_.reserve(32);  // Reserve for JSON parts
    }

    // Type-specific serialization methods - no std::any overhead
    void serializeProperty(const std::string &name, int value) override { addProperty(name, std::to_string(value)); }

    void serializeProperty(const std::string &name, long value) override { addProperty(name, std::to_string(value)); }

    void serializeProperty(const std::string &name, long long value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeProperty(const std::string &name, unsigned int value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeProperty(const std::string &name, unsigned long value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeProperty(const std::string &name, unsigned long long value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeProperty(const std::string &name, float value) override { addProperty(name, std::to_string(value)); }

    void serializeProperty(const std::string &name, double value) override { addProperty(name, std::to_string(value)); }

    void serializeProperty(const std::string &name, bool value) override {
        addProperty(name, value ? "true" : "false");
    }

    void serializeProperty(const std::string &name, char value) override {
        // Escape JSON string properly for single character
        addProperty(name, "\"" + escapeJsonString(std::string(1, value)) + "\"");
    }

    void serializeProperty(const std::string &name, const std::string &value) override {
        // Escape JSON string properly
        addProperty(name, "\"" + escapeJsonString(value) + "\"");
    }

    void serializeProperty(const std::string &name, const char *value) override {
        serializeProperty(name, std::string(value));
    }

    // Fixed-width integer types that are always distinct
    void serializeProperty(const std::string &name, std::int8_t value) override {
        addProperty(name, std::to_string(static_cast<int>(value)));
    }

    void serializeProperty(const std::string &name, std::int16_t value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeProperty(const std::string &name, std::uint8_t value) override {
        addProperty(name, std::to_string(static_cast<unsigned int>(value)));
    }

    void serializeProperty(const std::string &name, std::uint16_t value) override {
        addProperty(name, std::to_string(value));
    }

    // Helper methods for conditional fixed-width types
    void serializeInt32(const std::string &name, std::int32_t value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeInt64(const std::string &name, std::int64_t value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeUint32(const std::string &name, std::uint32_t value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeUint64(const std::string &name, std::uint64_t value) override {
        addProperty(name, std::to_string(value));
    }

    void serializeNestedObject(const std::string &name, const Serializable &value) override {
        JsonSerializer nested;
        value.serialize(nested);
        addProperty(name, nested.toJson());
    }

    // Container serialization methods
    void serializeArray(const std::string &name, const std::vector<std::string> &json_elements) override {
        addProperty(name, createJsonArray(json_elements));
    }

    void serializeObject(const std::string &name,
                         const std::vector<std::pair<std::string, std::string>> &json_pairs) override {
        addProperty(name, createJsonObject(json_pairs));
    }

    std::string toJson() const {
        if (properties_.empty()) {
            return "{}";
        }

        // Pre-calculate approximate size to reduce reallocations
        size_t estimated_size = 2;  // for {}
        for (const auto &[key, val] : properties_) {
            estimated_size += key.length() + val.length() + 5;  // quotes, colon, comma
        }

        std::string result;
        result.reserve(estimated_size);

        result += '{';
        bool first = true;
        for (const auto &[key, val] : properties_) {
            if (!first) result += ',';
            result += '\"';
            result += key;
            result += "\":";
            result += val;
            first = false;
        }
        result += '}';

        return result;
    }

    // Method to clear and reuse serializer
    void clear() {
        properties_.clear();
        json_parts_.clear();
    }

    // Make helper methods public for use in composition
    std::string escapeJsonString(const std::string &input) const override {
        std::string result;
        result.reserve(input.length() + 10);  // Reserve some extra space for escape chars

        for (char c : input) {
            switch (c) {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\b':
                    result += "\\b";
                    break;
                case '\f':
                    result += "\\f";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    if (c < 0x20) {
                        result += "\\u";
                        result += toHex((c >> 12) & 0xF);
                        result += toHex((c >> 8) & 0xF);
                        result += toHex((c >> 4) & 0xF);
                        result += toHex(c & 0xF);
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }

    std::string serializeNestedToString(const Serializable &obj) const override {
        JsonSerializer nested;
        obj.serialize(nested);
        return nested.toJson();
    }

    std::string createJsonArray(const std::vector<std::string> &elements) const override {
        if (elements.empty()) {
            return "[]";
        }

        std::string result = "[";
        bool first = true;
        for (const auto &element : elements) {
            if (!first) result += ",";
            result += element;
            first = false;
        }
        result += "]";
        return result;
    }

    std::string createJsonObject(const std::vector<std::pair<std::string, std::string>> &pairs) const override {
        if (pairs.empty()) {
            return "{}";
        }

        std::string result = "{";
        bool first = true;
        for (const auto &[key, value] : pairs) {
            if (!first) result += ",";
            result += key + ":" + value;
            first = false;
        }
        result += "}";
        return result;
    }

   protected:
    void serializeUnsupportedType(const std::string &name) override { addProperty(name, "\"[unsupported type]\""); }

   private:
    // Use unordered_map for better performance on large objects
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> json_parts_;  // For complex nested structures

    void addProperty(const std::string &name, const std::string &value) { properties_[name] = value; }

    char toHex(int digit) const { return digit < 10 ? '0' + digit : 'A' + digit - 10; }
};
}  // namespace az