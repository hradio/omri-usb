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

#include "dabensemble.h"

#include <functional>
#include <iostream>

#include "callbackhandle.h"
#include "timer.h"

DabEnsemble::DabEnsemble() {
    m_ficPtr = std::unique_ptr<FicParser>(new FicParser);
    registerCbs();
}

DabEnsemble::~DabEnsemble() {

}

void DabEnsemble::reset() {
    std::cout << m_logTag << " Reseting Ensemble informations" << std::endl;

    m_reseting = true;
    m_ensembleCollectFinished = false;

    m_ficPtr = std::unique_ptr<FicParser>(new FicParser);

    m_ensembleId = 0xFFFF;
    m_cifCntHigh = 0x00;
    m_cifCntLow = 0x00;
    m_cifCntHighNext = 0x00;
    m_cifCntLowNext = 0x00;
    m_announcementsSupported = false;
    m_ensembleEcc = 0xFF;

    m_labelCharset = 0x00;
    m_ensembleLabel = "";
    m_ensembleShortLabel = "";

    m_streamComponentsMap.clear();
    m_packetComponentsMap.clear();
    m_servicesMap.clear();

    registerCbs();

    m_reseting = false;
}

uint16_t DabEnsemble::getEnsembleId() const {
    return m_ensembleId;
}

uint8_t DabEnsemble::getCurrentCifCountHigh() const {
    return m_cifCntHigh;
}

uint8_t DabEnsemble::getCurrentCifCountLow() const {
    return m_cifCntLow;
}

bool DabEnsemble::isAlarmAnnouncemnetSupported() const {
    return m_announcementsSupported;
}

uint8_t DabEnsemble::getEnsembleLabelCharset() const {
    return m_labelCharset;
}

std::string DabEnsemble::getEnsembleLabel() const {
    return m_ensembleLabel;
}

std::string DabEnsemble::getEnsembleShortLabel() const {
    return m_ensembleShortLabel;
}

uint8_t DabEnsemble::getEnsembleEcc() const {
    return m_ensembleEcc;
}

std::vector<std::shared_ptr<DabService>> DabEnsemble::getDabServices() const {
    std::vector<std::shared_ptr<DabService>> services;
    for(const auto& srv : m_servicesMap) {
        services.push_back(std::make_shared<DabService>(srv.second));
    }
    return services;
}

void DabEnsemble::registerCbs() {
    m_fig001done = false;
    m_fig002done = false;
    m_fig003done = false;
    m_fig008done = false;
    m_fig013done = false;
    m_fig014done = false;
    m_fig017done = false;
    m_fig0done = false;

    m_fig100done = false;
    m_fig101done = false;
    m_fig104done = false;
    m_fig105done = false;
    m_fig106done = false;
    m_fig1done = false;

    m_00DonePtr = m_ficPtr.get()->registerFig_00_Done_Callback(std::bind(&DabEnsemble::fig_00_done_cb, this, std::placeholders::_1));
    m_02Ptr = m_ficPtr.get()->registerFig_00_02_Callback(std::bind(&DabEnsemble::fig00_02_input, this, std::placeholders::_1));

    m_00Ptr = m_ficPtr.get()->registerFig_00_00_Callback(std::bind(&DabEnsemble::fig00_00_input, this, std::placeholders::_1));

    m_010Ptr = m_ficPtr->registerFig_00_10_Callback(std::bind(&DabEnsemble::fig00_10_input, this, std::placeholders::_1));

    //ServiceFollowing
    m_06Ptr = m_ficPtr.get()->registerFig_00_06_Callback(std::bind(&DabEnsemble::fig00_06_input, this, std::placeholders::_1));
    m_21Ptr = m_ficPtr.get()->registerFig_00_21_Callback(std::bind(&DabEnsemble::fig00_21_input, this, std::placeholders::_1));
    m_24Ptr = m_ficPtr.get()->registerFig_00_24_Callback(std::bind(&DabEnsemble::fig00_24_input, this, std::placeholders::_1));

    //Announcements
    m_18Ptr = m_ficPtr.get()->registerFig_00_18_Callback(std::bind(&DabEnsemble::fig00_18_input, this, std::placeholders::_1));
    m_19Ptr = m_ficPtr.get()->registerFig_00_19_Callback(std::bind(&DabEnsemble::fig00_19_input, this, std::placeholders::_1));
}

void DabEnsemble::dataInput(const std::vector<uint8_t>& data, uint8_t subChId, bool synchronized) {
    if(!m_reseting) {
        if(subChId != 0x64) {
            auto compIter = m_streamComponentsMap.find(subChId);
            if(compIter != m_streamComponentsMap.cend()) {
                compIter->second->componentMscDataInput(data, synchronized);
            } else {
                auto dataCompIter = m_packetComponentsMap.cbegin();
                while(dataCompIter != m_packetComponentsMap.cend()) {
                    if(dataCompIter->second->getSubChannelId() == subChId) {
                        dataCompIter->second->componentMscDataInput(data, synchronized);
                    }
                    ++dataCompIter;
                }
            }
        } else {
            m_ficPtr->call(data);
        }
    }
}

