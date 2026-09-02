// Head Cursor: reading the optional modules.headCursor block, and choosing the
// settings that apply to one running application (its own profile, or the
// defaults when nothing matches it).
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "depthxr/config_parser.h"
#include "depthxr/settings_resolver.h"

namespace {

void Expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

bool Close(double lhs, double rhs) { return std::abs(lhs - rhs) < 1e-9; }

// A config whose only module is Head Cursor (the applications list is empty).
depthxr::ParseResult ParseHeadCursor(std::string_view module_json) {
    return depthxr::ParseConfig(std::string(R"({"version":3,"core":{"enabled":true},"applications":[],"modules":{"depthxr":{"enabled":false},"pivotxr":{"enabled":false},"headCursor":)") +
                                std::string(module_json) + "}}");
}

// The same, with one registered application (DCS) that profiles can name.
depthxr::ParseResult ParseHeadCursorForDcs(std::string_view module_json) {
    return depthxr::ParseConfig(std::string(R"({"version":3,"core":{"enabled":true},"applications":[{"id":"dcs","name":"DCS World","enabled":true,"match":{"exe":"DCS.exe"}}],"modules":{"depthxr":{"enabled":false},"pivotxr":{"enabled":false},"headCursor":)") +
                                std::string(module_json) + "}}");
}

// Two profiles naming DCS: the first enabled one wins, whatever the second says.
constexpr const char* kTwoProfiles = R"({
  "enabled": false,
  "defaults": {"yawSensitivity": 1},
  "profiles": [
    {"name": "Loose", "enabled": true, "applicationIds": ["dcs"],
     "settings": {"yawSensitivity": 4.5, "yawMultiplier": 6, "maxMovePerFrame": 400, "invertedToggle": true,
                  "toggleBinding": {"type": "keyboard", "chord": ["Alt", "F9"]}}},
    {"name": "Tight", "enabled": true, "applicationIds": ["dcs"], "settings": {"yawSensitivity": 9}}
  ]
})";

void TestConfigsWithoutHeadCursor() {
    const depthxr::ParseResult parsed = depthxr::ParseConfig(
        R"({"version":3,"core":{"enabled":true},"applications":[],"modules":{"depthxr":{"enabled":false},"pivotxr":{"enabled":false}}})");
    Expect(parsed.ok, "a config written before Head Cursor parses: " + parsed.error);
    Expect(!parsed.document.head_cursor.enabled, "such a config keeps Head Cursor off");

    const auto resolved = depthxr::ResolveHeadCursorSettings(parsed.document, "DCS.exe");
    Expect(!resolved.enabled, "Head Cursor does not switch itself on for an application nobody configured");
    Expect(Close(resolved.yaw_sensitivity, 1.0) && Close(resolved.pitch_sensitivity, 1.0) &&
               Close(resolved.yaw_multiplier, 2.0) && Close(resolved.pitch_multiplier, 2.0) &&
               Close(resolved.deadzone_degrees, 0.5) && resolved.max_move_per_frame == 200,
           "unconfigured Head Cursor uses the shipped defaults");
    Expect(resolved.toggle_binding.type == depthxr::InputBindingType::None, "and no toggle binding");
}

void TestSettingsRoundTrip() {
    const depthxr::ParseResult parsed = ParseHeadCursor(R"({
      "enabled": true,
      "defaults": {"yawSensitivity": 3.5, "pitchSensitivity": 2.25, "yawMultiplier": 6, "pitchMultiplier": 3,
                   "deadzoneDegrees": 1.5, "maxMovePerFrame": 400, "invertedToggle": true,
                   "toggleBinding": {"type": "keyboard", "chord": ["Ctrl", "F12"]}}
    })");
    Expect(parsed.ok, "a full Head Cursor block parses: " + parsed.error);

    const auto& defaults = parsed.document.head_cursor.defaults;
    Expect(parsed.document.head_cursor.enabled, "the module's own switch is read");
    Expect(Close(defaults.yaw_sensitivity, 3.5) && Close(defaults.pitch_sensitivity, 2.25) &&
               Close(defaults.yaw_multiplier, 6.0) && Close(defaults.pitch_multiplier, 3.0) &&
               Close(defaults.deadzone_degrees, 1.5) && defaults.max_move_per_frame == 400,
           "every tuning value survives parsing");
    Expect(defaults.inverted_toggle, "invertedToggle survives parsing");
    Expect(defaults.toggle_binding.type == depthxr::InputBindingType::Keyboard &&
               defaults.toggle_binding.chord.size() == 2 && defaults.toggle_binding.chord[1] == "F12",
           "the toggle binding keeps its chord");

    const auto resolved = depthxr::ResolveHeadCursorSettings(parsed.document, "AnyGame.exe");
    Expect(resolved.enabled, "with the module on, an unlisted application still gets the cursor");
    Expect(Close(resolved.yaw_sensitivity, 3.5), "from the defaults");
}

