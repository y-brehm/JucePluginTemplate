#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <WebUIDataEmbed.h>


using namespace juce;
using Resource = juce::WebBrowserComponent::Resource;

namespace
{
    constexpr auto LOCAL_DEV_SERVER_ADDRESS = "http://127.0.0.1:8080";

    static const std::unordered_map<String, const char*> mimeMap = {{{"htm"}, "text/html"},
                                                                    {{"html"}, "text/html"},
                                                                    {{"txt"}, "text/plain"},
                                                                    {{"jpg"}, "image/jpeg"},
                                                                    {{"jpeg"}, "image/jpeg"},
                                                                    {{"svg"}, "image/svg+xml"},
                                                                    {{"ico"}, "image/vnd.microsoft.icon"},
                                                                    {{"json"}, "application/json"},
                                                                    {{"png"}, "image/png"},
                                                                    {{"css"}, "text/css"},
                                                                    {{"map"}, "application/json"},
                                                                    {{"js"}, "text/javascript"},
                                                                    {{"woff2"}, "font/woff2"}};

    static inline const char* getMimeForExtension(const String& extension)
    {
        if (const auto it = mimeMap.find(extension.toLowerCase()); it != mimeMap.end())
            return it->second;

        jassertfalse;
        return "";
    }

    static inline auto streamToVector(InputStream& stream)
    {
        std::vector<std::byte> result((size_t) stream.getTotalLength());
        stream.setPosition(0);
        [[maybe_unused]] const auto bytesRead = stream.read(result.data(), result.size());
        jassert(bytesRead == (ssize_t) result.size());

        return result;
    }

    static inline std::vector<std::byte> getWebViewFileAsBytes(const juce::String& filepath)
    {
        juce::MemoryInputStream zipStream {BinaryData::WebUI_zip, BinaryData::WebUI_zipSize, false};
        juce::ZipFile zipFile {zipStream};

        const auto* zipEntry = zipFile.getEntry(filepath);

        if (zipEntry == nullptr)
        {
            juce::Logger::writeToLog("Resource Provider Internal: FAILED to find file: '" + filepath + "'");
            return {};
        }

        const std::unique_ptr<juce::InputStream> entryStream {zipFile.createStreamForEntry(*zipEntry)};
        if (entryStream == nullptr)
        {
            juce::Logger::writeToLog("Resource Provider Internal: FAILED to create stream for file: '" + filepath +
                                     "'");
            return {};
        }

        return streamToVector(*entryStream);
    }
}

static inline std::optional<Resource> getWebResource(const juce::String& url)
{
    const auto resourceToRetrieve = url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);

    const auto resource = getWebViewFileAsBytes(resourceToRetrieve);
    if (!resource.empty())
    {
        const auto extension = resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
        return Resource {std::move(resource), getMimeForExtension(extension)};
    }

    juce::Logger::writeToLog("Resource not found: " + resourceToRetrieve);

    return std::nullopt;
}

