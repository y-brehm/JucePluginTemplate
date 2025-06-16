#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace WebUtils
{
    /**
     * @brief Reads all data from an InputStream into a std::vector<std::byte>.
     *
     * This is a general-purpose utility for converting JUCE streams into byte
     * vectors suitable for web resources.
     * @param stream The input stream to read from. The stream's position will be reset to 0.
     * @return A vector containing the bytes of the stream.
     */
    static inline std::vector<std::byte> streamToVector(juce::InputStream& stream)
    {
        std::vector<std::byte> result((size_t) stream.getTotalLength());
        stream.setPosition(0);
        [[maybe_unused]] const auto bytesRead = stream.read(result.data(), result.size());
        jassert(bytesRead == (int) result.size()); // Ensure we read the whole stream

        return result;
    }
}
