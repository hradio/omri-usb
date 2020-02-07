package org.omri.radio.impl;

import android.os.Build;
import android.support.annotation.NonNull;

import java.io.Serializable;
import java.util.Objects;

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

public class RadioDnsEpgBearerIpHttp extends RadioDnsEpgBearer implements Serializable {

	private static final long serialVersionUID = -812677911372858911L;

	private final String mBearerId;
	private final int mCost;
	private final int mBitrate;
	private final String mMimeVal;

	RadioDnsEpgBearerIpHttp(String bearerId, int cost, String mimeVal, int bitrate) {
		mBearerId = bearerId;
		mCost = cost;
		mBitrate = bitrate;
		mMimeVal = mimeVal;
	}

	@Override
	public String getBearerId() {
		return mBearerId;
	}

	@Override
	public RadioDnsEpgBearerType getBearerType() {
		return RadioDnsEpgBearerType.IP_HTTP;
	}

	@Override
	public int getCost() {
		return mCost;
	}

	@Override
	public int getBitrate() {
		return mBitrate;
	}

	@Override
	public String getMimeValue() {
		return mMimeVal;
	}

	@Override
	public boolean equals(Object obj) {
		if (obj != null) {
			if (obj instanceof RadioDnsEpgBearerIpHttp) {
				RadioDnsEpgBearerIpHttp compBearer = (RadioDnsEpgBearerIpHttp) obj;
				return ((compBearer.getBearerId().equals(this.mBearerId)) && (compBearer.getCost() == this.mCost) && (compBearer.getBitrate() == this.mBitrate) && compBearer.getMimeValue().equals(this.mMimeVal));
			}
		}

		return false;
	}

	@Override
	public int hashCode() {
		if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			return Objects.hash(mBearerId, mCost, mBitrate, mMimeVal);
		} else {
			int hash = 4;
			hash = 61 * hash + this.mBearerId.hashCode() ^ (this.mBearerId.hashCode() >>> 32);
			hash = 61 * hash + (int)(this.mCost ^ (this.mCost >>> 32));
			hash = 61 * hash + (int)(this.mBitrate ^ (this.mBitrate >>> 32));
			hash = 61 * hash + this.mMimeVal.hashCode() ^ (this.mMimeVal.hashCode() >>> 32);

			return hash;
		}
	}

	@Override
	public int compareTo(@NonNull RadioDnsEpgBearer o) {
		return this.mBearerId.compareTo(o.getBearerId());
	}
}
