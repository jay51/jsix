#pragma once

#include <type_traits>

template<typename E>
struct is_enum_bitfield { static constexpr bool value = false; };

#define IS_BITFIELD(name) \
	template<> struct ::is_enum_bitfield<name> {static constexpr bool value=true;}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type
operator & (E lhs, F rhs)
{
	return static_cast<E> (
		static_cast<typename std::underlying_type<E>::type>(lhs) &
		static_cast<typename std::underlying_type<E>::type>(rhs));
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type
operator | (E lhs, F rhs)
{
	return static_cast<E> (
		static_cast<typename std::underlying_type<E>::type>(lhs) |
		static_cast<typename std::underlying_type<E>::type>(rhs));
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type
operator ^ (E lhs, F rhs)
{
	return static_cast<E> (
		static_cast<typename std::underlying_type<E>::type>(lhs) ^
		static_cast<typename std::underlying_type<E>::type>(rhs));
}

template <typename E>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type
operator ~ (E rhs)
{
	return static_cast<E>(~static_cast<typename std::underlying_type<E>::type>(rhs));
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type&
operator |= (E &lhs, F rhs)
{
	lhs = static_cast<E>(
		static_cast<typename std::underlying_type<E>::type>(lhs) |
		static_cast<typename std::underlying_type<E>::type>(rhs));

	return lhs;
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type&
operator &= (E &lhs, F rhs)
{
	lhs = static_cast<E>(
		static_cast<typename std::underlying_type<E>::type>(lhs) &
		static_cast<typename std::underlying_type<E>::type>(rhs));

	return lhs;
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type&
operator ^= (E &lhs, F rhs)
{
	lhs = static_cast<E>(
		static_cast<typename std::underlying_type<E>::type>(lhs) ^
		static_cast<typename std::underlying_type<E>::type>(rhs));

	return lhs;
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type&
operator -= (E &lhs, F rhs)
{
	lhs = static_cast<E>(
		static_cast<typename std::underlying_type<E>::type>(lhs) &
		~static_cast<typename std::underlying_type<E>::type>(rhs));

	return lhs;
}

template <typename E, typename F>
typename std::enable_if<is_enum_bitfield<E>::value,E>::type&
operator += (E &lhs, F rhs)
{
	lhs = static_cast<E>(
		static_cast<typename std::underlying_type<E>::type>(lhs) |
		static_cast<typename std::underlying_type<E>::type>(rhs));

	return lhs;
}

template <typename E>
typename std::enable_if<is_enum_bitfield<E>::value,bool>::type
operator ! (E rhs)
{
	return static_cast<typename std::underlying_type<E>::type>(rhs) == 0;
}

template <typename E>
typename std::enable_if<is_enum_bitfield<E>::value,bool>::type
bitfield_has(E set, E flag)
{
	return (set & flag) == flag;
}

// Overload the logical-and operator to be 'bitwise-and, bool-cast'
template <typename E>
typename std::enable_if<is_enum_bitfield<E>::value,bool>::type
operator && (E set, E flag)
{
	return (set & flag) == flag;
}
