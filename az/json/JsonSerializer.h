// JsonSerializer.h
#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "az/Serializable.h"
#include "az/TypedSerializer.h"

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
    std::string serializeToString(std::int64_t value) const override { return std::to_string(value); }

    std::string serializeToString(std::uint64_t value) const override { return std::to_string(value); }

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
            if (key.empty() || key.front() != '\"' || key.back() != '\"') {
                result += '\"' + key + '\"';
            } else {
                result += key;
            }
            result += ":" + value;
            first = false;
        }
        result += "}";
        return result;
    }

   private:
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> parts_;  // For complex nested structures

    void processProperty(const std::string &name, const std::string &value) override { properties_[name] = value; }

    char toHex(int digit) const {
        return digit < 10 ? static_cast<char>('0' + digit) : static_cast<char>('A' + digit - 10);
    }
};

// A lightweight heuristic to check if a string is likely valid JSON.
// This does NOT fully validate JSON, but catches common mistakes.
bool isLikelyValidJson(const std::string &json) {
    // Trim leading/trailing whitespace
    auto ltrim = [](const std::string &s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        return (start == std::string::npos) ? "" : s.substr(start);
    };
    auto rtrim = [](const std::string &s) {
        size_t end = s.find_last_not_of(" \t\n\r");
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    };
    std::string trimmed = ltrim(rtrim(json));

    if (trimmed.empty()) return false;
    // Must start and end with {} or []
    if (!((trimmed.front() == '{' && trimmed.back() == '}') || (trimmed.front() == '[' && trimmed.back() == ']'))) {
        return false;
    }

    // Check for balanced braces/brackets and quotes
    int brace = 0, bracket = 0;
    bool in_string = false;
    bool escape = false;
    for (char c : trimmed) {
        if (escape) {
            escape = false;
            continue;
        }
        if (c == '\\') {
            escape = true;
            continue;
        }
        if (c == '"') {
            in_string = !in_string;
            continue;
        }
        if (in_string) continue;
        if (c == '{') ++brace;
        if (c == '}') --brace;
        if (c == '[') ++bracket;
        if (c == ']') --bracket;
        if (brace < 0 || bracket < 0) return false;
    }
    if (brace != 0 || bracket != 0 || in_string) return false;

    // Basic check for colon in objects and comma separation
    if (trimmed.front() == '{') {
        if (trimmed.find(':') == std::string::npos) return false;
    }

    return true;
}

}  // namespace az
