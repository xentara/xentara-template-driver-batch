// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "Attributes.hpp"
#include "CommonReadState.hpp"

#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/utils/eh/Failable.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <memory>

// TODO: rename namespace
// This class contains all the read state information that is common to all values read as a batch.
namespace xentara::plugins::templateDriver
{

// Per-value state information for a read operation.
// This class contains all the read state information that is tracked separately for all the values read as a batch.
template <std::regular DataType>
class PerValueReadState final
{
public:
	// Resolves an attribute that belong to this state.
	// The value attribute is not resolved, as it may be writable as well, and thus shared with another object that takes care of the write direction.
	auto resolveAttribute(std::u16string_view name) -> const model::Attribute *;

	// Resolves an event.
	// Note: This function uses the aliasing constructor of std::shared_ptr, which will cause the returned pointer to the control block of the parent.
	// This is why the parent pointer is passed along.
	auto resolveEvent(std::u16string_view name, std::shared_ptr<void> parent) -> std::shared_ptr<process::Event>;

	// Createas a read-handle for an attribute that belong to this state.
	// The value attribute is not handled, it must be gotten separately using valueReadHandle().
	// This function returns std::nullopt if the attribute is unknown (including the value attribute)
	auto readHandle(const DataBlock &dataBlock, const model::Attribute &attribute) const noexcept
		-> std::optional<data::ReadHandle>;

	// Createas a read-handle for the value attribute
	auto valueReadHandle(const DataBlock &dataBlock) const noexcept -> data::ReadHandle;

	// Attaches the state to an I/O batch
	auto attach(memory::Array &dataArray, std::size_t &eventCount) -> void;

	// Updates the data and collects the events to send
	// Note: utils::eh::Failable is a variant that can hold either a value or an std::error_code
	auto update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::Failable<DataType> &valueOrError,
		const CommonReadState::Changes &batchChanges,
		PendingEventList &eventsToFire) -> void;

private:
	// This structure is used to represent the state inside the memory block
	struct State final
	{
		// The current value
		DataType _value {};
		// The change time stamp
		std::chrono::system_clock::time_point _changeTime { std::chrono::system_clock::time_point::min() };
	};

	// A Xentara event that is fired when the value changes
	process::Event _valueChangedEvent { model::Attribute::kValue };
	// A summary event that is fired when anything changes
	process::Event _changedEvent { io::Direction::Input };

	// The array element that contains the state
	memory::Array::ObjectHandle<State> _stateHandle;
};

// TODO: add extern template statements for other supported types
extern template class PerValueReadState<double>;

} // namespace xentara::plugins::templateDriver