//added to flush component decoders
void DabEnsemble::flushBufferedComponentData(uint8_t subChId) {
    if(!m_reseting) {
        auto compIter = m_streamComponentsMap.find(subChId);
        if(compIter != m_streamComponentsMap.cend()) {
            compIter->second->flushBufferedData();
        } else {
            auto dataCompIter = m_packetComponentsMap.cbegin();
            while(dataCompIter != m_packetComponentsMap.cend()) {
                if(dataCompIter->second->getSubChannelId() == subChId) {
                    dataCompIter->second->flushBufferedData();
                }
                ++dataCompIter;
            }
        }
    }
}

//added to flush component decoders
void DabEnsemble::flushAllBufferedComponentData() {
    auto audioCompIter = m_streamComponentsMap.cbegin();
    while(audioCompIter != m_streamComponentsMap.cend()) {
        audioCompIter->second->flushBufferedData();
        ++audioCompIter;
    }

    auto dataCompIter = m_packetComponentsMap.cbegin();
    while(dataCompIter != m_packetComponentsMap.cend()) {
        dataCompIter->second->flushBufferedData();
        ++dataCompIter;
    }
}

void DabEnsemble::fig00_00_input(const Fig_00_Ext_00& fig00) {
    if(m_ensembleId != fig00.getEnsembleId()) {
        m_isInitializing = true;
    }

    m_ensembleId = fig00.getEnsembleId();
    m_announcementsSupported = fig00.isAlarmSupported();

    m_cifCntHigh = fig00.getCifCountHigh();
    m_cifCntLow = fig00.getCifCountLow();

    if((m_cifCntHigh != m_cifCntHighNext || m_cifCntLow != m_cifCntLowNext) && !m_isInitializing) {
        std::cout << m_logTag << " CIF Counter interrupted" << std::endl;
        std::cout << m_logTag << " FIG 00 Ext 00 CifCnt    : " << +m_cifCntHigh << ":" << +m_cifCntLow << std::endl;
        std::cout << m_logTag << " FIG 00 Ext 00 CifCntNext: " << +m_cifCntHighNext << ":" << +m_cifCntLowNext << std::endl;
    }

    m_cifCntHighNext = static_cast<uint8_t>((m_cifCntLowNext >= 246) ? ((m_cifCntHigh+1) % 20) : m_cifCntHigh);
    m_cifCntLowNext = static_cast<uint8_t>((m_cifCntLow + 4) % 250);

    if(fig00.getChangeFlag() != Fig_00_Ext_00::NO_CHANGE) {
        std::cout << m_logTag << " Fig_00_Ext_00 ChangeFlag: " << +fig00.getChangeFlag() << " OccChange: " << +fig00.getOccurenceChange() << std::endl;
    }
    if(fig00.isNextConfiguration()) {
        std::cout << m_logTag << " Fig_00_Ext_00 nextConfiguration" << std::endl;
    }

    m_isInitializing = false;
}

void DabEnsemble::fig00_01_input(const Fig_00_Ext_01& fig01) {
    for(const auto& subOrga : fig01.getSubchannelOrganizations()) {
        std::shared_ptr<DabServiceComponent> component{nullptr};
        auto streamCompIter = m_streamComponentsMap.find(subOrga.subChannelId);
        if(streamCompIter != m_streamComponentsMap.cend()) {
            streamCompIter->second->setMscStartAddress(subOrga.startAddress);
            streamCompIter->second->setSubchannelSize(subOrga.subChannelSize);
            streamCompIter->second->setConvolutionalCodingRate(subOrga.convolutionalCodingRate);
            streamCompIter->second->setProtectionLevelString(fig01.PROTECTIONLEVEL_STRING[subOrga.protectionLevelType]);
            streamCompIter->second->setProtectionLevel(subOrga.protectionLevel);
            streamCompIter->second->setProtectionType(subOrga.protectionType);
            streamCompIter->second->setUepTableIndex(subOrga.uepTableIndex);
            streamCompIter->second->setSubchannelBitrate(subOrga.subChannelBitrate);
        } else {
            auto packetCompIter = m_packetComponentsMap.begin();
            while(packetCompIter != m_packetComponentsMap.end()) {
                if(packetCompIter->second->getSubChannelId() == subOrga.subChannelId) {
                    packetCompIter->second->setMscStartAddress(subOrga.startAddress);
                    packetCompIter->second->setSubchannelSize(subOrga.subChannelSize);
                    packetCompIter->second->setConvolutionalCodingRate(subOrga.convolutionalCodingRate);
                    packetCompIter->second->setProtectionLevelString(fig01.PROTECTIONLEVEL_STRING[subOrga.protectionLevelType]);
                    packetCompIter->second->setProtectionLevel(subOrga.protectionLevel);
                    packetCompIter->second->setProtectionType(subOrga.protectionType);
                    packetCompIter->second->setUepTableIndex(subOrga.uepTableIndex);
                    packetCompIter->second->setSubchannelBitrate(subOrga.subChannelBitrate);
                }

                packetCompIter++;
            }
        }
    }
}

