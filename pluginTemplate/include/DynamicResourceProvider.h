#pragma once

#include "WebUtils.h"
#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <optional>


/**
 * @class DynamicResourceProvider
 * @brief Manages and serves dynamically generated resources for the Web UI.
 *
 * This class acts as a router for data requests from the frontend that are not
 * static files (e.g., parameter values, metering data). It maintains a map of
 * resource names to handler functions.
 */
class DynamicResourceProvider
{
public:
    using Resource = juce::WebBrowserComponent::Resource;
    using Handler = std::function<std::optional<Resource>()>;

    /**
     * @brief Registers a handler function for a specific resource name.
     * @param resourceName The name of the resource, e.g., "outputLevel.json".
     * @param handler A function that returns a juce::WebBrowserComponent::Resource.
     */
    void registerHandler(const juce::String& resourceName, Handler handler)
    {
        handlers[resourceName] = std::move(handler);
    }

    /**
     * @brief Attempts to handle a request for a dynamic resource.
     * @param resourceName The name of the resource requested by the frontend.
     * @return An optional juce::WebBrowserComponent::Resource. If a handler is
     * found for the given name, it returns the resource; otherwise, std::nullopt.
     */
    std::optional<Resource> handleRequest(const juce::String& resourceName) const
    {
        if (const auto it = handlers.find(resourceName); it != handlers.end())
            return it->second();

        return std::nullopt;
    }

    /**
     * @brief A factory method to create a JSON resource from a juce::var.
     *
     * This is a utility to standardize JSON creation, ensuring consistency
     * and reducing code duplication in handler lambdas.
     *
     * @param data The juce::var containing the data to be serialized.
     * @return A juce::WebBrowserComponent::Resource containing the JSON data.
     */
    static Resource createJsonResource(const juce::var& data)
    {
        const auto jsonString = juce::JSON::toString(data);
        juce::MemoryInputStream stream{jsonString.getCharPointer(), jsonString.getNumBytesAsUTF8(), false};
        return {WebUtils::streamToVector(stream), "application/json"};
    }

private:
    std::unordered_map<juce::String, Handler> handlers;
};
