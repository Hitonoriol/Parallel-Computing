#ifndef META_HPP_
#define META_HPP_

#include <string_view>

template<typename ...Ts>
constexpr auto type_name()
{
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [Ts = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with Ts = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

template<class T>struct tag_t{};
template<class T>constexpr tag_t<T> tag{};

template<class T, class ...Ts>
constexpr bool is_char_t(tag_t<T>, Ts&&...) {return false;}
constexpr bool is_char_t(tag_t<char>) {return true;}
constexpr bool is_char_t(tag_t<wchar_t>) {return true;}

template<class T>
constexpr bool is_char_type = is_char_t(tag<T>);

#endif /* META_HPP_ */
