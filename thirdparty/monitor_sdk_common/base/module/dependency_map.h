//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:02
//  @file:      dependency_map.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_MODULE_DEPENDENCY_MAP_H
#define COMMON_BASE_MODULE_DEPENDENCY_MAP_H
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace common {
/// represent dependency rules, such as makefile rules:
/// A: B C
/// B: C
/// C:
typedef std::map<std::string, std::set<std::string> > DependencyMap;

/// Dependency map is a DAG(directed acyclic graph), do topological sort on the DAG
/// yield sorted result.
bool TopologicalSort(const DependencyMap& depends, std::vector<std::string>* result);

} // end of namespace common

#endif // COMMON_BASE_MODULE_DEPENDENCY_MAP_H
