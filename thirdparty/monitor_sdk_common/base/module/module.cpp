//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 11:48
//  @file:      module.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"

#include <string.h>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "thirdparty/monitor_sdk_common/base/binary_version.h"
#include "thirdparty/monitor_sdk_common/base/module/dependency_map.h"
#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/glog/raw_logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

namespace internal {

class ModuleManagerImpl : public Singleton<ModuleManagerImpl>
{
    struct Module
    {
        const char* filename; // for diagnose
        int line;             // for diagnose
        const char* name;
        ModuleCtor ctor;
        ModuleDtor dtor;
        bool initialized;
    public:
        Module():
            filename(NULL),
            line(0),
            name(NULL),
            ctor(NULL),
            dtor(NULL),
            initialized(false)
        {
        }
    };

public:
    ModuleManagerImpl() : m_initialized(false) {}

    void RegisterModuleCtor(
        const char* filename, int line,
        const char* name,
        ModuleCtor ctor
    );
    void RegisterModuleDtor(
        const char* filename, int line,
        const char* name,
        ModuleDtor dtor
    );
    void RegisterDependency(const char* this_module, const char* dep_name);
    void InitializeAll(
        int* argc,
        char*** argv,
        bool remove_flags
    );

private:
    void SortModules(std::vector<Module*>* sorted_modules);

private:
    typedef std::map<std::string, Module> ModuleMap;
    ModuleMap m_modules;
    DependencyMap m_depends;
    bool m_initialized; // Allow initialize repeatly for shared libraries
};

void ModuleManagerImpl::RegisterModuleCtor(
    const char* filename,
    int line,
    const char* name,
    ModuleCtor ctor
    )
{
    Module& module = m_modules[name];
    if (module.ctor != NULL)
    {
        RAW_LOG(FATAL, "%s:%d: Duplicated module definition %s: the previous is in %s:%d",
                filename, line, name, module.filename, module.line);
    }

    module.filename = filename;
    module.line = line;
    module.name = name;
    module.ctor = ctor;

    // make sure this module is registered into the depends
    m_depends.insert(std::make_pair(name, std::set<std::string>()));
}

void ModuleManagerImpl::RegisterModuleDtor(
    const char* filename,
    int line,
    const char* name,
    ModuleDtor dtor
    )
{
    Module& module = m_modules[name];
    if (module.dtor != NULL)
    {
        RAW_LOG(FATAL, "%s:%d Duplicated module dtor registered for %s: the previous is %s:%d",
                filename, line, name, module.filename, module.line);
    }
    CHECK_STREQ(module.name, name);
    module.dtor = dtor;
}

void ModuleManagerImpl::RegisterDependency(const char* this_module, const char* dep_name)
{
    // register this module to dependency module
    m_depends[this_module].insert(dep_name);
}

void ModuleManagerImpl::SortModules(std::vector<Module*>* sorted_modules)
{
    std::vector<std::string> sorted_names;
    if (!TopologicalSort(m_depends, &sorted_names))
    {
        // Dump all dependencies
        std::ostringstream oss;
        oss << "Cyclic dependency detected:\n";
        for (DependencyMap::const_iterator i = m_depends.begin();
             i != m_depends.end(); ++i)
        {
            // exclude acyclic module
            if (std::find(sorted_names.begin(), sorted_names.end(), i->first) !=
                sorted_names.end())
            {
                continue;
            }

            oss << i->first << ":";
            const std::set<std::string>& deps = i->second;
            for (std::set<std::string>::const_iterator j = deps.begin();
                 j != deps.end(); ++j)
            {
                oss << " " << *j;
            }
            oss << "\n";
        }
        LOG(FATAL) << oss.str();
    }

    std::reverse(sorted_names.begin(), sorted_names.end());

    for (size_t i = 0; i < sorted_names.size(); ++i)
    {
        const std::string& module_name = sorted_names[i];
        ModuleMap::iterator it = m_modules.find(module_name);
        CHECK(it != m_modules.end()) << "Unknown module name: " << module_name
            << ", did you forget to link corresponding library?";
        sorted_modules->push_back(&it->second);
    }
}

static std::string MakeVersionInfo()
{
    using namespace binary_version;

    std::ostringstream oss;
    oss << "\n"; // Open a new line in gflags --version output.

    if (kSvnInfoCount > 0)
    {
        oss << "Sources:\n"
            << "----------------------------------------------------------\n";
        for (int i = 0; i < kSvnInfoCount; ++i)
            oss << kSvnInfo[i];
        oss << "----------------------------------------------------------\n";
    }

    oss << "BuildTime: " << kBuildTime << "\n"
        << "BuildType: " << kBuildType << "\n"
        << "BuilderName: " << kBuilderName << "\n"
        << "HostName: " << kHostName << "\n"
        << "Compiler: " << kCompiler << "\n";

    return oss.str();
}

void ModuleManagerImpl::InitializeAll(
    int* argc,
    char*** argv,
    bool remove_flags)
{
    if (!m_initialized)
    {
#ifndef _WIN32
        // don't support on win32
        google::InstallFailureSignalHandler();
#endif
        google::SetVersionString(MakeVersionInfo());
    }

    // Allow command line to be parsed again to initialize extra flags load
    // from shared library.
    google::ParseCommandLineFlags(argc, argv, remove_flags);

    if (!m_initialized)
    {
        google::InitGoogleLogging((*argv)[0]);

        LOG(INFO) << "Program initializing ...";
        LOG(INFO) << "CommandLine: " << google::GetArgv();

        using namespace binary_version;
        LOG(INFO) << "BuildTime: " << kBuildTime;
        LOG(INFO) << "BuildType: " << kBuildType;
        LOG(INFO) << "BuilderName: " << kBuilderName;
        LOG(INFO) << "HostName: " << kHostName;
        LOG(INFO) << "Compiler: " << kCompiler;
        LOG(INFO) << "Version:";
        for (int i = 0; i < kSvnInfoCount; ++i)
        {
            LOG(INFO) << i << ":\n" << kSvnInfo[i];
        }
        m_initialized = true;
    }
    else
    {
        LOG(INFO) << "InitAllModules is called again";
    }

    LOG(INFO) << "Initializing all modules ...";
    std::vector<Module*> sorted_modules;
    SortModules(&sorted_modules);
    for (std::vector<Module*>::iterator i = sorted_modules.begin();
         i != sorted_modules.end(); ++i)
    {
        Module* module = *i;
        if (!module->initialized)
        {
            LOG(INFO) << "Initializing module " << module->name;
            if (!module->ctor())
            {
                LOG(ERROR) << "Module initialization error:" << "defined in "
                           << module->filename << ":" << module->line << ", exit";
                exit(1);
            }

            // register dtor to atexit
            // ATEXIT_MAX is large enough under glibc, so can be used unlimitedly.
            if (module->dtor)
                atexit(module->dtor);
            module->initialized = true;
        }
        else
        {
            VLOG(1) << "Module " << module->name
                      << " has already been initialized, skip";
        }
    }
    LOG(INFO) << "All modules are initialized successfully";
}

///////////////////////////////////////////////////////////////////////////////
// forward ModuleManager mtthods to Impl

void ModuleManager::RegisterModuleCtor(
    const char* filename,
    int line,
    const char* name,
    ModuleCtor ctor
)
{
    ModuleManagerImpl::Instance()->RegisterModuleCtor(filename, line, name, ctor);
}

void ModuleManager::RegisterModuleDtor(
    const char* filename,
    int line,
    const char* name,
    ModuleDtor dtor
)
{
    ModuleManagerImpl::Instance()->RegisterModuleDtor(filename, line, name, dtor);
}

void ModuleManager::LogModuleDtor(const char* name)
{
    LOG(INFO) << "Uninitializing module " << name;
}

void ModuleManager::RegisterDependency(
    const char* filename,
    int line,
    const char* this_module_name,
    const char* dep_name,
    ModuleRegisterer* module_registerer
    )
{
    // Module_registerer is used to verify the name and type of depended module
    // and make a symbolic linking dependency forcely (can be checked by linker
    // if missing).
    // But it become useless here, discard it now.
    ModuleManagerImpl::Instance()->RegisterDependency(this_module_name, dep_name);
}

void ModuleManager::InitializeAll(
    int* argc,
    char*** argv,
    bool remove_flags)
{
    ModuleManagerImpl::Instance()->InitializeAll(argc, argv, remove_flags);
}

} // end of namespace internal

} // end of namespace common

