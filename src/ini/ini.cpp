/**
 * MIT License
 *
 * Copyright (c) 2020 Philip Klatt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#include "ini.h"

namespace utinni
{
const static IniConfig::Value utinniSettings[] = {

    // Launcher settings
    { "Launcher", "swgClientName", "", IniConfig::Value::vt_string },
    { "Launcher", "swgClientPath", "", IniConfig::Value::vt_string },

    // UtinniCore settings
    { "UtinniCore", "enableInternalUi", "true", IniConfig::Value::vt_bool },
    { "UtinniCore", "enableOfflineScenes", "false", IniConfig::Value::vt_bool },
    { "UtinniCore", "useSwgOverrideCfg", "false", IniConfig::Value::vt_bool },
    { "UtinniCore", "autoLoadScene", "false", IniConfig::Value::vt_bool },
    { "UtinniCore", "autoLogin", "false", IniConfig::Value::vt_bool },
    { "UtinniCore", "autoLoginUsername", "Local", IniConfig::Value::vt_string },
    { "UtinniCore", "plugins", DEFAULT_PLUGINS, IniConfig::Value::vt_string },

    // Log settings
    { "Log", "writeClassName", "false", IniConfig::Value::vt_bool },
    { "Log", "writeFunctionName", "false", IniConfig::Value::vt_bool },
};

IniConfig::IniConfig() { }

IniConfig::IniConfig(const std::string& filename)
{
    iniFilename = filename;
}

void IniConfig::load()
{
    if (!ini.Load(iniFilename))
    {
        // Save the default values to file
        for (const auto& iniValue : settings)
        {
            ini.GetSection(iniValue.sectionName)->SetValue(iniValue.valueName, iniValue.value);
        }

        save();
    }
    else
    {
        // Validate the loaded values to make sure all needed values exist, if not, create them
        bool missingValues = false;
        for (const auto& iniValue : settings)
        {
            if (ini.GetSection(iniValue.sectionName)->GetValue(iniValue.valueName).AsString().empty())
            {
                ini.GetSection(iniValue.sectionName)->SetValue(iniValue.valueName, iniValue.value);
                missingValues = true;
            }
        }

        if (missingValues)
        {
            save();
        }
    }
}

void IniConfig::load(const std::string& filename)
{
    iniFilename = filename;
    load();
}

void IniConfig::save() const
{
    // why is this not const? :/
    const_cast<INI::File&>(ini).Save(iniFilename);
}

void IniConfig::createUtinniSettings()
{
    for (const Value& value : utinniSettings)
    {
        settings.emplace_back(value);
    }
}

void IniConfig::addSetting(const Value& value)
{
    settings.emplace_back(value);
}

void IniConfig::addSetting(const char* sectionName, const char* valueName, const char* value, Value::Types type)
{
    Value val = { sectionName, valueName, value, type };
    settings.emplace_back(val);
}

void IniConfig::deleteSection(const char* sectionName)
{
    ini.DeleteSection(sectionName);
}

std::string IniConfig::getString(const char* sectionName, const char* valueName)
{
    return ini.GetSection(sectionName)->GetValue(valueName).AsString();
}

bool IniConfig::getBool(const char* sectionName, const char* valueName)
{
    return ini.GetSection(sectionName)->GetValue(valueName).AsBool();
}

int IniConfig::getInt(const char* sectionName, const char* valueName)
{
    return ini.GetSection(sectionName)->GetValue(valueName).AsInt();
}

float IniConfig::getFloat(const char* sectionName, const char* valueName)
{
    return (float) ini.GetSection(sectionName)->GetValue(valueName).AsDouble();
}

void IniConfig::setString(const char* sectionName, const char* valueName, const char* value)
{
    ini.GetSection(sectionName)->SetValue(valueName, value);
}

void IniConfig::setBool(const char* sectionName, const char* valueName, bool value)
{
    ini.GetSection(sectionName)->SetValue(valueName, value);
}

void IniConfig::setInt(const char* sectionName, const char* valueName, int value)
{
    ini.GetSection(sectionName)->SetValue(valueName, value);
}

void IniConfig::setFloat(const char* sectionName, const char* valueName, float value)
{
    ini.GetSection(sectionName)->SetValue(valueName, value);
}

}
