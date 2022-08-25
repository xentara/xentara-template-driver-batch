// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/memory/Array.hpp>
#include <xentara/memory/ArrayBlock.hpp>
#include <xentara/memory/memoryResources.hpp>
#include <xentara/memory/writeSentinel.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/utils/core/FixedVector.hpp>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

class AbstractOutput;

/// @brief This is the type used for the data blocks that hold the data for the inputs and the outputs
using DataBlock = memory::ArrayBlock<memory::memoryResources::Data>;

/// @brief This is the type used for the data blocks that hold the data for the inputs and the outputs
using WriteSentinel = memory::WriteSentinel<DataBlock::MemoryResource, memory::Array>;

/// @brief This is the type used to hold the list of event to trigger
using PendingEventList = utils::core::FixedVector<std::reference_wrapper<process::Event>>;

/// @brief This is the type used to hold the list of outputs to notify
using OutputList = utils::core::FixedVector<std::reference_wrapper<AbstractOutput>>;

} // namespace xentara::plugins::templateDriver
