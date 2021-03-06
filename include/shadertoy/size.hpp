#ifndef _SHADERTOY_SIZE_HPP_
#define _SHADERTOY_SIZE_HPP_

#include "shadertoy/shadertoy_error.hpp"

#include <memory>

namespace shadertoy
{

/**
 * @brief Represents a 2D size
 *
 * @tparam T Type of the size elements
 */
template <typename T> struct basic_size
{
	/// Width component of this size
	T width;
	/// Height component of this size
	T height;

	/**
	 * @brief Create a default 2D size
	 */
	basic_size()
		: width(0), height(0)
	{}

	/**
	 * @brief Create a 2D size from the given dimensions
	 *
	 * @param width  Width of the created size object
	 * @param height Height of the created size object
	 */
	basic_size(T width, T height)
		: width(width), height(height)
	{}

	/**
	 * @brief Compare this size object with another
	 *
	 * @param rhs Size to compare
	 *
	 * @return true if this size and \p rhs are equal, false otherwise
	 */
	bool operator==(const basic_size<T> &rhs) const
	{
		return width == rhs.width && height == rhs.height;
	}

	/**
	 * @brief Compare this size object with another
	 *
	 * @param rhs Size to compare
	 *
	 * @return true if this size and \p rhs are different, false otherwise
	 */
	bool operator!=(const basic_size<T> &rhs) const
	{
		return width != rhs.width || height != rhs.height;
	}

	/**
	 * @brief Compare this size object with another
	 *
	 * The comparison is done by width and then by height.
	 *
	 * @param rhs Size to compare
	 *
	 * @return true if this size is less than \p rhs, false otherwise
	 */
	bool operator<(const basic_size<T> &rhs) const
	{
		return width < rhs.width && height < rhs.height;
	}
};

/**
 * @brief Base interface for objects that can return a size object
 *
 * @tparam T Type of the size object elements
 */
template <typename T> class size_ref_interface
{
public:
	virtual ~size_ref_interface() = default;

	/**
	 * @brief Resolve this size reference into an actual size object
	 *
	 * @return Resolved size object
	 */
	virtual basic_size<T> resolve() const = 0;

	/**
	 * @brief Cast this size reference into an actual size
	 *
	 * @see #resolve
	 *
	 * @return Resolved size object
	 */
	operator basic_size<T>() const { return resolve(); }
};

/**
 * @brief Represents a reference to another size_ref_interface object
 *
 * @tparam T Type of the size object elements
 */
template <typename T> class size_ref_interface_ref : public size_ref_interface<T>
{
	/// Reference interface
	const size_ref_interface<T> &int_ref_;

public:
	/**
	 * @brief Build a new size_ref_interface&lt;T&gt; from a reference
	 *
	 * \p int_ref must remain valid for as long as this object will be in use.
	 *
	 * @param int_ref Reference to an interface
	 */
	size_ref_interface_ref(const size_ref_interface<T> &int_ref)
		: int_ref_(int_ref)
	{}

	basic_size<T> resolve() const override { return int_ref_.resolve(); }
};

/**
 * @brief Constructs a reference to a size_ref_interface&lt;T&gt;
 *
 * @param int_ref Reference to a size_ref_interface&lt;T&gt;
 *
 * @see size_ref_interface_ref#size_ref_interface_ref
 *
 * @return Pointer to the constructed size_ref_interface_ref
 */
template <typename T>
std::unique_ptr<size_ref_interface<T>> make_ref(const size_ref_interface<T> &int_ref)
{
	return std::make_unique<size_ref_interface_ref<T>>(int_ref);
}

/**
 * @brief Represents a size object, wrapped in a size_ref_interface
 *
 * @tparam T Type of the size object elements
 */
template <typename T> class explicit_size : public size_ref_interface<T>
{
	/// Size object
	basic_size<T> size_;

public:
	/**
	 * @brief Build a new explicit_size from the size object \p size
	 *
	 * @param size Size of this explicit_size instance
	 */
	explicit_size(const basic_size<T> &size)
		: size_(size)
	{}

	basic_size<T> resolve() const override { return size_; }
};

/**
 * @brief Helper method to construct explicit_size objects
 *
 * @param  size Size of the new explicit_size instance
 * @tparam T    Type of the size object elements
 *
 * @see explicit_size#explicit_size
 *
 * @return Unique pointer to a new explicit_size instance
 */
template <typename T>
std::unique_ptr<size_ref_interface<T>> make_size(const basic_size<T> &size)
{
	return std::make_unique<explicit_size<T>>(size);
}

/**
 * @brief Represents a reference to a size object, wrapped in a size_ref_interface
 *
 * @tparam T Type of the size object elements
 */
template <typename T> class explicit_size_ref : public size_ref_interface<T>
{
	/// Size object reference
	const basic_size<T> &size_;

public:
	/**
	 * @brief Build a new explicit_size_ref&lt;T&gt; from a reference
	 *
	 * \p size must remain valid for as long as this object will be in use.
	 *
	 * @param size Reference to a size object
	 */
	explicit_size_ref(const basic_size<T> &size)
		: size_(size)
	{}

	basic_size<T> resolve() const override { return size_; }
};

/**
 * @brief Helper method to construct explicit_size_ref objects
 *
 * @param  size Size of the new explicit_size_ref instance
 * @tparam T    Type of the size object elements
 *
 * @see explicit_size_ref#explicit_size_ref
 *
 * @return Unique pointer to a new explicit_size_ref instance
 */
template <typename T>
std::unique_ptr<size_ref_interface<T>> make_size_ref(const basic_size<T> &size)
{
	return std::make_unique<explicit_size_ref<T>>(size);
}

/**
 * @brief Represents a reference to a size object that is obtained through a callback
 *
 * @tparam T Type of the size object elements
 * @tparam Callable Type of the callback object. Should have the signature basic_size&lt;\p T &gt;(void)
 */
template <typename T, typename Callable> class getter_size_ref : public size_ref_interface<T>
{
	/// Callback object
	Callable getter_;

public:
	/**
	 * @brief Buil a new getter_size_ref&lt;T&gt;
	 *
	 * @param getter Callback to obtain the size object
	 */
	getter_size_ref(Callable getter)
		: getter_(getter)
	{}

	basic_size<T> resolve() const override { return getter_(); }
};

/**
 * @brief Helper method to construct getter_size_ref objects
 *
 * @param getter Callable object for getter_size_ref
 * @see getter_size_ref#getter_size_ref
 *
 * @return Pointer to the constructed getter_size_ref&lt;T&gt;
 */
template <typename T, typename Callable>
std::unique_ptr<size_ref_interface<T>> make_size_getter(Callable getter)
{
	return std::make_unique<getter_size_ref<T, Callable>>(std::forward<Callable>(getter));
}

/// Unsigned integer render size
typedef basic_size<unsigned int> rsize;
/// Unsigned integer render size reference
typedef std::unique_ptr<size_ref_interface<unsigned int>> rsize_ref;

}

#endif /* _SHADERTOY_SIZE_HPP_ */