void DabEnsemble::fig00_02_input(const Fig_00_Ext_02 &fig02) {
    for(const auto& srvDesc : fig02.getServiceDescriptions()) {
        auto srvIter = m_servicesMap.find(srvDesc.serviceId);
        if(srvIter == m_servicesMap.end()) {

            DabService service;

            if(srvDesc.isProgrammeService) {
                service.setServiceId(srvDesc.serviceId & 0x0000FFFF);
            } else {
                service.setServiceId(srvDesc.serviceId);
            }

            service.setIsProgrammeService(srvDesc.isProgrammeService);
            service.setCaId(srvDesc.caId);
            service.setNumberOfServiceComponents(srvDesc.numServiceComponents);

            for(const Fig_00_Ext_02::ServiceComponentDescription& srvCom : srvDesc.serviceComponents) {
                std::shared_ptr<DabServiceComponent> componentPtr;

                switch (srvCom.transportModeId) {
                    case Fig_00_Ext_02::TMID::MSC_STREAM_AUDIO:
                    case Fig_00_Ext_02::TMID::MSC_STREAM_DATA: {
                        auto compIter = m_streamComponentsMap.find(srvCom.subChannelId);
                        if(compIter == m_streamComponentsMap.cend()) {
                            if(srvCom.transportModeId == Fig_00_Ext_02::TMID::MSC_STREAM_AUDIO) {
                                componentPtr = std::make_shared<DabServiceComponentMscStreamAudio>();
                                auto audioComponentPtr = std::static_pointer_cast<DabServiceComponentMscStreamAudio>(componentPtr);
                                audioComponentPtr->setAudioServiceComponentType(srvCom.serviceComponentType);
                            } else if(srvCom.transportModeId == Fig_00_Ext_02::TMID::MSC_STREAM_DATA) {
                                componentPtr = std::make_shared<DabServiceComponentMscStreamData>();
                                auto dataComponentPtr = std::static_pointer_cast<DabServiceComponentMscStreamData>(componentPtr);
                                dataComponentPtr->setDataServiceComponentType(srvCom.serviceComponentType);
                            }

                            componentPtr->setIsCaApplied(srvCom.isCaApplied);
                            componentPtr->setIsPrimary(srvCom.isPrimary);
                            componentPtr->setSubchannelId(srvCom.subChannelId);

                            m_streamComponentsMap.insert(std::make_pair(srvCom.subChannelId, componentPtr));

                            service.addServiceComponent(componentPtr);
                        }

                        break;
                    }
                    case Fig_00_Ext_02::TMID::MSC_PACKET_MODE_DATA: {
                        auto compIter = m_packetComponentsMap.find(srvCom.serviceComponentId);
                        if(compIter == m_packetComponentsMap.cend()) {
                            auto packetComponentPtr = std::make_shared<DabServiceComponentMscPacketData>();
                            packetComponentPtr->setIsCaApplied(srvCom.isCaApplied);
                            packetComponentPtr->setIsPrimary(srvCom.isPrimary);
                            packetComponentPtr->setDataServiceComponentId(srvCom.serviceComponentId);

                            m_packetComponentsMap.insert(std::make_pair(srvCom.serviceComponentId, packetComponentPtr));
                            service.addServiceComponent(packetComponentPtr);
                        } else {
                            service.addServiceComponent(compIter->second);
                        }

                        break;
                    }
                default:
                    break;
                }
            }

            service.setEnsembleFrequency(m_ensembleFrequency);
            m_servicesMap.emplace(srvDesc.serviceId, std::move(service));
        }
    }
}

void DabEnsemble::fig00_03_input(const Fig_00_Ext_03& fig03) {
    if(m_packetComponentsMap.empty()) {
        return;
    }

    for(const auto& packetDesc : fig03.getPacketModeServiceDescriptions()) {
        auto compIter = m_packetComponentsMap.find(packetDesc.serviceComponentId);
        if(compIter != m_packetComponentsMap.cend()) {
            std::cout << m_logTag << " PacketComponent: " << std::hex << packetDesc.serviceComponentId << " setting SubchannelId: " << +packetDesc.subchannelId << std::dec << " PacketAddress: " << +packetDesc.packetAddress << " updating" << std::endl;
            compIter->second->setSubchannelId(packetDesc.subchannelId);
            compIter->second->setDataServiceComponentType(packetDesc.dataServiceComponentType);
            compIter->second->setCaOrganization(packetDesc.caOrganization);
            compIter->second->setIsDataGroupTransportUsed(packetDesc.dataGroupTransportUsed);
            compIter->second->setPacketAddress(packetDesc.packetAddress);
        }
    }
}

