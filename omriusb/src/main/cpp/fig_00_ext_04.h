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

#ifndef FIG_00_EXT_04_H
#define FIG_00_EXT_04_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.3.3 Service component with Conditional Access in stream mode or FIC
 *
 * The Extension 4 of FIG type 0 (FIG 0/4) gives additional information about the service component description for
 * components with CA and carried in Stream mode.
 */
class Fig_00_Ext_04 : public Fig_00 {

public:
    explicit Fig_00_Ext_04(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_04();

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_04]"};
};

#endif // FIG_00_EXT_04_H
