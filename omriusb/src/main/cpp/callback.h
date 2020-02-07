/*
 * Copyright (C) 2018 IRT GmbH
 *
 * Author:
 *  Fabian Sattler
 *
 * This file is a part of IRT DAB library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef CALLBACK_H
#define CALLBACK_H

#include <vector>

class Callback {

public:
    explicit Callback() = default;
    Callback(Callback&&) = default;
    Callback& operator=(Callback&&) = default;
    virtual ~Callback() = default;

    virtual void call(const std::vector<uint8_t>& data) = 0;
};

#endif // CALLBACK_H
