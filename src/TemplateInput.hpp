// Copyright (c) embedded ocean GmbH
#pragma once

#include "AbstractInput.hpp"
#include "PerValueReadState.hpp"

#include <xentara/io/Io.hpp>
#include <xentara/io/IoClass.hpp>
#include <xentara/plugin/EnableSharedFromThis.hpp>

#include <functional>
#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

class TemplateIoComponent;
class TemplateIoBatch;

/// @brief A class representing a specific type of input.
/// @todo rename this class to something more descriptive
class TemplateInput final : public io::Io, public AbstractInput, public plugin::EnableSharedFromThis<TemplateInput>
{
private:
	/// @brief A structure used to store the class specific attributes within an element's configuration
	struct Config final
	{
		/// @todo Add custom config attributes
	};
	
public:
	/// @brief The class object containing meta-information about this element type
	class Class final : public io::IoClass
	{
	public:
		/// @brief Gets the global object
		static auto instance() -> Class&
		{
			return _instance;
		}

	    /// @brief Returns the array handle for the class specific attributes within an element's configuration
	    auto configHandle() const -> const auto &
        {
            return _configHandle;
        }

		/// @name Virtual Overrides for io::IoClass
		/// @{

		auto name() const -> std::string_view final
		{
			/// @todo change class name
			return "TemplateInput"sv;
		}
	
		auto uuid() const -> utils::core::Uuid final
		{
			/// @todo assign a unique UUID
			return "cccccccc-cccc-cccc-cccc-cccccccccccc"_uuid;
		}

		/// @}

	private:
	    /// @brief The array handle for the class specific attributes within an element's configuration
		memory::Array::ObjectHandle<Config> _configHandle { config().appendObject<Config>() };

		/// @brief The global object that represents the class
		static Class _instance;
	};

	/// @brief This constructor attaches the input to its I/O component
	TemplateInput(std::reference_wrapper<TemplateIoComponent> ioComponent) :
		_ioComponent(ioComponent)
	{
	}

	/// @name Virtual Overrides for io::Io
	/// @{
	
	auto dataType() const -> const data::DataType & final;

	auto directions() const -> io::Directions final;

	auto resolveAttribute(std::string_view name) -> const model::Attribute * final;

	auto resolveEvent(std::string_view name) -> std::shared_ptr<process::Event> final;

	auto readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle final;

	/// @}

	/// @name Virtual Overrides for AbstractInput
	/// @{

	auto ioComponent() const -> const TemplateIoComponent & final
	{
		return _ioComponent;
	}
	
	auto attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void final;

	auto updateReadState(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::expected<std::reference_wrapper<const ReadCommand::Payload>, std::error_code> &payloadOrError,
		const CommonReadState::Changes &commonChanges,
		PendingEventList &eventsToFire) -> void final;
		
	/// @}

	/// @brief A Xentara attribute containing the current value.
	/// @note This is a member of this class rather than of the attributes namespace, because the access flags
	/// and type may differ from class to class
	static const model::Attribute kValueAttribute;

protected:
	/// @name Virtual Overrides for io::Io
	/// @{

	auto loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void final;
		
	/// @}

private:
	/// @brief The I/O component this input belongs to
	/// @todo give this a more descriptive name, e.g. "_device"
	std::reference_wrapper<TemplateIoComponent> _ioComponent;

	/// @brief The I/O batch this input belongs to, or nullptr if it hasn't been loaded yet.
	/// @todo give this a more descriptive name, e.g. "_poll"
	TemplateIoBatch *_ioBatch { nullptr };

	/// @class xentara::plugins::templateDriver::TemplateInput
	/// @todo add information needed to decode the value from the payload of a read command, like e.g. a data offset.

	/// @brief The state
	/// @todo use the correct value type
	PerValueReadState<double> _state;
};

} // namespace xentara::plugins::templateDriver