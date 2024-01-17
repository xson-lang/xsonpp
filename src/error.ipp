#include "error.hpp"

namespace xson::error {
	const impl::category& category() noexcept {
		static impl::category c;
		return c;
	}
}