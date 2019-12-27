// C++11 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
// C++14 https://github.com/cplusplus/draft/raw/master/papers/n4140.pdf
// C++17 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf
// C++20 https://github.com/cplusplus/draft/raw/master/papers/n4842.pdf
// https://en.cppreference.com/w/cpp
// https://en.cppreference.com/w/cpp/header
// https://en.cppreference.com/w/cpp/links

alignas alignof asm
break case catch class const const_cast constexpr constinit continue
decltype default delete do dynamic_cast
else enum explicit export extern
false for friend goto
if inline
mutable
namespace new noexcept nullptr
operator
private protected public
register reinterpret_cast return
sizeof static static_assert static_cast struct switch
template this thread_local throw true try typedef typeid typename
union using
volatile
while

override final
auto bool char char16_t char32_t double float int long short signed unsigned wchar_t void
and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq

// Conditional inclusion
__has_include(header)
__has_cpp_attribute(pp-tokens)
// Argument substitution
__VA_OPT__
// Predefined macro names
__cplusplus
__STDCPP_DEFAULT_NEW_ALIGNMENT__
// Feature-test macros
__STDCPP_STRICT_POINTER_SAFETY__
__STDCPP_THREADS__

#include <cstddef>
namespace std { // Common definitions
	nullptr_t = decltype(nullptr);
	enum class byte : unsigned char {};
	template <class IntType>
	constexpr IntType to_integer(byte b) noexcept;
}

// Implementation properties
#include <version> // C++20

#include <limits>
namespace std { // Implementation properties
	enum float_round_style {
		round_indeterminate,
		round_toward_zero,
		round_to_nearest,
		round_toward_infinity,
		round_toward_neg_infinity,
	};
	enum float_denorm_style {
		denorm_indeterminate,
		denorm_absent,
		denorm_present,
	};

	template <class T>
	class numeric_limits {
		bool is_specialized;
		T min();
		T max();
		T lowest();

		int digits;
		int digits10;
		int max_digits10;
		bool is_signed;
		bool is_integer;
		bool is_exact;
		int radix;
		T epsilon();
		T round_error();

		int min_exponent;
		int min_exponent10;
		int max_exponent;
		int max_exponent10;

		bool has_infinity;
		bool has_quiet_NaN;
		bool has_signaling_NaN;
		float_denorm_style has_denorm;
		bool has_denorm_loss;
		T infinity();
		T quiet_NaN();
		T signaling_NaN();
		T denorm_min();

		bool is_iec559;
		bool is_bounded;
		bool is_modulo;

		bool traps;
		bool tinyness_before;
		float_round_style round_style;
	};
}

#include <new>
namespace std { // Dynamic memory management
	class bad_alloc : public exception {};
	class bad_array_new_length : public bad_alloc {};
	struct destroying_delete_t { // C++20
		explicit destroying_delete_t() = default;
	};
	inline constexpr destroying_delete_t destroying_delete {}; //C++20
	enum class align_val_t : size_t {};
	struct nothrow_t {};
	extern const nothrow_t nothrow;
	using new_handler = void (*)();
	new_handler get_new_handler() noexcept;
	new_handler set_new_handler(new_handler new_p) noexcept;
	template <class T> [[nodiscard]] constexpr T* launder(T* p) noexcept;
	size_t hardware_destructive_interference_size;
	size_t hardware_constructive_interference_size;

	[[nodiscard]] void* operator new(size_t size);
	void operator delete(void* ptr) noexcept;
	[[nodiscard]] void* operator new[](std::size_t size);
	void operator delete[](void* ptr) noexcept;
}

#include <typeinfo>
namespace std { // Type identification
	class type_info {
		bool before(const type_info& rhs) const noexcept;
		size_t hash_code() const noexcept;
		const char* name() const noexcept;
	};
	class bad_cast : public exception {};
	class bad_typeid : public exception {};
}

#include <source_location> // C++20
namespace std {
	struct source_location {
		static consteval source_location current() noexcept;
		constexpr uint_least32_t line() const noexcept;
		constexpr uint_least32_t column() const noexcept;
		constexpr const char* file_name() const noexcept;
		constexpr const char* function_name() const noexcept;
	};
}

#include <contract> // C++2x
namespace std { // Contract violation handling
	class contract_violation {
		uint_least32_t line_number() const noexcept;
		string_view file_name() const noexcept;
		string_view function_name() const noexcept;
		string_view comment() const noexcept;
		string_view assertion_level() const noexcept;
	};
}

#include <compare> // C++20
namespace std { // Comparisons
	class weak_equality {
		static const weak_equality equivalent;
		static const weak_equality nonequivalent;
	};
	class strong_equality {
		static const strong_equality equal;
		static const strong_equality nonequal;
		static const strong_equality equivalent;
		static const strong_equality nonequivalent;
	};
	class partial_ordering {
		static const partial_ordering less;
		static const partial_ordering equivalent;
		static const partial_ordering greater;
		static const partial_ordering unordered;
	};
	class weak_ordering {
		static const weak_ordering less;
		static const weak_ordering equivalent;
		static const weak_ordering greater;
	};
	class strong_ordering {
		static const strong_ordering less;
		static const strong_ordering equal;
		static const strong_ordering equivalent;
		static const strong_ordering greater;
	};
	constexpr bool is_eq(weak_equality cmp) noexcept { return cmp == 0; }
	constexpr bool is_neq(weak_equality cmp) noexcept { return cmp != 0; }
	constexpr bool is_lt(partial_ordering cmp) noexcept { return cmp < 0; }
	constexpr bool is_lteq(partial_ordering cmp) noexcept { return cmp <= 0; }
	constexpr bool is_gt(partial_ordering cmp) noexcept { return cmp > 0; }
	constexpr bool is_gteq(partial_ordering cmp) noexcept { return cmp >= 0; }
	template <class... Ts>
	struct common_comparison_category {
		using type;
	};
	template <class... Ts>
	using common_comparison_category_t = typename common_comparison_category<Ts...>::type;

	template <class T, class Cat = partial_ordering>
	concept three_way_comparable;
	template <class T, class U, class Cat = partial_ordering>
	concept three_way_comparable_with;

	template <class T, class U = T>
	struct compare_three_way_result;
	template <class T, class U = T>
	using compare_three_way_result_t = typename compare_three_way_result<T, U>::type;
	struct compare_three_way {
		using is_transparent;
	};

	inline namespace {
		inline constexpr strong_order(E, F);
		inline constexpr weak_order(E, F);
		inline constexpr partial_order(E, F);
		inline constexpr compare_strong_order_fallback(E, F);
		inline constexpr compare_weak_order_fallback(E, F);
		inline constexpr compare_partial_order_fallback(E, F);
	}
}

#include <coroutine> // C++20
namespace std { // Coroutines
	template <class R, class... ArgTypes>
	struct coroutine_traits {
		using promise_type = typename R::promise_type;
	};
	template <>
	struct coroutine_handle<void> {
		constexpr void* address() const noexcept;
		static constexpr coroutine_handle from_address(void* addr);
		bool done() const;
		void resume() const;
		void destroy() const;
	};
	template <class Promise>
	struct coroutine_handle : coroutine_handle<> {
		using coroutine_handle<>::coroutine_handle;
		static coroutine_handle from_promise(Promise&);
		static constexpr coroutine_handle from_address(void* addr);
		Promise& promise() const;
	};
	struct noop_coroutine_promise;
	template <> struct coroutine_handle<noop_coroutine_promise> : coroutine_handle<> {
		constexpr bool done() const noexcept;
		constexpr void resume() const noexcept;
		constexpr void destroy() const noexcept;
		noop_coroutine_promise& promise() const noexcept;
		constexpr void* address() const noexcept;
	};
	using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;
	noop_coroutine_handle noop_coroutine() noexcept;
	struct suspend_never {
		constexpr bool await_ready() const noexcept { return true; }
		constexpr void await_suspend(coroutine_handle<>) const noexcept {}
		constexpr void await_resume() const noexcept {}
	};
	struct suspend_always;
}

#include <concepts> // C++20
namespace std { // Concepts
	// language-related concepts
	template <class T, class U>
	concept same_as;
	template <class Derived, class Base>
	concept derived_from ;
	template <class From, class To>
	concept convertible_to;
	template <class T, class U>
	concept common_reference_with;
	template <class T, class U>
	concept common_with;
	template <class T>
	concept integral;
	template <class T>
	concept signed_integral;
	template <class T>
	concept unsigned_integral;
	template <class T>
	concept floating_point;
	template <class LHS, class RHS>
	concept assignable_from;
	template <class T>
	concept swappable;
	template <class T, class U>
	concept swappable_with;
	template <class T>
	concept destructible;
	template <class T, class... Args>
	concept constructible_from;
	template <class T>
	concept default_constructible;
	template <class T>
	concept move_constructible;
	template <class T>
	concept copy_constructible ;
	// comparison concepts
	template <class B>
	concept boolean;
	template <class T>
	concept equality_comparable;
	template <class T, class U>
	concept equality_comparable_with;
	template <class T>
	concept totally_ordered;
	template <class T, class U>
	concept totally_ordered_with;
	// object concepts
	template <class T>
	concept movable;
	template <class T>
	concept copyable;
	template <class T>
	concept semiregular;
	template <class T>
	concept regular;
	// callable concepts
	template <class F, class... Args>
	concept invocable;
	template <class F, class... Args>
	concept regular_invocable;
	template <class F, class... Args>
	concept predicate;
	template <class R, class T, class U>
	concept relation;
	template <class R, class T, class U>
	concept strict_weak_order;
}

#include <exception>
namespace std { // Exception handling
	class exception {
		const char* what() const noexcept;
	}
	class bad_exception : public exception {};
	class nested_exception {
		[[noreturn]] void rethrow_nested() const;
		exception_ptr nested_ptr() const noexcept;
	};

	using terminate_handler = void (*)();
	terminate_handler get_terminate() noexcept;
	terminate_handler set_terminate(terminate_handler f) noexcept;
	[[noreturn]] void terminate() noexcept;
	int uncaught_exceptions() noexcept;
	using exception_ptr;
	exception_ptr current_exception() noexcept;
	[[noreturn]] void rethrow_exception(exception_ptr p);
	template <class E> exception_ptr make_exception_ptr(E e) noexcept;
	template <class T> [[noreturn]] void throw_with_nested(T&& t);
	template <class E> void rethrow_if_nested(const E& e);
}

#include <initializer_list>
namespace std { // Initializer lists
	template <class E>
	class initializer_list {
		using value_type = E;
		using reference = const E&;
		using const_reference = const E&;
		using size_type = size_t;
		using iterator = const E*;
		using const_iterator = const E*;

		constexpr size_t size() const noexcept;
		constexpr const E* begin() const noexcept;
		constexpr const E* end() const noexcept;
	};
}

#include <stdexcept>
namespace std { // Exception classes
	class logic_error : public exception {};
		class domain_error : public logic_error {};
		class invalid_argument : public logic_error {};
		class length_error : public logic_error {};
		class out_of_range : public logic_error {};
	class runtime_error : public exception {};
		class range_error : public runtime_error {};
		class overflow_error : public runtime_error {};
		class underflow_error : public runtime_error {};
}

#include <system_error>
namespace std { // System error support
	class error_category {
		const char* name() const noexcept = 0;
		error_condition default_error_condition(int ev) const noexcept;
		bool equivalent(int code, const error_condition& condition) const noexcept;
		bool equivalent(const error_code& code, int condition) const noexcept;
		string message(int ev) const = 0;
	};

	const error_category& generic_category() noexcept;
	const error_category& system_category() noexcept;

	class error_code {
		void assign(int val, const error_category& cat) noexcept;
		void clear() noexcept;
		int value() const noexcept;
		const error_category& category() const noexcept;
		error_condition default_error_condition() const noexcept;
		string message() const;
	};
	class error_condition {
		void assign(int val, const error_category& cat) noexcept;
		void clear() noexcept;
		int value() const noexcept;
		const error_category& category() const noexcept;
		string message() const;
	};
	class system_error : public runtime_error {
		const error_code& code() const noexcept;
	};

	template <class T>
	struct is_error_code_enum : public false_type {};
	template <class T>
	struct is_error_condition_enum : public false_type {};

	enum class errc {
		address_family_not_supported,	// EAFNOSUPPORT
		address_in_use,					// EADDRINUSE
		address_not_available,			// EADDRNOTAVAIL
		already_connected,				// EISCONN
		argument_list_too_long,			// E2BIG
		argument_out_of_domain,			// EDOM
		bad_address,					// EFAULT
		bad_file_descriptor,			// EBADF
		bad_message,					// EBADMSG
		broken_pipe,					// EPIPE
		connection_aborted,				// ECONNABORTED
		connection_already_in_progress,	// EALREADY
		connection_refused,				// ECONNREFUSED
		connection_reset,				// ECONNRESET
		cross_device_link,				// EXDEV
		destination_address_required,	// EDESTADDRREQ
		device_or_resource_busy,		// EBUSY
		directory_not_empty,			// ENOTEMPTY
		executable_format_error,		// ENOEXEC
		file_exists,					// EEXIST
		file_too_large,					// EFBIG
		filename_too_long,				// ENAMETOOLONG
		function_not_supported,			// ENOSYS
		host_unreachable,				// EHOSTUNREACH
		identifier_removed,				// EIDRM
		illegal_byte_sequence,			// EILSEQ
		inappropriate_io_control_operation,	// ENOTTY
		interrupted,					// EINTR
		invalid_argument,				// EINVAL
		invalid_seek,					// ESPIPE
		io_error,						// EIO
		is_a_directory,					// EISDIR
		message_size,					// EMSGSIZE
		network_down,					// ENETDOWN
		network_reset,					// ENETRESET
		network_unreachable,			// ENETUNREACH
		no_buffer_space,				// ENOBUFS
		no_child_process,				// ECHILD
		no_link,						// ENOLINK
		no_lock_available,				// ENOLCK
		no_message_available,			// ENODATA
		no_message,						// ENOMSG
		no_protocol_option,				// ENOPROTOOPT
		no_space_on_device,				// ENOSPC
		no_stream_resources,			// ENOSR
		no_such_device_or_address,		// ENXIO
		no_such_device,					// ENODEV
		no_such_file_or_directory,		// ENOENT
		no_such_process,				// ESRCH
		not_a_directory,				// ENOTDIR
		not_a_socket,					// ENOTSOCK
		not_a_stream,					// ENOSTR
		not_connected,					// ENOTCONN
		not_enough_memory,				// ENOMEM
		not_supported,					// ENOTSUP
		operation_canceled,				// ECANCELED
		operation_in_progress,			// EINPROGRESS
		operation_not_permitted,		// EPERM
		operation_not_supported,		// EOPNOTSUPP
		operation_would_block,			// EWOULDBLOCK
		owner_dead,						// EOWNERDEAD
		permission_denied,				// EACCES
		protocol_error,					// EPROTO
		protocol_not_supported,			// EPROTONOSUPPORT
		read_only_file_system,			// EROFS
		resource_deadlock_would_occur,	// EDEADLK
		resource_unavailable_try_again,	// EAGAIN
		result_out_of_range,			// ERANGE
		state_not_recoverable,			// ENOTRECOVERABLE
		stream_timeout,					// ETIME
		text_file_busy,					// ETXTBSY
		timed_out,						// ETIMEDOUT
		too_many_files_open_in_system,	// ENFILE
		too_many_files_open,			// EMFILE
		too_many_links,					// EMLINK
		too_many_symbolic_link_levels,	// ELOOP
		value_too_large,				// EOVERFLOW
		wrong_protocol_type,			// EPROTOTYPE
	};

	template <> struct is_error_condition_enum<errc> : true_type {};
	error_code make_error_code(errc e) noexcept;
	error_condition make_error_condition(errc e) noexcept;

	template <class T> struct hash;
	template <> struct hash<error_code>;
	template <> struct hash<error_condition>;
	template <class T> inline constexpr bool is_error_code_enum_v = is_error_code_enum<T>::value;
	template <class T> inline constexpr bool is_error_condition_enum_v = is_error_condition_enum<T>::value;
}

#include <utility>
namespace std { // Utility components
	namespace rel_ops {}
	template <class T> void swap(T& a, T& b) noexcept();
	template <class T, size_t N> void swap(T (&a)[N], T (&b)[N]) noexcept(is_nothrow_swappable_v<T>);
	template <class T, class U = T> T exchange(T& obj, U&& new_val);

	// Forward/move helpers
	template <class T> constexpr T&& forward(remove_reference_t<T>& t) noexcept;
	template <class T> constexpr T&& forward(remove_reference_t<T>&& t) noexcept;
	template <class T> constexpr remove_reference_t<T>&& move(T&&) noexcept;
	template <class T> constexpr conditional_t<!is_nothrow_move_constructible_v<T> && is_copy_constructible_v<T>, const T&, T&&>
	move_if_noexcept(T& x) noexcept;
	template <class T> constexpr add_const_t<T>& as_const(T& t) noexcept;
	template <class T> void as_const(const T&&) = delete;
	template <class T> add_rvalue_reference_t<T> declval() noexcept;

	// Compile-time integer sequences
	template <class T, T...>
	struct integer_sequence {
		using value_type = T;
		size_t size();
	};
	template <size_t... I> using index_sequence = integer_sequence<size_t, I...>;
	template <class T, T N> using make_integer_sequence = integer_sequence<T,>;
	template <size_t N> using make_index_sequence = make_integer_sequence<size_t, N>;
	template <class... T> using index_sequence_for = make_index_sequence<sizeof...(T)>;

	// Pairs
	template <class T1, class T2>
	struct pair {
		using first_type;
		using second_type;
		T1 first;
		T2 second;
	};
	template <class T1, class T2> constexpr make_pair(T1&&, T2&&);

	template <class T> class tuple_size;
	template <size_t I, class T> class tuple_element;
	template <size_t I, class T1, class T2>
	constexpr tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>&) noexcept;

	// Piecewise construction
	struct piecewise_construct_t {
		explicit piecewise_construct_t() = default;
	};
	inline constexpr piecewise_construct_t piecewise_construct{};

	struct in_place_t {
		explicit in_place_t() = default;
	};
	inline constexpr in_place_t in_place{};
	template <class T>
	struct in_place_type_t {
		explicit in_place_type_t() = default;
	};
	template <class T> inline constexpr in_place_type_t<T> in_place_type{};
	template <size_t I>
	struct in_place_index_t {
		explicit in_place_index_t() = default;
	};
	template <size_t I> inline constexpr in_place_index_t<I> in_place_index{};
}

#include <tuple>
namespace std { // Tuples
	template <class... Types> class tuple {};
	inline constexpr ignore;
	template <class... TTypes> constexpr tuple<VTypes...> make_tuple(TTypes&&...);
	template <class... TTypes> constexpr tuple<TTypes&&...> forward_as_tuple(TTypes&&...) noexcept;
	template <class... TTypes> constexpr tuple<TTypes&...> tie(TTypes&...) noexcept;
	template <class... Tuples> constexpr tuple<CTypes...> tuple_cat(Tuples&&...);
	template <class F, class Tuple> constexpr decltype(auto) apply(F&& f, Tuple&& t);
	template <class T, class Tuple> constexpr T make_from_tuple(Tuple&& t);

	template <class... Types, class Alloc>
	struct uses_allocator<tuple<Types...>, Alloc>;
	template <class T> inline constexpr size_t tuple_size_v = tuple_size<T>::value;
}

