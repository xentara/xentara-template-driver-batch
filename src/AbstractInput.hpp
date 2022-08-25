// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "CommonReadState.hpp"
#include "ReadCommand.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/utils/eh/Failable.hpp>

#include <chrono>
#include <system_error>
#include <cstdlib>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

class TemplateIoComponent;

// Base class for inputs (and outputs that can also be read)
class AbstractInput
{
public:
	// Abstract classes need a virtual destructor
	// Note: This destructor is also abstract, which is not really necessary, but ensures that the class is abstract even
	// if all other abstract functions are removed. 
	virtual ~AbstractInput() = 0;

	// Gets the I/O component the input belongs to
	// TODO: give this a more descriptive name, e.g. "_device"
	virtual auto ioComponent() const -> const TemplateIoComponent & = 0;
	
	// Attaches the input to its I/O batch
	virtual auto attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void = 0;

	// Updates the read state and collects the events to send
	// Note: utils::eh::Failable is a variant that can hold either a value or an std::error_code
	virtual auto updateReadState(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError,
		const CommonReadState::Changes &batchChanges,
		PendingEventList &eventsToFire) -> void = 0;
};

inline AbstractInput::~AbstractInput() = default;

} // namespace xentara::plugins::templateDriver