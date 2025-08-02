// SerializationErrors.h
#pragma once

#include <exception>
#include <string>
#include <vector>

#include "az/Serializable.h"
#include "az/Serializer.h"

namespace az {
// Custom exception for serialization errors
class SerializationException : public std::exception {
   public:
    explicit SerializationException(const std::string &message) : message_(message) {}

    const char *what() const noexcept override { return message_.c_str(); }

   private:
    std::string message_;
};

// Error context for better debugging
struct SerializationError {
    std::string property_path;
    std::string error_message;
    std::string type_name;

    SerializationError(const std::string &path, const std::string &message, const std::string &type = "")
        : property_path(path), error_message(message), type_name(type) {}
};

// Error collection and reporting
class SerializationErrorCollector {
   public:
    void addError(const std::string &property_path, const std::string &message, const std::string &type = "") {
        errors_.emplace_back(property_path, message, type);
    }

    bool hasErrors() const { return !errors_.empty(); }

    const std::vector<SerializationError> &getErrors() const { return errors_; }

    void clear() { errors_.clear(); }

    std::string getFormattedErrors() const {
        std::string result;
        for (const auto &error : errors_) {
            result += "Property '" + error.property_path + "': " + error.error_message;
            if (!error.type_name.empty()) {
                result += " (type: " + error.type_name + ")";
            }
            result += "\n";
        }
        return result;
    }

   private:
    std::vector<SerializationError> errors_;
};

// Enhanced serializer with error handling
class ValidatedSerializer : public TypedSerializer {
   public:
    ValidatedSerializer() : current_path_("") {}

    // Get error collector
    const SerializationErrorCollector &getErrorCollector() const { return error_collector_; }

    bool hasErrors() const { return error_collector_.hasErrors(); }

    // Throws exception if there are errors
    void validateOrThrow() const {
        if (hasErrors()) {
            throw SerializationException("Serialization failed:\n" + error_collector_.getFormattedErrors());
        }
    }

   protected:
    void pushPath(const std::string &property_name) {
        path_stack_.push_back(current_path_);
        if (current_path_.empty()) {
            current_path_ = property_name;
        } else {
            current_path_ += "." + property_name;
        }
    }

    void popPath() {
        if (!path_stack_.empty()) {
            current_path_ = path_stack_.back();
            path_stack_.pop_back();
        }
    }

    void addError(const std::string &message, const std::string &type = "") {
        error_collector_.addError(current_path_, message, type);
    }

    const std::string &getCurrentPath() const { return current_path_; }

    // Pure virtual methods from TypedSerializer - subclasses must implement
    virtual void serializeProperty(const std::string &name, int value) override = 0;
    virtual void serializeProperty(const std::string &name, long value) override = 0;
    virtual void serializeProperty(const std::string &name, long long value) override = 0;
    virtual void serializeProperty(const std::string &name, unsigned int value) override = 0;
    virtual void serializeProperty(const std::string &name, unsigned long value) override = 0;
    virtual void serializeProperty(const std::string &name, unsigned long long value) override = 0;
    virtual void serializeProperty(const std::string &name, float value) override = 0;
    virtual void serializeProperty(const std::string &name, double value) override = 0;
    virtual void serializeProperty(const std::string &name, bool value) override = 0;
    virtual void serializeProperty(const std::string &name, char value) override = 0;
    virtual void serializeProperty(const std::string &name, const std::string &value) override = 0;
    virtual void serializeProperty(const std::string &name, const char *value) override = 0;
    virtual void serializeProperty(const std::string &name, std::int8_t value) override = 0;
    virtual void serializeProperty(const std::string &name, std::int16_t value) override = 0;
    virtual void serializeProperty(const std::string &name, std::uint8_t value) override = 0;
    virtual void serializeProperty(const std::string &name, std::uint16_t value) override = 0;
    virtual void serializeInt32(const std::string &name, std::int32_t value) override = 0;
    virtual void serializeInt64(const std::string &name, std::int64_t value) override = 0;
    virtual void serializeUint32(const std::string &name, std::uint32_t value) override = 0;
    virtual void serializeUint64(const std::string &name, std::uint64_t value) override = 0;
    virtual void serializeNestedObject(const std::string &name, const Serializable &value) override = 0;
    virtual void serializeArray(const std::string &name, const std::vector<std::string> &json_elements) override = 0;
    virtual void serializeObject(const std::string &name,
                                 const std::vector<std::pair<std::string, std::string>> &json_pairs) override = 0;
    virtual void serializeUnsupportedType(const std::string &name) override = 0;

    // Helper methods that subclasses must implement
    virtual std::string escapeJsonString(const std::string &input) const override = 0;
    virtual std::string serializeNestedToString(const Serializable &obj) const override = 0;
    virtual std::string createJsonArray(const std::vector<std::string> &elements) const override = 0;
    virtual std::string createJsonObject(
        const std::vector<std::pair<std::string, std::string>> &pairs) const override = 0;

   private:
    SerializationErrorCollector error_collector_;
    std::string current_path_;
    std::vector<std::string> path_stack_;
};
}  // namespace az