//TODO service linking
void DabEnsemble::fig00_06_input(const Fig_00_Ext_06& fig06) {
    //std::cout << m_logTag << " LinkDB Received SLI" << std::endl;

    /*
    for(const auto& linkInfo : fig06.getServiceLinkingInformations()) {
        //auto linkDbIter = m_serviceLinkDb.find(linkInfo.linkageSetNumber);
        auto linkDbIter = m_serviceLinkDb.find(linkInfo.linkDbKey);
        if(linkDbIter != m_serviceLinkDb.cend()) {
            //Linkageset already in Map
            if(!linkInfo.isChangeEvent) {
                if(linkInfo.isContinuation) {
                    for(const auto& srvLink : linkInfo.serviceLinks) {
                        if(!(*linkDbIter).second.containsServiceLinkList(srvLink)) {
                            (*linkDbIter).second.serviceLinks.push_back(srvLink);

                            std::cout << m_logTag << " LinkDB update " << (linkInfo.isSoftLink ? "SoftLink " : "HardLink ") << "LinkageSetNumber: 0x" << std::hex << +linkInfo.linkageSetNumber << " with KeyServiceID: 0x" << +(*linkDbIter).second.keyServiceId << std::dec <<
                                " links to " << "\n\t";
                                for(const auto& sll : (*linkDbIter).second.serviceLinks) {
                                    std::cout << +sll.idList.size() << (sll.idListQualifier == 0 ? " DAB ServiceIds: " : " FM RDS PIs: ") << "\n\t\t";
                                    for(const auto& sllSrvId : sll.idList) {
                                        std::cout << "0x" << std::hex << +sllSrvId << "\n\t\t";
                                    }
                                    std::cout << std::dec;
                                }
                                std::cout << std::endl;
                        }
                    }
                }
            } else {
                if((*linkDbIter).second.isSoftLink != linkInfo.isSoftLink) {
                    std::cout << m_logTag << " LinkDB ChangeEvent for LinkNum: " << (*linkDbIter).second.linkageSetNumber << " with KeyId 0x" << std::hex << +(*linkDbIter).second.keyServiceId << std::dec << " was " << ((*linkDbIter).second.isSoftLink ? "a SoftLink " : "a HardLink ") << "and is now" << (linkInfo.isSoftLink ? " a SoftLink" : " a HardLink") << std::endl;

                    (*linkDbIter).second.isSoftLink = linkInfo.isSoftLink;
                }

                if((*linkDbIter).second.linkageActive != linkInfo.linkageActive) {
                    std::cout << m_logTag << " LinkDB ChangeEvent for LinkNum: " << (*linkDbIter).second.linkageSetNumber << " with KeyId 0x" << std::hex << +(*linkDbIter).second.keyServiceId << std::dec << " LinkageActuator changed from " << ((*linkDbIter).second.linkageActive ? "active " : "inactive ") << "to " << (linkInfo.linkageActive ? "active" : "inactive") << std::endl;

                    (*linkDbIter).second.linkageActive = linkInfo.linkageActive;
                }
            }
        } else {
            //Linkageset not yet in Map
            if(!linkInfo.isContinuation && !linkInfo.isChangeEvent) {
                //std::cout << m_logTag << " LinkDB adding new linkageSet with LinkDbKey: " << std::hex << +linkInfo.linkDbKey << " and LinkageNumber: " << +linkInfo.linkageSetNumber << " KeySeviceID: 0x" << +linkInfo.keyServiceId << std::dec << std::endl;
                m_serviceLinkDb.insert(std::make_pair(linkInfo.linkDbKey, linkInfo));

                std::cout << m_logTag << " LinkDB adding " << (linkInfo.isSoftLink ? "SoftLink " : "HardLink ") << "LinkageSetNumber: 0x" << std::hex << +linkInfo.linkageSetNumber << " with KeyServiceID: 0x" << +linkInfo.keyServiceId << std::dec <<
                          " links to " << "\n\t";
                for(const auto& sll : linkInfo.serviceLinks) {
                    std::cout << +sll.idList.size() <<(sll.idListQualifier == 0 ? " DAB ServiceIds: " : " FM RDS PIs: ") << "\n\t\t";
                    for(const auto& sllSrvId : sll.idList) {
                        std::cout << "0x" << std::hex << +sllSrvId << "\n\t\t";
                    }
                    std::cout << std::dec;
                }
                std::cout << std::endl;
            }
        }
    }
    */
}

void DabEnsemble::fig00_21_input(const Fig_00_Ext_21& fig21) {
    /*
    std::cout << m_logTag << " FreqInfoDB received FrequencyInformation for OE: " << std::boolalpha << fig21.isOtherEnsemble() << std::noboolalpha << std::endl;

    for(const auto& freqInfo : fig21.getFrequencyInformations()) {
        auto freqInfoDbIter = m_frequencyInformationDb.find(freqInfo.id);
        if(freqInfoDbIter != m_frequencyInformationDb.cend()) {
            if(std::find((*freqInfoDbIter).second.begin(), (*freqInfoDbIter).second.end(), freqInfo) == (*freqInfoDbIter).second.end()) {
                std::cout << m_logTag << " FreqInfoDB adding FreqInfo entry for ID: 0x" << std::hex << +freqInfo.id << std::dec << std::endl;
                (*freqInfoDbIter).second.push_back(freqInfo);
            } else {
                std::cout << m_logTag << " FreqInfoDB already contains entry for ID: 0x" << std::hex << +freqInfo.id << std::dec << std::endl;
            }
        } else {
            //add new entry to db
            //TODO isCEI change Event Indication?
            std::cout << m_logTag << " FreqInfoDB adding new entry for ID: 0x" << std::hex << +freqInfo.id << std::dec << std::endl;
            m_frequencyInformationDb.insert(std::make_pair(freqInfo.id, std::vector<Fig_00_Ext_21::FrequencyInformation>{freqInfo}));
        }
    }
    */
}

