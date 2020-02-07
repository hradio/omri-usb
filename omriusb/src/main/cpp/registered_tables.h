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

#ifndef REGISTERED_TABLES
#define REGISTERED_TABLES

#include <string>

namespace registeredtables {

enum CHARACTER_SET {
    EBU_LATIN,
    UCS_2 = 6,
    UTF_8 = 15
};

static const std::string CHARACTER_SET_NAME[] {
    "EBU Latin",
    "", "", "", "", "",
    "UCS-2",
    "", "", "", "", "", "", "", "",
    "UTF-8"
};

constexpr uint8_t LANGUAGE_MAX = 0x7F;
static const std::string LANGUAGE[] = {
    "Unknown/not applicable",
    "Albanian",
    "Breton",
    "Catalan",
    "Croatian",
    "Welsh",
    "Czech",
    "Danish",
    "German",
    "English",
    "Spanish",
    "Esperanto",
    "Estonian",
    "Basque",
    "Faroese",
    "French",
    "Frisian",
    "Irish",
    "Gaelic",
    "Galician",
    "Icelandic",
    "Italian",
    "Lappish",
    "Latin",
    "Latvian",
    "Luxembourgian",
    "Lithuanian",
    "Hungarian",
    "Maltese",
    "Dutch",
    "Norwegian",
    "Occitan",
    "Polish",
    "Portuguese",
    "Romanian",
    "Romansh",
    "Serbian",
    "Slovak",
    "Slovene",
    "Finnish",
    "Swedish",
    "Turkish",
    "Flemish",
    "Walloon",
    "rfu_2C",
    "rfu_2D",
    "rfu_2E",
    "rfu_2F",
    "National_Assignemnt_30",
    "National_Assignemnt_31",
    "National_Assignemnt_32",
    "National_Assignemnt_33",
    "National_Assignemnt_34",
    "National_Assignemnt_35",
    "National_Assignemnt_36",
    "National_Assignemnt_37",
    "National_Assignemnt_38",
    "National_Assignemnt_39",
    "National_Assignemnt_3A",
    "National_Assignemnt_3B",
    "National_Assignemnt_3C",
    "National_Assignemnt_3D",
    "National_Assignemnt_3F",
    "Background sound/clean feed",
    "rfu_41",
    "rfu_42",
    "rfu_43",
    "rfu_44",
    "Zulu",
    "Vietnamese",
    "Uzbek",
    "Urdu",
    "Ukranian",
    "Thai",
    "Telugu",
    "Tatar",
    "Tamil",
    "Tadzhik",
    "Swahili",
    "Sranan Tongo",
    "Somali",
    "Sinhalese",
    "Shona",
    "Serbo-Croat",
    "Rusyn",
    "Russian",
    "Quechua",
    "Pushtu",
    "Punjabi",
    "Persian",
    "Papiamento",
    "Oriya",
    "Nepali",
    "Ndebele",
    "Marathi",
    "Moldavian",
    "Malaysian",
    "Malagasay",
    "Macedonian",
    "Laotian",
    "Korean",
    "Khmer",
    "Kazakh",
    "Kannada",
    "Japanese",
    "Indonesian",
    "Hindi",
    "Hebrew",
    "Hausa",
    "Gurani",
    "Gujurati",
    "Greek",
    "Georgian",
    "Fulani",
    "Dari",
    "Chuvash",
    "Chinese",
    "Burmese",
    "Bulgarian",
    "Bengali",
    "Belorussian",
    "Bambora",
    "Azerbaijani",
    "Assamese",
    "Armenian",
    "Arabic",
    "Amharic"
};

enum DATA_SERVICE_COMPONENT_TYPE {
    UNSPECIFIED_DATA,
    TRAFFIC_MESSAGE_CHANNEL,
    EMERGENCY_WARNING_SYSTEM,
    INTERACTIVE_TEXT_TRANSMISSION_SYSTEM,
    PAGING,
    TRANSPARENT_DATA_CHANNEL,

    MPEG_2_TRANSPORT_STREAM = 24,

    EMBEDDED_IP_PACKETS = 59,
    MULTIMEDIA_OBJECT_TRANSFER,
    PROPRIETARY_SERVICE,
    NOT_USED_62,
    NOT_USED_63
};

static const std::string DATA_SERVICE_COMPONENT_TYPE_NAME[64] {
    "Unspecified data",
    "Traffic Message Channel (TMC)",
    "Emergency Warning System (EWS)",
    "Interactive Text Transmission System (ITTS)",
    "Paging",
    "Transparent Data Channel (TDC)",
    "","","","",
    "","","","","","","","","","",
    "","","","","","","","","","",
    "","","","","","","","","","",
    "","","","","","","","","","",
    "","","","","","","","",
    "Embedded IP packets",
    "Multimedia Object Transfer (MOT)",
    "Proprietary Service",
    "Not used",
    "Not used"
};

enum USERAPPLICATIONTYPE {
    RFU,
    DYNAMIC_LABEL, //NOT_USED is here used for dynamic label
    MOT_SLIDESHOW,
    MOT_BROADCAST_WEBSITE,
    TPEG,
    DGPS,
    TMC,
    EPG,
    DAB_JAVA,
    DMB,
    IPDC_SERVICES,
    VOICE_APPLICATIONS,
    MIDDLEWARE,
    FILECASTING,

