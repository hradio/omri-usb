package org.omri.radio.impl;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.omri.radioservice.RadioServiceDabComponent;
import org.omri.radioservice.RadioServiceDabComponentListener;
import org.omri.radioservice.RadioServiceDabUserApplication;

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

public class RadioServiceDabComponentImpl implements RadioServiceDabComponent, Serializable {

	private static final long serialVersionUID = -600743978196068815L;
	
	private int mScBitrate = -1;
	private boolean mScCaFlag = false;
	private int mServiceId = -1;
	private String mScChannelIdString = "";
	private boolean mScDgFlag = false;
	private int mScId = -1;
	private String mScIdString = "";
	private String mScLabel = "";
	private int mScPacketAddress = -1;
	private String mScPacketAddressString = "";
	private boolean mScIsPrimary = false;
	private int mScIDsId;
	private String mScServiceCompIdString = "";
	private int mScTmId = -1;
	private String mScTmIdString = "";
	private int mScTypeId = -1;
	private String mScTypeIdString = "";
	private boolean mDataGroupsUsed = false;
	private List<RadioServiceDabUserApplication> mScUappList = new ArrayList<RadioServiceDabUserApplication>();
	
	@Override
	public int getBitrate() {
		return mScBitrate;
	}
	
	void setScBitrate(int scBitrate) {
		this.mScBitrate = scBitrate;
	}

	@Override
	public boolean isCaApplied() {
		return mScCaFlag;
	}
	
	void setIsScCaFlagSet(boolean scCaFlag) {
		this.mScCaFlag = scCaFlag;
	}

	@Override
	public int getServiceId() {
		return mServiceId;
	}

	void setServiceId(int scChannelId) {
		this.mServiceId = scChannelId;
	}

	@Override
	public int getSubchannelId() {
		return mScId;
	}
	
	void setSubchannelId(int scId) {
		this.mScId = scId;
	}

	@Override
	public String getLabel() {
		return mScLabel;
	}
	
	void setScLabel(String scLabel) {
		this.mScLabel = scLabel.trim();
	}

	@Override
	public int getPacketAddress() {
		return mScPacketAddress;
	}
	
	void setPacketAddress(int scPackAdd) {
		this.mScPacketAddress = scPackAdd;
	}

	@Override
	public boolean isPrimary() {
		return mScIsPrimary;
	}
	
	void setIsScPrimary(boolean scPrimary) {
		this.mScIsPrimary = scPrimary;
	}

	@Override
	public int getServiceComponentIdWithinService() {
		return mScIDsId;
	}
	
	void setServiceComponentIdWithinService(int scServiceCompId) {
		this.mScIDsId = scServiceCompId;
	}

	@Override
	public int getTmId() {
		return mScTmId;
	}
	
	void setTmId(int tmId) {
		this.mScTmId = tmId;
	}

	@Override
	public int getServiceComponentType() {
		return mScTypeId;
	}
	
	void setServiceComponentType(int scType) {
		this.mScTypeId = scType;
	}

	@Override
	public boolean isDatagroupTransportUsed() {
		return mDataGroupsUsed;
	}

	void setDatagroupTransportUsed(boolean dgFlag) {
		mDataGroupsUsed = dgFlag;
	}

	@Override
	public List<RadioServiceDabUserApplication> getUserApplications() {
		return mScUappList;
	}

	void addScUserApplication(RadioServiceDabUserApplication uApp) {
		this.mScUappList.add(uApp);
	}
	
	void addScUserApplications(List<RadioServiceDabUserApplication> uApp) {
		this.mScUappList.addAll(uApp);
	}

	@Override
	public void subscribe(RadioServiceDabComponentListener dabComponentListener) {

	}

	@Override	
	public void unsubscribe(RadioServiceDabComponentListener dabComponentListener) {

	}


	//new additions
	private int mMscStartAddress = -1;
	private int mSubchanSize = -1;
	private int mProtLvl = -1;
	private int mProtType = -1;
	private int mUepIdx = -1;
	private boolean mFecApplied = false;
	@Override
	public int getMscStartAddress() {
		return mMscStartAddress;
	}

	void setMscStartAddress(int mscStart) {
		mMscStartAddress = mscStart;
	}

	@Override
	public int getSubchannelSize() {
		return mSubchanSize;
	}

	void setSubchannelSize(int subChanSize) {
		mSubchanSize = subChanSize;
	}

	@Override
	public int getProtectionLevel() {
		return mProtLvl;
	}

	void setProtectionLevel(int protLvl) {
		mProtLvl = protLvl;
	}

	@Override
	public int getProtectionType() {
		return mProtType;
	}

	void setProtectionType(int protType) {
		mProtType = protType;
	}

	@Override
	public int getUepTableIndex() {
		return mUepIdx;
	}

	void setUepTableIndex(int tableIdx) {
		mUepIdx = tableIdx;
	}

	@Override
	public boolean isFecSchemeApplied() {
		return mFecApplied;
	}

	void setIsFecSchemeApplied(boolean fecApplied) {
		mFecApplied = fecApplied;
	}
}
