package org.omri.radio.impl;

import android.os.Build;
import android.util.Log;

import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceAudiodataListener;
import org.omri.radioservice.RadioServiceDab;
import org.omri.radioservice.RadioServiceIp;
import org.omri.radioservice.RadioServiceIpStream;
import org.omri.radioservice.RadioServiceListener;
import org.omri.radioservice.RadioServiceMimeType;
import org.omri.radioservice.RadioServiceRawAudiodataListener;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.ProgrammeServiceMetadataListener;
import org.omri.radioservice.metadata.TextualMetadataListener;
import org.omri.radioservice.metadata.VisualMetadataListener;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import eu.hradio.core.radiodns.radioepg.bearer.Bearer;
import eu.hradio.core.radiodns.radioepg.radiodns.RadioDns;

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

public class RadioServiceIpImpl extends RadioServiceImpl implements RadioServiceIp, Serializable {

	private static final long serialVersionUID = 1604769239642356483L;

	private final String TAG = "RadioServiceIp";

	private String mServiceLabel = "";
	private List<RadioServiceIpStream> mStreams = new ArrayList<>();

	private List<RadioDnsEpgBearer> mBearers = new ArrayList<>();

	private RadioDns mRadioDns = null;

	RadioServiceIpImpl(){}

	@Override
	public List<RadioServiceIpStream> getIpStreams() {
		return mStreams;
	}

	@Override
	public RadioServiceType getRadioServiceType() {
		return RadioServiceType.RADIOSERVICE_TYPE_IP;
	}

	@Override
	public String getServiceLabel() {
		return mServiceLabel;
	}

	void addStream(RadioServiceIpStream ipStream) {
		mStreams.add(ipStream);
	}

	void addStream(List<RadioServiceIpStream> ipStreams) {
		mStreams.addAll(ipStreams);
	}

	void setServiceLabel(String serviceLabel) {
		mServiceLabel = serviceLabel;
	}

	void setRadioDns(RadioDns rdns) {
		mRadioDns = rdns;
	}

	RadioDns getRadioDns() {
		return mRadioDns;
	}

	void addBearer(RadioDnsEpgBearer bearer) {
		if(!mBearers.contains(bearer)) {
			mBearers.add(bearer);
		}
	}

	void addBearer(Bearer bearer) {
		switch (bearer.getBearerType()) {
			case BEARER_TYPE_DAB: {
				if (!bearer.getBearerIdString().isEmpty()) {
					mBearers.add(new RadioDnsEpgBearerDab(bearer.getBearerIdString(), bearer.getCost(), bearer.getMimeType(), bearer.getBitrate()));
				}
				break;
			}
			case BEARER_TYPE_HTTP:
			case BEARER_TYPE_HTTPS: {
				if (!bearer.getBearerIdString().isEmpty()) {
					mBearers.add(new RadioDnsEpgBearerIpHttp(bearer.getBearerIdString(), bearer.getCost(), bearer.getMimeType(), bearer.getBitrate()));
				}
				break;
			}
			default:
				break;
		}
	}

	public List<RadioDnsEpgBearer> getBearers() {
		return mBearers;
	}


	private List<RadioServiceRawAudiodataListener> mRawListeners = new ArrayList<>();
	@Override
	public void subscribe(RadioServiceListener radioServiceListener) {
		if(DEBUG)Log.d(TAG, "Subscribing RadioServiceListener: " + radioServiceListener);
		if(radioServiceListener != null) {
			if(radioServiceListener instanceof TextualMetadataListener) {
				if(!mLabelListeners.contains(radioServiceListener)) {
					if(DEBUG)Log.d(TAG, "Subscribing TextualMetadataListener: " + radioServiceListener);
					this.mLabelListeners.add((TextualMetadataListener) radioServiceListener);
				}
			}
			if(radioServiceListener instanceof VisualMetadataListener) {
				if(!mSlideshowListeners.contains(radioServiceListener)) {
					if(DEBUG)Log.d(TAG, "Subscribing VisualMetadataListener: " + radioServiceListener);
					this.mSlideshowListeners.add((VisualMetadataListener) radioServiceListener);
				}
			}
			if(radioServiceListener instanceof RadioServiceAudiodataListener) {
				if(!mAudiodataListeners.contains(radioServiceListener)) {
					if(DEBUG)Log.d(TAG, "Subscribing RadioServiceAudiodataListener: " + radioServiceListener);
					mDecodeAudio = true;
					this.mAudiodataListeners.add((RadioServiceAudiodataListener) radioServiceListener);
				}
			}
			if(radioServiceListener instanceof RadioServiceRawAudiodataListener) {
				if(!mRawListeners.contains(radioServiceListener)) {
					if(DEBUG)Log.d(TAG, "Adding RawAudioListener");
					mRawListeners.add((RadioServiceRawAudiodataListener)radioServiceListener);
				}
			}
			if(radioServiceListener instanceof ProgrammeServiceMetadataListener) {
				if(!mSpiListeners.contains(radioServiceListener)) {
					if(DEBUG)Log.d(TAG, "Subscribing ProgrammeServiceMetadataListener: " + radioServiceListener);
					this.mSpiListeners.add((ProgrammeServiceMetadataListener) radioServiceListener);
				}
			}
		}
	}

	void rawStreamData(byte[] streamData, RadioServiceMimeType mime) {
		for(RadioServiceRawAudiodataListener rawListener : mRawListeners) {
			//TODO check adts header for audio params
			rawListener.rawAudioData(streamData, false, false, mime, 2, 44100);
		}
	}

	@Override
	public boolean equals(Object obj) {
		if(obj != null) {
			if(obj instanceof RadioServiceIp) {
				RadioServiceIp compSrv = (RadioServiceIp) obj;
				return ((compSrv.getServiceLabel().equals(mServiceLabel)) && (compSrv.getIpStreams().size() == mStreams.size()));
			}
		}

		return false;
	}

	@Override
	public int hashCode() {
		if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			return Objects.hash(mServiceLabel, mStreams);
		} else {
			int hash = 31;
			hash = 67 * hash + this.mServiceLabel.hashCode();
			hash = 67 * hash + this.mStreams.hashCode();

			return hash;
		}
	}

	@Override
	public boolean equalsRadioService(RadioService otherSrv) {
		if(otherSrv != null) {
			if(otherSrv instanceof RadioServiceIp) {
				RadioServiceIp compSrv = (RadioServiceIp) otherSrv;
				return ((compSrv.getServiceLabel().equals(mServiceLabel)) && (compSrv.getIpStreams().size() == mStreams.size()));
			} else if(otherSrv instanceof RadioServiceDab) {
				RadioServiceDab dabSrv = (RadioServiceDab)otherSrv;
				for(RadioDnsEpgBearer bearer : mBearers) {
					if(bearer.getBearerType() == RadioDnsEpgBearerType.DAB) {
						RadioDnsEpgBearerDab dabBearer = (RadioDnsEpgBearerDab)bearer;
						if(dabBearer.getServiceId() == dabSrv.getServiceId() && dabBearer.getEnsembleEcc() == dabSrv.getEnsembleEcc()) {
							return true;
						}
					}
				}
			}
		}

		return false;
	}
}