#include <optional>
namespace std { // Optional objects
	template <class T>
	class optional {
		using value_type = T;
		template <class... Args> T& emplace(Args&&...);
		constexpr bool has_value() const noexcept;
		constexpr const T& value() const&;
		template <class U> constexpr T value_or(U&&) const&;
		void reset() noexcept;
	};
	struct nullopt_t{};
	inline constexpr nullopt_t nullopt();
	class bad_optional_access : public exception {};
	template <class T> constexpr optional<> make_optional(T&&);
	template <class T, class... Args> constexpr optional<T> make_optional(Args&&... args);
}

#include <variant>
namespace std { // Variants
	template <class... Types>
	class variant {
		template <class T, class... Args>
		T& emplace(Args&&...);
		constexpr bool valueless_by_exception() const noexcept;
		constexpr size_t index() const noexcept;
	};
	template <class T> struct variant_size;
	template <class T>
	inline constexpr size_t variant_size_v = variant_size<T>::value;
	template <size_t I, class T> struct variant_alternative<I, const T>;
	template <size_t I, class T>
	using variant_alternative_t = typename variant_alternative<I, T>::type;
	inline constexpr size_t variant_npos = -1;
	template <class T, class... Types>
	constexpr bool holds_alternative(const variant<Types...>&) noexcept;
	template <size_t I, class... Types>
	constexpr variant_alternative_t<I, variant<Types...>>& get(variant<Types...>&);
	template <class T, class... Types>
	constexpr add_pointer_t<T> get_if(variant<Types...>*) noexcept;
	template <class Visitor, class... Variants>
	constexpr visit(Visitor&&, Variants&&...);
	struct monostate;
	class bad_variant_access : public exception {};
}

#include <any>
namespace std { // Storage for any type
	class bad_any_cast : public bad_cast {};
	class any {
		template <class T, class... Args>
		decay_t<T>& emplace(Args&& ...);
		void reset() noexcept;
		void swap(any& rhs) noexcept;
		bool has_value() const noexcept;
		const type_info& type() const noexcept;
	};
	template <class T, class... Args>
	any make_any(Args&& ...args);
	template <class T>
	T any_cast(const any& operand);
}

#include <bitset>
namespace std { // Bitsets
	template <size_t N> class bitset {
		// bit reference:
		class reference {
			reference& flip() noexcept; // for b[i].flip();
		};
		bitset<N>& set() noexcept;
		bitset<N>& set(size_t pos, bool val = true);
		bitset<N>& reset() noexcept;
		bitset<N>& reset(size_t pos);
		bitset<N>& flip() noexcept;
		bitset<N>& flip(size_t pos);
		unsigned long to_ulong() const;
		unsigned long long to_ullong() const;
		basic_string to_string() const;
		size_t count() const noexcept;
		constexpr size_t size() const noexcept;
		bool test(size_t pos) const;
		bool all() const noexcept;
		bool any() const noexcept;
		bool none() const noexcept;
	};
}

#include <memory>
namespace std { // Memory
	template <class Ptr> struct pointer_traits {
		using pointer = Ptr;
		using element_type;
		using difference_type;
		template <class U> using rebind;
		static pointer pointer_to(r);
	};
	template <class T> struct pointer_traits<T*>;
	template <class Ptr> auto to_address(const Ptr& p) noexcept; // C++20
	enum class pointer_safety { relaxed, preferred, strict };
	// Pointer safety
	void declare_reachable(void* p);
	template <class T> T* undeclare_reachable(T* p);
	void declare_no_pointers(char* p, size_t n);
	void undeclare_no_pointers(char* p, size_t n);
	pointer_safety get_pointer_safety() noexcept;
	// Align
	void* align(size_t alignment, size_t size, void*& ptr, size_t& space);
	template <size_t N, class T>
	[[nodiscard]] constexpr T* assume_aligned(T* ptr); // C++20
	// Allocator argument tag
	struct allocator_arg_t { explicit allocator_arg_t() = default; };
	inline constexpr allocator_arg_t allocator_arg{};
	// uses_allocator
	template <class T, class Alloc> struct uses_allocator;
	template <class T, class Alloc>
	inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;
	template <class T, class Alloc>
	auto uses_allocator_construction_args(const Alloc& alloc); // C++20
	template <class T, class Alloc, class... Args>
	T make_obj_using_allocator(const Alloc& alloc, Args&&... args); // C++20
	template <class T, class Alloc, class... Args>
	T* uninitialized_construct_using_allocator(T* p, const Alloc& alloc, Args&&... args); // C++20
	// Allocator traits
	template <class Alloc> struct allocator_traits {
		using allocator_type = Alloc;
		using value_type = typename Alloc::value_type;
		using pointer;
		using const_pointer;
		using void_pointer;
		using const_void_pointer;
		using propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment;
		using propagate_on_container_swap;
		using is_always_equal;

		template <class T> using rebind_alloc;
		template <class T> using rebind_traits;

		[[nodiscard]] static pointer allocate(Alloc& a, size_type n);
		[[nodiscard]] static pointer allocate(Alloc& a, size_type n, const_void_pointer hint);
		static void deallocate(Alloc& a, pointer p, size_type n);
		template <class T, class... Args>
		static void construct(Alloc& a, T* p, Args&&... args);
		template <class T>
		static void destroy(Alloc& a, T* p);
		static size_type max_size(const Alloc& a) noexcept;
		static Alloc select_on_container_copy_construction(const Alloc& rhs);
	};
	// The default allocator
	template <class T> class allocator;
	// Specialized algorithms
	template <class T> constexpr T* addressof(T& r) noexcept;
	template <class ForwardIterator>
	void uninitialized_default_construct(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator, class Size>
	ForwardIterator uninitialized_default_construct_n(ForwardIterator first, Size n)
	template <class ForwardIterator>
	void uninitialized_value_construct(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator, class Size>
	ForwardIterator uninitialized_value_construct_n(ForwardIterator first, Size n);
	template <class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);
	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result);
	template <class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result);
	template <class InputIterator, class Size, class ForwardIterator>
	pair<InputIterator, ForwardIterator> uninitialized_move_n(InputIterator first, Size n, ForwardIterator result);
	template <class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);
	template <class ForwardIterator, class Size, class T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x);
	template <class T>
	void destroy_at(T* location);
	template <class ForwardIterator>
	void destroy(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator, class Size>
	ForwardIterator destroy_n(ForwardIterator first, Size n);

	// unique_ptr
	template <class T> struct default_delete;
	template <class T> struct default_delete<T[]>;
	template <class T, class D = default_delete<T>>
	class unique_ptr {
		using pointer;
		using element_type = T;
		using deleter_type = D;

		pointer get() const noexcept;
		deleter_type& get_deleter() noexcept
		pointer release() noexcept;
		void reset(pointer p = pointer()) noexcept;
		void swap(unique_ptr& u) noexcept;
	};
	template <class T, class D> class unique_ptr<T[], D>;
	template <class T, class... Args> unique_ptr<T> make_unique(Args&&... args);
	template <class T> unique_ptr<T> make_unique(size_t n);
	template <class T> unique_ptr<T> make_unique_default_init(size_t n); // C++20
	template <class T, class D> void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept;
	// Shared-ownership pointers
	class bad_weak_ptr : public exception {};
	template <class T>
	class shared_ptr {
		using element_type = remove_extent_t<T>;
		using weak_type = weak_ptr<T>;

		void swap(shared_ptr& r) noexcept;
		void reset() noexcept;
		element_type* get() const noexcept;
		long use_count() const noexcept;
		template <class U> bool owner_before(const shared_ptr<U>& b) const noexcept;
	};
	template <class T, class... Args>
	shared_ptr<T> make_shared(Args&&... args);
	template <class T, class A, class... Args>
	shared_ptr<T> allocate_shared(const A& a, Args&&... args);
	template <class T> shared_ptr<T> make_shared_default_init(); // C++20
	template <class T, class U>
	shared_ptr<T> static_pointer_cast(const shared_ptr<U>& r) noexcept;
	template <class T, class U>
	shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& r) noexcept;
	template <class T, class U>
	shared_ptr<T> const_pointer_cast(const shared_ptr<U>& r) noexcept;
	template <class T, class U>
	shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U>& r) noexcept;
	template <class D, class T>
	D* get_deleter(const shared_ptr<T>& p) noexcept;
	template <class T>
	class weak_ptr {
		using element_type = T;
		long use_count() const noexcept;
		bool expired() const noexcept;
		shared_ptr<T> lock() const noexcept;
	};
	template <class T = void> struct owner_less;
	template <> struct owner_less<void> {
		using is_transparent;
	};
	template <class T> class enable_shared_from_this {
		shared_ptr<T> shared_from_this();
		shared_ptr<T const> shared_from_this() const;
		weak_ptr<T> weak_from_this() noexcept;
		weak_ptr<T const> weak_from_this() const noexcept;
	};
	template <class T>
	bool atomic_is_lock_free(const shared_ptr<T>* p);

	template <class Y> [[deprecated]] struct auto_ptr_ref {}; // C++03
	template <class X> [[deprecated]] class auto_ptr {}; // C++03
}

#include <memory_resource>
namespace std::pmr { // Memory resources
	class memory_resource {
		static constexpr size_t max_align = alignof(max_align_t);
		[[nodiscard]] void* allocate(size_t bytes, size_t alignment = max_align);
		void deallocate(void* p, size_t bytes, size_t alignment = max_align);
		bool is_equal(const memory_resource& other) const noexcept;
	};
	template <class Tp>
	class polymorphic_allocator {
		using value_type = Tp;
		[[nodiscard]] Tp* allocate(size_t n);
		void deallocate(Tp* p, size_t n);
		void* allocate_bytes(size_t nbytes, size_t alignment = alignof(max_align_t));
		void deallocate_bytes(void* p, size_t nbytes, size_t alignment = alignof(max_align_t));
		template <class T> T* allocate_object(size_t n = 1);
		template <class T> void deallocate_object(T* p, size_t n = 1);
		template <class T, class... CtorArgs> T* new_object(CtorArgs&&... ctor_args);
		template <class T> void delete_object(T* p);
		template <class T, class... Args>
		void construct(T* p, Args&&... args);
		template <class T>
		void destroy(T* p);
		polymorphic_allocator select_on_container_copy_construction() const;
		memory_resource* resource() const;
	};

	memory_resource* new_delete_resource() noexcept;
	memory_resource* null_memory_resource() noexcept;
	memory_resource* set_default_resource(memory_resource* r) noexcept;
	memory_resource* get_default_resource() noexcept;
	// Pool resource
	struct pool_options {
		size_t max_blocks_per_chunk = 0;
		size_t largest_required_pool_block = 0;
	};
	class synchronized_pool_resource : public memory_resource {
		void release();
		memory_resource* upstream_resource() const;
		pool_options options() const;
	};
	class unsynchronized_pool_resource : public memory_resource {};
	class monotonic_buffer_resource : public memory_resource {};
}

#include <scoped_allocator>
namespace std {
	template <class OuterAlloc, class... InnerAlloc>
	class scoped_allocator_adaptor :  public OuterAlloc {
		using outer_allocator_type = OuterAlloc;
		using inner_allocator_type;
		using propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment;
		using propagate_on_container_swap;
		using is_always_equal;

		template <class Tp>
		struct rebind {
			using other = scoped_allocator_adaptor<OuterTraits::template rebind_alloc<Tp>, InnerAllocs...>;
		};

		inner_allocator_type& inner_allocator() noexcept;
		const inner_allocator_type& inner_allocator() const noexcept;
		outer_allocator_type& outer_allocator() noexcept;
		const outer_allocator_type& outer_allocator() const noexcept;
		scoped_allocator_adaptor select_on_container_copy_construction() const;
	};
}

#include <functional>
namespace std { // Function objects
	template <class F, class... Args>
	invoke_result_t<F, Args...> invoke(F&& f, Args&&... args) noexcept(is_nothrow_invocable_v<F, Args...>);

	template <class T> class reference_wrapper {
		using type = T;
		T& get() const noexcept;
	};
	template <class T> reference_wrapper<T> ref(T&) noexcept;
	template <class T> reference_wrapper<const T> cref(const T&) noexcept;
	template <class T> void ref(const T&&) = delete;
	template <class T> void cref(const T&&) = delete;
	template <class T> struct unwrap_reference; // C++20
	template <class T> struct unwrap_ref_decay : unwrap_reference<decay_t<T>> {};
	template <class T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
	// arithmetic operations
	template <class T = void> struct plus;
	template <class T = void> struct minus;
	template <class T = void> struct multiplies;
	template <class T = void> struct divides;
	template <class T = void> struct modulus;
	template <class T = void> struct negate;
	// comparisons
	template <class T = void> struct equal_to;
	template <class T = void> struct not_equal_to;
	template <class T = void> struct greater;
	template <class T = void> struct less;
	template <class T = void> struct greater_equal;
	template <class T = void> struct less_equal;
	// logical operations
	template <class T = void> struct logical_and;
	template <class T = void> struct logical_or;
	template <class T = void> struct logical_not;
	// bitwise operations
	template <class T = void> struct bit_and;
	template <class T = void> struct bit_or;
	template <class T = void> struct bit_xor;
	template <class T = void> struct bit_not;

	struct identity { // // C++20
		using is_transparent
	};
	template <class F>
	not_fn(F&& f);
	template <class F, class... Args> bind_front(F&&, Args&&...); // C++20

	template <class T> struct is_bind_expression;
	template <class T> struct is_placeholder;
	template <class F, class... BoundArgs>
	bind(F&&, BoundArgs&&...);
	template <class R, class T>
	mem_fn(R T::*) noexcept;

	class bad_function_call : public exception {};
	template <class R, class... ArgTypes>
	class function<R(ArgTypes...)> {
		using result_type = R;
		const type_info& target_type() const noexcept;
		template <class T> T* target() noexcept;
		template <class T> const T* target() const noexcept;
	};

	template <class ForwardIterator, class BinaryPredicate = equal_to<>> class default_searcher;
	template <class RandomAccessIterator, class Hash = hash<typename iterator_traits<RandomAccessIterator>::value_type>>, class boyer_moore_searcher;
	template <class RandomAccessIterator, class Hash = hash<typename iterator_traits<RandomAccessIterator>::value_type>, class BinaryPredicate = equal_to<>>
	class boyer_moore_horspool_searcher;

	template <class T> inline constexpr bool is_bind_expression_v = is_bind_expression<T>::value;
	template <class T> inline constexpr int is_placeholder_v = is_placeholder<T>::value;

	namespace ranges { // C++20
		// concept-constrained comparisons
		struct equal_to;
		struct not_equal_to;
		struct greater;
		struct less;
		struct greater_equal;
		struct less_equal;
	}

	// deprecated C++03 function objects and binders
	template <class Arg, class Result> struct unary_function {}; // C++03
	template <class Arg1, class Arg2, class Result> struct binary_function {}; // C++03
	template <class Arg, class Result> class pointer_to_unary_function : public unary_function<Arg, Result> {}; // C++03
	template <class Arg1, class Arg2, class Result> class pointer_to_binary_function : public binary_function<Arg1,Arg2,Result> {}; // C++03
	template <class S, class T> class mem_fun_t : public unary_function<T*, S> {}; // C++03
	template <class S, class T, class A> class mem_fun1_t : public binary_function<T*, A, S> {};  // C++03
	template <class S, class T> class mem_fun_ref_t : public unary_function<T, S> {};  // C++03
	template <class S, class T, class A> class mem_fun1_ref_t : public binary_function<T, A, S> {};  // C++03
	template <class S, class T> class const_mem_fun_t : public unary_function<const T*, S> {};  // C++03
	template <class S, class T, class A> class const_mem_fun1_t : public binary_function<const T*, A, S> {};  // C++03
	template <class S, class T, class A> class const_mem_fun1_ref_t : public binary_function<T, A, S> {};  // C++03
	template <class Fn> class binder1st : public unary_function<typename Fn::second_argument_type, typename Fn::result_type> {};  // C++03
	template <class Fn> class binder2nd : public unary_function<typename Fn::first_argument_type, typename Fn::result_type> {};  // C++03
}

