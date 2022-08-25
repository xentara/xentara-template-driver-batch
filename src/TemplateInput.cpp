// Copyright (c) embedded ocean GmbH
#include "TemplateInput.hpp"

#include "Attributes.hpp"
#include "TemplateIoBatch.hpp"

#include <xentara/config/Resolver.hpp>
#include <xentara/data/DataType.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/utils/io/FileInputStream.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>

namespace xentara::plugins::templateDriver
{
	
using namespace std::literals;

TemplateInput::Class TemplateInput::Class::_instance;

const model::Attribute TemplateInput::kValueAttribute { model::Attribute::kValue, model::Attribute::Access::ReadOnly, data::DataType::kFloatingPoint };

auto TemplateInput::loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void
{
	// Get a reference that allows us to modify our own config attributes
    auto &&configAttributes = initializer[Class::instance().configHandle()];

	// Go through all the members of the JSON object that represents this object
	bool ioBatchLoaded = false;
	for (auto && [name, value] : jsonObject)
    {
		// TODO: custom configuration parameters
		if (name == u8"TODO"sv)
		{
			// TODO: parse the value correctly
			auto todo = value.asNumber<std::uint64_t>();

			// TODO: check that the value is valid
			if (!"TODO")
			{
				// TODO: use an error message that tells the user exactly what is wrong
				utils::json::decoder::throwWithLocation(value, std::runtime_error("TODO is wrong with TODO parameter of template input"));
			}

			// TODO: set the appropriate member variables, and update configAttributes accordingly (if necessary) 
		}
		// TODO: use a more descriptive keword, e.g. "poll"
		else if (name == u8"ioBatch"sv)
		{
			resolver.submit<TemplateIoBatch>(value, [this](std::reference_wrapper<TemplateIoBatch> ioBatch)
				{ 
					_ioBatch = &ioBatch.get();
					ioBatch.get().addInput(*this);
				});
			ioBatchLoaded = true;
		}
		else
		{
			// Pass any unknown parameters on to the fallback handler, which will load the built-in parameters ("id" and "uuid"),
			// and throw an exception if the key is unknown
            fallbackHandler(name, value);
		}
    }

	// TODO: perform consistency and completeness checks
	if (!"TODO")
	{
		// TODO: use an error message that tells the user exactly what is wrong
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("TODO is wrong with template input"));
	}
	if (!ioBatchLoaded)
	{
		// TODO: replace "I/O batch" and "template input" with more descriptive names
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("Missing I/O batch in template input"));
	}
}

auto TemplateInput::dataType() const -> const data::DataType &
{
	return kValueAttribute.dataType();
}

auto TemplateInput::directions() const -> io::Directions
{
	return io::Direction::Input;
}

auto TemplateInput::resolveAttribute(std::u16string_view name) -> const model::Attribute *
{
	// resolveAttribute() must not be called before references have been resolved, so the I/O batch should have been
	// set already.
	if (!_ioBatch)
	{
		throw std::logic_error("Internal error: xentara::plugins::templateDriver::TemplateInput::resolveAttribute() called before cross references have been resolved");
	}

	// Check all the attributes we support directly
	// TODO: add any additional attributes this class supports, including attributes inherited from the I/O component and the I/O batch
	if (auto attribute = model::Attribute::resolve(name,
		kValueAttribute))
	{
		return attribute;
	}

	// Check the read state attributes
	if (auto attribute = _state.resolveAttribute(name))
	{
		return attribute;
	}
	// Also check the common read state attributes from the I/O batch
	if (auto attribute = _ioBatch->resolveReadStateAttribute(name))
	{
		return attribute;
	}

	return nullptr;
}

auto TemplateInput::resolveEvent(std::u16string_view name) -> std::shared_ptr<process::Event>
{
	// resolveEvent() must not be called before references have been resolved, so the I/O batch should have been
	// set already.
	if (!_ioBatch)
	{
		throw std::logic_error("Internal error: xentara::plugins::templateDriver::TemplateInput::resolveEvent() called before cross references have been resolved");
	}

	// TODO: add any events this class supports directly

	// Check the read state events
	if (auto event = _state.resolveEvent(name, sharedFromThis()))
	{
		return event;
	}
	// Also check the common read state events from the I/O batch
	if (auto event = _ioBatch->resolveReadStateEvent(name))
	{
		return event;
	}

	return nullptr;
}

auto TemplateInput::readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle
{
	// readHandle() must not be called before references have been resolved, so the I/O batch should have been
	// set already.
	if (!_ioBatch)
	{
		// Don't throw an exception, because this function is noexcept
		return std::make_error_code(std::errc::invalid_argument);
	}
	// Get the data block
	const auto &dataBlock = _ioBatch->readDataBlock();
	
	// TODO: add any additional attributes this class supports
	if (attribute == kValueAttribute)
	{
		return _state.valueReadHandle(dataBlock);
	}
	
	// Check the read state attributes
	if (auto handle = _state.readHandle(dataBlock, attribute))
	{
		return *handle;
	}
	// Also check the common read state attributes from the I/O batch
	if (auto handle = _ioBatch->inputStateReadHandle(attribute))
	{
		return *handle;
	}

	// TODO: add any additional attributes inherited from the I/O component and the I/O batch

	return data::ReadHandle::Error::Unknown;
}

auto TemplateInput::attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	_state.attach(dataArray, eventCount);
}

auto TemplateInput::updateReadState(WriteSentinel &writeSentinel,
	std::chrono::system_clock::time_point timeStamp,
	const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError,
	const CommonReadState::Changes &batchChanges,
	PendingEventList &eventsToFire) -> void
{
	// Check if we have a valid payload
	if (const auto payload = payloadOrError.value())
	{
		// TODO: decode the value from the payload data
		double value = {};

		// Update the state
		_state.update(writeSentinel, timeStamp, value, batchChanges, eventsToFire);
	}
	// We have an error
	else
	{
		// Update the state with the error
		_state.update(writeSentinel, timeStamp, payloadOrError.error(), batchChanges, eventsToFire);
	}
}

} // namespace xentara::plugins::templateDriver