// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-02
// Description:

#ifndef COMMON_BASE_PREPROCESS_STRINGIZE_H
#define COMMON_BASE_PREPROCESS_STRINGIZE_H
#pragma once

/// Converts the parameter X to a string after macro replacement
/// on X has been performed.
/// example: COMMON_PP_STRINGIZE(UCHAR_MAX) -> "255"
#define COMMON_PP_STRINGIZE(X) COMMON_PP_DO_STRINGIZE(X)
#define COMMON_PP_DO_STRINGIZE(X) #X

#endif // COMMON_BASE_PREPROCESS_STRINGIZE_H