#include <type_traits>
namespace std { // Metaprogramming and type traits
	template <class T, T v>
	struct integral_constant {
		static constexpr T value = v;
		using value_type = T;
		using type = integral_constant<T, v>;
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; }
	};
	template <bool B>
	using bool_constant = integral_constant<bool, B>;
	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;
	// primary type categories
	template <class T> struct is_void;
	template <class T> struct is_null_pointer;
	template <class T> struct is_integral;
	template <class T> struct is_floating_point;
	template <class T> struct is_array;
	template <class T> struct is_pointer;
	template <class T> struct is_lvalue_reference;
	template <class T> struct is_rvalue_reference;
	template <class T> struct is_member_object_pointer;
	template <class T> struct is_member_function_pointer;
	template <class T> struct is_enum;
	template <class T> struct is_union;
	template <class T> struct is_class;
	template <class T> struct is_function;
	// composite type categories
	template <class T> struct is_reference;
	template <class T> struct is_arithmetic;
	template <class T> struct is_fundamental;
	template <class T> struct is_object;
	template <class T> struct is_scalar;
	template <class T> struct is_compound;
	template <class T> struct is_member_pointer;
	// type properties
	template <class T> struct is_const;
	template <class T> struct is_volatile;
	template <class T> struct is_trivial;
	template <class T> struct is_trivially_copyable;
	template <class T> struct is_standard_layout;
	template <class T> [[deprecated]] struct is_pod; // C++17
	template <class T> struct is_empty;
	template <class T> struct is_polymorphic;
	template <class T> struct is_abstract;
	template <class T> struct is_final;
	template <class T> struct is_aggregate;

	template <class T> struct is_signed;
	template <class T> struct is_unsigned;
	template <class T> struct is_bounded_array; // C++20
	template <class T> struct is_unbounded_array; // C++20

	template <class T, class... Args> struct is_constructible;
	template <class T> struct is_default_constructible;
	template <class T> struct is_copy_constructible;
	template <class T> struct is_move_constructible;

	template <class T, class U> struct is_assignable;
	template <class T> struct is_copy_assignable;
	template <class T> struct is_move_assignable;

	template <class T, class U> struct is_swappable_with;
	template <class T> struct is_swappable;
	template <class T> struct is_destructible;

	template <class T, class... Args> struct is_trivially_constructible;
	template <class T> struct is_trivially_default_constructible;
	template <class T> struct is_trivially_copy_constructible;
	template <class T> struct is_trivially_move_constructible;

	template <class T, class U> struct is_trivially_assignable;
	template <class T> struct is_trivially_copy_assignable;
	template <class T> struct is_trivially_move_assignable;
	template <class T> struct is_trivially_destructible;

	template <class T, class... Args> struct is_nothrow_constructible;
	template <class T> struct is_nothrow_default_constructible;
	template <class T> struct is_nothrow_copy_constructible;
	template <class T> struct is_nothrow_move_constructible;

	template <class T, class U> struct is_nothrow_assignable;
	template <class T> struct is_nothrow_copy_assignable;
	template <class T> struct is_nothrow_move_assignable;

	template <class T, class U> struct is_nothrow_swappable_with;
	template <class T> struct is_nothrow_swappable;

	template <class T> struct is_nothrow_destructible;
	template <class T> struct has_virtual_destructor;
	template <class T> struct has_unique_object_representations;
	// type property queries
	template <class T> struct alignment_of;
	template <class T> struct rank;
	template <class T, unsigned I = 0> struct extent;
	// type relations
	template <class T, class U> struct is_same;
	template <class Base, class Derived> struct is_base_of;
	template <class From, class To> struct is_convertible;
	template <class From, class To> struct is_nothrow_convertible; // C++20
	template <class T, class U> struct is_layout_compatible; // C++20
	template <class Base, class Derived> struct is_pointer_interconvertible_base_of; // C++20

	template <class Fn, class... ArgTypes> struct is_invocable;
	template <class R, class Fn, class... ArgTypes> struct is_invocable_r;
	template <class Fn, class... ArgTypes> struct is_nothrow_invocable;
	template <class R, class Fn, class... ArgTypes> struct is_nothrow_invocable_r;
	// const-volatile modifications
	template <class T> struct remove_const;
	template <class T> struct remove_volatile;
	template <class T> struct remove_cv;
	template <class T> struct add_const;
	template <class T> struct add_volatile;
	template <class T> struct add_cv;
	template <class T> using remove_const_t = typename remove_const<T>::type;
	template <class T> using remove_volatile_t = typename remove_volatile<T>::type;
	template <class T> using remove_cv_t = typename remove_cv<T>::type;
	template <class T> using add_const_t = typename add_const<T>::type;
	template <class T> using add_volatile_t = typename add_volatile<T>::type;
	template <class T> using add_cv_t = typename add_cv<T>::type;
	// reference modifications
	template <class T> struct remove_reference;
	template <class T> struct add_lvalue_reference;
	template <class T> struct add_rvalue_reference;
	template <class T> using remove_reference_t = typename remove_reference<T>::type;
	template <class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
	template <class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
	// sign modifications
	template <class T> struct make_signed;
	template <class T> struct make_unsigned;
	template <class T> using make_signed_t = typename make_signed<T>::type;
	template <class T> using make_unsigned_t = typename make_unsigned<T>::type;
	// array modifications
	template <class T> struct remove_extent;
	template <class T> struct remove_all_extents;
	template <class T> using remove_extent_t = typename remove_extent<T>::type;
	template <class T> using remove_all_extents_t = typename remove_all_extents<T>::type;
	// pointer modifications
	template <class T> struct remove_pointer;
	template <class T> struct add_pointer;
	template <class T> using remove_pointer_t = typename remove_pointer<T>::type;
	template <class T> using add_pointer_t = typename add_pointer<T>::type;
	// other transformations
	template <class T> struct type_identity; // C++20
	template <size_t Len, size_t Align = default-alignment > struct aligned_storage;
	template <size_t Len, class... Types> struct aligned_union;
	template <class T> struct remove_cvref; // C++20
	template <class T> struct decay;
	template <bool, class T = void> struct enable_if;
	template <bool, class T, class F> struct conditional;
	template <class... T> struct common_type;
	template <class T, class U, template <class> class TQual, template <class> class UQual>
	struct basic_common_reference {}; // C++20
	template <class... T> struct common_reference; // C++20
	template <class T> struct underlying_type;
	template <class Fn, class... ArgTypes> struct invoke_result;

	template <class T> using type_identity_t = typename type_identity<T>::type;
	template <size_t Len, size_t Align = default-alignment> using aligned_storage_t = typename aligned_storage<Len, Align>::type;
	template <size_t Len, class... Types> using aligned_union_t = typename aligned_union<Len, Types...>::type;
	template <class T> using remove_cvref_t = typename remove_cvref<T>::type; // C++20
	template <class T> using decay_t = typename decay<T>::type;
	template <bool b, class T = void> using enable_if_t = typename enable_if<b, T>::type;
	template <bool b, class T, class F> using conditional_t = typename conditional<b, T, F>::type;
	template <class... T> using common_type_t = typename common_type<T...>::type;
	template <class... T> using common_reference_t = typename common_reference<T...>::type; // C++20
	template <class T> using underlying_type_t = typename underlying_type<T>::type;
	template <class Fn, class... ArgTypes> using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
	template <class...> using void_t = void;
	// logical operator traits
	template <class... B> struct conjunction;
	template <class... B> struct disjunction;
	template <class B> struct negation;
	enum class endian { // C++20
		little,
		big,
		native
	};
	// primary type categories
	template <class T> inline constexpr bool is_void_v = is_void<T>::value;
	template <class T> inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
	template <class T> inline constexpr bool is_integral_v = is_integral<T>::value;
	template <class T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
	template <class T> inline constexpr bool is_array_v = is_array<T>::value;
	template <class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;
	template <class T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
	template <class T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
	template <class T> inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
	template <class T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
	template <class T> inline constexpr bool is_enum_v = is_enum<T>::value;
	template <class T> inline constexpr bool is_union_v = is_union<T>::value;
	template <class T> inline constexpr bool is_class_v = is_class<T>::value;
	template <class T> inline constexpr bool is_function_v = is_function<T>::value;
	// composite type categories
	template <class T> inline constexpr bool is_reference_v = is_reference<T>::value;
	template <class T> inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
	template <class T> inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
	template <class T> inline constexpr bool is_object_v = is_object<T>::value;
	template <class T> inline constexpr bool is_scalar_v = is_scalar<T>::value;
	template <class T> inline constexpr bool is_compound_v = is_compound<T>::value;
	template <class T> inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
	// type properties
	template <class T> inline constexpr bool is_const_v = is_const<T>::value;
	template <class T> inline constexpr bool is_volatile_v = is_volatile<T>::value;
	template <class T> inline constexpr bool is_trivial_v = is_trivial<T>::value;
	template <class T> inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
	template <class T> inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
	template <class T> [[deprecated]] inline constexpr bool is_pod_v = is_pod<T>::value; // C++17
	template <class T> inline constexpr bool is_empty_v = is_empty<T>::value;
	template <class T> inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
	template <class T> inline constexpr bool is_abstract_v = is_abstract<T>::value;
	template <class T> inline constexpr bool is_final_v = is_final<T>::value;
	template <class T> inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
	template <class T> inline constexpr bool is_signed_v = is_signed<T>::value;
	template <class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
	template <class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value; // C++20
	template <class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value; // C++20
	template <class T, class... Args> inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
	template <class T> inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
	template <class T> inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
	template <class T> inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;
	template <class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
	template <class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
	template <class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;
	template <class T, class U> inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
	template <class T> inline constexpr bool is_swappable_v = is_swappable<T>::value;
	template <class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;
	template <class T, class... Args> inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;
	template <class T> inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
	template <class T> inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
	template <class T> inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
	template <class T, class U> inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
	template <class T> inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
	template <class T> inline constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;
	template <class T> inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
	template <class T, class... Args> inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
	template <class T> inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;
	template <class T> inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;
	template <class T> inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;
	template <class T, class U> inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
	template <class T> inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
	template <class T> inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;
	template <class T, class U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
	template <class T> inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
	template <class T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;
	template <class T> inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
	template <class T> inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;
	template <class T> inline constexpr bool has_strong_structural_equality_v = has_strong_structural_equality<T>::value; // C++20
	// type property queries
	template <class T> inline constexpr size_t alignment_of_v = alignment_of<T>::value;
	template <class T> inline constexpr size_t rank_v = rank<T>::value;
	template <class T, unsigned I = 0> inline constexpr size_t extent_v = extent<T, I>::value;
	// type relations
	template <class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;
	template <class Base, class Derived> inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
	template <class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
	template <class From, class To> inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value; // C++20
	template <class T, class U> inline constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value; // C++20
	template <class Base, class Derived> inline constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<Base, Derived>::value; // C++20
	template <class Fn, class... ArgTypes> inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;
	template <class R, class Fn, class... ArgTypes> inline constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, ArgTypes...>::value;
	template <class Fn, class... ArgTypes> inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, ArgTypes...>::value;
	template <class R, class Fn, class... ArgTypes> inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;
	// logical operator traits
	template <class... B> inline constexpr bool conjunction_v = conjunction<B...>::value;
	template <class... B> inline constexpr bool disjunction_v = disjunction<B...>::value;
	template <class B> inline constexpr bool negation_v = negation<B>::value;
	// member relationships
	template <class S, class M>
	constexpr bool is_pointer_interconvertible_with_class(M S::*m) noexcept;
	template <class S1, class S2, class M1, class M2>
	constexpr bool is_corresponding_member(M1 S1::*m1, M2 S2::*m2) noexcept;
	// constant evaluation context
	constexpr bool is_constant_evaluated() noexcept;
}

#include <ratio>
namespace std { // Compile-time rational arithmetic
	template <intmax_t N, intmax_t D = 1>
	class ratio {
		static constexpr intmax_t num;
		static constexpr intmax_t den;
		using type = ratio<num, den>;
	};
	// ratio arithmetic
	template <class R1, class R2> using ratio_add;
	template <class R1, class R2> using ratio_subtract;
	template <class R1, class R2> using ratio_multiply;
	template <class R1, class R2> using ratio_divide;
	// ratio comparison
	template <class R1, class R2> struct ratio_equal;
	template <class R1, class R2> struct ratio_not_equal;
	template <class R1, class R2> struct ratio_less;
	template <class R1, class R2> struct ratio_less_equal;
	template <class R1, class R2> struct ratio_greater;
	template <class R1, class R2> struct ratio_greater_equal;
	template <class R1, class R2> inline constexpr bool ratio_equal_v = ratio_equal<R1, R2>::value;
	template <class R1, class R2> inline constexpr bool ratio_not_equal_v = ratio_not_equal<R1, R2>::value;
	template <class R1, class R2> inline constexpr bool ratio_less_v = ratio_less<R1, R2>::value;
	template <class R1, class R2> inline constexpr bool ratio_less_equal_v = ratio_less_equal<R1, R2>::value;
	template <class R1, class R2> inline constexpr bool ratio_greater_v = ratio_greater<R1, R2>::value;
	template <class R1, class R2> inline constexpr bool ratio_greater_equal_v = ratio_greater_equal<R1, R2>::value;

	// convenience SI typedefs
	using yocto;
	using zepto;
	using atto;
	using femto;
	using pico;
	using nano;
	using micro;
	using milli;
	using centi;
	using deci;
	using deca;
	using hecto;
	using kilo;
	using mega;
	using giga;
	using tera;
	using peta;
	using exa;
	using zetta;
	using yotta;
}

#include <utility> // C++17
#include <charconv> // C++20
namespace std { // Primitive numeric conversions
	enum class chars_format {
		scientific ,
		fixed,
		hex,
		general = fixed | scientific
	};
	struct to_chars_result {
		char* ptr;
		error_code ec;
	};
	to_chars_result to_chars(char* first, char* last, float value);
	struct from_chars_result {
		const char* ptr;
		error_code ec;
	};
	from_chars_result from_chars(const char* first, const char* last, float& value,
	chars_format fmt = chars_format::general);
}

#include <chrono>
namespace std::chrono { // Time
	// class template duration
	template <class Rep, class Period = ratio<1>>
	class duration {
		using rep = Rep;
		using period = typename Period::type;
		constexpr rep count() const;
		static constexpr duration zero();
		static constexpr duration min();
		static constexpr duration max();
	};
	// class template time_point
	template <class Clock, class Duration = typename Clock::duration>
	class time_point {
		using clock = Clock;
		using duration = Duration;
		using rep = typename duration::rep;
		using period = typename duration::period;
		constexpr duration time_since_epoch() const;
		static constexpr time_point min();
		static constexpr time_point max();
	};

	// customization traits
	template <class Rep> struct treat_as_floating_point;
	template <class Rep> struct duration_values {
		static constexpr Rep zero();
		static constexpr Rep min();
		static constexpr Rep max();
	};
	template <class Rep> inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<Rep>::value;
	template <class T> struct is_clock; // C++20
	template <class T> inline constexpr bool is_clock_v = is_clock<T>::value;		// duration_cast
	template <class ToDuration, class Rep, class Period> constexpr ToDuration duration_cast(const duration<Rep, Period>& d);
	template <class ToDuration, class Rep, class Period> constexpr ToDuration floor(const duration<Rep, Period>& d);
	template <class ToDuration, class Rep, class Period> constexpr ToDuration ceil(const duration<Rep, Period>& d);
	template <class ToDuration, class Rep, class Period> constexpr ToDuration round(const duration<Rep, Period>& d);
	template <class charT, class traits, class Rep, class Period>
	basic_ostream<charT, traits>& to_stream(basic_ostream<charT, traits>& os, const charT* fmt, const duration<Rep, Period>& d); // C++20
	template <class charT, class traits, class Rep, class Period, class Alloc = allocator<charT>>
	basic_istream<charT, traits>& from_stream(basic_istream<charT, traits>& is, const charT* fmt,duration<Rep, Period>& d, basic_string<charT, traits, Alloc>* abbrev = nullptr, minutes* offset = nullptr); // C++20
	// convenience typedefs
	using nanoseconds;
	using microseconds;
	using milliseconds;
	using seconds;
	using minutes;
	using hours;
	using days; // C++20
	using weeks; // C++20
	using years; // C++20
	using months; // C++20
	// time_point_cast
	template <class ToDuration, class Clock, class Duration>
	constexpr time_point<Clock, ToDuration> time_point_cast(const time_point<Clock, Duration>& t);
	template <class ToDuration, class Clock, class Duration>
	constexpr time_point<Clock, ToDuration> floor(const time_point<Clock, Duration>& tp);
	template <class ToDuration, class Clock, class Duration>
	constexpr time_point<Clock, ToDuration> ceil(const time_point<Clock, Duration>& tp);
	template <class ToDuration, class Clock, class Duration>
	constexpr time_point<Clock, ToDuration> round(const time_point<Clock, Duration>& tp);
	template <class Rep, class Period>
	constexpr duration<Rep, Period> abs(duration<Rep, Period> d);
	// clocks
	class system_clock {
		using time_point = chrono::time_point<system_clock>;
		static constexpr bool is_steady ;
		static time_point now() noexcept;
		static time_t to_time_t(const time_point& t) noexcept;
		static time_point from_time_t(time_t t) noexcept;
	};
	template <class Duration>
	using sys_time = time_point<system_clock, Duration>; // C++20
	using sys_seconds = sys_time<seconds>;
	using sys_days = sys_time<days>;
	class utc_clock { // C++20
		template <class Duration>
		static sys_time<common_type_t<Duration, seconds>> to_sys(const utc_time<Duration>& t);
		template <class Duration>
		static utc_time<common_type_t<Duration, seconds>> from_sys(const sys_time<Duration>& t);
	};
	template <class Duration>
	using utc_time = time_point<utc_clock, Duration>;
	using utc_seconds = utc_time<seconds>;
	class tai_clock { // C++20
		template <class Duration>
		static utc_time<common_type_t<Duration, seconds>> to_utc(const tai_time<Duration>&) noexcept;
		template <class Duration>
		static tai_time<common_type_t<Duration, seconds>> from_utc(const utc_time<Duration>&) noexcept;
	};
	struct leap_second_info { // C++20
		bool is_leap_second;
		seconds elapsed;
	};
	template <class Duration>
	leap_second_info get_leap_second_info(const utc_time<Duration>& ut);
	template <class Duration>
	using tai_time = time_point<tai_clock, Duration>;
	using tai_seconds = tai_time<seconds>;
	class gps_clock; // C++20
	template <class Duration>
	using gps_time = time_point<gps_clock, Duration>;
	using gps_seconds = gps_time<seconds>;
	using file_clock; // C++20
	template <class Duration>
	using file_time = time_point<file_clock, Duration>;
	class steady_clock;
	class high_resolution_clock;
	// local time C++20
	struct local_t {};
	template <class Duration>
	using local_time = time_point<local_t, Duration>;
	using local_seconds = local_time<seconds>;
	using local_days = local_time<days>;
	template <class DestClock, class SourceClock>
	struct clock_time_conversion;
	template <class DestClock, class SourceClock, class Duration>
	auto clock_cast(const time_point<SourceClock, Duration>& t);
	struct last_spec;
	class day;
	class month;
	class year;
	class weekday;
	class weekday_indexed;
	class weekday_last;
	class month_day;
	class month_day_last;
	class month_weekday;
	class month_weekday_last;
	class year_month;
	class year_month_day;
	class year_month_day_last;
	class year_month_weekday;
	class year_month_weekday_last;
	template <class Duration> class hh_mm_ss {
		static constexpr unsigned fractional_width;
		using precision;
		constexpr bool is_negative() const noexcept;
		constexpr chrono::hours hours() const noexcept;
		constexpr chrono::minutes minutes() const noexcept;
		constexpr chrono::seconds seconds() const noexcept;
		constexpr precision subseconds() const noexcept;
		constexpr precision to_duration() const noexcept;
	};
	constexpr bool is_am(const hours& h) noexcept;
	constexpr bool is_pm(const hours& h) noexcept;
	constexpr hours make12(const hours& h) noexcept;
	constexpr hours make24(const hours& h, bool is_pm) noexcept;
	// time zone database
	struct tzdb {
		string version;
		vector<time_zone> zones;
		vector<link> links;
		vector<leap> leaps;
		const time_zone* locate_zone(string_view tz_name) const;
		const time_zone* current_zone() const;
	};
	class tzdb_list {
		class const_iterator;
		const tzdb& front() const noexcept;
		const_iterator erase_after(const_iterator p);
	};
	const tzdb& get_tzdb();
	tzdb_list& get_tzdb_list();
	const time_zone* locate_zone(string_view tz_name);
	const time_zone* current_zone();
	const tzdb& reload_tzdb();
	string remote_version();
	class nonexistent_local_time : public runtime_error {};
	class ambiguous_local_time : public runtime_error {};
	struct sys_info {
		sys_seconds begin;
		sys_seconds end;
		seconds offset;
		minutes save;
		string abbrev;
	};
	struct local_info {
		static constexpr int unique = 0;
		static constexpr int nonexistent = 1;
		static constexpr int ambiguous = 2;
		int result;
		sys_info first;
		sys_info second;
	};
	enum class choose {earliest, latest};
	class time_zone {
		string_view name() const noexcept;
		template <class Duration> sys_info get_info(const sys_time<Duration>& st) const;
		template <class Duration>
		local_info get_info(const local_time<Duration>& tp) const;
		template <class Duration>
		sys_time<common_type_t<Duration, seconds>> to_sys(const local_time<Duration>& tp) const;
		template <class Duration>
		local_time<common_type_t<Duration, seconds>> to_local(const sys_time<Duration>& tp) const;
	};
	template <class T> struct zoned_traits {
		static const time_zone* default_zone();
		static const time_zone* locate_zone(string_view name);
	};
	template <class Duration, class TimeZonePtr = const time_zone*>
	class zoned_time {
		TimeZonePtr get_time_zone() const;
		local_time<duration> get_local_time() const;
		sys_time<duration> get_sys_time() const;
		sys_info get_info() const;
	};
	using zoned_seconds = zoned_time<seconds>;
	class leap {
		constexpr sys_seconds date() const noexcept;
	};
	class link {
		string_view name() const noexcept;
		string_view target() const noexcept;
	};
	template <class charT, class Streamable>
	basic_string<charT> format(const charT* fmt, const Streamable& s);
	template <class charT, class traits, class Alloc, class Parsable>
	parse(const basic_string<charT, traits, Alloc>& format, Parsable& tp);
	// calendrical constants
	inline constexpr last_spec last{};
	inline constexpr weekday Sunday{0};
	inline constexpr weekday Monday{1};
	inline constexpr weekday Tuesday{2};
	inline constexpr weekday Wednesday{3};
	inline constexpr weekday Thursday{4};
	inline constexpr weekday Friday{5};
	inline constexpr weekday Saturday{6};
	inline constexpr month January{1};
	inline constexpr month February{2};
	inline constexpr month March{3};
	inline constexpr month April{4};
	inline constexpr month May{5};
	inline constexpr month June{6};
	inline constexpr month July{7};
	inline constexpr month August{8};
	inline constexpr month September{9};
	inline constexpr month October{10};
	inline constexpr month November{11};
	inline constexpr month December{12}
}

