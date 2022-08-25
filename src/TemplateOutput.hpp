// Copyright (c) embedded ocean GmbH
#pragma once

#include "AbstractInput.hpp"
#include "AbstractOutput.hpp"
#include "PerValueReadState.hpp"
#include "WriteState.hpp"
#include "SingleValueQueue.hpp"

#include <xentara/io/Io.hpp>
#include <xentara/io/IoClass.hpp>
#include <xentara/plugin/EnableSharedFromThis.hpp>

#include <functional>
#include <string_view>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

using namespace std::literals;

class TemplateIoComponent;
class TemplateIoBatch;

// A class representing a specific type of output.
// TODO: rename this class to something more descriptive
class TemplateOutput final :
	public io::Io,
	public AbstractInput,
	public AbstractOutput,
	public plugin::EnableSharedFromThis<TemplateOutput>
{
private:
	// A structure used to store the class specific attributes within an element's configuration
	struct Config final
	{
		// TODO: Add custom config attributes
	};
	
public:
	// The class object containing meta-information about this element type
	class Class final : public io::IoClass
	{
	public:
		// Gets the global object
		static auto instance() -> Class&
		{
			return _instance;
		}

	    // Returns the array handle for the class specific attributes within an element's configuration
	    auto configHandle() const -> const auto &
        {
            return _configHandle;
        }

		auto name() const -> std::u16string_view final
		{
			// TODO: change class name
			return u"TemplateOutput"sv;
		}
	
		auto uuid() const -> utils::core::Uuid final
		{
			// TODO: assign a unique UUID
			return "dddddddd-dddd-dddd-dddd-dddddddddddd"_uuid;
		}

	private:
	    // The array handle for the class specific attributes within an element's configuration
		memory::Array::ObjectHandle<Config> _configHandle { config().appendObject<Config>() };

		// The global object that represents the class
		static Class _instance;
	};

	// This constructor attaches the output to its I/O component
	TemplateOutput(std::reference_wrapper<TemplateIoComponent> ioComponent) :
		_ioComponent(ioComponent)
	{
	}

	auto dataType() const -> const data::DataType &;

	auto directions() const -> io::Directions;

	auto resolveAttribute(std::u16string_view name) -> const model::Attribute * final;

	auto resolveEvent(std::u16string_view name) -> std::shared_ptr<process::Event> final;

	auto readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle final;

	auto writeHandle(const model::Attribute &attribute) noexcept -> data::WriteHandle final;

	auto ioComponent() const -> const TemplateIoComponent & final
	{
		return _ioComponent;
	}
	
	auto attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void final;

	auto updateReadState(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError,
		const CommonReadState::Changes &batchChanges,
		PendingEventList &eventsToFire) -> void final;

	auto addToWriteCommand(WriteCommand &command) -> bool final;

	auto attachOutput(memory::Array &dataArray, std::size_t &eventCount) -> void final;

	auto updateWriteState(
		WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> void final;

	// A Xentara attribute containing the current value. This is a member of this class rather than
	// of the attributes namespace, because the access flags and type may differ from class to class
	static const model::Attribute kValueAttribute;

protected:
	auto loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void final;

private:
	// Schedules a value to be written. This function is called by the value write handle.
	// TODO: use the correct value type
	auto scheduleOutputValue(double value) noexcept
	{
		_pendingOutputValue.enqueue(value);
	}

	// The I/O component this output belongs to
	// TODO: give this a more descriptive name, e.g. "_device"
	std::reference_wrapper<TemplateIoComponent> _ioComponent;
	
	// The I/O batch this input belongs to, or nullptr if it hasn't been loaded yet.
	// TODO: give this a more descriptive name, e.g. "_poll"
	TemplateIoBatch *_ioBatch { nullptr };

	// TODO: add information needed to decode the value from the payload of a read command, like e.g. a data offset.

	// The read state
	// TODO: use the correct value type
	PerValueReadState<double> _readState;
	// The write state
	WriteState _writeState;

	// The queue for the pending output value
	// TODO: use the correct value type
	SingleValueQueue<double> _pendingOutputValue;
};

} // namespace xentara::plugins::templateDriver