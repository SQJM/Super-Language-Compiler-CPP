module;
#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <variant>
export module Super.SuperValueType;

export namespace Super::SVT
{
	typedef void SVoid;
	typedef bool SBool;
	typedef char SChar;
	typedef char8_t SChar8;
	typedef char16_t SChar16;
	typedef char32_t SChar32;
	typedef std::int8_t SInt8;
	typedef std::uint8_t SUInt8;
	typedef std::int16_t SInt16;
	typedef std::uint16_t SUInt16;
	typedef std::int32_t SInt32;
	typedef std::uint32_t SUInt32;
	typedef std::int64_t SInt64;
	typedef std::uint64_t SUInt64;
	typedef boost::multiprecision::int128_t SInt128;
	typedef boost::multiprecision::uint128_t SUInt128;
	typedef float SFloat;
	typedef double SDouble;
	typedef std::string SString;

	constexpr auto SNull = 0;
	constexpr auto SNullptr = nullptr;

	typedef std::variant <
		SBool,
		SChar,
		SChar8,
		SChar16,
		SChar32,
		SInt8,
		SUInt8,
		SInt16,
		SUInt16,
		SInt32,
		SUInt32,
		SInt64,
		SUInt64,
		SInt128,
		SUInt128,
		SFloat,
		SDouble,
		SString > RValue;
}  // namespace Super::SVT