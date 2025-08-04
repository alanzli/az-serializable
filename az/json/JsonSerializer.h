// JsonSerializer.h
#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "az/Serializable.h"
#include "az/TypedSerializer.h"

namespace az {
template <typename PropertyContainer, bool reverse = false>
class JsonSerializerBase : public TypedSerializer {
   public:
    JsonSerializerBase() = default;

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

        // Iterate over properties in reverse order if requested
        if constexpr (reverse) {
            for (auto it = properties_.rbegin(); it != properties_.rend(); ++it) {
                if (!first) result += ',';
                result += '\"';
                result += it->first;
                result += "\":";
                result += it->second;
                first = false;
            }
        } else {
            for (const auto &[key, val] : properties_) {
                if (!first) result += ',';
                result += '\"';
                result += key;
                result += "\":";
                result += val;
                first = false;
            }
        }

        result += '}';

        return result;
    }

    // Method to clear and reuse serializer
    void clear() {
        properties_.clear();
        if constexpr (!az::is_associative_container<PropertyContainer>::value) {
            existed_properties_.clear();
        }
    }

   protected:
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
        JsonSerializerBase<PropertyContainer, reverse> nested;
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

    void processProperty(const std::string &name, const std::string &value) override {
        if constexpr (az::is_associative_container<PropertyContainer>::value) {
            properties_[name] = value;
        } else {
            auto it = existed_properties_.find(name);
            if (it != existed_properties_.end()) {
                // Property already exists, update value at its index
                properties_[it->second].second = value;
            } else {
                properties_.emplace_back(name, value);
                existed_properties_[name] = static_cast<int>(properties_.size());
            }
        }
    }

    char toHex(int digit) const {
        return digit < 10 ? static_cast<char>('0' + digit) : static_cast<char>('A' + digit - 10);
    }

   private:
    PropertyContainer properties_;
    std::conditional_t<az::is_associative_container<PropertyContainer>::value, std::monostate,
                       std::unordered_map<std::string, int>>
        existed_properties_;
};

using UnorderedJsonSerializer = JsonSerializerBase<std::unordered_map<std::string, std::string>>;

using OrderedJsonSerializer = JsonSerializerBase<std::map<std::string, std::string>>;

using FIFOJsonSerializer = JsonSerializerBase<std::vector<std::pair<std::string, std::string>>>;

using LIFOJsonSerializer = JsonSerializerBase<std::vector<std::pair<std::string, std::string>>, true>;

using JsonSerializer = UnorderedJsonSerializer;  // Default to unordered for simplicity

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