void TestNumbersAreKeptNotClamped() {
    // The app's sliders and the JSON schema carry Head Cursor's bounds; the layer
    // only checks the types, so what it reads is what it moves the cursor with.
    const depthxr::ParseResult parsed = ParseHeadCursor(
        R"({"defaults":{"yawSensitivity":10,"pitchSensitivity":10,"yawMultiplier":10,"pitchMultiplier":10,"deadzoneDegrees":0,"maxMovePerFrame":500}})");
    Expect(parsed.ok, "the widest values the app writes parse: " + parsed.error);
    const auto& defaults = parsed.document.head_cursor.defaults;
    Expect(Close(defaults.yaw_sensitivity, 10.0) && Close(defaults.pitch_sensitivity, 10.0),
           "the top of the widened sensitivity range survives");
    Expect(Close(defaults.deadzone_degrees, 0.0) && defaults.max_move_per_frame == 500,
           "and the ends of the other two ranges");
}

void TestWrongTypesFail() {
    struct Case { const char* json; const char* error; };
    const Case cases[] = {
        {R"({"enabled":1})", "headCursor.enabled must be a boolean"},
        {R"({"defaults":{"yawSensitivity":"fast"}})", "headCursor.yawSensitivity must be a number"},
        {R"({"defaults":{"pitchSensitivity":"2"}})", "headCursor.pitchSensitivity must be a number"},
        {R"({"defaults":{"maxMovePerFrame":{}}})", "headCursor.maxMovePerFrame must be a number"},
        {R"({"defaults":{"invertedToggle":"yes"}})", "headCursor.invertedToggle must be a boolean"},
        {R"({"defaults":{"toggleBinding":[]}})", "headCursor.toggleBinding must be an object"},
    };
    for (const Case& case_ : cases) {
        const depthxr::ParseResult parsed = ParseHeadCursor(case_.json);
        Expect(!parsed.ok, std::string("a wrong type is refused: ") + case_.json + " (ok=" + std::to_string(parsed.ok) +
                                      ", error=" + parsed.error + ", warnings=" + std::to_string(parsed.warnings.size()) + ")");
        Expect(parsed.error.find(case_.error) != std::string::npos,
               std::string("the error names the field (wanted \"") + case_.error + "\", got: " + parsed.error);
    }
}

void TestUnknownKeysAreWarnings() {
    const depthxr::ParseResult parsed = ParseHeadCursor(R"({"defaults":{"deadzone":0.4,"smoothFactor":0.2}})");
    Expect(parsed.ok, "a key this layer does not know does not break the config: " + parsed.error);
    std::string warnings;
    for (const std::string& warning : parsed.warnings) warnings += warning + '\n';
    Expect(warnings.find("smoothFactor") != std::string::npos,
           "and it is reported once by name: " + warnings);
}

void TestProfileWinsForItsApplication() {
    const depthxr::ParseResult parsed = ParseHeadCursorForDcs(kTwoProfiles);
    Expect(parsed.ok, "two Head Cursor profiles parse: " + parsed.error);

    const auto dcs = depthxr::ResolveHeadCursorSettings(parsed.document, "DCS.exe");
    Expect(dcs.enabled, "an enabled profile for the running application turns Head Cursor on");
    Expect(Close(dcs.yaw_sensitivity, 4.5) && Close(dcs.yaw_multiplier, 6.0) && dcs.max_move_per_frame == 400,
           "with that profile's tuning");
    Expect(dcs.inverted_toggle && dcs.toggle_binding.chord.size() == 2 && dcs.toggle_binding.chord[1] == "F9",
           "and its toggle binding");

    const auto other = depthxr::ResolveHeadCursorSettings(parsed.document, "FlightSimulator2024.exe");
    Expect(!other.enabled && Close(other.yaw_sensitivity, 1.0), "another application keeps the module's off state and defaults");
}

