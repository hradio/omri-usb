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

#ifndef EDIINPUT_H
#define EDIINPUT_H


#include "../../dabensemble.h"
#include "../../dabinput.h"
#include "jdabservice.h"

class EdiInput : public DabEnsemble {

public:
    explicit EdiInput(JavaVM* javaVm, JNIEnv* env, jobject tunerEdiStream);
    virtual ~EdiInput();

    //delete copy and assignment constructors
    EdiInput(EdiInput&) = delete;
    void operator=(EdiInput&) = delete;

    virtual void setJavaClassEdiTuner(JNIEnv* env, jclass ediTunerClass);
    virtual void setJavaClassDabService(JNIEnv* env, jclass dabServiceClass);
    virtual void setJavaClassDabServiceComponent(JNIEnv* env, jclass dabServiceComponentClass);
    virtual void setJavaClassDabServiceUserApplication(JNIEnv* env, jclass dabServiceUserAppClass);
    virtual void setJavaClassTermId(JNIEnv* env, jclass dabServiceClass);
    virtual void setJavaClassDabTime(JNIEnv* env, jclass ediDabTimeClass);

    void startService(std::shared_ptr<JDabService> serviceLink);
    void stopService(const DabService& service);
    void stopAllRunningServices();

    void ediDataInput(const std::vector<uint8_t>& data, int size);

    void flushComponentBuffer();

private:
    JavaVM* m_javaVm;
    JNIEnv* m_env;

    //The existing EdiTuner object reference
    jobject m_ediTunerObject;

    //EdiTuner class definition
    jclass m_ediTunerClass;

    //EdiTuner methods
    jmethodID m_ediTunerServiceStartedMId;
    jmethodID m_ediTunerServiceStoppedMId;
    jmethodID m_ediTunerServiceUpdated;
    jmethodID m_ediTunerIsScanningMId;
    //SBT
    jmethodID m_ediTunerDabTimeUpdate;
    jmethodID m_ediTunerNewLiveItem;
    //SBT DABTime class
    jclass m_dabTimeClass;
    jmethodID m_dabTimeConstructorMID;
    //RadioInstance
    jobject m_radioInstanceObj;
    jclass m_radioClass;
    jclass m_radioImplClass;
    jmethodID m_radioClassGetInstanceMID;
    jmethodID m_radioInstanceDabTimeMID;

    //DABService
    jclass m_dabServiceClass;
    jmethodID m_dabServiceConstructorMId;
    jmethodID m_dabServiceSetEnsembleEccMId;
    jmethodID m_dabServiceSetEnsembleIdMId;
    jmethodID m_dabServiceSetEnsembleLabelMId;

    jmethodID m_dabServiceSetEnsembleShortLabelMId;
    jmethodID m_dabServiceSetIsCaAppliedMId;
    jmethodID m_dabServiceSetCaIdMId;

    jmethodID m_dabServiceSetEnsembleFrequencyMId;
    jmethodID m_dabServiceSetServiceLabelMId;
    jmethodID m_dabServiceSetServiceShortLabelMId;
    jmethodID m_dabServiceSetServiceIdMId;
    jmethodID m_dabServiceSetServiceIsProgrammeMId;
    jmethodID m_dabServiceAddServiceComponentMId;

    jmethodID m_dabServiceAddGenreTermIdMId;

    //DabServiceComponent
    jclass m_dabServiceComponentClass;
    jmethodID m_dabServiceComponentConstructorMId;
    jmethodID m_dabServiceComponentSetBitrateMId;
    jmethodID m_dabServiceComponentSetCaFlagMId;
    jmethodID m_dabServiceComponentSetServiceIdMId;
    jmethodID m_dabServiceComponentSetSubchannelIdMId;
    jmethodID m_dabServiceComponentSetLabelMId;
    jmethodID m_dabServiceComponentSetPacketAddressMId;
    jmethodID m_dabServiceComponentSetIsPrimaryMId;
    jmethodID m_dabServiceComponentSetScIDsMId;
    jmethodID m_dabServiceComponentSetTransportModeIdMId;
    jmethodID m_dabServiceComponentSetScTypeMId;
    jmethodID m_dabServiceComponentSetIsDgUsedMId;
    jmethodID m_dabServiceComponentSetMscStartAddressMId;
    jmethodID m_dabServiceComponentSetSubchanSizeMId;
    jmethodID m_dabServiceComponentSetProtectionLvlMId;
    jmethodID m_dabServiceComponentSetProtectionTypeMId;
    jmethodID m_dabServiceComponentSetUepTblIdxMId;
    jmethodID m_dabServiceComponentSetIsFecAppliedMId;
    jmethodID m_dabServiceComponentAddUserApplicationMId;

    //DabServiceUserApplication
    jclass m_dabServiceUserApplicationClass;
    jmethodID m_dabServiceUserApplicationConstructorMId;
    jmethodID m_dabServiceUserApplicationSetAppTypeMId;
    jmethodID m_dabServiceUserApplicationSetIsCaAppliedMId;
    jmethodID m_dabServiceUserApplicationSetCaOrgMId;
    jmethodID m_dabServiceUserApplicationSetIsXPadMId;
    jmethodID m_dabServiceUserApplicationSetXPadAppTypeMId;
    jmethodID m_dabServiceUserApplicationSetIsDgUsedMId;
    jmethodID m_dabServiceUserApplicationSetDSCTyMId;
    jmethodID m_dabServiceUserApplicationSetUappDataMId;

    //TermId
    jclass m_termIdClass;
    jmethodID m_termIdConstructorMId;
    jmethodID m_termIdSetGenreHrefMId;
    jmethodID m_termIdSetTermIdMId;
    jmethodID m_termIdSetGenreTextMId;

private:
    const std::string LOG_TAG{"[EdiInput] "};

    std::shared_ptr<std::function<void()>> m_ensembleFinishedCb;
    std::shared_ptr<DabEnsemble::Date_Time_Callback> m_dabTimeCallback;
    std::shared_ptr<JDabService> m_startServiceLink{nullptr};

private:
    void ensembleCollectFinished();
    void dabTimeUpdate(const Fig_00_Ext_10::DabTime& dabTime);
    void newLiveItem(std::vector<uint8_t> liveItemData);

private:
    std::vector<uint8_t> m_ediBuffer;

};


#endif //EDIINPUT_H
