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

#ifndef FIG_00_EXT_05_H
#define FIG_00_EXT_05_H

#include "fig_00.h"

#include "registered_tables.h"

/*
 * ETS 300 401 clause 8.1.2 Service component language
 *
 * The service component language feature is used to signal a language associated with a service component; the spoken
 * language of an audio component or the language of the content of a data component. This information can be used for
 * user selection or display. The feature is encoded in Extension 5 of FIG type 0 (FIG 0/5).
 *
 * The FIG 0/5 has a repetition rate of once per second.
 */
class Fig_00_Ext_05 : public Fig_00 {

public:
    explicit Fig_00_Ext_05(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_05();

    /*
     * Language: this 8-bit field shall indicate the language of the audio or data service component.
     * It shall be coded according to ETSI TS 101 756, tables 9 and 10.
     */
    virtual uint8_t getLanguageCode() const;

    /*
     * Language string as defined in ETSI TS 101 756 - v1.8.1, tables 9 and 10
     */
    virtual std::string getLanguage() const;

    /*
     * L/S flag: this 1-bit flag shall indicate whether the service component identifier takes the short or the long form, as follows:
     * 0: short form;
     * 1: long form.
     */
    virtual bool isShortForm() const;

    /*
     * if shortform is used
     * SubChId (Sub-channel Identifier): this 6-bit field shall identify the sub-channel in
     * which the service component is carried;
     */
    virtual uint8_t getSubchannelId() const;

    /*
     * if longform is used
     * SCId: this 12-bit field shall identify the service component;
     */
    virtual uint16_t getServiceComponentId() const;

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_05]"};

    uint8_t m_langCode;
    bool m_isShortForm;
    uint8_t m_subChanId{0xFF};
    uint16_t m_scid{0xFFFF};
};

#endif // FIG_00_EXT_05_H