#include <typeindex>
namespace std { // Class type_index
	class type_index {
		size_t hash_code() const noexcept;
		const char* name() const noexcept;
	};
}

#include <execution>
namespace std { // Execution policies
	template <class T> struct is_execution_policy;
	template <class T> inline constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

	namespace execution {
		class sequenced_policy;
		class parallel_policy;
		class parallel_unsequenced_policy;
		class unsequenced_policy;
		inline constexpr sequenced_policy seq;
		inline constexpr parallel_policy par;
		inline constexpr parallel_unsequenced_policy par_unseq;
		inline constexpr unsequenced_policy unseq;
	}
}

#include <format> // C++20
namespace std { // Formatting
	template <class... Args>
	string format(string_view fmt, const Args&... args);
	string vformat(string_view fmt, format_args args);
	template <class Out, class... Args>
	Out format_to(Out out, string_view fmt, const Args&... args);
	template <class Out>
	Out vformat_to(Out out, string_view fmt, format_args_t<Out, char> args);
	template <class Out> struct format_to_n_result {
		Out out;
		iter_difference_t<Out> size;
	};
	template <class Out, class... Args>
	format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n, string_view fmt, const Args&... args);
	template <class... Args>
	size_t formatted_size(string_view fmt, const Args&... args);
	template <class T, class charT = char> struct formatter;
	template <class charT>
	class basic_format_parse_context {
		constexpr void advance_to(const_iterator it);
		constexpr size_t next_arg_id();
		constexpr void check_arg_id(size_t id);
	};
	using format_parse_context = basic_format_parse_context<char>;
	using wformat_parse_context = basic_format_parse_context<wchar_t>;
	template <class Out, class charT>
	class basic_format_context {
		template <class T> using formatter_type = formatter<T, charT>;
		basic_format_arg<basic_format_context> arg(size_t id) const;
		std::locale locale();
		iterator out();
		void advance_to(iterator it);
	};
	using format_context = basic_format_context<unspecified, char>;
	using wformat_context = basic_format_context<unspecified, wchar_t>;
	template <class Context> class basic_format_arg {
		class handle;
	};
	template <class Visitor, class Context> visit_format_arg(Visitor&& vis, basic_format_arg<Context> arg);
	template <class Context = format_context, class... Args>
	format-arg-store <Context, Args...> make_format_args(const Args&... args);
	template <class... Args>
	format-arg-store <wformat_context, Args...> make_wformat_args(const Args&... args);
	template <class Context>
	class basic_format_args {
		basic_format_arg<Context> get(size_t i) const noexcept;
	};
	using format_args = basic_format_args<format_context>;
	using wformat_args = basic_format_args<wformat_context>;
	template <class Out, class charT>
	using format_args_t = basic_format_args<basic_format_context<Out, charT>>;
	class format_error : public runtime_error {};
}

#include <string>
namespace std { // Strings
	template <class charT> struct char_traits;
	template <> struct char_traits<char> {
		using char_type = char;
		using int_type = int;
		using off_type = streamoff;
		using pos_type = streampos;
		using state_type = mbstate_t;
		using comparison_category = strong_ordering; // C++20
		static constexpr void assign(char_type& c1, const char_type& c2) noexcept;
		static constexpr bool eq(char_type c1, char_type c2) noexcept;
		static constexpr bool lt(char_type c1, char_type c2) noexcept;
		static constexpr int compare(const char_type* s1, const char_type* s2, size_t n);
		static constexpr size_t length(const char_type* s);
		static constexpr const char_type* find(const char_type* s, size_t n, const char_type& a);
		static char_type* move(char_type* s1, const char_type* s2, size_t n);
		static char_type* copy(char_type* s1, const char_type* s2, size_t n);
		static char_type* assign(char_type* s, size_t n, char_type a);
		static constexpr int_type not_eof(int_type c) noexcept;
		static constexpr char_type to_char_type(int_type c) noexcept;
		static constexpr int_type to_int_type(char_type c) noexcept;
		static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept;
		static constexpr int_type eof() noexcept;
	};
	template <> struct char_traits<char8_t> { // C++20
		using char_type = char8_t;
		using int_type = unsigned int;
		using off_type = streamoff;
		using pos_type = u8streampos;
		using state_type = mbstate_t;
	}
	template <> struct char_traits<char16_t> {
		using char_type = char16_t;
		using int_type = uint_least16_t;
		using off_type = streamoff;
		using pos_type = u16streampos;
		using state_type = mbstate_t;
	};
	template <> struct char_traits<char32_t> {
		using char_type = char32_t;
		using int_type = uint_least32_t;
		using off_type = streamoff;
		using pos_type = u32streampos;
		using state_type = mbstate_t;
	};
	template <> struct char_traits<wchar_t> {
		using char_type = wchar_t;
		using int_type = wint_t;
		using off_type = streamoff;
		using pos_type = wstreampos;
		using state_type = mbstate_t;
	};

	template <class charT, class traits, class Allocator>
	basic_istream<charT, traits>& getline(basic_istream<charT, traits>& is, basic_string<charT, traits, Allocator>& str, charT delim);
	template <class charT, class traits, class Allocator, class U>
	void erase(basic_string<charT, traits, Allocator>& c, const U& value); // C++20
	template <class charT, class traits, class Allocator, class Predicate>
	void erase_if(basic_string<charT, traits, Allocator>& c, Predicate pred); // C++20

	using string = basic_string<char>;
	using u8string = basic_string<char8_t>; // C++20
	using u16string = basic_string<char16_t>;
	using u32string = basic_string<char32_t>;
	using wstring = basic_string<wchar_t>;

	int stoi(const string& str, size_t* idx = 0, int base = 10);
	long stol(const string& str, size_t* idx = 0, int base = 10);
	unsigned long stoul(const string& str, size_t* idx = 0, int base = 10);
	long long stoll(const string& str, size_t* idx = 0, int base = 10);
	unsigned long long stoull(const string& str, size_t* idx = 0, int base = 10);
	float stof(const string& str, size_t* idx = 0);
	double stod(const string& str, size_t* idx = 0);
	long double stold(const string& str, size_t* idx = 0);
	string to_string(int val);
	wstring to_wstring(int val);

	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_string {
		using traits_type = traits;
		using value_type = charT;
		using allocator_type = Allocator;
		using size_type = typename allocator_traits<Allocator>::size_type;
		using difference_type = typename allocator_traits<Allocator>::difference_type;
		using pointer = typename allocator_traits<Allocator>::pointer;
		using const_pointer = typename allocator_traits<Allocator>::const_pointer;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator;
		using const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		static const size_type npos = -1;

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		reverse_iterator rend() noexcept;
		const_reverse_iterator rend() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;
		const_reverse_iterator crbegin() const noexcept;
		const_reverse_iterator crend() const noexcept;

		size_type size() const noexcept;
		size_type length() const noexcept;
		size_type max_size() const noexcept;
		void resize(size_type n, charT c);
		void resize(size_type n);
		size_type capacity() const noexcept;
		void reserve(size_type res_arg = 0);
		void shrink_to_fit();
		void clear() noexcept;
		[[nodiscard]] bool empty() const noexcept;

		const_reference operator[](size_type pos) const;
		reference operator[](size_type pos);
		const_reference at(size_type n) const;
		reference at(size_type n);
		const charT& front() const;
		charT& front();
		const charT& back() const;
		charT& back();

		basic_string& append(const charT* s);
		void push_back(charT c);
		basic_string& assign(const basic_string& str);
		basic_string& insert(size_type pos, const basic_string& str);
		basic_string& erase(size_type pos = 0, size_type n = npos)
		void pop_back();
		basic_string& replace(size_type pos1, size_type n1, const basic_string& str);
		size_type copy(charT* s, size_type n, size_type pos = 0) const;
		void swap(basic_string& str) noexcept;

		const charT* c_str() const noexcept;
		const charT* data() const noexcept;
		constexpr allocator_type get_allocator() const noexcept;

		size_type find(basic_string_view<charT, traits> sv, size_type pos = 0) const noexcept;
		size_type rfind(basic_string_view<charT, traits> sv, size_type pos = npos) const noexcept;
		size_type find_first_of(basic_string_view<charT, traits> sv, size_type pos = 0) const noexcept;
		size_type find_last_of(basic_string_view<charT, traits> sv, size_type pos = npos) const noexcept;
		size_type find_first_not_of(basic_string_view<charT, traits> sv, size_type pos = 0) const noexcept;
		size_type find_last_not_of(basic_string_view<charT, traits> sv, size_type pos = npos) const noexcept;
		basic_string substr(size_type pos = 0, size_type n = npos) const;
		int compare(basic_string_view<charT, traits> sv) const noexcept;
		bool starts_with(basic_string_view<charT, traits> x) const noexcept; // C++20
		bool ends_with(basic_string_view<charT, traits> x) const noexcept; // C++20
	};
}

#include <string_view>
namespace std { // String view
	using string_view = basic_string_view<char>;
	using u8string_view = basic_string_view<char8_t>; // C++20
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
	using wstring_view = basic_string_view<wchar_t>;

	template <class charT, class traits = char_traits<charT>>
	class basic_string_view {
		constexpr void remove_prefix(size_type n);
		constexpr void remove_suffix(size_type n);
		size_type copy(charT* s, size_type n, size_type pos = 0) const;
	};
}

#include <locale>
#include <codecvt>
namespace std { // Localization
	class locale {
		class facet;
		class id;
		using category = int;
		static const category
			none,
			collate,
			ctype,
			monetary,
			numeric,
			time,
			messages,
			all = collate | ctype | monetary | numeric | time | messages;

		basic_string<char> name() const;
		static locale global(const locale&);
		static const locale& classic();
	};

	template <class Facet> const Facet& use_facet(const locale&);
	template <class Facet> bool has_facet(const locale&) noexcept;

	// ctype
	class ctype_base {
		using mask = T;
		static const mask space;
		static const mask print;
		static const mask cntrl;
		static const mask upper;
		static const mask lower;
		static const mask alpha;
		static const mask digit;
		static const mask punct;
		static const mask xdigit;
		static const mask blank;
		static const mask alnum = alpha | digit;
		static const mask graph = alnum | punct;
	};

	template <class charT>
	class ctype : public locale::facet, public ctype_base {
		using char_type = charT;
		bool is(mask m, charT c) const;
		const charT* is(const charT* low, const charT* high, mask* vec) const;
		const charT* scan_is(mask m, const charT* low, const charT* high) const;
		const charT* scan_not(mask m, const charT* low, const charT* high) const;
		charT toupper(charT c) const;
		const charT* toupper(charT* low, const charT* high) const;
		charT tolower(charT c) const;
		const charT* tolower(charT* low, const charT* high) const;
		charT widen(char c) const;
		const char* widen(const char* low, const char* high, charT* to) const;
		char narrow(charT c, char dfault) const;
		const charT* narrow(const charT* low, const charT* high, char dfault, char* to) const;
		static locale::id id;
	};
	template <> class ctype<char> : public locale::facet, public ctype_base {};
	template <class charT> class ctype_byname : public ctype<charT> {};

	class codecvt_base {
		enum result { ok, partial, error, noconv };
	};
	template <class internT, class externT, class stateT>
	class codecvt : public locale::facet, public codecvt_base {
		using intern_type = internT;
		using extern_type = externT;
		using state_type = stateT;
		result out(stateT& state, const internT* from, const internT* from_end, const internT*& from_next, externT* to, externT* to_end, externT*& to_next) const;
		result unshift(stateT& state, externT* to, externT* to_end, externT*& to_next) const;
		result in(stateT& state, const externT* from, const externT* from_end, const externT*& from_next, internT* to, internT* to_end, internT*& to_next) const;
		int encoding() const noexcept;
		bool always_noconv() const noexcept;
		int length(stateT&, const externT* from, const externT* end, size_t max) const;
		int max_length() const noexcept;
		static locale::id id;
	};
	template <class internT, class externT, class stateT>
	class codecvt_byname : public codecvt<internT, externT, stateT> {};

	// deprecated C++14 code conversion facets
	enum codecvt_mode { // C++14
		consume_header,
		generate_header,
		little_endian
	};
	template <class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
	class codecvt_utf8 : public codecvt<Elem, char, mbstate_t> {}; // C++14
	template <class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
	class codecvt_utf16 : public codecvt<Elem, char, mbstate_t> {}; // C++14
	template <class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
	class codecvt_utf8_utf16 : public codecvt<Elem, char, mbstate_t> {}; // C++14
	template <class Codecvt, class Elem = wchar_t, class Wide_alloc = allocator<Elem>, class Byte_alloc = allocator<char>>
	class wstring_convert { // C++14
		wide_string from_bytes(char byte);
		byte_string to_bytes(Elem wchar);
		size_t converted() const noexcept;
		state_type state() const;
	};
	template <class Codecvt, class Elem = wchar_t, class Tr = char_traits<Elem>>
	class wbuffer_convert; // C++14

	// numeric
	template <class charT, class InputIterator = istreambuf_iterator<charT>>
	class num_get : public locale::facet {
		using char_type = charT;
		using iter_type = InputIterator;
		iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, bool& v) const;
		static locale::id id;
	};
	template <class charT, class OutputIterator = ostreambuf_iterator<charT>>
	class num_put : public locale::facet {
		using char_type = charT;
		using iter_type = OutputIterator;
		iter_type put(iter_type s, ios_base& f, char_type fill, bool v) const;
		static locale::id id;
	};
	template <class charT>
	class numpunct : public locale::facet {
		using char_type = charT;
		using string_type = basic_string<charT>;
		char_type decimal_point() const;
		char_type thousands_sep() const;
		string grouping() const;
		string_type truename() const;
		string_type falsename() const;
		static locale::id id;
	};
	template <class charT>
	class numpunct_byname : public numpunct<charT> {};

	// collation
	template <class charT>
	class collate : public locale::facet {
		using char_type = charT;
		using string_type = basic_string<charT>;
		int compare(const charT* low1, const charT* high1, const charT* low2, const charT* high2) const;
		string_type transform(const charT* low, const charT* high) const;
		long hash(const charT* low, const charT* high) const;
		static locale::id id;
	};
	template <class charT> class collate_byname : public collate<charT> {};

	// date and time
	class time_base {
		enum dateorder { no_order, dmy, mdy, ymd, ydm };
	};
	template <class charT, class InputIterator = istreambuf_iterator<charT>>
	class time_get : public locale::facet {
		using char_type = charT;
		using iter_type = InputIterator;
		dateorder date_order() const { return do_date_order(); }
		iter_type get_time(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t) const;
		iter_type get_date(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t) const;
		iter_type get_weekday(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t) const;
		iter_type get_monthname(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t) const;
		iter_type get_year(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t) const;
		iter_type get(iter_type s, iter_type end, ios_base& f, ios_base::iostate& err, tm* t, char format, char modifier = 0) const;
		static locale::id id;
	};
	template <class charT, class InputIterator = istreambuf_iterator<charT>>
	class time_get_byname : public time_get<charT, InputIterator> {};
	template <class charT, class OutputIterator = ostreambuf_iterator<charT>>
	class time_put : public locale::facet {
		using char_type = charT;
		using iter_type = OutputIterator;
		iter_type put(iter_type s, ios_base& f, char_type fill, const tm* tmb, const charT* pattern, const charT* pat_end) const;
		static locale::id id;
	};
	template <class charT, class OutputIterator = ostreambuf_iterator<charT>>
	class time_put_byname : public time_put<charT, OutputIterator> {};

	// money
	class money_base {
		enum part { none, space, symbol, sign, value };
		struct pattern { char field[4]; };
	};
	template <class charT, class InputIterator = istreambuf_iterator<charT>>
	class money_get : public locale::facet {
		using char_type = charT;
		using iter_type = InputIterator;
		using string_type = basic_string<charT>;
		iter_type get(iter_type s, iter_type end, bool intl, ios_base& f, ios_base::iostate& err,
		long double& units) const;
		static locale::id id;
	};
	template <class charT, class OutputIterator = ostreambuf_iterator<charT>>
	class money_put {
		using char_type = charT;
		using iter_type = OutputIterator;
		using string_type = basic_string<charT>;
		iter_type put(iter_type s, bool intl, ios_base& f, char_type fill, long double units) const;
		static locale::id id;
	};
	template <class charT, bool Intl = false>
	class moneypunct : public locale::facet, public money_base {
		using char_type = charT;
		using string_type = basic_string<charT>;
		charT decimal_point() const;
		charT thousands_sep() const;
		string grouping() const;
		string_type curr_symbol() const;
		string_type positive_sign() const;
		string_type negative_sign() const;
		int frac_digits() const;
		pattern pos_format() const;
		pattern neg_format() const;
		static locale::id id;
		static const bool intl = International;
	};
	template <class charT, bool Intl = false>
	class moneypunct_byname : public moneypunct<charT, Intl> {
		using pattern = money_base::pattern;
		using string_type = basic_string<charT>;
	};

	// message retrieval
	class messages_base {
		using catalog;
	};
	template <class charT>
	class messages : public locale::facet, public messages_base {
		using char_type = charT;
		using string_type = basic_string<charT>;
		catalog open(const basic_string<char>& fn, const locale&) const;
		string_type get(catalog c, int set, int msgid, const string_type& dfault) const;
		void close(catalog c) const;
		static locale::id id;
	};
	template <class charT>
	class messages_byname : public messages<charT> {
		using catalog = messages_base::catalog;
		using string_type = basic_string<charT>;
	};
}

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
namespace std { // Sequence containers
	template <class T, size_t N>
	struct array {
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator;
		using const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		void fill(const T& u);
		void swap(array&) noexcept(is_nothrow_swappable_v<T>);
		// iterators
		constexpr iterator begin() noexcept;
		constexpr const_iterator begin() const noexcept;
		constexpr iterator end() noexcept;
		constexpr const_iterator end() const noexcept;
		constexpr reverse_iterator rbegin() noexcept;
		constexpr const_reverse_iterator rbegin() const noexcept;
		constexpr reverse_iterator rend() noexcept;
		constexpr const_reverse_iterator rend() const noexcept;
		constexpr const_iterator cbegin() const noexcept;
		constexpr const_iterator cend() const noexcept;
		constexpr const_reverse_iterator crbegin() const noexcept;
		constexpr const_reverse_iterator crend() const noexcept;
		// capacity
		constexpr bool empty() const noexcept;
		constexpr size_type size() const noexcept;
		constexpr size_type max_size() const noexcept;
		// element access
		constexpr reference operator[](size_type n);
		constexpr const_reference operator[](size_type n) const;
		constexpr reference at(size_type n);
		constexpr const_reference at(size_type n) const;
		constexpr reference front();
		constexpr const_reference front() const;
		constexpr reference back();
		constexpr const_reference back() const;
		constexpr T * data() noexcept;
		constexpr const T * data() const noexcept;
	};