void DabEnsemble::fig00_24_input(const Fig_00_Ext_24& fig24) {
    //std::cout << m_logTag << " OtherEnsembleSrvDb received OeServiceInformation" << std::endl;
}

void DabEnsemble::fig00_08_input(const Fig_00_Ext_08& fig08) {
    if(m_streamComponentsMap.empty()) {
        std::cout << m_logTag << " FIG 00 Ext 08 Maps still empty" << std::endl;
        return;
    }

    for(const auto& srvGlobalDef : fig08.getGLobalDefinitions()) {
        if(srvGlobalDef.isShortForm) {
            auto subChanIter = m_streamComponentsMap.find(srvGlobalDef.subchannelId);
            if(subChanIter != m_streamComponentsMap.cend()) {
                subChanIter->second->setServiceComponentIdWithinService(srvGlobalDef.scIdS);
            }
        } else {
            auto packCompIter = m_packetComponentsMap.find(srvGlobalDef.serviceComponentId);
            if(packCompIter != m_packetComponentsMap.cend()) {
                std::cout << m_logTag << " FIG 00 Ext 08 setting ScIdS for SId: " << std::hex << +srvGlobalDef.serviceId << ", ScId: " << +srvGlobalDef.serviceComponentId << ", SubChanId: " << +packCompIter->second->getSubChannelId() << " to: " << +srvGlobalDef.scIdS << std::dec << std::endl;
                packCompIter->second->setServiceComponentIdWithinService(srvGlobalDef.scIdS);
            }
        }
    }
}

void DabEnsemble::fig00_09_input(const Fig_00_Ext_09& fig09) {
    std::cout << m_logTag << " Ensemble ECC: " << std::hex << +fig09.getEnsembleEcc() << std::dec << std::endl;
    m_ensembleEcc = fig09.getEnsembleEcc();
}

void DabEnsemble::fig00_10_input(const Fig_00_Ext_10& fig10) {
    std::cout << m_logTag << " DateAndTime: " << +fig10.getDabTime().unixTimestampSeconds << std::endl;

    m_dateAndTimeDispatcher.invoke(fig10.getDabTime());
}

void DabEnsemble::fig00_13_input(const Fig_00_Ext_13& fig13) {
    for(const auto& uAppInfo : fig13.getUserApplicationInformations()) {
        auto serviceIter = m_servicesMap.find(uAppInfo.serviceID);
        if(serviceIter != m_servicesMap.end()) {
            for(const auto& component : (*serviceIter).second.getServiceComponents()) {
                if(component->getServiceComponentIdWithinService() == uAppInfo.scIdS) {
                    for(const auto& uApp : uAppInfo.userApplications) {
                        std::cout << m_logTag << " FIG 00 Ext 13 Adding UserApplication: " << uApp.userAppType << std::endl;
                        DabUserApplication userApp;
                        userApp.setCaOrganization(uApp.caOrganization);
                        userApp.setDataServiceComonentType(uApp.dataServiceComponentType);
                        userApp.setIsCaApplied(uApp.caApplied);
                        userApp.setIsDataGroupsUsed(uApp.dataGroupsUsed);
                        userApp.setUserApplicationData(uApp.userAppData);
                        userApp.setUserApplicationType(uApp.userAppType);
                        userApp.setXpadAppType(uApp.xpadAppType);
                        userApp.setIsXpadApp(uApp.isXpadData);

                        component->addUserApplication(userApp);
                    }
                }
            }
        } else {
            std::cout << m_logTag << " SCIDS UserApplication not found" << std::endl;
        }
    }
}

void DabEnsemble::fig00_14_input(const Fig_00_Ext_14& fig14) {
    if(m_packetComponentsMap.empty()) {
        return;
    }

    for(const Fig_00_Ext_14::FecSchemeDescription& fecDesc : fig14.getFecSchemeDescriptions()) {
        if(fecDesc.fecScheme == Fig_00_Ext_14::FEC_SCHEME::APPLIED) {
            auto packCompIter = m_packetComponentsMap.cbegin();
            while(packCompIter != m_packetComponentsMap.cend()) {
                if(packCompIter->second->getSubChannelId() == fecDesc.subChannelId) {
                    packCompIter->second->setIsFecSchemeApplied(true);
                    break;
                }
                packCompIter++;
            }
        }
    }
}

void DabEnsemble::fig00_17_input(const Fig_00_Ext_17& fig17) {
    for(auto ptyInfo : fig17.getProgrammeTypeInformations()) {
        auto serviceIter = m_servicesMap.find(ptyInfo.serviceId);
        if(serviceIter != m_servicesMap.cend()) {
            serviceIter->second.setProgrammeTypeIsDynamic(ptyInfo.isDynamic);
            serviceIter->second.setProgrammeTypeCode(ptyInfo.intPtyCode);
        }
    }
}


