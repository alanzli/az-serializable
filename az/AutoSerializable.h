// AutoSerializable.h
#pragma once
#include <string>
#include <tuple>

#include "az/Serializable.h"

namespace az {

namespace detail {
// Helper to store member pointer and name - simplified approach
template <typename ClassType, typename MemberType>
struct member_info {
    MemberType ClassType::* ptr;
    const char* name;

    // Constructor
    constexpr member_info(MemberType ClassType::* p, const char* n) : ptr(p), name(n) {}
};

// Helper function to create member_info with type deduction
template <typename ClassType, typename MemberType>
constexpr auto make_member_info(MemberType ClassType::* ptr, const char* name) {
    return member_info<ClassType, MemberType>{ptr, name};
}

// Helper to serialize a tuple of member_info structs
template <typename T, typename Tuple, std::size_t... I>
void serialize_members_impl(const T& obj, az::TypedSerializer& serializer, const Tuple& members,
                            std::index_sequence<I...>) {
    // Fold expression to call serializeProperty for each member
    ((serializer.serializeProperty(std::get<I>(members).name, obj.*(std::get<I>(members).ptr))), ...);
}

template <typename T, typename Tuple>
void serialize_members(const T& obj, az::TypedSerializer& serializer, const Tuple& members) {
    serialize_members_impl(obj, serializer, members, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}
}  // namespace detail

// Base class for automatic serialization using CRTP (Curiously Recurring Template Pattern)
template <typename Derived>
class AutoSerializable : public az::Serializable {
   protected:
    void visitProperties(az::TypedSerializer& serializer) const override {
        // Call the derived class's serialize_auto method
        static_cast<const Derived*>(this)->serialize_auto(serializer);
    }
};

}  // namespace az

// Macro to declare serializable members - generates the serialize_auto method
#define AZ_SERIALIZABLE_MEMBERS(...)                               \
    void serialize_auto(az::TypedSerializer& serializer) const {   \
        constexpr auto members = std::make_tuple(__VA_ARGS__);     \
        az::detail::serialize_members(*this, serializer, members); \
    }

// Helper macro to create member_info with proper type deduction
#define AZ_MEMBER(member_name) az::detail::make_member_info(&std::decay_t<decltype(*this)>::member_name, #member_name)

// Alternative macro for adding auto-serialization to existing classes
#define AZ_ENABLE_AUTO_SERIALIZATION(...) AZ_SERIALIZABLE_MEMBERS(__VA_ARGS__)