	template <class T, size_t N>
	constexpr array<remove_cv_t<T>, N> to_array(T (&a)[N]); // C++20
	template <size_t I, class T, size_t N>
	constexpr T& get(array<T, N>&) noexcept; // C++20

	template <class T, class Allocator = allocator<T>>
	class deque {
		using allocator_type = Allocator;
		template <class InputIterator>
		void assign(initializer_list<T>);
		allocator_type get_allocator() const noexcept;
		// capacity
		void resize(size_type sz, const T& c);
		void shrink_to_fit();
		// modifiers
		template <class... Args> reference emplace_front(Args&&... args);
		template <class... Args> reference emplace_back(Args&&... args);
		template <class... Args> iterator emplace(const_iterator position, Args&&... args);
		void push_front(const T& x);
		void push_back(const T& x);
		iterator insert(const_iterator position, const T& x);
		void pop_front();
		void pop_back();
		iterator erase(const_iterator position);
		void swap(deque&) noexcept(allocator_traits<Allocator>::is_always_equal::value);
		void clear() noexcept;
	};

	template <class T, class Allocator = allocator<T>>
	class forward_list {
		iterator before_begin() noexcept;
		const_iterator cbefore_begin() const noexcept;
		// modifiers
		template <class... Args> iterator emplace_after(const_iterator position, Args&&... args);
		iterator insert_after(const_iterator position, const T& x);
		iterator erase_after(const_iterator position);
		// forward_list operations
		void splice_after(const_iterator position, forward_list& x);
		void remove(const T& value);
		template <class Predicate> void remove_if(Predicate pred);
		void unique();
		void merge(forward_list& x);
		void sort();
		template <class Compare> void sort(Compare comp);
		void reverse() noexcept;
	};
	namespace pmr {
		template <class T>
		using forward_list = std::forward_list<T, polymorphic_allocator<T>>;
	}

	template <class T, class Allocator = allocator<T>>
	class list {
		// list operations
		void splice(const_iterator position, list& x);
	};
	namespace pmr {
		template <class T>
		using list = std::list<T, polymorphic_allocator<T>>;
	}

	template <class T, class Allocator = allocator<T>> class vector;
	template <class Allocator> class vector<bool, Allocator> {
		class reference {
			void flip() noexcept;
		};
		void flip() noexcept;
	};
	namespace pmr {
		template <class T>
		using vector = std::vector<T, polymorphic_allocator<T>>;
	}
}

#include <map>
#include <set>
namespace std { // Associative containers
	template <class Key, class T, class Compare = less<Key>, class Allocator = allocator<pair<const Key, T>>>
	class map {
		using key_type = Key;
		using mapped_type = T;
		using key_compare = Compare;
		using node_type;
		using insert_return_type;
		class value_compare {};
		template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
		node_type extract(const_iterator position);
		template <class... Args> pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);
		template <class M> pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);
		iterator find(const key_type& x);
		size_type count(const key_type& x) const;
		bool contains(const key_type& x) const; // C++20
		template <class K> size_type count(const K& x) const;
		iterator lower_bound(const key_type& x);
		iterator upper_bound(const key_type& x);
		pair<iterator, iterator> equal_range(const key_type& x);
	};
	template <class Key, class T, class Compare = less<Key>, class Allocator = allocator<pair<const Key, T>>>
	class multimap;
	namespace pmr {
		template <class Key, class T, class Compare = less<Key>>
		using map = std::map<Key, T, Compare, polymorphic_allocator<pair<const Key, T>>>;
		template <class Key, class T, class Compare = less<Key>>
		using multimap = std::multimap<Key, T, Compare, polymorphic_allocator<pair<const Key, T>>>;
	}

	template <class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
	class set;
	template <class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
	class multiset;
	namespace pmr {
		template <class Key, class Compare = less<Key>>
		using set = std::set<Key, Compare, polymorphic_allocator<Key>>;
		template <class Key, class Compare = less<Key>>
		using multiset = std::multiset<Key, Compare, polymorphic_allocator<Key>>;
	}
}

#include <unordered_map>
#include <unordered_set>
namespace std { // Unordered associative containers
	template <class Key, class T, class Hash = hash<Key>, class Pred = equal_to<Key>, class Alloc = allocator<pair<const Key, T>>>
	class unordered_map {
		using hasher = Hash;
		using key_equal = Pred;
		using local_iterator;
		using const_local_iterator;
		hasher hash_function() const;
		key_equal key_eq() const;
		// bucket interface
		size_type bucket_count() const noexcept;
		size_type max_bucket_count() const noexcept;
		size_type bucket_size(size_type n) const;
		size_type bucket(const key_type& k) const;
		// hash policy
		float load_factor() const noexcept;
		float max_load_factor() const noexcept;
		void max_load_factor(float z);
		void rehash(size_type n);
		void reserve(size_type n);
	};
	template <class Key, class T, class Hash = hash<Key>, class Pred = equal_to<Key>, class Alloc = allocator<pair<const Key, T>>>
	class unordered_multimap;
	namespace pmr {
		template <class Key, class T, class Hash = hash<Key>, class Pred = equal_to<Key>>
		using unordered_map = std::unordered_map<Key, T, Hash, Pred, polymorphic_allocator<pair<const Key, T>>>;
		template <class Key, class T, class Hash = hash<Key>, class Pred = equal_to<Key>>
		using unordered_multimap = std::unordered_multimap<Key, T, Hash, Pred, polymorphic_allocator<pair<const Key, T>>>;
	}

	template <class Key, class Hash = hash<Key>, class Pred = equal_to<Key>, class Alloc = allocator<Key>>
	class unordered_set;
	template <class Key, class Hash = hash<Key>, class Pred = equal_to<Key>, class Alloc = allocator<Key>>
	class unordered_multiset;
	namespace pmr {
		template <class Key, class Hash = hash<Key>, class Pred = equal_to<Key>>
		using unordered_set = std::unordered_set<Key, Hash, Pred, polymorphic_allocator<Key>>;
		template <class Key, class Hash = hash<Key>, class Pred = equal_to<Key>>
		using unordered_multiset = std::unordered_multiset<Key, Hash, Pred, polymorphic_allocator<Key>>;
	}
}

#include <queue>
#include <stack>
namespace std { // Container adaptors
	template <class T, class Container = deque<T>> class queue {
		using container_type = Container;
		void push(const value_type& x);
		void pop();
	};
	template <class T, class Container = vector<T>, class Compare = less<typename Container::value_type>>
	class priority_queue;
	template <class T, class Container = deque<T>> class stack {
		reference top();
	};
}

#include <span> // C++20
namespace std { // Views
	inline constexpr size_t dynamic_extent = numeric_limits<size_t>::max();
	template <class ElementType, size_t Extent = dynamic_extent>
	class span {
		static constexpr index_type extent = Extent;
		constexpr index_type size_bytes() const noexcept;
	};
	template <class ElementType, size_t Extent>
	span<const byte> as_bytes(span<ElementType, Extent> s) noexcept;
	template <class ElementType, size_t Extent>
	span<byte> as_writable_bytes(span<ElementType, Extent> s) noexcept;
}

#include <iterator>
namespace std { // Iterators
	template <class> struct incrementable_traits; // C++20
	template <class T> using iter_difference_t;
	template <class> struct readable_traits; // C++20
	template <class T> using iter_value_t;
	// primitives
	template <class Iterator> struct iterator_traits;
	template <class T> struct iterator_traits<T*> {
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = random_access_iterator_tag;
	};
	template <T> using iter_reference_t = decltype(*declval<T&>()); // C++20
	using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<T&>()));
	// C++20 iterator concepts
	// iterator concepts
	template <class In> concept readable;
	template <readable T>
	using iter_common_reference_t = common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;
	template <class Out, class T> concept writable;
	template <class I> concept weakly_incrementable;
	template <class I> concept incrementable;
	template <class I> concept input_or_output_iterator;
	template <class S, class I> concept sentinel_for;
	template <class S, class I>
	inline constexpr bool disable_sized_sentinel = false;
	template <class S, class I> concept sized_sentinel_for;
	template <class I> concept input_iterator;
	template <class I, class T> concept output_iterator;
	template <class I> concept forward_iterator;
	template <class I> concept bidirectional_iterator;
	template <class I> concept random_access_iterator;
	template <class I> concept contiguous_iterator;
	// indirect callable requirements
	template <class F, class I> concept indirectly_unary_invocable;
	template <class F, class I> concept indirectly_regular_unary_invocable;
	template <class F, class I> concept indirectly_unary_predicate;
	template <class F, class I1, class I2 = I1> concept indirect_relation;
	template <class F, class I1, class I2 = I1> concept indirect_strict_weak_order;
	template <class F, class... Is>
	requires (readable<Is> && ...) && invocable<F, iter_reference_t<Is>...>
	using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>...>;
	template <readable I, indirectly_regular_unary_invocable<I> Proj>
	struct projected;
	template <weakly_incrementable I, class Proj>
	struct incrementable_traits<projected<I, Proj>>;
	// common algorithm requirements
	template <class In, class Out> concept indirectly_movable;
	template <class In, class Out> concept indirectly_movable_storable;
	template <class In, class Out> concept indirectly_copyable;
	template <class In, class Out> concept indirectly_copyable_storable;
	template <class I1, class I2 = I1> concept indirectly_swappable;
	template <class I1, class I2, class R, class P1 = identity, class P2 = identity> concept indirectly_comparable;
	template <class I> concept permutable;
	template <class I1, class I2, class Out, class R = ranges::less, class P1 = identity, class P2 = identity> concept mergeable;
	template <class I, class R = ranges::less, class P = identity> concept sortable;
	// primitives
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag: public input_iterator_tag {};
	struct bidirectional_iterator_tag: public forward_iterator_tag {};
	struct random_access_iterator_tag: public bidirectional_iterator_tag {};
	struct contiguous_iterator_tag: public random_access_iterator_tag {}; // C++20

	// iterator operations
	template <class InputIterator, class Distance>
	constexpr void advance(InputIterator& i, Distance n);
	template <class InputIterator>
	constexpr typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last);
	template <class InputIterator>
	constexpr InputIterator next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1);
	template <class BidirectionalIterator>
	constexpr BidirectionalIterator prev(BidirectionalIterator x, typename iterator_traits<BidirectionalIterator>::difference_type n = 1);

	// predefined iterators
	template <class Iterator> class reverse_iterator {
		constexpr Iterator base() const;
		iter_move(const reverse_iterator& i); // C++20
		iter_swap(const reverse_iterator& x, const reverse_iterator<Iterator2>& y); // C++20
	};
	template <class Iterator>
	constexpr reverse_iterator<Iterator> make_reverse_iterator(Iterator i);
	template <class Container> class back_insert_iterator;
	template <class Container>
	back_insert_iterator<Container> back_inserter(Container& x);
	template <class Container> class front_insert_iterator;
	template <class Container>
	front_insert_iterator<Container> front_inserter(Container& x);
	template <class Container> class insert_iterator;
	template <class Container>
	insert_iterator<Container> inserter(Container& x, typename Container::iterator i);
	template <class Iterator> class move_iterator;
	template <class Iterator>
	constexpr move_iterator<Iterator> make_move_iterator(Iterator i);

	template <Semiregular S> class move_sentinel; // C++20
	template <Iterator I, Sentinel<I> S>
	requires (!same_as<I, S>) class common_iterator; // C++20
	template <class I, class S>
	struct incrementable_traits<common_iterator<I, S>>;
	template <InputIterator I, class S>
	struct iterator_traits<common_iterator<I, S>>;
	struct default_sentinel_t;
	inline constexpr default_sentinel_t default_sentinel{};
	template <Iterator I> class counted_iterator; // C++20
	template <class I>
	struct incrementable_traits<counted_iterator<I>>;
	template <InputIterator I>
	struct iterator_traits<counted_iterator<I>>;
	struct unreachable_sentinel_t; // C++20
	inline constexpr unreachable_sentinel_t unreachable_sentinel{};

	// stream iterators
	template <class T, class charT = char, class traits = char_traits<charT>, class Distance = ptrdiff_t>
	class istream_iterator {
		using traits_type = traits;
		using istream_type = basic_istream<charT,traits>;
	};
	template <class T, class charT = char, class traits = char_traits<charT>>
	class ostream_iterator {
		using ostream_type = basic_ostream<charT,traits>;
	};
	template <class charT, class traits = char_traits<charT>>
	class istreambuf_iterator {
		using streambuf_type = basic_streambuf<charT,traits>;
		using istream_type = basic_istream<charT,traits>;
	};
	template <class charT, class traits = char_traits<charT>>
	class ostreambuf_iterator {
		using ostream_type = basic_ostream<charT,traits>;
	};

	template <class T, ptrdiff_t N> constexpr ptrdiff_t ssize(const T (&array)[N]) noexcept; // C++20
}

#include <ranges> // C++20
namespace std::ranges { // Ranges
	template <class T> concept range;
	template <range R>
	using iterator_t = decltype(ranges::begin(declval<R&>()));
	template <range R>
	using sentinel_t = decltype(ranges::end(declval<R&>()));
	template <range R>
	using range_difference_t = iter_difference_t<iterator_t<R>>;
	template <range R>
	using range_value_t = iter_value_t<iterator_t<R>>;
	template <range R>
	using range_reference_t = iter_reference_t<iterator_t<R>>;
	template <range R>
	using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;
	template <class>
	inline constexpr bool disable_sized_range = false;
	template <class T> concept sized_range;
	template <class T>
	inline constexpr bool enable_view;
	struct view_base {};
	template <class T> concept view;
	// other range refinements
	template <class R, class T> concept output_range;
	template <class T> concept input_range;
	template <class T> concept forward_range;
	template <class T> concept bidirectional_range;
	template <class T> concept random_access_range;
	template <class T> concept contiguous_range;
	template <class T> concept common_range;
	template <class T> concept viewable_range;
	template <class D>
	requires is_class_v<D> && same_as<D, remove_cv_t<D>>
	class view_interface : public view_base {};
	enum class subrange_kind : bool { unsized, sized };
	template <input_or_output_iterator I, sentinel_for<I> S = I, subrange_kind K>
	requires (K == subrange_kind::sized || !sized_sentinel_for<S, I>)
	class subrange : public view_interface<subrange<I, S, K>> {};
	struct dangling;
	template <range R>
	using safe_iterator_t = conditional_t<forwarding-range <R>, iterator_t<R>, dangling>;
	template <range R>
	using safe_subrange_t = conditional_t<forwarding-range <R>, subrange<iterator_t<R>>, dangling>;
	// Range factories
	template <class T>
	requires is_object_v<T>
	class empty_view : public view_interface<empty_view<T>> {};
	namespace views {
		template <class T>
		inline constexpr empty_view<T> empty{};
	}
	template <copy_constructible T>
	requires is_object_v<T>
	class single_view : public view_interface<single_view<T>> {};
	template <weakly_incrementable W, semiregular Bound = unreachable_sentinel_t>
	class iota_view : public view_interface<iota_view<W, Bound>> {};
	namespace views {
		inline constexpr iota_view{E} iota(E);
		inline constexpr iota_view{E, F} iota(E, F);
	}
	template <viewable_range R>
	using all_view = decltype(views::all(declval<R>()));
	namespace views {
		inline constexpr ref_view{E} all(E);
		inline constexpr subrange{E} all(E);
	}
	template <range R>
	requires is_object_v<R>
	class ref_view : public view_interface<ref_view<R>> {};
	template <input_range V, indirect_unary_predicate<iterator_t<V>> Pred>
	requires view<V> && is_object_v<Pred>
	class filter_view : public view_interface<filter_view<V, Pred>> {};
	namespace views { inline constexpr filter_view{E, P} filter(E, P); }
	template <input_range V, copy_constructible F>
	requires view<V> && is_object_v<F> && regular_invocable<F&, range_reference_t<V>>
	class transform_view : public view_interface<transform_view<V, F>> {};
	namespace views { inline constexpr transform_view{E, F} transform(E, F); }
	template <view> class take_view : public view_interface<take_view<V>> {};
	namespace views { inline constexpr take_view{E, F} take(E, F); }
	template <view R, class Pred>
	requires input_range<R> && is_object_v<Pred> && indirect_unary_predicate<const Pred, iterator_t<R>>
	class take_while_view : public view_interface<take_while_view<R, Pred>> {};
	namespace views { inline constexpr take_while_view{E, F} take_while(E, F); }
	template <view R>
	class drop_view : public view_interface<drop_view<R>> {};
	namespace views { inline constexpr drop_view{E, F} drop(E, F); }
	template <view R, class Pred>
	requires input_range<R> && is_object_v<Pred> && indirect_unary_predicate<const Pred, iterator_t<R>>
	class drop_while_view : public view_interface<drop_while_view<R, Pred>> {};
	namespace views { inline constexpr drop_while_view{E, F} drop_while(E, F); }
	template <input_range V>
	requires view<V> && input_range<range_reference_t<V>> && (is_reference_v<range_reference_t<V>> || view<range_value_t<V>>)
	class join_view : public view_interface<join_view<V>> {};
	namespace views { inline constexpr join_view{E} join(E); }
	template <input_range V, forward_range Pattern>
	requires view<V> && view<Pattern> && indirectly_comparable<iterator_t<V>, iterator_t<Pattern>, ranges::equal_to> && (forward_range<V> || tiny-range <Pattern>)
	class split_view : public view_interface<split_view<V, Pattern>> {};
	namespace views {
		inline constexpr split_view{E, F} split(E, F);
		inline constexpr subrange{counted_iterator{E, F}, default_sentinel} counted(E, F);
	}
	template <view V>
	requires (!common_range<V>)
	class common_view : public view_interface<common_view<V>> {};
	namespace views { inline constexpr common_view{E} common(E); }
	template <view V>
	requires bidirectional_range<V>
	class reverse_view : public view_interface<reverse_view<V>> {};
	namespace views { inline constexpr reverse_view{E} reverse(E); }
	template <movable Val, class CharT, class Traits = char_traits<CharT>>
	class basic_istream_view : public view_interface<basic_istream_view<Val, CharT, Traits>> {};
	template <class Val, class CharT, class Traits>
	basic_istream_view<Val, CharT, Traits> istream_view(basic_istream<CharT, Traits>& s);
	template <input_range R, size_t N>
	class elements_view : public view_interface<elements_view<R, N>> {};
	template <class R>
	using keys_view = elements_view<all_view<R>, 0>;
	template <class R>
	using values_view = elements_view<all_view<R>, 1>;
	namespace views {
		template <size_t N>
		inline constexpr elements_view{R} elements(R);
		inline constexpr keys_view{R} keys(R);
		inline constexpr values_view{R} values(R);
	}
}

