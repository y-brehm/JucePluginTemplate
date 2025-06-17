#include "PluginEditor.h"
#include "WebResourceHelper.h"
#include "ParameterIDs.h"

#include <juce_core/juce_core.h>


JucePluginTemplateAudioProcessorEditor::JucePluginTemplateAudioProcessorEditor (JucePluginTemplateAudioProcessor& p)
    : AudioProcessorEditor (&p),
      _processorRef (p),
      _gainRelay{GAIN.getParamID()},
      _bypassRelay{BYPASS.getParamID()},
      _clippingRelay{CLIPPING.getParamID()},
      _webBrowserComponent(juce::WebBrowserComponent::Options{}
                              .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
                              .withWinWebView2Options(
                                  juce::WebBrowserComponent::Options::WinWebView2{}
                                      .withBackgroundColour(juce::Colours::white)
                                      // this may be necessary for some DAWs; include for safety
                                      .withUserDataFolder(juce::File::getSpecialLocation(
                                          juce::File::SpecialLocationType::tempDirectory)))
                              .withNativeIntegrationEnabled() // Crucial for window.JUCE object and .withNativeFunction
                              .withResourceProvider(
                                  [this](const auto& url)
                                  {
                                      return getWebResource(url, _dynamicResourceProvider);
                                  },
                                  juce::URL {LOCAL_DEV_SERVER_ADDRESS}.getOrigin())
                              .withOptionsFrom(_gainRelay)
                              .withOptionsFrom(_bypassRelay)
                              .withOptionsFrom(_clippingRelay)
                             ),
      _gainSliderAttachment{
          *_processorRef.getValueTreeState().getParameter(GAIN.getParamID()),
          _gainRelay,
          nullptr},
      _bypassAttachment{
          *_processorRef.getValueTreeState().getParameter(BYPASS.getParamID()),
          _bypassRelay,
          nullptr},
      _clippingAttachment{
          *_processorRef.getValueTreeState().getParameter(CLIPPING.getParamID()),
          _clippingRelay,
          nullptr}
{
    registerDynamicEndpoints();

    addAndMakeVisible(_webBrowserComponent);
    setSize (600, 400);

    _webBrowserComponent.goToURL(_webBrowserComponent.getResourceProviderRoot());
    // webBrowserComponent.goToURL("https://www.google.com"); // toString(true) for proper URL encoding
    
    startTimer(60);
}

void JucePluginTemplateAudioProcessorEditor::registerDynamicEndpoints()
{
    _dynamicResourceProvider.registerHandler(
        "meterLevels.json",
        [this]
        {
            juce::DynamicObject::Ptr data{new juce::DynamicObject{}};
            data->setProperty("input", _processorRef.getMonoInputPeakLevelDb());
            data->setProperty("output", _processorRef.getMonoOutputPeakLevelDb());
            return DynamicResourceProvider::createJsonResource(data.get());
        });
}

JucePluginTemplateAudioProcessorEditor::~JucePluginTemplateAudioProcessorEditor()
{
    // It's good practice to clear the resource provider if it holds onto resources
    // or if the WebBrowserComponent might outlive captures in the lambda, though
    // with a static function like getLocalResource, this is less critical here.
    // webBrowserComponent.setResourceProvider (nullptr, {});
}

//==============================================================================
void JucePluginTemplateAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JucePluginTemplateAudioProcessorEditor::resized()
{
    _webBrowserComponent.setBounds (getLocalBounds());
}

void JucePluginTemplateAudioProcessorEditor::timerCallback()
{
    _webBrowserComponent.emitEventIfBrowserIsVisible("meterLevels", juce::var{});
}
