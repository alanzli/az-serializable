// Validator.h
#pragma once

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace az {

// Exception thrown when validation fails
class ValidationException : public std::runtime_error {
   public:
    ValidationException(const std::string& message) : std::runtime_error(message) {}
};

// Validation result structure
struct ValidationResult {
    bool is_valid;
    std::string error_message;

    ValidationResult(bool valid = true, const std::string& message = "") : is_valid(valid), error_message(message) {}

    operator bool() const { return is_valid; }
};

// Base validation rule interface
class ValidationRule {
   public:
    virtual ~ValidationRule() = default;
    virtual ValidationResult validate(const std::string& property_name, const std::any& value,
                                      const std::string& serialized_value) const = 0;
    virtual std::string getDescription() const = 0;
};

// Template-based validation rule for type safety
template <typename T>
class TypedValidationRule : public ValidationRule {
   public:
    using ValidatorFunction = std::function<ValidationResult(const std::string&, const T&, const std::string&)>;

    TypedValidationRule(ValidatorFunction validator, const std::string& description)
        : validator_(validator), description_(description) {}

    ValidationResult validate(const std::string& property_name, const std::any& value,
                              const std::string& serialized_value) const override {
        try {
            const T& typed_value = std::any_cast<const T&>(value);
            return validator_(property_name, typed_value, serialized_value);
        } catch (const std::bad_any_cast&) {
            return ValidationResult(false, "Type mismatch in validation rule for property: " + property_name);
        }
    }

    std::string getDescription() const override { return description_; }

   private:
    ValidatorFunction validator_;
    std::string description_;
};

// Property-specific validation rule
class PropertyValidationRule : public ValidationRule {
   public:
    using ValidatorFunction = std::function<ValidationResult(const std::string&, const std::string&)>;

    PropertyValidationRule(const std::string& property_name, ValidatorFunction validator,
                           const std::string& description)
        : property_name_(property_name), validator_(validator), description_(description) {}

    ValidationResult validate(const std::string& property_name, const std::any& /*value*/,
                              const std::string& serialized_value) const override {
        if (property_name != property_name_) {
            return ValidationResult(true);  // Skip validation for other properties
        }
        return validator_(property_name, serialized_value);
    }

    std::string getDescription() const override { return "Property '" + property_name_ + "': " + description_; }

   private:
    std::string property_name_;
    ValidatorFunction validator_;
    std::string description_;
};

// Validator class that manages validation rules
class Validator {
   public:
    Validator() = default;

    // Add a type-specific validation rule
    template <typename T>
    void addRule(std::function<ValidationResult(const std::string&, const T&, const std::string&)> validator,
                 const std::string& description = "") {
        auto rule = std::unique_ptr<TypedValidationRule<T>>(new TypedValidationRule<T>(validator, description));
        type_rules_[std::type_index(typeid(T))].push_back(std::move(rule));
    }

    // Add a property-specific validation rule
    void addPropertyRule(const std::string& property_name,
                         std::function<ValidationResult(const std::string&, const std::string&)> validator,
                         const std::string& description = "") {
        auto rule =
            std::unique_ptr<PropertyValidationRule>(new PropertyValidationRule(property_name, validator, description));
        property_rules_.push_back(std::move(rule));
    }

    // Add a general validation rule that applies to all serialized values
    void addGeneralRule(std::function<ValidationResult(const std::string&, const std::string&)> validator,
                        const std::string& description = "") {
        auto rule = std::unique_ptr<PropertyValidationRule>(new PropertyValidationRule(
            "*",
            [validator](const std::string& prop, const std::string& serialized) { return validator(prop, serialized); },
            description));
        general_rules_.push_back(std::move(rule));
    }

    // Validate a property with its value and serialized representation
    template <typename T>
    ValidationResult validate(const std::string& property_name, const T& value,
                              const std::string& serialized_value) const {
        std::any any_value = value;

        // Run type-specific rules
        auto type_it = type_rules_.find(std::type_index(typeid(T)));
        if (type_it != type_rules_.end()) {
            for (const auto& rule : type_it->second) {
                auto result = rule->validate(property_name, any_value, serialized_value);
                if (!result.is_valid) {
                    return result;
                }
            }
        }

        // Run property-specific rules
        for (const auto& rule : property_rules_) {
            auto result = rule->validate(property_name, any_value, serialized_value);
            if (!result.is_valid) {
                return result;
            }
        }

        // Run general rules
        for (const auto& rule : general_rules_) {
            auto result = rule->validate(property_name, any_value, serialized_value);
            if (!result.is_valid) {
                return result;
            }
        }

        return ValidationResult(true);
    }

    // Validate just the serialized value (when original value is not available)
    ValidationResult validateSerialized(const std::string& property_name, const std::string& serialized_value) const {
        // Run property-specific rules
        for (const auto& rule : property_rules_) {
            std::any dummy_value;
            auto result = rule->validate(property_name, dummy_value, serialized_value);
            if (!result.is_valid) {
                return result;
            }
        }

        // Run general rules
        for (const auto& rule : general_rules_) {
            std::any dummy_value;
            auto result = rule->validate(property_name, dummy_value, serialized_value);
            if (!result.is_valid) {
                return result;
            }
        }

        return ValidationResult(true);
    }

    // Clear all validation rules
    void clear() {
        type_rules_.clear();
        property_rules_.clear();
        general_rules_.clear();
    }

    // Get summary of all rules
    std::vector<std::string> getRuleDescriptions() const {
        std::vector<std::string> descriptions;

        for (const auto& [type, rules] : type_rules_) {
            for (const auto& rule : rules) {
                descriptions.push_back("Type rule: " + rule->getDescription());
            }
        }

        for (const auto& rule : property_rules_) {
            descriptions.push_back(rule->getDescription());
        }

        for (const auto& rule : general_rules_) {
            descriptions.push_back("General rule: " + rule->getDescription());
        }

        return descriptions;
    }

   private:
    // Type-specific rules indexed by type
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<ValidationRule>>> type_rules_;

    // Property-specific rules
    std::vector<std::unique_ptr<ValidationRule>> property_rules_;

    // General rules that apply to all properties
    std::vector<std::unique_ptr<ValidationRule>> general_rules_;
};

}  // namespace az