#include <algorithm>
namespace std { // Algorithms
	namespace ranges { // C++20
		template <class I, class F>
		struct for_each_result;
		template <class I1, class I2>
		struct mismatch_result;
		template <class I, class O>
		struct copy_result;
		template <class I, class O>
		using copy_n_result = copy_result<I, O>;
		template <class I, class O>
		using copy_if_result = copy_result<I, O>;
		template <class I1, class I2>
		using copy_backward_result = copy_result<I1, I2>;
		template <class I, class O>
		using move_result = copy_result<I, O>;
		template <class I1, class I2>
		using move_backward_result = copy_result<I1, I2>;
		template <class I1, class I2>
		using swap_ranges_result = mismatch_result<I1, I2>;
		template <class I, class O>
		using unary_transform_result = copy_result<I, O>;
		template <class I1, class I2, class O>
		struct binary_transform_result;
		template <class I, class O>
		using replace_copy_result = copy_result<I, O>;
		template <class I, class O>
		using replace_copy_if_result = copy_result<I, O>;
		template <class I, class O>
		using remove_copy_result = copy_result<I, O>;
		template <class I, class O>
		using remove_copy_if_result = copy_result<I, O>;
		template <class I, class O>
		using unique_copy_result = copy_result<I, O>;
		template <class I, class O>
		using reverse_copy_result = copy_result<I, O>;
		template <class I, class O>
		using rotate_copy_result = copy_result<I, O>;
		template <class I, class O1, class O2>
		struct partition_copy_result;
		template <class I1, class I2, class O>
		using merge_result = binary_transform_result<I1, I2, O>;
		template <class I1, class I2, class O>
		using set_union_result = binary_transform_result<I1, I2, O>;
		template <class I1, class I2, class O>
		using set_intersection_result = binary_transform_result<I1, I2, O>;
		template <class I, class O>
		using set_difference_result = copy_result<I, O>;
		template <class I1, class I2, class O>
		using set_symmetric_difference_result = binary_transform_result<I1, I2, O>;
		template <class T>
		struct minmax_result;
		template <class I>
		using minmax_element_result = minmax_result<I>;
	}

	template <class InputIterator, class Predicate>
	bool all_of(InputIterator first, InputIterator last, Predicate pred);
	template <class InputIterator, class Predicate>
	bool any_of(InputIterator first, InputIterator last, Predicate pred);
	template <class InputIterator, class Predicate>
	bool none_of(InputIterator first, InputIterator last, Predicate pred);
	template <class InputIterator, class Function>
	Function for_each(InputIterator first, InputIterator last, Function f);
	template <class InputIterator, class Size, class Function>
	InputIterator for_each_n(InputIterator first, Size n, Function f);
	template <class InputIterator, class T>
	InputIterator find(InputIterator first, InputIterator last, const T& value);
	template <class InputIterator, class Predicate>
	InputIterator find_if(InputIterator first, InputIterator last, Predicate pred);
	template <class InputIterator, class Predicate>
	InputIterator find_if_not(InputIterator first, InputIterator last, Predicate pred);
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2);
	template <class InputIterator, class ForwardIterator>
	InputIterator find_first_of(InputIterator first1, InputIterator last1, ForwardIterator first2, ForwardIterator last2);
	template <class ForwardIterator>
	ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last);
	template <class InputIterator, class T>
	typename iterator_traits<InputIterator>::difference_type count(InputIterator first, InputIterator last, const T& value);
	template <class InputIterator, class Predicate>
	typename iterator_traits<InputIterator>::difference_type count_if(InputIterator first, InputIterator last, Predicate pred);
	template <class InputIterator1, class InputIterator2>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2);
	template <class InputIterator1, class InputIterator2>
	bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2);
	template <class ForwardIterator1, class ForwardIterator2>
	bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2);
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2);
	template <class ForwardIterator, class Size, class T>
	ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value);
	template <class InputIterator, class OutputIterator>
	OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result);
	template <class InputIterator, class Size, class OutputIterator>
	OutputIterator copy_n(InputIterator first, Size n, OutputIterator result);
	template <class InputIterator, class OutputIterator, class Predicate>
	OutputIterator copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred);
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result);
	template <class InputIterator, class OutputIterator>
	OutputIterator move(InputIterator first, InputIterator last, OutputIterator result);
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	BidirectionalIterator2 move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result);
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2);
	template <class ForwardIterator1, class ForwardIterator2>
	void iter_swap(ForwardIterator1 a, ForwardIterator2 b);
	template <class InputIterator, class OutputIterator, class UnaryOperation>
	OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op);
	template <class ForwardIterator, class T>
	void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value);
	template <class ForwardIterator, class Predicate, class T>
	void replace_if(ForwardIterator first, ForwardIterator last, Predicate pred, const T& new_value);
	template <class InputIterator, class OutputIterator, class T>
	OutputIterator replace_copy(InputIterator first, InputIterator last,OutputIterator result, const T& old_value, const T& new_value);
	template <class InputIterator, class OutputIterator, class Predicate, class T>
	OutputIterator replace_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred, const T& new_value);
	template <class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value);
	template <class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value);
	template <class ForwardIterator, class Generator>
	void generate(ForwardIterator first, ForwardIterator last, Generator gen);
	template <class OutputIterator, class Size, class Generator>
	OutputIterator generate_n(OutputIterator first, Size n, Generator gen);
	template <class ForwardIterator, class T>
	ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value);
	template <class ForwardIterator, class Predicate>
	ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred);
	template <class InputIterator, class OutputIterator, class T>
	OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T& value);
	template <class InputIterator, class OutputIterator, class Predicate>
	OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred);
	template <class ForwardIterator>
	ForwardIterator unique(ForwardIterator first, ForwardIterator last);
	template <class InputIterator, class OutputIterator>
	OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result);
	template <class BidirectionalIterator>
	void reverse(BidirectionalIterator first, BidirectionalIterator last);
	template <class BidirectionalIterator, class OutputIterator>
	OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result);
	template <class ForwardIterator>
	ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last);
	template <class ForwardIterator, class OutputIterator>
	OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result);
	template <class PopulationIterator, class SampleIterator, class Distance, class UniformRandomBitGenerator>
	SampleIterator sample(PopulationIterator first, PopulationIterator last, SampleIterator out, Distance n, UniformRandomBitGenerator&& g);
	template <class RandomAccessIterator, class UniformRandomBitGenerator>
	void shuffle(RandomAccessIterator first, RandomAccessIterator last, UniformRandomBitGenerator&& g);
	template <class ForwardIterator>
	constexpr ForwardIterator shift_left(ForwardIterator first, ForwardIterator last, typename iterator_traits<ForwardIterator>::difference_type n); // C++20
	template <class ForwardIterator>
	constexpr ForwardIterator shift_right(ForwardIterator first, ForwardIterator last, typename iterator_traits<ForwardIterator>::difference_type n); // C++20
	// partitions
	template <class InputIterator, class Predicate>
	bool is_partitioned(InputIterator first, InputIterator last, Predicate pred);
	template <class ForwardIterator, class Predicate>
	ForwardIterator partition(ForwardIterator first, ForwardIterator last, Predicate pred);
	template <class BidirectionalIterator, class Predicate>
	BidirectionalIterator stable_partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred);
	template <class InputIterator, class OutputIterator1, class OutputIterator2, class Predicate>
	pair<OutputIterator1, OutputIterator2> partition_copy(InputIterator first, InputIterator last, OutputIterator1 out_true, OutputIterator2 out_false, Predicate pred);
	template <class ForwardIterator, class Predicate>
	ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, Predicate pred);
	// sorting and related operations
	template <class RandomAccessIterator>
	void sort(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	void stable_sort(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last);
	template <class InputIterator, class RandomAccessIterator>
	RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last, RandomAccessIterator result_first, RandomAccessIterator result_last);
	template <class ForwardIterator>
	bool is_sorted(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator>
	ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last);
	template <class RandomAccessIterator>
	void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last);
	// binary search
	template <class ForwardIterator, class T>
	ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value);
	template <class ForwardIterator, class T>
	ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value);
	template <class ForwardIterator, class T>
	pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, const T& value);
	template <class ForwardIterator, class T>
	bool binary_search(ForwardIterator first, ForwardIterator last, const T& value);
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	// merge
	OutputIterator merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result);
	template <class BidirectionalIterator>
	void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last);
	template <class InputIterator1, class InputIterator2>
	bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2);
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result);
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result);
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result);
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result);
	// heap operations
	template <class RandomAccessIterator>
	void push_heap(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	void pop_heap(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	void make_heap(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	bool is_heap(RandomAccessIterator first, RandomAccessIterator last);
	template <class RandomAccessIterator>
	RandomAccessIterator is_heap_until(RandomAccessIterator first, RandomAccessIterator last);
	// minimum and maximum
	template <class T> constexpr const T& min(const T& a, const T& b);
	template <class T> constexpr const T& max(const T& a, const T& b);
	template <class T> constexpr pair<const T&, const T&> minmax(const T& a, const T& b);
	template <class ForwardIterator>
	constexpr ForwardIterator min_element(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator>
	constexpr ForwardIterator max_element(ForwardIterator first, ForwardIterator last);
	template <class ForwardIterator>
	constexpr pair<ForwardIterator, ForwardIterator> minmax_element(ForwardIterator first, ForwardIterator last);
	// bounded value
	template <class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi);
	// lexicographical comparison
	template <class InputIterator1, class InputIterator2>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2);
	// three-way comparison algorithms C++20
	template <class InputIterator1, class InputIterator2, class Cmp>
	constexpr auto lexicographical_compare_three_way(InputIterator1 b1, InputIterator1 e1, InputIterator2 b2, InputIterator2 e2, Cmp comp);
	// permutations
	template <class BidirectionalIterator>
	bool next_permutation(BidirectionalIterator first, BidirectionalIterator last);
	template <class BidirectionalIterator>
	bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last);
}

#include <complex>
namespace std { // Complex numbers
	template <class T> class complex {
		constexpr T real() const;
		constexpr T imag() const;
	};
	template <> class complex<float>;
	template <> class complex<double>;
	template <> class complex<long double>;

	// values
	template <class T> constexpr T real(const complex<T>&);
	template <class T> constexpr T imag(const complex<T>&);
	template <class T> T abs(const complex<T>&);
	template <class T> T arg(const complex<T>&);
	template <class T> T norm(const complex<T>&);
	template <class T> complex<T> conj(const complex<T>&);
	template <class T> complex<T> proj(const complex<T>&);
	template <class T> complex<T> polar(const T&, const T& = 0);
}

#include <random>
namespace std { // Random number generation
	template <class G>
	concept uniform_random_bit_generator;
	template <class UIntType, UIntType a, UIntType c, UIntType m>
	class linear_congruential_engine {
		using result_type = UIntType;
		static constexpr result_type multiplier = a;
		static constexpr result_type increment = c;
		static constexpr result_type modulus = m;
		static constexpr result_type min() { return c == 0u ? 1u: 0u; }
		static constexpr result_type max() { return m - 1u; }
		static constexpr result_type default_seed = 1u;
		void seed(result_type s = default_seed);
		template <class Sseq> void seed(Sseq& q);
		result_type operator()();
		void discard(unsigned long long z);
	};

	template <class UIntType, size_t w, size_t n, size_t m, size_t r, UIntType a, size_t u, UIntType d, size_t s, UIntType b, size_t t, UIntType c, size_t l, UIntType f>
	class mersenne_twister_engine {
		static constexpr size_t word_size = w;
		static constexpr size_t state_size = n;
		static constexpr size_t shift_size = m;
		static constexpr size_t mask_bits = r;
		static constexpr UIntType xor_mask = a;
		static constexpr size_t tempering_u = u;
		static constexpr UIntType tempering_d = d;
		static constexpr size_t tempering_s = s;
		static constexpr UIntType tempering_b = b;
		static constexpr size_t tempering_t = t;
		static constexpr UIntType tempering_c = c;
		static constexpr size_t tempering_l = l;
		static constexpr UIntType initialization_multiplier = f;
	};

	template <class UIntType, size_t w, size_t s, size_t r>
	class subtract_with_carry_engine {
		static constexpr size_t word_size = w;
		static constexpr size_t short_lag = s;
		static constexpr size_t long_lag = r;
		static constexpr result_type min() { return 0; }
		static constexpr result_type max() { return m − 1; }
		static constexpr result_type default_seed = 19780503u;
	};

	template <class Engine, size_t p, size_t r>
	class discard_block_engine {
		static constexpr size_t block_size = p;
		static constexpr size_t used_block = r;
		static constexpr result_type min() { return Engine::min(); }
		static constexpr result_type max() { return Engine::max(); }
		const Engine& base() const noexcept { return e; };
	};
	template <class Engine, size_t w, class UIntType>
	class independent_bits_engine;

	template <class Engine, size_t k>
	class shuffle_order_engine;

	using minstd_rand0;
	using minstd_rand;
	using mt19937;
	using mt19937_64;
	using ranlux24_base;
	using ranlux48_base;
	using ranlux24;
	using ranlux48;
	using knuth_b;
	using default_random_engine;

	class random_device {
		double entropy() const noexcept;
	};
	class seed_seq {
		size_t size() const noexcept;
		template <class OutputIterator>
		void param(OutputIterator dest) const;
	};
	template <class RealType, size_t bits, class URBG>
	RealType generate_canonical(URBG& g);

	template <class IntType = int>
	class uniform_int_distribution {
		using result_type = IntType;
		using param_type;
		void reset();
		result_type a() const;
		result_type b() const;
		param_type param() const;
		void param(const param_type& parm);
		result_type min() const;
		result_type max() const;
	};
	template <class RealType = double>
	class uniform_real_distribution;
	class bernoulli_distribution {
		double p() const;
	};
	template <class IntType = int>
	class binomial_distribution {
		IntType t() const;
		double p() const;
	};
	template <class IntType = int>
	class geometric_distribution {
		double p() const;
	};
	template <class IntType = int>
	class negative_binomial_distribution {
		IntType k() const;
		double p() const;
	};
	template <class IntType = int>
	class poisson_distribution {
		double mean() const;
	};
	template <class RealType = double>
	class exponential_distribution {
		RealType lambda() const;
	};
	template <class RealType = double>
	class gamma_distribution {
		RealType alpha() const;
		RealType beta() const;
	};
	template <class RealType = double>
	class weibull_distribution {
		RealType a() const;
		RealType b() const;
	};
	template <class RealType = double>
	class extreme_value_distribution {
		RealType a() const;
		RealType b() const;
	};
	template <class RealType = double>
	class normal_distribution {
		RealType mean() const;
		RealType stddev() const;
	};
	template <class RealType = double>
	class lognormal_distribution {
		RealType m() const;
		RealType s() const;
	};
	template <class RealType = double>
	class chi_squared_distribution {
		RealType n() const;
	};
	template <class RealType = double>
	class cauchy_distribution {
		RealType a() const;
		RealType b() const;
	};
	template <class RealType = double>
	class fisher_f_distribution {
		RealType m() const;
		RealType n() const;
	};
	template <class RealType = double>
	class student_t_distribution {
		RealType n() const;
	};
	template <class IntType = int>
	class discrete_distribution {
		vector<double> probabilities() const;
	};
	template <class RealType = double>
	class piecewise_constant_distribution {
		vector<result_type> intervals() const;
		vector<result_type> densities() const;
	};
	template <class RealType = double>
	class piecewise_linear_distribution {
		vector<result_type> intervals() const;
		vector<result_type> densities() const;
	};
}

#include <valarray>
namespace std { // Numeric arrays
	template <class T> class valarray {
		valarray shift(int) const;
		valarray cshift(int) const;
		valarray apply(T func(T)) const;
		valarray apply(T func(const T&)) const;
	};
	class slice {
		size_t start() const;
		size_t size() const;
		size_t stride() const;
	};
	template <class T> class slice_array;
	class gslice;
	template <class T> class gslice_array;
	template <class T> class mask_array;
	template <class T> class indirect_array;
}

#include <numeric>
namespace std { // Generalized numeric operations
	template <class InputIterator, class T>
	T accumulate(InputIterator first, InputIterator last, T init);
	template <class InputIterator>
	typename iterator_traits<InputIterator>::value_type reduce(InputIterator first, InputIterator last);
	template <class InputIterator1, class InputIterator2, class T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init);
	template <class InputIterator1, class InputIterator2, class T>
	T transform_reduce(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init);
	template <class InputIterator, class OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result);
	template <class InputIterator, class OutputIterator, class T>
	OutputIterator exclusive_scan(InputIterator first, InputIterator last, OutputIterator result, T init);
	template <class InputIterator, class OutputIterator>
	OutputIterator inclusive_scan(InputIterator first, InputIterator last, OutputIterator result);
	template <class InputIterator, class OutputIterator, class T,
	class BinaryOperation, class UnaryOperation>
	OutputIterator transform_exclusive_scan(InputIterator first, InputIterator last, OutputIterator result, T init, BinaryOperation binary_op, UnaryOperation unary_op);
	template <class InputIterator, class OutputIterator, class BinaryOperation, class UnaryOperation>
	OutputIterator transform_inclusive_scan(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op, UnaryOperation unary_op);
	template <class InputIterator, class OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result);
	template <class ForwardIterator, class T>
	void iota(ForwardIterator first, ForwardIterator last, T value);
	template <class M, class N>
	constexpr common_type_t<M,N> gcd(M m, N n);
	template <class M, class N>
	constexpr common_type_t<M,N> lcm(M m, N n);
	template <class T>
	constexpr T midpoint(T a, T b) noexcept; // C++20
}

#include <bit> // C++20
namespace std { // Bit manipulation
	template <typename To, typename From>
	constexpr To bit_cast(const From& from) noexcept;
	template <class T>
	constexpr bool ispow2(T x) noexcept;
	template <class T>
	constexpr T ceil2(T x) noexcept;
	template <class T>
	constexpr T floor2(T x) noexcept;
	template <class T>
	constexpr T log2p1(T x) noexcept;
	template <class T>
	[[nodiscard]] constexpr T rotl(T x, int s) noexcept;
	template <class T>
	[[nodiscard]] constexpr T rotr(T x, int s) noexcept;
	template <class T>
	constexpr int countl_zero(T x) noexcept;
	template <class T>
	constexpr int countl_one(T x) noexcept;
	template <class T>
	constexpr int countr_zero(T x) noexcept;
	template <class T>
	constexpr int countr_one(T x) noexcept;
	template <class T>
	constexpr int popcount(T x) noexcept;
	enum class endian {
		little,
		big,
		native
	};
}

#include <cmath>
namespace std { // Mathematical functions
	constexpr double lerp(double a, double b, double t); // C++20
	// mathematical special functions
	double assoc_laguerre(unsigned n, unsigned m, double x);
	double assoc_legendre(unsigned l, unsigned m, double x);
	double beta(double x, double y);
	double comp_ellint_1(double k);
	double comp_ellint_2(double k);
	double comp_ellint_3(double k, double nu);
	double cyl_bessel_i(double nu, double x);
	double cyl_bessel_j(double nu, double x);
	double cyl_bessel_k(double nu, double x);
	double cyl_neumann(double nu, double x);
	double ellint_1(double k, double phi);
	double ellint_2(double k, double phi);
	double ellint_3(double k, double nu, double phi);
	double expint(double x);
	double hermite(unsigned n, double x);
	double laguerre(unsigned n, double x);
	double legendre(unsigned l, double x);
	double riemann_zeta(double x);
	double sph_bessel(unsigned n, double x);
	double sph_legendre(unsigned l, unsigned m, double theta);
	double sph_neumann(unsigned n, double x);
}

