//===----------------------------------------------------------------------===//
// This file is automatically generated by scripts/generate_serialization.py
// Do not edit this file manually, your changes will be overwritten
//===----------------------------------------------------------------------===//

#include "duckdb/common/serializer/format_serializer.hpp"
#include "duckdb/common/serializer/format_deserializer.hpp"
#include "duckdb/common/extra_type_info.hpp"

namespace duckdb {

void ExtraTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	serializer.WriteProperty("type", type);
	serializer.WriteProperty("alias", alias);
}

shared_ptr<ExtraTypeInfo> ExtraTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto type = deserializer.ReadProperty<ExtraTypeInfoType>("type");
	auto alias = deserializer.ReadProperty<string>("alias");
	shared_ptr<ExtraTypeInfo> result;
	switch (type) {
	case ExtraTypeInfoType::AGGREGATE_STATE_TYPE_INFO:
		result = AggregateStateTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::DECIMAL_TYPE_INFO:
		result = DecimalTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::ENUM_TYPE_INFO:
		result = EnumTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::GENERIC_TYPE_INFO:
		result = make_shared<ExtraTypeInfo>(type);
		break;
	case ExtraTypeInfoType::INVALID_TYPE_INFO:
		return nullptr;
	case ExtraTypeInfoType::LIST_TYPE_INFO:
		result = ListTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::STRING_TYPE_INFO:
		result = StringTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::STRUCT_TYPE_INFO:
		result = StructTypeInfo::FormatDeserialize(deserializer);
		break;
	case ExtraTypeInfoType::USER_TYPE_INFO:
		result = UserTypeInfo::FormatDeserialize(deserializer);
		break;
	default:
		throw SerializationException("Unsupported type for deserialization of ExtraTypeInfo!");
	}
	result->alias = std::move(alias);
	return result;
}

void DecimalTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("width", width);
	serializer.WriteProperty("scale", scale);
}

shared_ptr<ExtraTypeInfo> DecimalTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<DecimalTypeInfo>(new DecimalTypeInfo());
	deserializer.ReadProperty("width", result->width);
	deserializer.ReadProperty("scale", result->scale);
	return std::move(result);
}

void StringTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("collation", collation);
}

shared_ptr<ExtraTypeInfo> StringTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<StringTypeInfo>(new StringTypeInfo());
	deserializer.ReadProperty("collation", result->collation);
	return std::move(result);
}

void ListTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("child_type", child_type);
}

shared_ptr<ExtraTypeInfo> ListTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<ListTypeInfo>(new ListTypeInfo());
	deserializer.ReadProperty("child_type", result->child_type);
	return std::move(result);
}

void StructTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("child_types", child_types);
}

shared_ptr<ExtraTypeInfo> StructTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<StructTypeInfo>(new StructTypeInfo());
	deserializer.ReadProperty("child_types", result->child_types);
	return std::move(result);
}

void AggregateStateTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("function_name", state_type.function_name);
	serializer.WriteProperty("return_type", state_type.return_type);
	serializer.WriteProperty("bound_argument_types", state_type.bound_argument_types);
}

shared_ptr<ExtraTypeInfo> AggregateStateTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<AggregateStateTypeInfo>(new AggregateStateTypeInfo());
	deserializer.ReadProperty("function_name", result->state_type.function_name);
	deserializer.ReadProperty("return_type", result->state_type.return_type);
	deserializer.ReadProperty("bound_argument_types", result->state_type.bound_argument_types);
	return std::move(result);
}

void UserTypeInfo::FormatSerialize(FormatSerializer &serializer) const {
	ExtraTypeInfo::FormatSerialize(serializer);
	serializer.WriteProperty("user_type_name", user_type_name);
}

shared_ptr<ExtraTypeInfo> UserTypeInfo::FormatDeserialize(FormatDeserializer &deserializer) {
	auto result = duckdb::shared_ptr<UserTypeInfo>(new UserTypeInfo());
	deserializer.ReadProperty("user_type_name", result->user_type_name);
	return std::move(result);
}

} // namespace duckdb