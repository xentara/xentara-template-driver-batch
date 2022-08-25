// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "Attributes.hpp"

#include <xentara/data/Quality.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/process/Event.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <memory>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

// Common state information for a read operation.
// This class contains all the read state information that is common to all values read as a batch.
class CommonReadState final
{
public:
	// Changes that may occur when updating the data
	struct Changes
	{
		// Whether the quality has changed
		bool _qualityChanged { false };
		// Whether the error changed
		bool _errorChanged { false };

		// Determine whether anything changed
		constexpr explicit operator bool() const noexcept
		{
			return _qualityChanged || _errorChanged;
		}
	};

	// Resolves an attribute that belong to this state.
	auto resolveAttribute(std::u16string_view name) -> const model::Attribute *;

	// Resolves an event.
	// Note: This function uses the aliasing constructor of std::shared_ptr, which will cause the returned pointer to the control block of the parent.
	// This is why the parent pointer is passed along.
	auto resolveEvent(std::u16string_view name, std::shared_ptr<void> parent) -> std::shared_ptr<process::Event>;

	// Createas a read-handle for an attribute that belong to this state.
	// This function returns std::nullopt if the attribute is unknown (including the value attribute)
	auto readHandle(const DataBlock &dataBlock, const model::Attribute &attribute) const noexcept
		-> std::optional<data::ReadHandle>;

	// Attaches the state to its I/O batch
	auto attach(memory::Array &dataArray, std::size_t &eventCount) -> void;

	// Updates the data and collects the events to send
	auto update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> Changes;

private:
	// This structure is used to represent the state inside the memory block
	struct State final
	{
		// The update time stamp
		std::chrono::system_clock::time_point _updateTime { std::chrono::system_clock::time_point::min() };
		// The quality of the value
		data::Quality _quality { data::Quality::Bad };
		// The error code when reading the value, or 0 for none.
		attributes::ErrorCode _error { attributes::errorCode(CustomError::NoData) };
	};

	// A Xentara event that is fired when the inputs were read (sucessfully or not)
	process::Event _readEvent { io::Direction::Input };
	// A Xentara event that is fired when the quality changes
	process::Event _qualityChangedEvent { model::Attribute::kQuality };

	// The array element that contains the state
	memory::Array::ObjectHandle<State> _stateHandle;
};

} // namespace xentara::plugins::templateDriver