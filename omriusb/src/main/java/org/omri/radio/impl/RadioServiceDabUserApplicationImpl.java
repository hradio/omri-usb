package org.omri.radio.impl;

import java.io.Serializable;

import org.omri.radioservice.RadioServiceDabDataServiceComponentType;
import org.omri.radioservice.RadioServiceDabUserApplication;
import org.omri.radioservice.RadioServiceDabUserApplicationType;

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

public class RadioServiceDabUserApplicationImpl implements RadioServiceDabUserApplication, Serializable {

	private static final long serialVersionUID = -2789012667334731485L;
	
	private RadioServiceDabUserApplicationType mApptype = RadioServiceDabUserApplicationType.RFU;
	private boolean mIsCaProtected = false;
	private int mCaOrg = -1;
	private boolean mIsXpadApptype = false;
	private int mXpadApptype = -1;
	private boolean mDgUsed = false;
	private RadioServiceDabDataServiceComponentType mDSCTy = RadioServiceDabDataServiceComponentType.UNSPECIFIED_DATA;
	private byte[] mUappSpecificData = null;

	@Override
	public RadioServiceDabUserApplicationType getType() {
		return mApptype;
	}

	void setUserApplicationType(int type) {
		mApptype = RadioServiceDabUserApplicationType.getUserApplicationTypeByType(type);
	}

	@Override
	public boolean isCaProtected() {
		return mIsCaProtected;
	}

	void setIsCaProtected(boolean caApplied) {
		mIsCaProtected = caApplied;
	}

	@Override
	public int getCaOrganization() {
		return mCaOrg;
	}

	void setCaOrganization(int caOrg) {
		mCaOrg = caOrg;
	}

	@Override
	public boolean isXpadApptype() {
		return mIsXpadApptype;
	}

	void setIsXpadApptype(boolean isXpad) {
		mIsXpadApptype = isXpad;
	}

	@Override
	public int getXpadAppType() {
		return mXpadApptype;
	}

	void setXpadApptype(int xpadType) {
		mXpadApptype = xpadType;
	}

	@Override
	public boolean isDatagroupTransportUsed() {
		return mDgUsed;
	}

	void setIsDatagroupsUsed(boolean dgUsed) {
		mDgUsed = dgUsed;
	}

	@Override
	public RadioServiceDabDataServiceComponentType getDataServiceComponentType() {
		return mDSCTy;
	}

	void setDSCTy(int dscty) {
		mDSCTy = RadioServiceDabDataServiceComponentType.getDSCTyByType(dscty);
	}

	@Override
	public byte[] getUserApplicationData() {
		return mUappSpecificData;
	}

	void setUappdata(byte[] uAppdata) {
		mUappSpecificData = uAppdata;
	}
}
