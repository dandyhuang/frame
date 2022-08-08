//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:55
//  @file:      export_variable.h
//  @author:    	
//  @brief:     
//
//********************************************************************

#ifndef COMMON_BASE_EXPORT_VARIABLE_H
#define COMMON_BASE_EXPORT_VARIABLE_H

#include <stddef.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

namespace common {

class ExportedVariable
{
    COMMON_DECLARE_UNCOPYABLE(ExportedVariable);
protected:
    ExportedVariable() {}
public:
    virtual ~ExportedVariable() {}
    virtual std::string ToString() const = 0;
};

namespace internal {

template <typename T>
class ExportedNormalVariable : public ExportedVariable
{
public:
    explicit ExportedNormalVariable(const T* ptr) : m_ptr(ptr) {}

    virtual std::string ToString() const // override
    {
        std::ostringstream os;
        os << *m_ptr;
        return os.str();
    }

private:
    const T* m_ptr;
};

template <typename T>
class ExportedFunctionVariable : public ExportedVariable
{
public:
    explicit ExportedFunctionVariable(T (*function)()) : m_function(function) {}

    virtual std::string ToString() const // override
    {
        std::ostringstream os;
        os << m_function();
        return os.str();
    }

private:
    T (*m_function)();
};

template <typename T, typename Class>
class ExportedConstMethodVariable : public ExportedVariable
{
    typedef T (Class::*MethodType)() const;

public:
    explicit ExportedConstMethodVariable(
        const Class* object,
        T (Class::*method)() const)
    :
        m_object(object), m_method(method)
    {
    }

    virtual std::string ToString() const // override
    {
        std::ostringstream os;
        os << (m_object->*m_method)();
        return os.str();
    }

private:
    const Class* m_object;
    MethodType m_method;
};

template <typename T, typename Class>
class ExportedNonConstMethodVariable : public ExportedVariable
{
    typedef T (Class::*MethodType)();

public:
    explicit ExportedNonConstMethodVariable(
        Class* object,
        T (Class::*method)())
    :
        m_object(object), m_method(method)
    {
    }

    virtual std::string ToString() const // override
    {
        std::ostringstream os;
        os << (m_object->*m_method)();
        return os.str();
    }

private:
    Class* m_object;
    MethodType m_method;
};

} // namespace internal

// Registry represent a group of exported variables
class VariableRegistry
{
    COMMON_DECLARE_UNCOPYABLE(VariableRegistry);
    friend class VariableRegisterer;
    friend class VariableExporter;

public:
    VariableRegistry();
    ~VariableRegistry();
    bool First(std::string* name, ExportedVariable** var);
    ExportedVariable* FindByName(const std::string& name);
    void Dump(std::ostream& os, // NOLINT(runtime/references)
              const std::string& output_format,
              const std::vector<std::string>* variables = NULL) const;
    std::string ToString() const;
    std::string ToStringAs(const std::string& format) const;

private:
    bool Register(const std::string& name, ExportedVariable* var);
    bool Unregister(const std::string& name);

private:
    mutable Mutex m_mutex;
    typedef std::map<std::string, ExportedVariable*> MapType;
    MapType m_variables;
};

// Static interface to access global registry.
class ExportedVariables
{
public:
    static bool GetFirst(std::string* name, ExportedVariable** var);
    static ExportedVariable* Find(const std::string& name);
    static void Dump(std::ostream& os, // NOLINT
                     const std::string& output_format,
                     const std::vector<std::string>* variables = NULL);
    static std::string ToString();
    static std::string ToStringAs(const std::string& format);
    static VariableRegistry* Registry();

private:
    ExportedVariables();
    ~ExportedVariables();

private:
    VariableRegistry m_registry;

    // 避免全局对象的析构函数中调用ExportedVariables的静态函数时导致coredump
    // 因为ExportedVariables::Registry有可能先于该全局对象析构
    bool m_is_alive;
};

// Register one variable when constructed, and unregister it automatically
// when destructed.
class VariableRegisterer
{
public:
    template <typename T>
    VariableRegisterer(const std::string& name,
                                const T* address,
                                VariableRegistry* registry = NULL)
        : m_registry(registry), m_name(name)
    {
        Register(new internal::ExportedNormalVariable<T>(address));
    }

    template <typename T>
    VariableRegisterer(const std::string& name,
                       T (*function)(),
                       VariableRegistry* registry = NULL)
        : m_registry(registry), m_name(name)
    {
        Register(new internal::ExportedFunctionVariable<T>(function));
    }

    template <typename T, typename Class>
    VariableRegisterer(const std::string& name,
                       const Class* object,
                       T (Class::*method)() const,
                       VariableRegistry* registry = NULL)
        : m_registry(registry), m_name(name)
    {
        Register(new internal::ExportedConstMethodVariable<T, Class>(object, method));
    }

    template <typename T, typename Class>
    VariableRegisterer(const std::string& name,
                       Class* object,
                       T (Class::*method)(),
                       VariableRegistry* registry = NULL)
        : m_registry(registry), m_name(name)
    {
        Register(new internal::ExportedNonConstMethodVariable<T, Class>(object, method));
    }

    ~VariableRegisterer();

private:
    VariableRegisterer(const VariableRegisterer&);
    VariableRegisterer& operator=(const VariableRegisterer&);
    VariableRegistry* Registry();
    void Register(ExportedVariable* var);

private:
    VariableRegistry* m_registry;
    const std::string m_name;
    ExportedVariable* m_variable;
};

// Deprecated misspelling name.
DEPRECATED_BY(VariableRegisterer) typedef VariableRegisterer VariableRegister;

// Export multiple variables as a group, and unregister all variables register
// by it automatically.
//
// It is useful when you want to export multiple variables but don't want to
// unregister them one by one.
//
class VariableExporter
{
public:
    // Export to default registry.
    VariableExporter();

    // Export to user specified registry.
    explicit VariableExporter(VariableRegistry* registry);

    ~VariableExporter();

    template <typename T>
    bool Export(const std::string& name, const T* address)
    {
        return Register(name, new internal::ExportedNormalVariable<T>(address));
    }

    template <typename T>
    bool Export(const std::string& name, T (*function)())
    {
        return Register(name, new internal::ExportedFunctionVariable<T>(function));
    }

    template <typename T, typename Class>
    bool Export(
        const std::string& name,
        const Class* object,
        T (Class::*method)() const)
    {
        return Register(name,
                        new internal::ExportedConstMethodVariable<T, Class>(object, method));
    }

    template <typename T, typename Class>
    bool Export(
        const std::string& name,
        Class* object,
        T (Class::*method)())
    {
        return Register(name,
                        new internal::ExportedNonConstMethodVariable<T, Class>(object, method));
    }

private:
    bool Register(const std::string& name, ExportedVariable* variable);
    VariableRegistry* Registry();

private:
    VariableRegistry* m_registry;
    std::map<std::string, ExportedVariable*> m_variables;
};

} // end of namespace common

// Register a variable to specified registry.
#define EXPORT_VARIABLE_TO_REGISTRY(registry, name, ...) \
namespace registered_variables { \
    VariableRegisterer g_##variable_##name##_register(#name, __VA_ARGS__, registry); \
}

// Register a variable to global registry.
#define EXPORT_VARIABLE(name, ...) EXPORT_VARIABLE_TO_REGISTRY(NULL, name, __VA_ARGS__)

#endif // COMMON_BASE_EXPORT_VARIABLE_H
