#pragma once
#include <cstdint>

namespace Exterminate {

class MosfetDriver {
public:
	explicit MosfetDriver(uint8_t mosfetPin);
	void initialize();
	void set(bool on);
private:
	uint8_t pin_;
	bool initialized_ = false;
};

} // namespace Exterminate
