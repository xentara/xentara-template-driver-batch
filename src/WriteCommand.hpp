// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/utils/tools/Unique.hpp>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

// A command used to write outputs
// TODO: implement a proper write command
class WriteCommand final : private utils::tools::Unique
{
};

} // namespace xentara::plugins::templateDriver