// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/utils/tools/Unique.hpp>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

// A command used to read inputs
// TODO: implement a proper read command
class ReadCommand final : private utils::tools::Unique
{
public:
	// Class that represents the data received from the device
	// TODO: use a suitable class to represent the data
	class Payload final
	{
	};
};

} // namespace xentara::plugins::templateDriver