#include <numbers> // C++20
namespace std::numbers { // Numbers
	template <class T> inline constexpr T e_v;
	template <class T> inline constexpr T log2e_v;
	template <class T> inline constexpr T log10e_v;
	template <class T> inline constexpr T pi_v;
	template <class T> inline constexpr T inv_pi_v;
	template <class T> inline constexpr T inv_sqrtpi_v;
	template <class T> inline constexpr T ln2_v;
	template <class T> inline constexpr T ln10_v;
	template <class T> inline constexpr T sqrt2_v;
	template <class T> inline constexpr T sqrt3_v;
	template <class T> inline constexpr T inv_sqrt3_v;
	template <class T> inline constexpr T egamma_v;
	template <class T> inline constexpr T phi_v;

	template <floating_point T> inline constexpr T e_v<T>;
	template <floating_point T> inline constexpr T log2e_v<T>;
	template <floating_point T> inline constexpr T log10e_v<T>;
	template <floating_point T> inline constexpr T pi_v<T>;
	template <floating_point T> inline constexpr T inv_pi_v<T>;
	template <floating_point T> inline constexpr T inv_sqrtpi_v<T>;
	template <floating_point T> inline constexpr T ln2_v<T>;
	template <floating_point T> inline constexpr T ln10_v<T>;
	template <floating_point T> inline constexpr T sqrt2_v<T>;
	template <floating_point T> inline constexpr T sqrt3_v<T>;
	template <floating_point T> inline constexpr T inv_sqrt3_v<T>;
	template <floating_point T> inline constexpr T egamma_v<T>;
	template <floating_point T> inline constexpr T phi_v<T>;

	inline constexpr double e = e_v<double>;
	inline constexpr double log2e = log2e_v<double>;
	inline constexpr double log10e = log10e_v<double>;
	inline constexpr double pi = pi_v<double>;
	inline constexpr double inv_pi = inv_pi_v<double>;
	inline constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
	inline constexpr double ln2 = ln2_v<double>;
	inline constexpr double ln10 = ln10_v<double>;
	inline constexpr double sqrt2 = sqrt2_v<double>;
	inline constexpr double sqrt3 = sqrt3_v<double>;
	inline constexpr double inv_sqrt3 = inv_sqrt3_v<double>;
	inline constexpr double egamma = egamma_v<double>;
	inline constexpr double phi = phi_v<double>;
}

#include <iosfwd>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <syncstream> // C++20
namespace std { // Input/output
	// <iosfwd>
	template <class charT> struct char_traits;
	template <> struct char_traits<char>;
	template <> struct char_traits<char8_t>;
	template <> struct char_traits<char16_t>;
	template <> struct char_traits<char32_t>;
	template <> struct char_traits<wchar_t>;
	template <class T> class allocator;
	template <class charT, class traits = char_traits<charT>>
	class istreambuf_iterator;
	template <class charT, class traits = char_traits<charT>>
	class ostreambuf_iterator;

	// <ios>
	using streamoff;
	using streamsize;
	template <class stateT> class fpos {
		stateT state() const;
		void state(stateT);
	};
	using streampos = fpos<char_traits<char>::state_type>;
	using wstreampos = fpos<char_traits<wchar_t>::state_type>;
	using u8streampos = fpos<char_traits<char8_t>::state_type>;
	using u16streampos = fpos<char_traits<char16_t>::state_type>;
	using u32streampos = fpos<char_traits<char32_t>::state_type>;

	class ios_base {
		class failure : public system_error {};
		using fmtflags;
		static constexpr fmtflags boolalpha;
		static constexpr fmtflags dec;
		static constexpr fmtflags fixed;
		static constexpr fmtflags hex;
		static constexpr fmtflags internal;
		static constexpr fmtflags left;
		static constexpr fmtflags oct;
		static constexpr fmtflags right;
		static constexpr fmtflags scientific;
		static constexpr fmtflags showbase;
		static constexpr fmtflags showpoint;
		static constexpr fmtflags showpos;
		static constexpr fmtflags skipws;
		static constexpr fmtflags unitbuf;
		static constexpr fmtflags uppercase;
		static constexpr fmtflags adjustfield;
		static constexpr fmtflags basefield;
		static constexpr fmtflags floatfield;
		using iostate;
		static constexpr iostate badbit;
		static constexpr iostate eofbit;
		static constexpr iostate failbit;
		static constexpr iostate goodbit;
		using openmode;
		static constexpr openmode app;
		static constexpr openmode ate;
		static constexpr openmode binary;
		static constexpr openmode in;
		static constexpr openmode out;
		static constexpr openmode trunc;
		using seekdir;
		static constexpr seekdir beg;
		static constexpr seekdir cur;
		static constexpr seekdir end;
		class Init {};

		fmtflags flags() const;
		fmtflags flags(fmtflags fmtfl);
		fmtflags setf(fmtflags fmtfl);
		fmtflags setf(fmtflags fmtfl, fmtflags mask);
		void unsetf(fmtflags mask);

		streamsize precision() const;
		streamsize precision(streamsize prec);
		streamsize width() const;
		streamsize width(streamsize wide);

		locale imbue(const locale& loc);
		locale getloc() const;

		static int xalloc();
		long& iword(int index);
		void*& pword(int index);

		enum event { erase_event, imbue_event, copyfmt_event };
		using event_callback = void (*)(event, ios_base&, int index);
		void register_callback(event_callback fn, int index);
		static bool sync_with_stdio(bool sync = true);
	};

	template <class charT, class traits = char_traits<charT>>
	class basic_ios : public ios_base {
		using char_type = charT;
		using int_type = typename traits::int_type;
		using pos_type = typename traits::pos_type;
		using off_type = typename traits::off_type;
		using traits_type = traits;

		explicit operator bool() const;
		bool operator!() const;
		iostate rdstate() const;
		void clear(iostate state = goodbit);
		void setstate(iostate state);
		bool good() const;
		bool eof() const;
		bool fail() const;
		bool bad() const;
		iostate exceptions() const;
		void exceptions(iostate except);

		basic_ostream<charT, traits>* tie() const;
		basic_ostream<charT, traits>* tie(basic_ostream<charT, traits>* tiestr);
		basic_streambuf<charT, traits>* rdbuf() const;
		basic_streambuf<charT, traits>* rdbuf(basic_streambuf<charT, traits>* sb);
		basic_ios& copyfmt(const basic_ios& rhs);
		char_type fill() const;
		char_type fill(char_type ch);
		locale imbue(const locale& loc);
		char narrow(char_type c, char dfault) const;
		char_type widen(char c) const;

	protected:
		void init(basic_streambuf<charT, traits>* sb);
		void move(basic_ios& rhs);
		void move(basic_ios&& rhs);
		void swap(basic_ios& rhs) noexcept;
		void set_rdbuf(basic_streambuf<charT, traits>* sb);
	};
	using ios = basic_ios<char>;
	using wios = basic_ios<wchar_t>;

	// manipulators
	ios_base& boolalpha(ios_base& str);
	ios_base& noboolalpha(ios_base& str);
	ios_base& showbase(ios_base& str);
	ios_base& noshowbase(ios_base& str);
	ios_base& showpoint(ios_base& str);
	ios_base& noshowpoint(ios_base& str);
	ios_base& showpos(ios_base& str);
	ios_base& noshowpos(ios_base& str);
	ios_base& skipws(ios_base& str);
	ios_base& noskipws(ios_base& str);
	ios_base& uppercase(ios_base& str);
	ios_base& nouppercase(ios_base& str);
	ios_base& unitbuf(ios_base& str);
	ios_base& nounitbuf(ios_base& str);
	// adjustfield
	ios_base& internal(ios_base& str);
	ios_base& left(ios_base& str);
	ios_base& right(ios_base& str);
	// basefield
	ios_base& dec(ios_base& str);
	ios_base& hex(ios_base& str);
	ios_base& oct(ios_base& str);
	// floatfield
	ios_base& fixed(ios_base& str);
	ios_base& scientific(ios_base& str);
	ios_base& hexfloat(ios_base& str);
	ios_base& defaultfloat(ios_base& str);

	enum class io_errc {
		stream = 1
	};
	template <> struct is_error_code_enum<io_errc> : public true_type {};
	error_code make_error_code(io_errc e) noexcept;
	error_condition make_error_condition(io_errc e) noexcept;
	const error_category& iostream_category() noexcept;

	// <streambuf>
	template <class charT, class traits = char_traits<charT>>
	class basic_streambuf {
		locale pubimbue(const locale& loc);
		locale getloc() const;
		basic_streambuf* pubsetbuf(char_type* s, streamsize n);
		pos_type pubseekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
		pos_type pubseekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);
		int pubsync();
		streamsize in_avail();
		int_type snextc();
		int_type sbumpc();
		int_type sgetc();
		streamsize sgetn(char_type* s, streamsize n);
		int_type sputbackc(char_type c);
		int_type sungetc();
		int_type sputc(char_type c);
		streamsize sputn(const char_type* s, streamsize n);

	protected:
		void swap(basic_streambuf& rhs);
		char_type* eback() const;
		char_type* gptr() const;
		char_type* egptr() const;
		void gbump(int n);
		void setg(char_type* gbeg, char_type* gnext, char_type* gend);
		char_type* pbase() const;
		char_type* pptr() const;
		char_type* epptr() const;
		void pbump(int n);
		void setp(char_type* pbeg, char_type* pend);
		virtual void imbue(const locale& loc);
		virtual basic_streambuf* setbuf(char_type* s, streamsize n);
		virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
		virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);
		virtual int sync();
		virtual streamsize showmanyc();
		virtual streamsize xsgetn(char_type* s, streamsize n);
		virtual int_type underflow();
		virtual int_type uflow();
		virtual int_type pbackfail(int_type c = traits::eof());
		virtual streamsize xsputn(const char_type* s, streamsize n);
		virtual int_type overflow(int_type c = traits::eof());
	};
	using streambuf = basic_streambuf<char>;
	using wstreambuf = basic_streambuf<wchar_t>;

	// <istream>
	template <class charT, class traits = char_traits<charT>>
	class basic_istream : virtual public basic_ios<charT, traits> {
		class sentry {};
		streamsize gcount() const;
		int_type get();
		basic_istream<charT, traits>& get(char_type& c);
		basic_istream<charT, traits>& getline(char_type* s, streamsize n);
		basic_istream<charT, traits>& ignore(streamsize n = 1, int_type delim = traits::eof());
		int_type peek();
		basic_istream<charT, traits>& read(char_type* s, streamsize n);
		streamsize readsome(char_type* s, streamsize n);
		basic_istream<charT, traits>& putback(char_type c);
		basic_istream<charT, traits>& unget();
		int sync();
		pos_type tellg();
		basic_istream<charT, traits>& seekg(pos_type);
		basic_istream<charT, traits>& seekg(off_type, ios_base::seekdir);
	};
	using istream = basic_istream<char>;
	using wistream = basic_istream<wchar_t>;
	extern istream cin;
	extern wistream wcin;

	template <class charT, class traits>
	basic_istream<charT, traits>& ws(basic_istream<charT, traits>& is);

	// <ostream>
	template <class charT, class traits = char_traits<charT>>
	class basic_ostream {
		class sentry {};
		basic_ostream<charT, traits>& put(char_type c);
		basic_ostream<charT, traits>& write(const char_type* s, streamsize n);
		basic_ostream<charT, traits>& flush();
		pos_type tellp();
		basic_ostream<charT, traits>& seekp(pos_type);
		basic_ostream<charT, traits>& seekp(off_type, ios_base::seekdir);
	};
	using ostream = basic_ostream<char>;
	using wostream = basic_ostream<wchar_t>;
	extern ostream cout;
	extern ostream cerr;
	extern ostream clog;
	extern wostream wcout;
	extern wostream wcerr;
	extern wostream wclog;

	// manipulators
	template <class charT, class traits>
	basic_ostream<charT, traits>& endl(basic_ostream<charT, traits>& os);
	template <class charT, class traits>
	basic_ostream<charT, traits>& ends(basic_ostream<charT, traits>& os);
	template <class charT, class traits>
	basic_ostream<charT, traits>& flush(basic_ostream<charT, traits>& os);
	template <class charT, class traits>
	basic_ostream<charT, traits>& emit_on_flush(basic_ostream<charT, traits>& os); // C++20
	template <class charT, class traits>
	basic_ostream<charT, traits>& noemit_on_flush(basic_ostream<charT, traits>& os); // C++20
	template <class charT, class traits>
	basic_ostream<charT, traits>& flush_emit(basic_ostream<charT, traits>& os); // C++20

	// <iomanip>
	T1 resetiosflags(ios_base::fmtflags mask);
	T2 setiosflags (ios_base::fmtflags mask);
	T3 setbase(int base);
	template <charT> T4 setfill(charT c);
	T5 setprecision(int n);
	T6 setw(int n);
	template <class moneyT> T7 get_money(moneyT& mon, bool intl = false);
	template <class moneyT> T8 put_money(const moneyT& mon, bool intl = false);
	template <class charT> T9 get_time(struct tm* tmb, const charT* fmt);
	template <class charT> T10 put_time(const struct tm* tmb, const charT* fmt);
	template <class charT> T11 quoted(const charT* s, charT delim = charT('"'), charT escape = charT('\\'));

	// <iostream>
	template <class charT, class traits = char_traits<charT>>
	class basic_iostream : public basic_istream<charT, traits>, public basic_ostream<charT, traits> {};
	using iostream = basic_iostream<char>;
	using wiostream = basic_iostream<wchar_t>;

	// <sstream>
	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_stringbuf : public basic_streambuf<charT, traits> {
		basic_string<charT, traits, Allocator> str() const;
		void str(const basic_string<charT, traits, Allocator>& s);
	};
	using stringbuf = basic_stringbuf<char>;
	using wstringbuf = basic_stringbuf<wchar_t>;

	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_istringstream : public basic_istream<charT, traits> {
		basic_stringbuf<charT, traits, Allocator>* rdbuf() const;
		basic_string<charT, traits, Allocator> str() const;
		void str(const basic_string<charT, traits, Allocator>& s);
	};
	using istringstream = basic_istringstream<char>;
	using wistringstream = basic_istringstream<wchar_t>;

	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_ostringstream : public basic_ostream<charT, traits> {
		basic_stringbuf<charT, traits, Allocator>* rdbuf() const;
		basic_string<charT, traits, Allocator> str() const;
		void str(const basic_string<charT, traits, Allocator>& s);
	};
	using ostringstream = basic_ostringstream<char>;
	using wostringstream = basic_ostringstream<wchar_t>;

	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_stringstream : public basic_iostream<charT, traits> {
		basic_stringbuf<charT, traits, Allocator>* rdbuf() const;
		basic_string<charT, traits, Allocator> str() const;
		void str(const basic_string<charT, traits, Allocator>& str);
	};
	using stringstream = basic_stringstream<char>;
	using wstringstream = basic_stringstream<wchar_t>;

	// <fstream>
	template <class charT, class traits = char_traits<charT>>
	class basic_filebuf : public basic_streambuf<charT, traits> {
		bool is_open() const;
		basic_filebuf* open(const char* s, ios_base::openmode mode);
		basic_filebuf* close();
	};
	using filebuf = basic_filebuf<char>;
	using wfilebuf = basic_filebuf<wchar_t>;

	template <class charT, class traits = char_traits<charT>>
	class basic_ifstream : public basic_istream<charT, traits> {
		basic_filebuf<charT, traits>* rdbuf() const;
		bool is_open() const;
		void open(const char* s, ios_base::openmode mode = ios_base::in);
		void close();
	};
	using ifstream = basic_ifstream<char>;
	using wifstream = basic_ifstream<wchar_t>;

	template <class charT, class traits = char_traits<charT>>
	class basic_ofstream : public basic_ostream<charT, traits> {
		basic_filebuf<charT, traits>* rdbuf() const;
		bool is_open() const;
		void open(const char* s, ios_base::openmode mode = ios_base::out);
		void close();
	};
	using ofstream = basic_ofstream<char>;
	using wofstream = basic_ofstream<wchar_t>;

	template <class charT, class traits = char_traits<charT>>
	class basic_fstream : public basic_iostream<charT, traits> {
		basic_filebuf<charT, traits>* rdbuf() const;
		bool is_open() const;
		void open(const char* s, ios_base::openmode mode = ios_base::in | ios_base::out);
		void close();
	};
	using fstream = basic_fstream<char>;
	using wfstream = basic_fstream<wchar_t>;

	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_syncbuf : public basic_streambuf<charT, traits> {
		bool emit();
		streambuf_type* get_wrapped() const noexcept;
		allocator_type get_allocator() const noexcept;
		void set_emit_on_sync(bool) noexcept;
	};
	using syncbuf = basic_syncbuf<char>;
	using wsyncbuf = basic_syncbuf<wchar_t>;
	template <class charT, class traits = char_traits<charT>, class Allocator = allocator<charT>>
	class basic_osyncstream : public basic_ostream<charT, traits> {
		using allocator_type = Allocator;
		using streambuf_type = basic_streambuf<charT, traits>;
		using syncbuf_type = basic_syncbuf<charT, traits, Allocator>;
	};
	using osyncstream = basic_osyncstream<char>;
	using wosyncstream = basic_osyncstream<wchar_t>;
}
#include <strstream> // C++98
namespace std { // deprecated char* streams
	class strstreambuf : public basic_streambuf<char> {
		void freeze(bool freezefl = true);
		char* str();
		int pcount();
	protected:
		int_type overflow (int_type c = EOF) override;
		int_type pbackfail(int_type c = EOF) override;
		int_type underflow() override;
		pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out) override;
		pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out) override;
		streambuf* setbuf(char* s, streamsize n) override;
	};
	class istrstream : public basic_istream<char> {};
	class ostrstream : public basic_ostream<char> {};
	class strstream : public basic_iostream<char> {};
}

