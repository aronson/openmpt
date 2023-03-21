/* SPDX-License-Identifier: BSL-1.0 OR BSD-3-Clause */

#ifndef MPT_EXCEPTION_TEXT_EXCEPTION_TEXT_HPP
#define MPT_EXCEPTION_TEXT_EXCEPTION_TEXT_HPP



#include "mpt/base/namespace.hpp"
#include "mpt/string/types.hpp"
#include "mpt/string_transcode/transcode.hpp"

#include <exception>
#include <stdexcept>

#include <cstring>



namespace mpt {
inline namespace MPT_INLINE_NS {



class exception_ustring_wrapper_base {
protected:
	exception_ustring_wrapper_base() = default;
	virtual ~exception_ustring_wrapper_base() = default;
public:
	virtual mpt::ustring uwhat() const = 0;
};

template <typename T>
class exception_ustring_wrapper : public T, public virtual mpt::exception_ustring_wrapper_base {
private:
	mpt::ustring m_what;
public:
	exception_ustring_wrapper(mpt::ustring str)
		: T(mpt::transcode<std::string>(mpt::exception_encoding, str))
		, m_what(str)
	{
		return;
	}
	~exception_ustring_wrapper() override = default;
	mpt::ustring uwhat() const override {
		return m_what;
	}
};

using runtime_error = mpt::exception_ustring_wrapper<std::runtime_error>;
using logic_error = mpt::exception_ustring_wrapper<std::logic_error>;



template <typename Tstring>
inline Tstring get_exception_text(const std::exception & e) {
	if (const mpt::exception_ustring_wrapper_base * pue = dynamic_cast<const mpt::exception_ustring_wrapper_base *>(&e)) {
		const mpt::exception_ustring_wrapper_base & ue = *pue;
		mpt::ustring what = ue.uwhat();
		if (what.length() > 0) {
			return mpt::transcode<Tstring>(std::move(what));
		} else if (typeid(ue).name() && (std::strlen(typeid(ue).name()) > 0)) {
			return mpt::transcode<Tstring>(mpt::source_string{typeid(ue).name()});
		} else {
			return mpt::transcode<Tstring>(mpt::source_string{"unknown exception name"});
		}
	} else if (e.what() && (std::strlen(e.what()) > 0)) {
		return mpt::transcode<Tstring>(mpt::exception_string{e.what()});
	} else if (typeid(e).name() && (std::strlen(typeid(e).name()) > 0)) {
		return mpt::transcode<Tstring>(mpt::source_string{typeid(e).name()});
	} else {
		return mpt::transcode<Tstring>(mpt::source_string{"unknown exception name"});
	}
}

template <>
inline std::string get_exception_text<std::string>(const std::exception & e) {
	if (e.what() && (std::strlen(e.what()) > 0)) {
		return std::string{e.what()};
	} else if (typeid(e).name() && (std::strlen(typeid(e).name()) > 0)) {
		return std::string{typeid(e).name()};
	} else {
		return std::string{"unknown exception name"};
	}
}



template <typename Tstring>
inline Tstring get_current_exception_text() {
	try {
		throw;
	} catch (const std::exception & e) {
		return mpt::get_exception_text<Tstring>(e);
	} catch (...) {
		return mpt::transcode<Tstring>(mpt::source_string{"unknown exception"});
	}
}

template <>
inline std::string get_current_exception_text<std::string>() {
	try {
		throw;
	} catch (const std::exception & e) {
		return mpt::get_exception_text<std::string>(e);
	} catch (...) {
		return std::string{"unknown exception"};
	}
}



} // namespace MPT_INLINE_NS
} // namespace mpt



#endif // MPT_EXCEPTION_TEXT_EXCEPTION_TEXT_HPP
