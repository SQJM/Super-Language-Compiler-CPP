module;
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
module Super.Util.CommandParsing;
import Super.Error;
import Super.Util.ConsoleStyle;

namespace Super::Util
{
	CommandParsing::CommandParsing(
		const std::vector<std::string>& args, const std::vector<std::pair<std::string, std::type_index>>& argvType, const std::string& appName)
		: _argvType(argvType),
		_commands(),
		_options(),
		_appName(appName)
	{
		auto result = ParseArguments(args);
		_commands = std::move(result.first);
		_options = std::move(result.second);

		SetCallback(
			"help",
			[&](const std::vector<std::string>& args, const std::map<std::string, std::string>& options)
			{
				PrintHelp();
				return 0;
			},
			"Display help information",
			"");
	}

	void CommandParsing::SetCallback(
		const std::string& name,
		std::function<int(const std::vector<std::string>&, const std::map<std::string, std::string>&)> fn,
		const std::string& helpText,
		const std::string& paramFormat)
	{
		if (name.empty() || !fn || helpText.empty())
			throw std::invalid_argument("Invalid argument");

		_callbacks.emplace_back(name, CallbackInfo(fn, helpText, paramFormat));
	}

	int CommandParsing::ExitCode()
	{
		for (const auto& arg : _commands)
		{
			try
			{
				return InvokeCallback(arg.first, arg.second, _options);
			}
			catch (const std::out_of_range& ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		}
		return -1;
	}

	std::pair<std::map<std::string, std::vector<std::string>>, std::map<std::string, std::string>>
		CommandParsing::ParseArguments(const std::vector<std::string>& args)
	{
		std::map<std::string, std::vector<std::string>> commands;
		std::map<std::string, std::string> options;

		for (size_t i = 0; i < args.size(); ++i)
		{
			if (args[i].starts_with("-"))
			{
				if (args[i].starts_with("--"))
				{
					std::string key = args[i].substr(2);
					std::string value = "";
					if (i + 1 < args.size() && !args[i + 1].starts_with("-"))
					{
						value = args[i + 1];
						++i;
					}
					options[key] = value;
				}
				else
				{
					std::string key = args[i].substr(1);
					std::vector<std::string> values;
					while (i + 1 < args.size() && !args[i + 1].starts_with("-"))
					{
						values.push_back(args[i + 1]);
						++i;
					}
					commands[key] = values;
				}
			}
			else
			{
				std::cerr << "Invalid argument: " << args[i] << std::endl;
			}
		}

		return { commands, options };
	}

	int CommandParsing::InvokeCallback(
		const std::string& name,
		const std::vector<std::string>& value,
		const std::map<std::string, std::string>& options)
	{
		for (const auto& [callbackName, callback] : _callbacks)
		{
			if (callbackName == name)
			{
				std::map<std::string, std::string> filteredOptions;
				if (!callback.paramFormat.empty())
				{
					size_t requiredCount = 0;
					struct OptionSpec
					{
						std::string key;
						std::vector<std::string> allowedValues;
					};
					std::vector<OptionSpec> optionSpecs;
					size_t pos = 0;
					while (true)
					{
						size_t start = callback.paramFormat.find('<', pos);
						if (start == std::string_view::npos)
							break;
						size_t end = callback.paramFormat.find('>', start);
						if (end == std::string_view::npos)
							break;

						std::string_view param = callback.paramFormat.substr(start + 1, end - start - 1);
						if (param.starts_with('-'))
						{
							size_t colon = param.find(':');
							OptionSpec spec;
							if (colon != std::string_view::npos)
							{
								spec.key = std::string(param.substr(1, colon - 1));
								std::string_view valuesStr = param.substr(colon + 1);
								std::string current;
								bool inQuotes = false;
								for (char c : valuesStr)
								{
									if (c == '"')
									{
										inQuotes = !inQuotes;
									}
									else if (c == '|' && !inQuotes)
									{
										if (!current.empty())
										{
											spec.allowedValues.push_back(current);
											current.clear();
										}
									}
									else
									{
										current += c;
									}
								}
								if (!current.empty())
								{
									spec.allowedValues.push_back(current);
								}
							}
							else
							{
								spec.key = std::string(param.substr(1));
							}
							optionSpecs.push_back(spec);
						}
						else
						{
							requiredCount++;
						}
						pos = end + 1;
					}

					if (value.size() < requiredCount)
					{
						SUPER_ERROR_THROW_MESSAGE(
							"Command '" + name + "' requires at least " + std::to_string(requiredCount) +
							" parameters, but only " + std::to_string(value.size()) + " were given. ");
					}

					for (const auto& spec : optionSpecs)
					{
						auto it = options.find(spec.key);
						if (it != options.end())
						{
							std::string inputValue = it->second;
							if (!inputValue.empty() && inputValue.front() == '"' && inputValue.back() == '"')
							{
								inputValue = inputValue.substr(1, inputValue.size() - 2);
							}

							if (!spec.allowedValues.empty())
							{
								bool valid = false;
								for (const auto& allowed : spec.allowedValues)
								{
									if (inputValue == allowed)
									{
										valid = true;
										break;
									}
								}
								if (!valid)
								{
									std::string allowedStr;
									for (size_t i = 0; i < spec.allowedValues.size(); ++i)
									{
										if (i > 0) allowedStr += "|";
										allowedStr += spec.allowedValues[i];
									}
									SUPER_ERROR_THROW_MESSAGE(
										"Option '-" + spec.key + "' must be one of: " + allowedStr +
										", but got: " + inputValue);
								}
							}
							filteredOptions[spec.key] = inputValue;
						}
					}
				}

				return callback.fn(value, filteredOptions);
			}
		}
		throw std::out_of_range(std::string("Callback with name '") + name + "' not found");
	}

	void CommandParsing::PrintHelp()
	{
		using namespace Super::Util::ConsoleStyle;
		std::cout << fg::cyan() << "Usage: " << reset() << _appName << fg::cyan() << " [command] [options]\n" << reset();
		std::cout << fg::cyan() << "Commands:\n" << reset();

		size_t maxNameLength = 0;
		for (const auto& [name, callback] : _callbacks)
		{
			maxNameLength = std::max(maxNameLength, name.size());
		}

		for (const auto& [name, callback] : _callbacks)
		{
			std::string padding(maxNameLength - name.size() + 2, ' ');
			std::cout << fg::yellow() << "  -" << reset() << bold() << name << reset() << padding << callback.helpText << std::endl;
			if (callback.paramFormat.size() > 0)
			{
				std::string params = callback.paramFormat;
				size_t spacePos = params.find(' ');
				if (spacePos != std::string::npos)
				{
					params = params.substr(spacePos + 1);
					while (true)
					{
						size_t start = params.find('<');
						if (start == std::string::npos)
						{
							break;
						}
						size_t end = params.find('>', start);
						if (end == std::string::npos)
						{
							break;
						}

						std::string param = params.substr(start, end - start + 1);
						std::string newParam;

						size_t colonPos = param.find(':');
						if (colonPos != std::string::npos)
						{
							std::string key = param.substr(1, colonPos - 1);
							if (key[0] == '-')
							{
								newParam = "[-" + key + ":" + param.substr(colonPos + 1, param.size() - colonPos - 2) + "]";
							}
							else
							{
								newParam = "[" + key + "]";
							}
						}

						params.replace(start, param.size(), newParam);
						start = params.find('<', start + newParam.size());
					}
					std::cout << "      " << fg::magenta() << params << reset() << std::endl;
				}
			}
		}
	}
} // namespace Super::Util