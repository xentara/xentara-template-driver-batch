// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "CommonReadState.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>

#include <chrono>
#include <system_error>
#include <cstdlib>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

class WriteCommand;

// Base class for outputs
class AbstractOutput
{
public:
	// Abstract classes need a virtual destructor
	// Note: This destructor is also abstract, which is not really necessary, but ensures that the class is abstract even
	// if all other abstract functions are removed. 
	virtual ~AbstractOutput() = 0;

	// Gets the I/O component the input belongs to
	// TODO: give this a more descriptive name, e.g. "_device"
	virtual auto ioComponent() const -> const TemplateIoComponent & = 0;
		
	// Attaches the output to its I/O batch
	virtual auto attachOutput(memory::Array &dataArray, std::size_t &eventCount) -> void = 0;

	// Adds any pending output to a write command payload.
	// This function must return true if data was added, oe false if no value was pending.
	virtual auto addToWriteCommand(WriteCommand &command) -> bool = 0;

	// Updates the write state and collects the events to send
	virtual auto updateWriteState(
		WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> void = 0;
};

inline AbstractOutput::~AbstractOutput() = default;

} // namespace xentara::plugins::templateDriver