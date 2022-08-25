// Copyright (c) embedded ocean GmbH
#pragma once

#include "Attributes.hpp"
#include "CommonReadState.hpp"
#include "WriteState.hpp"
#include "CustomError.hpp"
#include "Types.hpp"
#include "ReadCommand.hpp"
#include "ReadTask.hpp"
#include "WriteTask.hpp"

#include <xentara/io/IoBatch.hpp>
#include <xentara/io/IoBatchClass.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/plugin/EnableSharedFromThis.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/utils/core/Uuid.hpp>
#include <xentara/utils/eh/Failable.hpp>

#include <string_view>
#include <functional>
#include <memory>

// TODO: rename namespace
namespace xentara::plugins::templateDriver
{

using namespace std::literals;

class TemplateIoComponent;
class AbstractInput;
class AbstractOutput;

// A class representing a specific type of I/O batch.
// TODO: rename this class to something more descriptive
class TemplateIoBatch final : public io::IoBatch, public plugin::EnableSharedFromThis<TemplateIoBatch>
{
private:
	// A structure used to store the class specific attributes within an element's configuration
	struct Config final
	{
		// TODO: Add custom config attributes
	};
	
public:
	// The class object containing meta-information about this element type
	class Class final : public io::IoBatchClass
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
			return u"TemplateIoBatch"sv;
		}
	
		auto uuid() const -> utils::core::Uuid final
		{
			// TODO: assign a unique UUID
			return "eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee"_uuid;
		}

	private:
	    // The array handle for the class specific attrifutes within an element's configuration
		memory::Array::ObjectHandle<Config> _configHandle { config().appendObject<Config>() };

		// The global object that represents the class
		static Class _instance;
	};

	// This constructor attaches the batch to its I/O component
	TemplateIoBatch(std::reference_wrapper<TemplateIoComponent> ioComponent) :
		_ioComponent(ioComponent)
	{
	}
	
	// This function adds an input to be processed by the batch
	auto addInput(std::reference_wrapper<AbstractInput> input) -> void;

	// Resolves an attribute that belong to the common read state.
	auto resolveReadStateAttribute(std::u16string_view name) -> const model::Attribute *;

	// Resolves an event that belong to the common read state.
	auto resolveReadStateEvent(std::u16string_view name) -> std::shared_ptr<process::Event>;

	// Createas a read-handle for an attribute that belong to the common read state.
	// This function returns std::nullopt if the attribute is unknown
	auto inputStateReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>;

	// Getters for the data block that holds the data for the read operations
	constexpr auto readDataBlock() noexcept -> DataBlock &
	{
		return _readDataBlock;
	}
	constexpr auto readDataBlock() const noexcept -> const DataBlock &
	{
		return _readDataBlock;
	}
	
	// This function adds an output to be processed by the batch
	auto addOutput(std::reference_wrapper<AbstractOutput> output) -> void;

	// Getters for the data block that holds the data for the write operations
	constexpr auto writeDataBlock() noexcept -> DataBlock &
	{
		return _writeDataBlock;
	}
	constexpr auto writeDataBlock() const noexcept -> const DataBlock &
	{
		return _writeDataBlock;
	}
	
	auto resolveAttribute(std::u16string_view name) -> const model::Attribute * final;

	auto resolveEvent(std::u16string_view name) -> std::shared_ptr<process::Event> final;

	auto readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle final;

	auto realize() -> void final;

	auto prepare() -> void final;

protected:
	auto loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void final;

private:
	// The tasks need access to out private member functions
	friend class ReadTask<TemplateIoBatch>;
	friend class WriteTask<TemplateIoBatch>;

	// This function is called by the "read" task. It attempts to read the value if the I/O component is up.
	auto performReadTask(const process::ExecutionContext &context) -> void;
	// Attempts to read the data from the I/O component and updates the state accordingly.
	auto read(std::chrono::system_clock::time_point timeStamp) -> void;

	// This function is called by the "write" task. It attempts to write the value if the I/O component is up.
	auto performWriteTask(const process::ExecutionContext &context) -> void;
	// Attempts to write any pending value to the I/O component and updates the state accordingly.
	auto write(std::chrono::system_clock::time_point timeStamp) -> void;	

	// Invalidates any read data
	auto invalidateData(std::chrono::system_clock::time_point timeStamp) -> void;

	// Updates the inputs with valid data and sends events
	auto updateInputs(std::chrono::system_clock::time_point timeStamp, const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError) -> void;

	// Updates the outputs and sends events
	auto updateOutputs(std::chrono::system_clock::time_point timeStamp, std::error_code error, const OutputList &outputs) -> void;

	// The I/O component this batch belongs to
	// TODO: give this a more descriptive name, e.g. "_device"
	std::reference_wrapper<TemplateIoComponent> _ioComponent;

	// TODO: some Some I/O components may need to have the read and write command split into several commands each.
	// The Some I/O components may require a separate command for each data type, for example, or may only be able to
	// read or write objects with continuous addresses. if this is the case, each separate command needs its onw list
	// of inputs and/or output, as well as its own read state and write state.

	// The list of inputs
	std::list<std::reference_wrapper<AbstractInput>> _inputs;
	// The list of outputs
	std::list<std::reference_wrapper<AbstractOutput>> _outputs;

	// The read command to send, or nullptr if it hasn't been constructed yet.
	std::unique_ptr<ReadCommand> _readCommand;

	// Note: There is no member for the write command, as the write command is constructed on-the-fly,
	// depending on which outputs wave to be written.

	// The array that describes the structure of the read data block
	memory::Array _readDataArray;
	// The data block that holds the data for the inputs
	DataBlock _readDataBlock { _readDataArray };

	// The array that describes the structure of the write data block
	memory::Array _writeDataArray;
	// The data block that holds the data for the outputs
	DataBlock _writeDataBlock { _writeDataArray };

	// The common state for all inputs
	// TODO: use the correct value type
	CommonReadState _readState;
	
	// The state for the last write command 
	WriteState _writeState;

	// The "read" task
	ReadTask<TemplateIoBatch> _readTask { *this };
	// The "write" task
	WriteTask<TemplateIoBatch> _writeTask { *this };

	// This structure contains preallocated buffers for data needed when sending commands.
	// the buffers are preallocated to avoid memory allocations in the read() and write() functions,
	// which would not be real-time safe.
	struct
	{
		// The list of events to trigger after a read or write
		PendingEventList _eventsToFire;

		// The outputs to notify after a write operation
		OutputList _outputsToNotify;
	} _runtimeBuffers;

	// A sentinel that performs initialization and cleanup of a runtime buffer
	template <typename Buffer>
	class RuntimeBufferSentinel;
};

} // namespace xentara::plugins::templateDriver