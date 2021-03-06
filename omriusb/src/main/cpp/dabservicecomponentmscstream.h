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

#ifndef DABSERVICECOMPONENTMSCSTREAM_H
#define DABSERVICECOMPONENTMSCSTREAM_H

#include "dabservicecomponent.h"

class DabServiceComponentMscStream : public DabServiceComponent {

public:
    virtual ~DabServiceComponentMscStream();
    virtual void flushBufferedData();

protected:
    explicit DabServiceComponentMscStream();

};

#endif // DABSERVICECOMPONENTMSCSTREAM_H
