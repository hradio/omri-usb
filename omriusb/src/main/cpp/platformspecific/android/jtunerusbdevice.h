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

#ifndef JTUNERUSBDEVICE_H
#define JTUNERUSBDEVICE_H

#include "jusbdevice.h"
#include "../../dabensemble.h"

class JTunerUsbDevice : public JUsbDevice {

public:
    enum TUNER_CALLBACK_TYPE {
        TUNER_CALLBACK_READY = 0,
        TUNER_CALLBACK_FAILED,
        TUNER_CALLBACK_FREQUENCY_LOCKED,
        TUNER_CALLBACK_FREQUENCY_NOT_LOCKED,
        TUNER_SCAN_IN_PROGRESS
    };

public:
    explicit JTunerUsbDevice(JavaVM* javaVm, JNIEnv* env, jobject tunerUsbDevice);
    virtual ~JTunerUsbDevice();

    virtual const JavaVM* getJavaVM() const;

    virtual void callCallback(TUNER_CALLBACK_TYPE callbackType);
    virtual void scanProgress(int percentDone);
    virtual void ensembleReady(const DabEnsemble& ensemble);
    virtual void serviceStarted(jobject dabService);
    virtual void serviceStopped(jobject dabService);
    virtual void receptionStatistics(bool rfLock, int qual);

    virtual void setJavaClassUsbTuner(JNIEnv* env, jclass usbTunerClass);
    virtual void setJavaClassDabService(JNIEnv* env, jclass dabServiceClass);
    virtual void setJavaClassDabServiceComponent(JNIEnv* env, jclass dabServiceComponentClass);
    virtual void setJavaClassDabServiceUserApplication(JNIEnv* env, jclass dabServiceUserAppClass);
    virtual void setJavaClassTermId(JNIEnv* env, jclass dabServiceClass);

private:
    const std::string m_logTag{"[JTunerUsbDevice] "};

private:
    JavaVM* m_javaVm;
    JNIEnv* m_env;

    //Radio class definition
    jmethodID m_radioGetInstanceMId;

    //The existing Tuner object reference
    jobject m_usbTunerObject;

    //Tuner class definition
    jclass m_usbTunerClass;

    //TunerUsb interface
    jmethodID m_usbTunerCallbackMId;
    jmethodID m_usbTunerScanProgressMId;
    jmethodID m_usbTunerServiceFoundMId;
    jmethodID m_usbTunerServiceStartedMId;
    jmethodID m_usbTunerServiceStoppedMId;
    jmethodID m_usbTunerReceptionStatisticsMId;

    //Tuner interface
    jmethodID m_usbTunerGetServicesMId;

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
};


#endif //JTUNERUSBDEVICE_H