void TestApplicationMatchIgnoresCase() {
    const depthxr::ParseResult parsed = ParseHeadCursorForDcs(kTwoProfiles);
    Expect(parsed.ok, "the profile config parses: " + parsed.error);
    Expect(Close(depthxr::ResolveHeadCursorSettings(parsed.document, "dcs.exe").yaw_sensitivity, 4.5),
           "the application is found whatever case the game reports its exe in");
}

void TestDisabledProfilesAreSkipped() {
    const depthxr::ParseResult parsed = ParseHeadCursorForDcs(R"({
      "enabled": false,
      "defaults": {"yawSensitivity": 1},
      "profiles": [
        {"name": "Loose", "enabled": false, "applicationIds": ["dcs"], "settings": {"yawSensitivity": 4.5}},
        {"name": "Tight", "enabled": true, "applicationIds": ["dcs"], "settings": {"yawSensitivity": 9}}
      ]
    })");
    Expect(parsed.ok, "a disabled profile parses: " + parsed.error);
    const auto dcs = depthxr::ResolveHeadCursorSettings(parsed.document, "DCS.exe");
    Expect(dcs.enabled && Close(dcs.yaw_sensitivity, 9.0), "a disabled profile does not shadow the enabled one behind it");

    const depthxr::ParseResult all_off = ParseHeadCursorForDcs(R"({
      "enabled": false,
      "defaults": {"yawSensitivity": 1},
      "profiles": [{"name": "Loose", "enabled": false, "applicationIds": ["dcs"], "settings": {"yawSensitivity": 4.5}}]
    })");
    Expect(all_off.ok, "profiles that are all off parse: " + all_off.error);
    const auto quiet = depthxr::ResolveHeadCursorSettings(all_off.document, "DCS.exe");
    Expect(!quiet.enabled && Close(quiet.yaw_sensitivity, 1.0), "and none of them turns the cursor on");
}

void TestProfileForAnUnregisteredApplicationIsIgnored() {
    const depthxr::ParseResult parsed = ParseHeadCursorForDcs(R"({
      "enabled": false,
      "defaults": {"yawSensitivity": 1},
      "profiles": [{"name": "MSFS", "enabled": true, "applicationIds": ["msfs2024"], "settings": {"yawSensitivity": 7}}]
    })");
    Expect(parsed.ok, "a profile for another application's id parses: " + parsed.error);
    const auto dcs = depthxr::ResolveHeadCursorSettings(parsed.document, "DCS.exe");
    Expect(!dcs.enabled && Close(dcs.yaw_sensitivity, 1.0), "it changes nothing for an application that is not registered");
}

void TestProfileErrors() {
    struct Case { const char* json; const char* error; };
    const Case cases[] = {
        {R"({"profiles":[{"name":5}]})", "headCursor profile name must be a string"},
        {R"({"profiles":[{"enabled":"yes"}]})", "headCursor profile enabled must be a boolean"},
        {R"({"profiles":[{"applicationIds":"dcs"}]})", "headCursor profile applicationIds"},
        {R"({"profiles":[{"settings":[]}]})", "headCursor profile settings must be an object"},
    };
    for (const Case& case_ : cases) {
        const depthxr::ParseResult parsed = ParseHeadCursor(case_.json);
        Expect(!parsed.ok, std::string("a broken profile is refused: ") + case_.json);
        Expect(parsed.error.find(case_.error) != std::string::npos,
               std::string("the error names the field (wanted \"") + case_.error + "\", got: " + parsed.error);
    }
}

} // namespace

int main() {
    TestConfigsWithoutHeadCursor();
    TestSettingsRoundTrip();
    TestNumbersAreKeptNotClamped();
    TestWrongTypesFail();
    TestUnknownKeysAreWarnings();
    TestProfileWinsForItsApplication();
    TestApplicationMatchIgnoresCase();
    TestDisabledProfilesAreSkipped();
    TestProfileForAnUnregisteredApplicationIsIgnored();
    TestProfileErrors();
    std::cout << "head_cursor_tests passed\n";
    return 0;
}
