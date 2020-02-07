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

#ifndef FIG_00_EXT_07_H
#define FIG_00_EXT_07_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.4.2 Configuration information
 *
 * The configuration information provides information about the currently tuned ensemble configuration to enable
 * receivers to rapidly check already stored MCI
 *
 * The FIG 0/7 shall be transmitted in the same FIB as the FIG 0/0 and shall be the second FIG in the FIB.
 * A receiver can use the Services field to determine if the complete MCI has been received. This is especially useful
 * under weak signal conditions when FIBs with erroneous CRCs are detected.
 * A receiver can use the Count field to verify if the configuration of an already known ensemble remains unchanged - if
 * the received Count matches the stored Count then the configuration is the same. This can be used to speed up use cases
 * like background scanning. The rest of the MCI need not be collected to judge if the stored configuration is still valid.
 */
class Fig_00_Ext_07 : public Fig_00 {

public:
    explicit Fig_00_Ext_07(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_07();

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_07]"};
};

#endif // FIG_00_EXT_07_H