void DabEnsemble::fig00_18_input(const Fig_00_Ext_18& fig18) {
    //TODO Announcement support indication
    /*
    for(const auto& aSup : fig18.getAnnouncementSupports()) {
        std::cout << "Fig_00_Ext_18 for SId: 0x" << std::hex << +aSup.serviceId << std::dec
        << " with ClusterIds: ";
        for(const auto& clId : aSup.clusterIds) {
            std::cout << "0x" << std::hex << +clId << std::dec << ", ";
        }
         std::cout << "received" << std::endl;
    }
    */
}

void DabEnsemble::fig00_19_input(const Fig_00_Ext_19& fig19) {
    for(const auto& aSwitched : fig19.getSwitchedAnnouncements()) {
        if(aSwitched.isNewlyIntroduced) {
            std::cout << "[Fig_00_Ext_19]" << " Announcement for ClusterId: 0x" << std::hex << +aSwitched.clusterId << std::dec << " received. SwitchSize: " << +aSwitched.announcementsSwitched.size() << std::endl;
        }
        auto switchedIter = m_activeAnnouncements.find(aSwitched.clusterId);
        if(switchedIter != m_activeAnnouncements.cend()) {
            std::cout << "[Fig_00_Ext_19]" << " Announcement for ClusterId: 0x" << std::hex << +aSwitched.clusterId << std::dec << " already in map, newly: " <<
            std::boolalpha << aSwitched.isNewlyIntroduced << std::noboolalpha << ", AnnounceSize: " << +aSwitched.announcementsSwitched.size() << std::endl;
            //if(!aSwitched.isNewlyIntroduced) {
            //Announcement in list, switched-list is empty....announcement ended
            if(aSwitched.announcementsSwitched.empty()) {
                std::cout << "[Fig_00_Ext_19]" << " Announcement for ClusterId: 0x" << std::hex << +(*switchedIter).second.clusterId << std::dec << " ended, removing from map" << std::endl;
                m_activeAnnouncements.erase(switchedIter);
            }
            //}
        } else {
            if(aSwitched.isNewlyIntroduced) {
                if(!aSwitched.announcementsSwitched.empty()) {
                    std::cout << "[Fig_00_Ext_19]" << " Adding newlyIntroduced Announcement with ClusterId: 0x" << std::hex << +aSwitched.clusterId  << std::dec << std::endl;
                    m_activeAnnouncements.insert(std::make_pair(aSwitched.clusterId, aSwitched));
                } else {
                    std::cout << "[Fig_00_Ext_19]" << " Not adding newlyIntroduced Announcement with ClusterId: 0x" << std::hex << +aSwitched.clusterId  << std::dec << " , switched is empty" << std::endl;
                }
            }
        }
    }
}

void DabEnsemble::fig01_00_input(const Fig_01_Ext_00& fig10) {
    if(m_ensembleLabel.empty() && m_ensembleShortLabel.empty()) {
        if(fig10.getEnsembleId() == m_ensembleId) {
            m_ensembleLabel = fig10.getEnsembleLabel();
            m_ensembleShortLabel = fig10.getEnsembleShortLabel();

            m_labelCharset = fig10.getCharset();
            m_10Ptr = nullptr;
        }
    }
}

void DabEnsemble::fig01_01_input(const Fig_01_Ext_01& fig11) {
    std::cout << m_logTag << " ServiceSanity FIC 01_01: " << std::hex << +fig11.getProgrammeServiceId() << std::dec << " : " << fig11.getProgrammeServiceLabel() << std::endl;
    auto serviceIter = m_servicesMap.find(fig11.getProgrammeServiceId());
    if(serviceIter != m_servicesMap.cend()) {
        serviceIter->second.setLabelCharset(fig11.getCharset());
        serviceIter->second.setServiceLabel(fig11.getProgrammeServiceLabel());
        serviceIter->second.setServiceShortLabel(fig11.getProgrammeServiceShortLabel());
    }
}

void DabEnsemble::fig01_04_input(const Fig_01_Ext_04& fig14) {
    std::cout << m_logTag << " ServiceSanity FIC 01_04: " << std::hex << +fig14.getServiceId() << std::dec << ", SCIDs: " << +fig14.getServiceComponentIdWithinService() << " : " << fig14.getServiceComponentLabel() << std::endl;
    auto serviceIter = m_servicesMap.find(fig14.getServiceId());
    if(serviceIter != m_servicesMap.cend()) {
        for(auto component : serviceIter->second.getServiceComponents()) {
            if(component->getServiceComponentIdWithinService() == fig14.getServiceComponentIdWithinService()) {
                component->setLabelCharset(fig14.getCharset());
                component->setServiceComponentLabel(fig14.getServiceComponentLabel());
                component->setServiceComponentShortLabel(fig14.getServiceComponentShortLabel());
            }
        }
    }
}

void DabEnsemble::fig01_05_input(const Fig_01_Ext_05& fig15) {
    std::cout << m_logTag << " ServiceSanity FIC 01_05: " << std::hex << +fig15.getDataServiceId() << std::dec << " : " << fig15.getDataServiceLabel() << std::endl;
    auto serviceIter = m_servicesMap.find(fig15.getDataServiceId());
    if(serviceIter != m_servicesMap.cend()) {
        serviceIter->second.setLabelCharset(fig15.getCharset());
        serviceIter->second.setServiceLabel(fig15.getDataServiceLabel());
        serviceIter->second.setServiceShortLabel(fig15.getDataServiceShortLabel());
    }
}

