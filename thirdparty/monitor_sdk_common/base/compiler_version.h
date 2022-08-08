//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:45
//  @file:      compiler_version.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPILER_VERSION_H
#define COMMON_BASE_COMPILER_VERSION_H
#pragma once

#ifdef __GNUC__
/// convert gcc version to a number. eg, gcc 4.5.1 -> 40501
#define GCC_VERSION_NUMBER() (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)
#endif

#endif // COMMON_BASE_COMPILER_VERSION_H
