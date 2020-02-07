package org.omri.radio.impl;

import android.os.Build;
import android.support.annotation.NonNull;
import android.util.Log;

import org.omri.radioservice.RadioServiceMimeType;

import java.io.Serializable;
import java.util.Objects;

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

public class RadioDnsEpgBearerDab extends RadioDnsEpgBearer implements Serializable {

	private static final long serialVersionUID = -4897926993570436519L;

	private final String TAG = "RadioDnsEpgBearerDab";

	private final String mBearerId;
	private final int mCost;
	private final int mBitrate;
	private final String mMimeVal;

	private final int mEnsembleEcc;
	private final int mEnsembleId;
	private final int mServiceId;
	private final int mScid;

	private final RadioServiceMimeType mMimeType;

	RadioDnsEpgBearerDab(String bearerId, int cost, String mimeVal, int bitrate) {
		mBearerId = bearerId;
		mCost = cost;
		mBitrate = bitrate;
		mMimeVal = mimeVal;

		if(mBearerId.startsWith("dab:")) {
			if(DEBUG)Log.d(TAG, "Parsing: " + bearerId);

			String[] valSplit = mBearerId.split(":");

			if(DEBUG)Log.d(TAG, "Split: " + valSplit.length);
			if(valSplit.length == 2) {
				String[] vals = valSplit[1].split("\\.");

				for(String val : vals) {
					if(DEBUG)Log.d(TAG, "Value: " + val);
				}

				//TODO clean this mess up
				if(vals.length == 4) {
					mEnsembleEcc = Integer.parseInt(vals[0].substring(1), 16);
					mEnsembleId = Integer.parseInt(vals[1], 16);
					mServiceId = Integer.parseInt(vals[2].trim(), 16);
					mScid = Integer.parseInt(vals[3], 16);

					if(mimeVal != null) {
						if (mMimeVal.equalsIgnoreCase("audio/mpeg")) {
							mMimeType = RadioServiceMimeType.AUDIO_MPEG;
						} else if (mMimeVal.equalsIgnoreCase("audio/aac")) {
							mMimeType = RadioServiceMimeType.AUDIO_AAC;
						} else {
							mMimeType = RadioServiceMimeType.UNKNOWN;
						}
					} else {
						mMimeType = RadioServiceMimeType.UNKNOWN;
					}

					if(DEBUG)Log.d(TAG, "Parsed  ECC: " + mEnsembleEcc + " : " + Integer.toHexString(mEnsembleEcc));
					if(DEBUG)Log.d(TAG, "Parsed  EID: " + mEnsembleId + " : " + Integer.toHexString(mEnsembleId));
					if(DEBUG)Log.d(TAG, "Parsed  SID: " + mServiceId + " : " + Integer.toHexString(mServiceId));
					if(DEBUG)Log.d(TAG, "Parsed SCID: " + mScid + " : " + Integer.toHexString(mScid));
					if(DEBUG)Log.d(TAG, "Parsed MIME: " + mMimeType.toString());
				} else {
					mEnsembleEcc = -1;
					mEnsembleId = -1;
					mScid = -1;
					mServiceId = -1;
					mMimeType = RadioServiceMimeType.UNKNOWN;
				}
			} else {
				mEnsembleEcc = -1;
				mEnsembleId = -1;
				mScid = -1;
				mServiceId = -1;
				mMimeType = RadioServiceMimeType.UNKNOWN;
			}
		} else {
			mEnsembleEcc = -1;
			mEnsembleId = -1;
			mScid = -1;
			mServiceId = -1;
			mMimeType = RadioServiceMimeType.UNKNOWN;
		}
	}

	@Override
	public String getBearerId() {
		return mBearerId;
	}

	@Override
	public RadioDnsEpgBearerType getBearerType() {
		return RadioDnsEpgBearerType.DAB;
	}

	@Override
	public int getCost() {
		return 0;
	}

	@Override
	public int getBitrate() {
		return 0;
	}

	@Override
	public String getMimeValue() {
		return mMimeVal;
	}

	public int getServiceId() {
		return mServiceId;
	}

	public int getEnsembleEcc() {
		return mEnsembleEcc;
	}

	public int getServiceComponentId() {
		return mScid;
	}

	public int getEnsembleId() {
		return mEnsembleId;
	}

	@Override
	public boolean equals(Object obj) {
		if(obj != null) {
			if(obj instanceof RadioDnsEpgBearerDab) {
				RadioDnsEpgBearerDab compSrv = (RadioDnsEpgBearerDab) obj;
				return ((compSrv.getEnsembleId() == this.mEnsembleId) && compSrv.getBearerId().equals(this.mBearerId) && (compSrv.getServiceId() == this.mServiceId) && (compSrv.getEnsembleEcc() == this.mEnsembleEcc) && (compSrv.getServiceComponentId() == this.mScid));
			}
		}

		return false;
	}

	@Override
	public int hashCode() {
		if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			return Objects.hash(mEnsembleId, mBearerId, mServiceId, mEnsembleEcc, mScid, mCost, mBitrate, mMimeVal);
		} else {
			int hash = 2;
			hash = 63 * hash + (int)(this.mEnsembleId ^ (this.mEnsembleId >>> 32));
			hash = 63 * hash + this.mBearerId.hashCode() ^ (this.mBearerId.hashCode() >>> 32);
			hash = 63 * hash + (int)(this.mServiceId ^ (this.mServiceId >>> 32));
			hash = 63 * hash + (int)(this.mEnsembleEcc ^ (this.mEnsembleEcc >>> 32));
			hash = 63 * hash + (int)(this.mScid ^ (this.mScid >>> 32));
			hash = 63 * hash + (int)(this.mCost ^ (this.mCost >>> 32));
			hash = 63 * hash + (int)(this.mBitrate ^ (this.mBitrate >>> 32));
			hash = 63 * hash + this.mMimeVal.hashCode() ^ (this.mMimeVal.hashCode() >>> 32);

			return hash;
		}
	}

	@Override
	public int compareTo(@NonNull RadioDnsEpgBearer o) {
		return this.mBearerId.compareTo(o.getBearerId());
	}
}