void DabEnsemble::fig01_06_input(const Fig_01_Ext_06& fig16) {

}


void DabEnsemble::fig_00_done_cb(Fig::FIG_00_TYPE type) {
    std::cout << m_logTag << " FIG 00 Extension: " << +type << " done ###########" << std::endl;

    switch (type) {
        case Fig::FIG_00_TYPE::BASIC_SUBCHANNEL_ORGANIZATION: {
            if(m_fig003done && !m_fig001done) {
                std::cout << m_logTag << " ServiceSanity FIC 00_01 Done!" << std::endl;
                m_08Ptr = m_ficPtr->registerFig_00_08_Callback(std::bind(&DabEnsemble::fig00_08_input, this, std::placeholders::_1));
                m_fig001done = true;
            }
            break;
        }
        case Fig::FIG_00_TYPE::BASIC_SERVICE_COMPONENT_DEFINITION: {
            if(!m_fig002done) {
                std::cout << m_logTag << " ServiceSanity FIC 00_02 Done!" << std::endl;
                //There are Packet streams
                if(!m_packetComponentsMap.empty()) {
                    m_03Ptr = m_ficPtr->registerFig_00_03_Callback(std::bind(&DabEnsemble::fig00_03_input, this, std::placeholders::_1));
                } else {
                    m_fig003done = true;
                    m_fig014done = true;
                    m_fig105done = true;
                    m_fig013done = true;
                    m_01Ptr = m_ficPtr->registerFig_00_01_Callback(std::bind(&DabEnsemble::fig00_01_input, this, std::placeholders::_1));
                }
                m_fig002done = true;
            }
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_PACKET_MODE: {
            if(!m_fig003done && !m_fig008done) {
                std::cout << m_logTag << " ServiceSanity FIC 00_03 Done!" << std::endl;
                m_01Ptr = m_ficPtr->registerFig_00_01_Callback(std::bind(&DabEnsemble::fig00_01_input, this, std::placeholders::_1));
                m_fig003done = true;
            }
            break;
        }
        case Fig::FIG_00_TYPE::SERVICE_COMPONENT_GLOBAL_DEFINITION: {
            if(!m_fig008done && m_fig001done && m_fig003done) {
                m_013Ptr = m_ficPtr->registerFig_00_13_Callback(std::bind(&DabEnsemble::fig00_13_input, this, std::placeholders::_1));
                m_014Ptr = m_ficPtr->registerFig_00_14_Callback(std::bind(&DabEnsemble::fig00_14_input, this, std::placeholders::_1));
                m_017Ptr = m_ficPtr->registerFig_00_17_Callback(std::bind(&DabEnsemble::fig00_17_input, this, std::placeholders::_1));
                m_09Ptr = m_ficPtr->registerFig_00_09_Callback(std::bind(&DabEnsemble::fig00_09_input, this, std::placeholders::_1));

                m_10Ptr = m_ficPtr->registerFig_01_00_Callback(std::bind(&DabEnsemble::fig01_00_input, this, std::placeholders::_1));
                m_11Ptr = m_ficPtr->registerFig_01_01_Callback(std::bind(&DabEnsemble::fig01_01_input, this, std::placeholders::_1));
                m_14Ptr = m_ficPtr->registerFig_01_04_Callback(std::bind(&DabEnsemble::fig01_04_input, this, std::placeholders::_1));
                m_15Ptr = m_ficPtr->registerFig_01_05_Callback(std::bind(&DabEnsemble::fig01_05_input, this, std::placeholders::_1));
                m_16Ptr = m_ficPtr->registerFig_01_06_Callback(std::bind(&DabEnsemble::fig01_06_input, this, std::placeholders::_1));
                m_01DonePtr = m_ficPtr->registerFig_01_Done_Callback(std::bind(&DabEnsemble::fig_01_done_cb, this, std::placeholders::_1));

                m_fig008done = true;
            }
            break;
        }
        case Fig::FIG_00_TYPE::USERAPPLICATION_INFORMATION: {
            m_fig013done = true;
            break;
        }
        case Fig::FIG_00_TYPE::FEC_SUBCHANNEL_ORGANIZATION: {
            m_fig014done = true;
            break;
        }
        case Fig::FIG_00_TYPE::PROGRAMME_TYPE: {
            m_fig017done = true;
            break;
        }
        default:
            break;
    }

    std::cout << m_logTag << "m_fig001done: " << m_fig001done << std::endl;
    std::cout << m_logTag << "m_fig002done: " << m_fig002done << std::endl;
    std::cout << m_logTag << "m_fig003done: " << m_fig003done << std::endl;
    std::cout << m_logTag << "m_fig008done: " << m_fig008done << std::endl;
    std::cout << m_logTag << "m_fig013done: " << m_fig013done << std::endl;
    std::cout << m_logTag << "m_fig014done: " << m_fig014done << std::endl;
    std::cout << m_logTag << "m_fig1done: " << m_fig1done << std::endl;
    if(m_fig001done && m_fig002done && m_fig003done && m_fig008done && m_fig013done) {
        m_fig0done = true;
        if(m_fig1done) {
            checkServiceSanity();
        }
    }
}

void DabEnsemble::fig_01_done_cb(Fig::FIG_01_TYPE type) {
    std::cout << m_logTag << " FIG 01 Extension: " << +type << " done ###########" << std::endl;
    switch (type) {
        case Fig::FIG_01_TYPE::ENSEMBLE_LABEL: {
            m_fig100done = true;
            break;
        }
        case Fig::FIG_01_TYPE::PROGRAMME_SERVICE_LABEL: {
            m_fig101done = true;
            break;
        }
        case Fig::FIG_01_TYPE::SERVICE_COMPONENT_LABEL: {
            m_fig104done = true;
            break;
        }
        case Fig::FIG_01_TYPE::DATA_SERVICE_LABEL: {
            m_fig105done = true;
            break;
        }
        case Fig::FIG_01_TYPE::XPAD_USERAPPLICATION_LABEL: {
            m_fig106done = true;
            break;
        }
        default: {
            break;
        }
    }

    //TODO packetcomponent label not sent for MOT servicecomponent in own subchannel
    if(m_fig100done && m_fig101done) {
        m_fig1done = true;
    }
}

//TODO this needs some more work
void DabEnsemble::checkServiceSanity() {
    for(const auto& srvMap : m_servicesMap) {
        auto srv = srvMap.second;
        for(const auto& srvComp : srv.getServiceComponents()) {
            std::cout << m_logTag << " ServiceSanity Check for SId: " << std::hex << +srv.getServiceId() << std::dec << " Components: " << +srv.getServiceComponents().size() << " : " << +srv.getNumberServiceComponents() << std::endl;
            if(srvComp->getSubChannelId() != 0xFF && srvComp->getMscStartAddress() != 0xFFFF && srvComp->getSubchannelSize() != 0xFFFF && (srv.getNumberServiceComponents() == srv.getServiceComponents().size()) ) {
                switch(srvComp->getServiceComponentType()) {
                    case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_PACKET_MODE_DATA: {
                        std::shared_ptr<DabServiceComponentMscPacketData> packComp = std::static_pointer_cast<DabServiceComponentMscPacketData>(srvComp);
                        if(packComp->getProtectionLevel() != 0xFF && packComp->getProtectionType() != 0xFF) {
                            continue;
                        } else {
                            std::cout << m_logTag << " ServiceSanity failed for SId: " << std::hex << +srv.getServiceId() << ", SubchanId: "  << +srvComp->getSubChannelId() << std::dec << ", MSC: " << +srvComp->getMscStartAddress() << ", SubSize: " << +srvComp->getSubchannelSize() << std::endl;
                            return;
                        }
                    }
                    case DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_AUDIO: {
                        std::shared_ptr<DabServiceComponentMscStreamAudio> mscSaComp = std::static_pointer_cast<DabServiceComponentMscStreamAudio>(srvComp);
                        if(srv.getServiceLabel().empty()) {
                            std::cout << m_logTag << " ServiceSanity failed for empty servicelabel" << std::endl;
                            return;
                        }
                        if(m_ensembleEcc == 0xFF) {
                            std::cout << m_logTag << " ServiceSanity failed for EnsembleECC is 0xFF" << std::endl;
                            return;
                        }

                        break;
                    }
                    default: {
                        break;
                    }
                }

                continue;
            } else {
                std::cout << m_logTag << " ServiceSanity failed for SId: " << std::hex << +srv.getServiceId() << ", SubchanId: "  << +srvComp->getSubChannelId() << std::dec << ", MSC: " << +srvComp->getMscStartAddress() << ", SubSize: " << +srvComp->getSubchannelSize() << " Components: " << +srv.getServiceComponents().size() << " : " << +srv.getNumberServiceComponents() << std::endl;
                return;
            }
        }
    }

    m_00DonePtr = nullptr;
    m_01Ptr = nullptr;
    m_02Ptr = nullptr;
    m_03Ptr = nullptr;
    m_09Ptr = nullptr;
    m_08Ptr = nullptr;
    m_013Ptr = nullptr;
    m_014Ptr = nullptr;
    m_017Ptr = nullptr;

    m_01DonePtr = nullptr;
    m_10Ptr = nullptr;
    m_11Ptr = nullptr;
    m_14Ptr = nullptr;
    m_15Ptr = nullptr;
    m_16Ptr = nullptr;

    std::cout << m_logTag << " ServiceSanity passed!" << std::endl;
    m_ensembleCollectFinished = true;

    m_ensembleCollectDoneDispatcher.invoke();
}

std::shared_ptr<std::function<void()>> DabEnsemble::registerEnsembleCollectDoneCallback(std::function<void()> cb) {
    return m_ensembleCollectDoneDispatcher.add(cb);
}

std::shared_ptr<DabEnsemble::Date_Time_Callback> DabEnsemble::registerDateTimeCallback(DabEnsemble::Date_Time_Callback cb) {
    return m_dateAndTimeDispatcher.add(cb);
}