    JOURNALINE = 0x044A,
};

static const std::string PROGRAMME_TYPE_NAME[32][3] {
    // Programme Type , 16-character abbrev., 8 character abbrev.
    {"No programme type", "None", "None"},
    {"News", "News", "News"},
    {"Current Affairs", "Current_Affairs", "Affairs"},
    {"Information", "Information", "Info"},
    {"Sport", "Sport", "Sport"},
    {"Education", "Education", "Educate"},
    {"Drama", "Drama", "Drama"},
    {"Culture", "Arts", "Arts"},
    {"Science", "Science", "Science"},
    {"Varied", "Talk", "Talk"},
    {"Pop Music", "Pop_Music", "Pop"},
    {"Rock Music", "Rock_Music", "Rock"},
    {"Easy Listening Music", "Easy_Listening", "Easy"},
    {"Light Classical", "Light_Classical", "Classics"},
    {"Serious Classical", "Classical_Music", "Classics"},
    {"Other Music", "Other_Music", "Other_M"},
    {"Weather/Meteorology", "Weather", "Weather"},
    {"Finance/Business", "Finance", "Finance"},
    {"Children\'s programmes", "Children\'s", "Children"},
    {"Social Affairs", "Factual", "Factual"},
    {"Religion", "Religion", "Religion"},
    {"Phone In", "Phone_In", "Phone_In"},
    {"Travel", "Travel", "Travel"},
    {"Leisure", "Leisure", "Leisure"},
    {"Jazz Music", "Jazz_and_Blues", "Jazz"},
    {"Country Music", "Country_Music", "Country"},
    {"National Music", "National_Music", "Nation_M"},
    {"Oldies Music", "Oldies_Music", "Oldies"},
    {"Folk Music", "Folk_Music", "Folk"},
    {"Documentary", "Documentary" "Document"},
    {"Not used", "Not_Used", "Not_Used"},
    {"Not used", "Not_Used", "Not_Used"}
};

enum ANNOUNCEMENT_TYPE {
    ALARM,                  //Information about a serious or even life-threatening situation such as: nuclear fallout or war.
    ROAD_TRAFFIC_FLASH,     //Information about problems on the road.
    TRANSPORT_FLASH,        //Information concerning Public Transport, such as: schedules of buses, ferries, planes or trains.
    WARNING_SERVICE,        //Information about incidents of less importance than Alarm Announcements, such as: disruption of power or water supplies.
    NEWS_FLASH,             //News bulletin.
    AREA_WEATHER_FLASH,     //Weather bulletin.
    EVENT_ANNOUNCEMENT,     //Information about commercial, cultural or public events.
    SPECIAL_EVENT,          //Information on unscheduled or previously unforeseen events.
    PROGRAMME_INFORMATION,  //Information about current or future radio broadcasts or programmes.
    SPORT_REPORT,           //Bulletin of news about sport.
    FINANCIAL_REPORT,       //Bulletin of news about finance or business.
    RESERVED_B11,
    RESERVED_B12,
    RESERVED_B13,
    RESERVED_B14,
    RESERVED_B15
};

static const std::string ANNOUNCEMENT_TYPE_NAME[16][3] {
    {"Alarm", "Alarm", "Alarm"},
    {"Road Traffic flash", "Traffic_News", "Traffic"},
    {"Transport flash", "Transport_News", "Travel"},
    {"Warning/Service", "Warning", "Warning"},
    {"News flash", "News", "News"},
    {"Area weather flash", "Weather", "Weather"},
    {"Event announcement", "Event", "Event"},
    {"Special event", "Special_event", "Special"},
    {"Programme Information", "Radio_info", "Rad_info"},
    {"Sport report", "Sports_news", "Sports"},
    {"Financial report", "Financial_news", "Finance"},
    {"Reserved_B11", "Reserved_B11", "Reserved_B11"},
    {"Reserved_B12", "Reserved_B12", "Reserved_B12"},
    {"Reserved_B13", "Reserved_B13", "Reserved_B13"},
    {"Reserved_B14", "Reserved_B14", "Reserved_B14"},
    {"Reserved_B15", "Reserved_B15", "Reserved_B15"}
};

}

#endif // REGISTERED_TABLES

