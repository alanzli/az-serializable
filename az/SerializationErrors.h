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

   private:
    SerializationErrorCollector error_collector_;
    std::string current_path_;
    std::vector<std::string> path_stack_;
};
}  // namespace az