#include <filesystem>
namespace std::filesystem { // File systems
	class path {
		using value_type;
		using string_type = basic_string<value_type>;
		static constexpr value_type preferred_separator;
		enum format {
			native_format,
			generic_format,
			auto_format,
		};
		path& assign(string_type&& source);
		template <class Source>
		path& append(const Source& source);
		template <class Source>
		path& concat(const Source& x);
		void clear() noexcept;
		path& make_preferred();
		path& remove_filename();
		path& replace_filename(const path& replacement);
		path& replace_extension(const path& replacement = path());
		const string_type& native() const noexcept;
		const value_type* c_str() const noexcept;
		std::string string() const;
		std::wstring wstring() const;
		std::u8string u8string() const;
		std::u16string u16string() const;
		std::u32string u32string() const;
		std::string generic_string() const;
		std::wstring generic_wstring() const;
		std::u8string generic_u8string() const;
		std::u16string generic_u16string() const;
		std::u32string generic_u32string() const;
		int compare(const path& p) const noexcept;
		// decomposition
		path root_name() const;
		path root_directory() const;
		path root_path() const;
		path relative_path() const;
		path parent_path() const;
		path filename() const;
		path stem() const;
		path extension() const;
		// query
		[[nodiscard]] bool empty() const noexcept;
		bool has_root_name() const;
		bool has_root_directory() const;
		bool has_root_path() const;
		bool has_relative_path() const;
		bool has_parent_path() const;
		bool has_filename() const;
		bool has_stem() const;
		bool has_extension() const;
		bool is_absolute() const;
		bool is_relative() const;
		// generation
		path lexically_normal() const;
		path lexically_relative(const path& base) const;
		path lexically_proximate(const path& base) const;
	};
	size_t hash_value(const path& p) noexcept;
	template <class Source>
	[[deprecated]] path u8path(const Source& source); // C++17
	template <class InputIterator>
	[[deprecated]] path u8path(InputIterator first, InputIterator last); // C++17
	class filesystem_error : public system_error {
		const path& path1() const noexcept;
		const path& path2() const noexcept;
	};
	class directory_entry {
		void assign(const path& p);
		void replace_filename(const path& p);
		void refresh();
		const path& path() const noexcept;
		bool exists() const;
		bool is_block_file() const;
		bool is_character_file() const;
		bool is_directory() const;
		bool is_fifo() const;
		bool is_other() const;
		bool is_regular_file() const;
		bool is_socket() const;
		bool is_symlink() const;
		uintmax_t file_size() const;
		uintmax_t hard_link_count() const;
		file_time_type last_write_time() const;
		file_status status() const;
		file_status symlink_status() const;
	};
	class directory_iterator {
		directory_iterator& increment(error_code& ec);
	};
	class recursive_directory_iterator {
		directory_options options() const;
		int depth() const;
		bool recursion_pending() const;
		void pop();
		void disable_recursion_pending();
	};
	class file_status {
		void type(file_type ft) noexcept;
		void permissions(perms prms) noexcept;
		file_type type() const noexcept;
		perms permissions() const noexcept;
	};
	struct space_info {
		uintmax_t capacity;
		uintmax_t free;
		uintmax_t available;
	};
	enum class file_type {
		none,
		not_found,
		regular,
		directory,
		symlink,
		block,
		character,
		fifo,
		socket,
		unknown,
	};
	enum class perms {
		none = 0,
		owner_read = 0400,	// S_IRUSR
		owner_write = 0200,	// S_IWUSR
		owner_exec = 0100,	// S_IXUSR
		owner_all = 0700,	// S_IRWXU
		group_read = 040,	// S_IRGRP
		group_write = 020,	// S_IWGRP
		group_exec = 010,	// S_IXGRP
		group_all = 070,	// S_IRWXG
		others_read = 04,	// S_IROTH
		others_write = 02,	// S_IWOTH
		others_exec = 01,	// S_IXOTH
		others_all = 07,	// S_IRWXO
		all = 0777,
		set_uid = 04000,	// S_ISUID
		set_gid = 02000,	// S_ISGID
		sticky_bit = 01000,	// S_ISVTX
		mask = 07777,
		unknown = 0xFFFF,
	};
	enum class perm_options {
		replace,
		add,
		remove,
		nofollow,
	};
	enum class copy_options {
		none,
		skip_existing,
		overwrite_existing,
		update_existing,
		recursive,
		copy_symlinks,
		skip_symlinks,
		directories_only,
		create_symlinks,
		create_hard_links,
	};
	enum class directory_options {
		none,
		follow_directory_symlink,
		skip_permission_denied,
	};
	using file_time_type = chrono::time_point<chrono::file_clock>;
	path absolute(const path& p, const path& base = current_path());
	path canonical(const path& p, const path& base = current_path());
	path canonical(const path& p, error_code& ec);
	void copy(const path& from, const path& to);
	bool copy_file(const path& from, const path& to);
	void copy_symlink(const path& existing_symlink, const path& new_symlink);
	bool create_directories(const path& p);
	bool create_directory(const path& p);
	bool create_directory(const path& p, const path& attributes);
	void create_directory_symlink(const path& to, const path& new_symlink);
	void create_hard_link(const path& to, const path& new_hard_link);
	void create_symlink(const path& to, const path& new_symlink);
	path current_path();
	bool exists(file_status s) noexcept;
	bool equivalent(const path& p1, const path& p2);
	uintmax_t file_size(const path& p);
	uintmax_t hard_link_count(const path& p);
	bool is_block_file(file_status s) noexcept;
	bool is_character_file(file_status s) noexcept;
	bool is_directory(file_status s) noexcept;
	bool is_empty(const path& p);
	bool is_fifo(file_status s) noexcept;
	bool is_other(file_status s) noexcept;
	bool is_regular_file(file_status s) noexcept;
	bool is_socket(file_status s) noexcept;
	bool is_symlink(file_status s) noexcept;
	file_time_type last_write_time(const path& p);
	void permissions(const path& p, perms prms, perm_options opts=perm_options::replace)
	path proximate(const path& p, error_code& ec);
	path read_symlink(const path& p);
	path relative(const path& p, error_code& ec);
	bool remove(const path& p);
	uintmax_t remove_all(const path& p);
	void rename(const path& from, const path& to);
	void resize_file(const path& p, uintmax_t size);
	space_info space(const path& p);
	file_status status(const path& p);
	bool status_known(file_status s) noexcept;
	file_status symlink_status(const path& p);
	path temp_directory_path();
	path weakly_canonical(const path& p);
}

#include <regex>
namespace std { // Regular expressions
	namespace regex_constants {
		using syntax_option_type;
		inline constexpr syntax_option_type icase;
		inline constexpr syntax_option_type nosubs;
		inline constexpr syntax_option_type optimize;
		inline constexpr syntax_option_type collate;
		inline constexpr syntax_option_type ECMAScript;
		inline constexpr syntax_option_type basic;
		inline constexpr syntax_option_type extended;
		inline constexpr syntax_option_type awk;
		inline constexpr syntax_option_type grep;
		inline constexpr syntax_option_type egrep;
		inline constexpr syntax_option_type multiline;

		using match_flag_type;
		inline constexpr match_flag_type match_default = {};
		inline constexpr match_flag_type match_not_bol;
		inline constexpr match_flag_type match_not_eol;
		inline constexpr match_flag_type match_not_bow;
		inline constexpr match_flag_type match_not_eow;
		inline constexpr match_flag_type match_any;
		inline constexpr match_flag_type match_not_null;
		inline constexpr match_flag_type match_continuous;
		inline constexpr match_flag_type match_prev_avail;
		inline constexpr match_flag_type format_default = {};
		inline constexpr match_flag_type format_sed;
		inline constexpr match_flag_type format_no_copy;
		inline constexpr match_flag_type format_first_only;

		using error_type;
		inline constexpr error_type error_collate;
		inline constexpr error_type error_ctype;
		inline constexpr error_type error_escape;
		inline constexpr error_type error_backref;
		inline constexpr error_type error_brack;
		inline constexpr error_type error_paren;
		inline constexpr error_type error_brace;
		inline constexpr error_type error_badbrace;
		inline constexpr error_type error_range;
		inline constexpr error_type error_space;
		inline constexpr error_type error_badrepeat;
		inline constexpr error_type error_complexity;
		inline constexpr error_type error_stack;
	}
	class regex_error : public runtime_error {};
	template <class charT>
	struct regex_traits {
		using char_type = charT;
		using string_type = basic_string<char_type>;
		using locale_type = locale;
		using char_class_type = bitmask_type;
		static size_t length(const char_type* p);
		charT translate(charT c) const;
		charT translate_nocase(charT c) const;
		template <class ForwardIterator>
		string_type transform(ForwardIterator first, ForwardIterator last) const;
		template <class ForwardIterator>
		string_type transform_primary(ForwardIterator first, ForwardIterator last) const;
		template <class ForwardIterator>
		string_type lookup_collatename(ForwardIterator first, ForwardIterator last) const;
		template <class ForwardIterator>
		char_class_type lookup_classname(ForwardIterator first, ForwardIterator last, bool icase = false) const;
		bool isctype(charT c, char_class_type f) const;
		int value(charT ch, int radix) const;
		locale_type imbue(locale_type l);
		locale_type getloc() const;
	};
	template <class charT, class traits = regex_traits<charT>>
	class basic_regex {
		using value_type = charT;
		using traits_type = traits;
		using string_type = typename traits::string_type;
		using flag_type = regex_constants::syntax_option_type;
		using locale_type = typename traits::locale_type;
		basic_regex& assign(const basic_regex& that);
		unsigned mark_count() const;
		flag_type flags() const;
	};
	using regex = basic_regex<char>;
	using wregex = basic_regex<wchar_t>;
	template <class BidirectionalIterator>
	class sub_match {
		difference_type length() const;
		operator string_type() const;
		string_type str() const;
		int compare(const sub_match& s) const;
	};
	using csub_match = sub_match<const char*>;
	using wcsub_match = sub_match<const wchar_t*>;
	using ssub_match = sub_match<string::const_iterator>;
	using wssub_match = sub_match<wstring::const_iterator>;
	template <class BidirectionalIterator, class Allocator = allocator<sub_match<BidirectionalIterator>>>
	class match_results {
		bool ready() const;
		difference_type length(size_type sub = 0) const;
		difference_type position(size_type sub = 0) const;
		string_type str(size_type sub = 0) const;
		const_reference prefix() const;
		const_reference suffix() const;
		template <class OutputIter>
		OutputIter format(OutputIter out, const char_type* fmt_first, const char_type* fmt_last, regex_constants::match_flag_type flags = regex_constants::format_default) const;
	};
	using cmatch = match_results<const char*>;
	using wcmatch = match_results<const wchar_t*>;
	using smatch = match_results<string::const_iterator>;
	using wsmatch = match_results<wstring::const_iterator>;
	template <class BidirectionalIterator, class Allocator, class charT, class traits>
	bool regex_match(BidirectionalIterator first, BidirectionalIterator last, match_results<BidirectionalIterator, Allocator>& m, const basic_regex<charT, traits>& e, regex_constants::match_flag_type flags = regex_constants::match_default);
	template <class BidirectionalIterator, class Allocator, class charT, class traits>
	bool regex_search(BidirectionalIterator first, BidirectionalIterator last, match_results<BidirectionalIterator, Allocator>& m, const basic_regex<charT, traits>& e, regex_constants::match_flag_type flags = regex_constants::match_default);
	template <class OutputIterator, class BidirectionalIterator,
	class traits, class charT, class ST, class SA>
	OutputIterator regex_replace(OutputIterator out, BidirectionalIterator first, BidirectionalIterator last, const basic_regex<charT, traits>& e, const basic_string<charT, ST, SA>& fmt, regex_constants::match_flag_type flags = regex_constants::match_default);
	template <class BidirectionalIterator, class charT = typename iterator_traits<BidirectionalIterator>::value_type,
	class traits = regex_traits<charT>>
	class regex_iterator {
		using regex_type = basic_regex<charT, traits>;
		using iterator_category = forward_iterator_tag;
	};
	using cregex_iterator = regex_iterator<const char*>;
	using wcregex_iterator = regex_iterator<const wchar_t*>;
	using sregex_iterator = regex_iterator<string::const_iterator>;
	using wsregex_iterator = regex_iterator<wstring::const_iterator>;
	template <class BidirectionalIterator, class charT = typename iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT>>
	class regex_token_iterator;
	using cregex_token_iterator = regex_token_iterator<const char*>;
	using wcregex_token_iterator = regex_token_iterator<const wchar_t*>;
	using sregex_token_iterator = regex_token_iterator<string::const_iterator>;
	using wsregex_token_iterator = regex_token_iterator<wstring::const_iterator>;
	namespace pmr {
		template <class BidirectionalIterator>
		using match_results = std::match_results<BidirectionalIterator, polymorphic_allocator<sub_match<BidirectionalIterator>>>;
		using cmatch = match_results<const char*>;
		using wcmatch = match_results<const wchar_t*>;
		using smatch = match_results<string::const_iterator>;
		using wsmatch = match_results<wstring::const_iterator>;
	}
}

#include <atomic>
namespace std { // Atomic operations
	template <class T> struct atomic_ref {}; // C++20
	template <class T>
	struct atomic {
		static constexpr bool is_always_lock_free;
		bool is_lock_free() const volatile noexcept;
		void store(T, memory_order = memory_order_seq_cst) volatile noexcept;
		T load(memory_order = memory_order_seq_cst) const volatile noexcept;
		T exchange(T, memory_order = memory_order_seq_cst) volatile noexcept;
		bool compare_exchange_weak(T&, T, memory_order, memory_order) volatile noexcept;
		bool compare_exchange_strong(T&, T, memory_order, memory_order) volatile noexcept;
		// Specializations for integers
		integral fetch_add(integral, memory_order = memory_order_seq_cst) volatile noexcept;
		integral fetch_sub(integral, memory_order = memory_order_seq_cst) volatile noexcept;
		integral fetch_and(integral, memory_order = memory_order_seq_cst) volatile noexcept;
		integral fetch_or(integral, memory_order = memory_order_seq_cst) volatile noexcept;
		integral fetch_xor(integral, memory_order = memory_order_seq_cst) volatile noexcept;
	};
	template <class T> struct atomic<T*>;
	struct atomic_flag {
		bool test_and_set(memory_order = memory_order_seq_cst) volatile noexcept;
		void clear(memory_order = memory_order_seq_cst) volatile noexcept;
	};
}

#include <stop_token> // C++20
namespace std { // Stop tokens
	class stop_token {
		[[nodiscard]] bool stop_requested() const noexcept;
		[[nodiscard]] bool stop_possible() const noexcept;
	};
	class stop_source {
		[[nodiscard]] stop_token get_token() const noexcept;
		[[nodiscard]] bool stop_possible() const noexcept;
		[[nodiscard]] bool stop_requested() const noexcept;
		bool request_stop() noexcept;
	};
	struct nostopstate_t {
		explicit nostopstate_t() = default;
	};
	inline constexpr nostopstate_t nostopstate{};
	template <class Callback>
	class stop_callback;
}

#include <thread>
namespace std { // Threads
	template <class T>
	decay_t<T> decay_copy(T&& v);

	namespace this_thread {
		thread::id get_id() noexcept;
		void yield() noexcept;
		template <class Clock, class Duration>
		void sleep_until(const chrono::time_point<Clock, Duration>& abs_time);
		template <class Rep, class Period>
		void sleep_for(const chrono::duration<Rep, Period>& rel_time);
	}

	class thread {
		class id {};
		using native_handle_type;

		void swap(thread&) noexcept;
		bool joinable() const noexcept;
		void join();
		void detach();
		id get_id() const noexcept;
		native_handle_type native_handle();
		static unsigned hardware_concurrency() noexcept;
	};

	class jthread { // C++20
		[[nodiscard]] stop_source get_stop_source() noexcept;
		[[nodiscard]] stop_token get_stop_token() const noexcept;
		bool request_stop() noexcept;
	};
}

#include <mutex>
#include <shared_mutex>
namespace std { // Mutual exclusion
	class mutex {
		void lock();
		bool try_lock();
		void unlock();

		using native_handle_type;
		native_handle_type native_handle();
	};
	class recursive_mutex;

	class timed_mutex {
		template <class Rep, class Period>
		bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);
		template <class Clock, class Duration>
		bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time);
	};
	class recursive_timed_mutex;

	class shared_mutex {
		void lock_shared();
		bool try_lock_shared();
		void unlock_shared();
	};
	class shared_timed_mutex {
		template <class Rep, class Period>
		bool try_lock_shared_for(const chrono::duration<Rep, Period>& rel_time);
		template <class Clock, class Duration>
		bool try_lock_shared_until(const chrono::time_point<Clock, Duration>& abs_time);
	};

	struct defer_lock_t { explicit defer_lock_t() = default; };
	struct try_to_lock_t { explicit try_to_lock_t() = default; };
	struct adopt_lock_t { explicit adopt_lock_t() = default; };
	inline constexpr defer_lock_t defer_lock {};
	inline constexpr try_to_lock_t try_to_lock {};
	inline constexpr adopt_lock_t adopt_lock {};

	template <class Mutex> class lock_guard;
	template <class... MutexTypes> class scoped_lock;
	template <class Mutex> class unique_lock {
		bool owns_lock() const noexcept;
	};

	template <class Mutex> class shared_lock;

	template <class L1, class L2, class... L3> int try_lock(L1&, L2&, L3&...);
	template <class L1, class L2, class... L3> void lock(L1&, L2&, L3&...);

	struct once_flag;
	template <class Callable, class... Args>
	void call_once(once_flag& flag, Callable&& func, Args&&... args);
}

#include <condition_variable>
namespace std { // Condition variables
	void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);
	enum class cv_status { no_timeout, timeout };

	class condition_variable {
		void notify_one() noexcept;
		void notify_all() noexcept;
		void wait(unique_lock<mutex>& lock);
		template <class Predicate>
		void wait(unique_lock<mutex>& lock, Predicate pred);
		template <class Clock, class Duration>
		cv_status wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time);
		template <class Clock, class Duration, class Predicate>
		bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred);
		template <class Rep, class Period>
		cv_status wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time);
		template <class Rep, class Period, class Predicate>
		bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred);

		using native_handle_type;
		native_handle_type native_handle();
	};
	class condition_variable_any;
}

#include <semaphore> // C++20
namespace std { // Semaphore
	template <ptrdiff_t least_max_value>
	class counting_semaphore {
		void release(ptrdiff_t update = 1);
		void acquire();
		bool try_acquire() noexcept;
		template <class Rep, class Period>
		bool try_acquire_for(const chrono::duration<Rep, Period>& rel_time);
		template <class Clock, class Duration>
		bool try_acquire_until(const chrono::time_point<Clock, Duration>& abs_time);
	};
	using binary_semaphore = counting_semaphore<1>;
}

#include <latch> // C++20
namespace std { // Latches
	class latch {
		void count_down(ptrdiff_t update = 1);
		bool try_wait() const noexcept;
		void wait() const;
		void arrive_and_wait(ptrdiff_t update = 1);
	};
}

#include <barrier> // C++20
namespace std { // Barriers
	template <class CompletionFunction>
	class barrier {
		using arrival_token;
		[[nodiscard]] arrival_token arrive(ptrdiff_t update = 1);
		void wait(arrival_token&& arrival) const;
		void arrive_and_wait();
		void arrive_and_drop();
	};
}

#include <future>
namespace std { // Futures
	enum class future_errc {
		broken_promise,
		future_already_retrieved,
		promise_already_satisfied,
		no_state
	};
	enum class launch {
		async,
		deferred
	};
	enum class future_status {
		ready,
		timeout,
		deferred
	};

	template <> struct is_error_code_enum<future_errc> : public true_type {};
	error_code make_error_code(future_errc e) noexcept;
	error_condition make_error_condition(future_errc e) noexcept;
	const error_category& future_category() noexcept;
	class future_error : public logic_error {};
	template <class R> class promise {
		future<R> get_future();
		void set_value();
		void set_exception(exception_ptr p);
		void set_value_at_thread_exit();
		void set_exception_at_thread_exit(exception_ptr p);
	};
	template <class R> class promise<R&>;
	template <> class promise<void>;

	template <class R> class future {
		shared_future<R> share() noexcept;
		get();
		bool valid() const noexcept;
		void wait() const;
		template <class Rep, class Period>
		future_status wait_for(const chrono::duration<Rep, Period>& rel_time) const;
		template <class Clock, class Duration>
		future_status wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
	};
	template <class R> class future<R&>;
	template <> class future<void>;
	template <class R> class shared_future;
	template <class R> class shared_future<R&>;
	template <> class shared_future<void>;
	template <class> class packaged_task;
	template <class R, class... ArgTypes>
	class packaged_task<R(ArgTypes...)> {
		future<R> get_future();
		void make_ready_at_thread_exit(ArgTypes...);
		void reset();
	};

	template <class F, class... Args>
	[[nodiscard]] future<invoke_result_t<decay_t<F>, decay_t<Args>...>> async(F&& f, Args&&... args);
	template <class F, class... Args>
	[[nodiscard]] future<invoke_result_t<decay_t<F>, decay_t<Args>...>> async(launch policy, F&& f, Args&&... args);
}
