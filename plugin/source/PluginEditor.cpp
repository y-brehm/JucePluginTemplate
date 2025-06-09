#include "PluginEditor.h"
#include "WebResourceHelper.h"
#include "ParameterIDs.h"

#include <juce_core/juce_core.h>


JucePluginTemplateAudioProcessorEditor::JucePluginTemplateAudioProcessorEditor (JucePluginTemplateAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      gainRelay{GAIN.getParamID()},
      bypassRelay{BYPASS.getParamID()},
      webBrowserComponent(juce::WebBrowserComponent::Options{}
                              .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
                              .withResourceProvider(
                                  [](const auto& url)
                                  {
                                      return getWebResource(url);
                                  },
                                  juce::URL {LOCAL_DEV_SERVER_ADDRESS}.getOrigin())
                              .withNativeIntegrationEnabled() // Crucial for window.JUCE object and .withNativeFunction
                              .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{})
                              .withOptionsFrom(gainRelay)
                              .withOptionsFrom(bypassRelay)
                             ),
      gainSliderAttachment{
          *processorRef.getValueTreeState().getParameter(GAIN.getParamID()),
          gainRelay,
          nullptr},
      bypassAttachment{
          *processorRef.getValueTreeState().getParameter(BYPASS.getParamID()),
          bypassRelay,
          nullptr}
{

    addAndMakeVisible(webBrowserComponent);
    setSize (600, 400);

    webBrowserComponent.goToURL(webBrowserComponent.getResourceProviderRoot());
    // webBrowserComponent.goToURL("https://www.juce.com"); // toString(true) for proper URL encoding
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
    // (Our component is covered by the web browser, but good to have a background)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JucePluginTemplateAudioProcessorEditor::resized()
{
    // This is called when the MainContentComponentis resized.
    // If you add any child components, this is where you'd
    // update their positions.
    webBrowserComponent.setBounds (getLocalBounds());
}
