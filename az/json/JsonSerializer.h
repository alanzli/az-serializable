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
    JsonSerializer() = default;

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
        parts_.clear();
    }

    // Make helper methods public for use in composition
    std::string escapeString(const std::string &input) const override {
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

   protected:
    std::string serializeToString(long long value) const override { return std::to_string(value); }

    std::string serializeToString(unsigned long long value) const override { return std::to_string(value); }

    std::string serializeToString(double value) const override { return std::to_string(value); }

    std::string serializeToString(bool value) const override { return value ? "true" : "false"; }

    std::string serializeToString(char value) const override {
        // Escape JSON string properly for single character
        return "\"" + escapeString(std::string(1, value)) + "\"";
    }

    std::string serializeToString(const std::string &value) const override {
        // Escape JSON string properly
        return "\"" + escapeString(value) + "\"";
    }

    std::string serializeToString(const az::Serializable &value) const override {
        JsonSerializer nested;
        value.serialize(nested);
        return nested.toJson();
    }

    std::string serializeToString(const std::vector<std::string> &elements) const override {
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

    std::string serializeToString(const std::vector<std::pair<std::string, std::string>> &pairs) const override {
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

   private:
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> parts_;  // For complex nested structures

    void processProperty(const std::string &name, const std::string &value) override { properties_[name] = value; }

    char toHex(int digit) const { return digit < 10 ? '0' + digit : 'A' + digit - 10; }
};
}  // namespace az