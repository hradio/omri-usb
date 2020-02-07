package org.omri.radio.impl;

import android.os.Build;
import android.util.Log;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceDab;
import org.omri.radioservice.RadioServiceDabComponent;
import org.omri.radioservice.RadioServiceType;

import static org.omri.BuildConfig.DEBUG;

/**
 * Copyright (C) 2018 IRT GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author Fabian Sattler, IRT GmbH
 */

public class RadioServiceDabImpl extends RadioServiceImpl implements RadioServiceDab, Serializable {

	private static final long serialVersionUID = 6561664196086864931L;
	
	private int mEnsembleEcc;
	private int mEnsembleId;
	private String mEnsembleLabel = "";
	private String mEnsembleShortLabel = "";
	private int mEnsembleFrequency = 0;
	private boolean mIsCaApplied = false;
	private int mCaId = -1;
	private String mServiceLabel = "";
	private String mShortServiceLabel = "";
	private int mServiceId;
	private boolean mIsProgrammeService = false;
	private List<RadioServiceDabComponent> mServiceComponents = new ArrayList<RadioServiceDabComponent>();

	RadioServiceDabImpl() {	}

	@Override
	public RadioServiceType getRadioServiceType() {
		return RadioServiceType.RADIOSERVICE_TYPE_DAB;
	}

	@Override
	public int getEnsembleEcc() {
		return mEnsembleEcc;
	}
	
	void setEnsembleEcc(int ensembleEcc) {
		this.mEnsembleEcc = ensembleEcc;
		if(DEBUG)Log.d("DabService", " Ensemble ECC: " + mEnsembleEcc);
	}

	@Override
	public int getEnsembleId() {
		return mEnsembleId;
	}
	
	void setEnsembleId(int ensembleId) {
		this.mEnsembleId = ensembleId;
		if(DEBUG)Log.d("DabService", " Ensemble ID: " + mEnsembleId);
	}

	@Override
	public String getEnsembleLabel() {
		return mEnsembleLabel;
	}
	
	void setEnsembleLabel(String ensembleLabel) {
		this.mEnsembleLabel = ensembleLabel.trim();
	}

	@Override
	public String getEnsembleShortLabel() {
		return mEnsembleShortLabel;
	}

	void setEnsembleShortLabel(String ensembleShortLabel) {
		this.mEnsembleShortLabel = ensembleShortLabel.trim();
	}

	@Override
	public int getEnsembleFrequency() {
		return mEnsembleFrequency;
	}
	
	void setEnsembleFrequency(int ensembleFreq) {
		this.mEnsembleFrequency = ensembleFreq;
	}

	@Override
	public boolean isCaProtected() {
		return mIsCaApplied;
	}

	void setIsCaProtected(boolean ca) {
		mIsCaApplied = ca;
	}

	@Override
	public int getCaId() {
		return mCaId;
	}

	void setCaId(int caId) {
		mCaId = caId;
	}

	@Override
	public String getServiceLabel() {
		return mServiceLabel;
	}
	
	void setServiceLabel(String srvLabel) {
		this.mServiceLabel = srvLabel.trim();
	}

	@Override
	public String getShortLabel() {
		return mShortServiceLabel;
	}

	void setShortLabel(String shortLabel) {
		mShortServiceLabel = shortLabel.trim();
	}

	@Override
	public int getServiceId() {
		return mServiceId;
	}
	
	void setServiceId(int srvId) {
		this.mServiceId = srvId;
	}

	@Override
	public boolean isProgrammeService() {
		return mIsProgrammeService;
	}
	
	void setIsProgrammeService(boolean isProg) {
		mIsProgrammeService = isProg;
	}

	@Override
	public List<RadioServiceDabComponent> getServiceComponents() {
		return mServiceComponents;
	}

	void addServiceComponent(RadioServiceDabComponent dabComp) {
		this.mServiceComponents.add(dabComp);
	}
	
	void addServiceComponent(List<RadioServiceDabComponent> dabComp) {
		this.mServiceComponents.addAll(dabComp);
	}

	@Override
	public boolean equals(Object obj) {
		if(obj != null) {
			if(obj instanceof RadioServiceDab) {
				//A DAB service is uniquely identified by its Service Identifier (SId) and in conjunction with the Extended Country Code unique world-wide
				RadioServiceDab compSrv = (RadioServiceDab) obj;
				return ((compSrv.getEnsembleId() == mEnsembleId) && (compSrv.getEnsembleFrequency() == mEnsembleFrequency) && (compSrv.getServiceId() == mServiceId) && (compSrv.getEnsembleEcc() == mEnsembleEcc));
			}
		}

		return false;
	}

	@Override
	public int hashCode() {
		if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			return Objects.hash(mEnsembleId, mEnsembleFrequency, mServiceId, mEnsembleEcc);
		} else {
			int hash = 5;
			hash = 67 * hash + (int)(this.mEnsembleId ^ (this.mEnsembleId >>> 32));
			hash = 67 * hash + (int)(this.mEnsembleFrequency ^ (this.mEnsembleFrequency >>> 32));
			hash = 67 * hash + (int)(this.mServiceId ^ (this.mServiceId >>> 32));
			hash = 67 * hash + (int)(this.mEnsembleEcc ^ (this.mEnsembleEcc >>> 32));

			return hash;
		}
	}

	@Override
	public boolean equalsRadioService(RadioService otherSrv) {
		if(otherSrv != null) {
			if(otherSrv instanceof RadioServiceDab) {
				//A DAB service is uniquely identified by its Service Identifier (SId) and in conjunction with the Extended Country Code unique world-wide
				RadioServiceDab compSrv = (RadioServiceDab) otherSrv;
				return (compSrv.getServiceId() == this.mServiceId) && (compSrv.getEnsembleEcc() == this.mEnsembleEcc);
			} else if(otherSrv instanceof RadioServiceIpImpl) {
				RadioServiceIpImpl ipSrv = (RadioServiceIpImpl) otherSrv;
				for(RadioDnsEpgBearer bearer : ipSrv.getBearers()) {
					if(bearer.getBearerType() == RadioDnsEpgBearerType.DAB) {
						RadioDnsEpgBearerDab dabBearer = (RadioDnsEpgBearerDab)bearer;
						if(dabBearer.getServiceId() == this.mServiceId && dabBearer.getEnsembleEcc() == this.mEnsembleEcc) {
							return true;
						}
					}
				}
			}
		}

		return false;
	}
}
