//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 14:05
//  @file:      export_variable.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/export_variable.h"
#include "thirdparty/jsoncpp/value.h"
#include "thirdparty/jsoncpp/writer.h"

/////////////////////////////////////////////////////////////////////////////
// VariableRegistry staff

namespace common {

VariableRegistry::VariableRegistry() {}

VariableRegistry::~VariableRegistry() {}

bool VariableRegistry::First(std::string* name, ExportedVariable** var)
{
    MutexLocker locker(&m_mutex);
    if (!m_variables.empty())
    {
        MapType::iterator iter = m_variables.begin();
        *name = iter->first;
        *var = iter->second;
        return true;
    }
    return false;
}

ExportedVariable* VariableRegistry::FindByName(const std::string& name)
{
    MutexLocker locker(&m_mutex);
    MapType::const_iterator iter = m_variables.find(name);
    if (iter != m_variables.end())
    {
        return iter->second;
    }
    return NULL;
}

bool VariableRegistry::Register(const std::string& name, ExportedVariable* var)
{
    MutexLocker locker(&m_mutex);
    return m_variables.insert(std::make_pair(name, var)).second;
}

bool VariableRegistry::Unregister(const std::string& name)
{
    MutexLocker locker(&m_mutex);
    return m_variables.erase(name) == 1U;
}

void VariableRegistry::Dump(std::ostream& os, // NOLINT(runtime/references)
                            const std::string& output_format,
                            const std::vector<std::string>* variables) const
{
    MutexLocker locker(&m_mutex);
    if (output_format == "text") {
        if (variables == NULL || variables->empty()) {
            for (MapType::const_iterator i = m_variables.begin();
                 i != m_variables.end(); ++i) {
                os << i->first << " = " << i->second->ToString() << '\n';
            }
        } else {
            for (std::vector<std::string>::const_iterator j = variables->begin();
                 j != variables->end(); ++j) {
                MapType::const_iterator i = m_variables.find(*j);
                if (i == m_variables.end()) {
                    os << *j << " = " << " " << '\n';
                } else {
                    os << i->first << " = " << i->second->ToString() << '\n';
                }
            }
        }
    } else if (output_format == "json") {
        Json::Value value;
        if (variables == NULL || variables->empty()) {
            for (MapType::const_iterator i = m_variables.begin();
                 i != m_variables.end(); ++i) {
                value[i->first] = i->second->ToString();
            }
        } else {
            for (std::vector<std::string>::const_iterator j = variables->begin();
                 j != variables->end(); ++j) {
                MapType::const_iterator i = m_variables.find(*j);
                if (i == m_variables.end()) {
                    value[*j] = " ";
                } else {
                    value[i->first] = i->second->ToString();
                }
            }
        }

        Json::FastWriter fast_writer;
        os << fast_writer.write(value);
    }
}

std::string VariableRegistry::ToStringAs(const std::string& format) const
{
    std::ostringstream os;
    Dump(os, format);
    return os.str();
}

std::string VariableRegistry::ToString() const
{
    return ToStringAs("text");
}

/////////////////////////////////////////////////////////////////////////////
// VariableRegisterer staff

VariableRegisterer::~VariableRegisterer()
{
    if (m_variable)
    {
        VariableRegistry* registry = Registry();
        if (registry)
            registry->Unregister(m_name);
        delete m_variable;
    }
}

VariableRegistry* VariableRegisterer::Registry()
{
    return m_registry ? m_registry : ExportedVariables::Registry();
}

void VariableRegisterer::Register(ExportedVariable* var)
{
    VariableRegistry* registry = Registry();
    if (!registry || !registry->Register(m_name, var))
    {
        delete var;
        m_variable = NULL;
        return;
    }

    m_variable = var;
}

/////////////////////////////////////////////////////////////////////////////
// ExportedVariables staff

ExportedVariables::ExportedVariables()
{
    m_is_alive = true;
}

ExportedVariables::~ExportedVariables()
{
    m_is_alive = false;
}

bool ExportedVariables::GetFirst(std::string* name, ExportedVariable** var)
{
    VariableRegistry* registry = Registry();
    if (!registry) return false;

    return registry->First(name, var);
}

ExportedVariable* ExportedVariables::Find(const std::string& name)
{
    VariableRegistry* registry = Registry();
    if (!registry) return NULL;
    return registry->FindByName(name);
}

void ExportedVariables::Dump(std::ostream& os, // NOLINT(runtime/references)
                             const std::string& output_format = "text",
                             const std::vector<std::string>* variables)
{
    VariableRegistry* registry = Registry();
    if (!registry) return;
    return registry->Dump(os, output_format, variables);
}

std::string ExportedVariables::ToString()
{
    VariableRegistry* registry = Registry();
    if (!registry) return "";
    return registry->ToString();
}

std::string ExportedVariables::ToStringAs(const std::string& format)
{
    VariableRegistry* registry = Registry();
    if (!registry) return "";
    return registry->ToStringAs(format);
}

VariableRegistry* ExportedVariables::Registry()
{
    static ExportedVariables instance;
    if (instance.m_is_alive) {
        return &instance.m_registry;
    } else {
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
// VariableExporter staff

VariableExporter::VariableExporter() : m_registry(NULL) {}

VariableExporter::VariableExporter(VariableRegistry* registry) :
    m_registry(registry) {}

VariableExporter::~VariableExporter()
{
    VariableRegistry* registry = Registry();
    std::map<std::string, ExportedVariable*>::iterator i;
    for (i = m_variables.begin(); i != m_variables.end(); ++i)
    {
        if (registry)
            registry->Unregister(i->first);
        delete i->second;
    }
    m_variables.clear();
}

bool VariableExporter::Register(const std::string& name, ExportedVariable* variable)
{
    if (!Registry()->Register(name, variable))
    {
        delete variable;
        return false;
    }
    m_variables[name] = variable;
    return true;
}

VariableRegistry* VariableExporter::Registry()
{
    return m_registry ? m_registry : ExportedVariables::Registry();
}

} // end of namespace common
