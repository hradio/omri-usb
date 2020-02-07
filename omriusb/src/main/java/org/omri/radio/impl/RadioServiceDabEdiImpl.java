package org.omri.radio.impl;

import android.util.Log;

import org.omri.BuildConfig;
import org.omri.radioservice.RadioServiceDabEdi;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.SbtItem;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

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

public class RadioServiceDabEdiImpl extends RadioServiceDabImpl implements RadioServiceDabEdi, Serializable {

	private final static String TAG = "RadioServiceDabEdiImpl";

	private static final long serialVersionUID = 7627546102669976593L;

	private String mUrl;

	private boolean mIsSbt = false;
	private long mSbtMax = 0;
	private ArrayList<SbtCallback> mSbtCallbacks = new ArrayList<>();
	private ArrayList<SbtControlCallback> mSbtCtrlCallbacks = new ArrayList<>();
	private HashMap<Long, SbtItem> mSbtItems = new HashMap<Long, SbtItem>();
	ArrayList<Timer> mDelItemsTasks = new ArrayList<>();

	private String mTimeshiftToken = null;
	private String mInitialTimeshiftToken = null;
	private long mInitialOffset = -1;
	private long mInitialToggleId = -1;
	private long mInitialUts = -1;

	public RadioServiceDabEdiImpl(String url) {
		mUrl = url;
	}

	@Override
	public RadioServiceType getRadioServiceType() {
		return RadioServiceType.RADIOSERVICE_TYPE_EDI;
	}

	@Override
	public String getUrl() {
		return mUrl;
	}

	@Override
	public int getEnsembleFrequency() {
		return 0;
	}

	@Override
	public long getSbtMax() {
		return mSbtMax;
	}

	@Override
	public void seekSbt(long ms) {
		for(SbtControlCallback cb : mSbtCtrlCallbacks) {
			cb.seekMs(ms);
		}
	}

	@Override
	public void setToggleSbt(long toggleId) {
		for(SbtItem item : mSbtItems.values()) {
			if(item.getId() == toggleId) {
				for(SbtControlCallback cb : mSbtCtrlCallbacks) {
					cb.setToggle(toggleId);
				}

				break;
			}
		}
	}

	@Override
	public void pauseSbt(boolean pause) {
		for(SbtControlCallback cb : mSbtCtrlCallbacks) {
			cb.pause(pause);
		}
	}

	@Override
	public List<SbtItem> getSbtItems() {
		if(BuildConfig.DEBUG) Log.d(TAG, "SBT returning " + mSbtItems.size() + " items");
		ArrayList<SbtItem> sbtVals = new ArrayList<>(mSbtItems.values());
		Collections.sort(sbtVals, new Comparator<SbtItem>() {
			@Override
			public int compare(SbtItem o1, SbtItem o2) {
				return o1.getPosixTime() < o2.getPosixTime() ? -1
						: o1.getPosixTime() > o2.getPosixTime() ? 1
						: 0;
			}
		});

		return new ArrayList<>(mSbtItems.values());
	}

	@Override
	public String getSbtToken() {
		return mTimeshiftToken;
	}

	@Override
	public boolean sbtEnabled() {
		return mIsSbt;
	}

	@Override
	public void setInitialSbtToken(String token) {
		mInitialTimeshiftToken = token;
	}

	@Override
	public void setInitialSbtOffset(long offset) {
		mInitialOffset = offset;
	}

	@Override
	public void setInitialSbtToggleId(long toggleId) {
		mInitialToggleId = toggleId;
	}

	//TODO add to OMRI_BASE
	@Override
	public void setInitialTimePosix(long posixSeconds) {
		mInitialUts = posixSeconds;
	}

	/* callback to clients */
	@Override
	public void addSbtCallback(SbtCallback callback) {
		if (!mSbtCallbacks.contains(callback)) {
			mSbtCallbacks.add(callback);
		}
	}

	@Override
	public void removeSbtCallback(SbtCallback callback) {
		mSbtCallbacks.remove(callback);
	}

	String getInitialTimeshiftToken() {
		return mInitialTimeshiftToken;
	}

	long getInitialOffset() {
		return mInitialOffset;
	}

	long getInitialToggleId() {
		return mInitialToggleId;
	}

	long getInitialTimePosix() {
		return mInitialUts;
	}

	void setSbtToken(String token) {
		mTimeshiftToken = token;
	}

	void setIsSbt(boolean isSbt) {
		mIsSbt = isSbt;
	}

	void setSbtMax(long sbtMax) {
		mSbtMax = sbtMax;
	}

	@Override
	void serviceStopped() {
		super.serviceStopped();

		mSbtItems.clear();
		mSbtCallbacks.clear();
		mSbtCtrlCallbacks.clear();
		mSbtMax = 0;
		mIsSbt = false;

		if(BuildConfig.DEBUG)Log.d(TAG, "Clearing " + mDelItemsTasks.size() + " pending DelTimer tasks");
		for(int i = 0; i < mDelItemsTasks.size(); i++) {
			mDelItemsTasks.get(i).cancel();
		}
	}

	@Override
	public long getRealtimePosixMs() {
		return RadioImpl.getNtpPosixMs();
	}

	void setToggleItems(List<SbtItem> items) {
		for(SbtItem item : items) {
			long delTime = item.getPosixTime() + mSbtMax;
			long nowTime = RadioImpl.getNtpPosixMs();

			if(delTime < nowTime) {
				if(BuildConfig.DEBUG)Log.e(TAG, "No DelTimer for past item, Now: " + new Date(nowTime).toString() + ", DelTime: " + new Date(delTime).toString());
			} else {

				mSbtItems.put(item.getPosixTime(), item);

				addDelTimer(item);
			}
		}
	}

	private void addDelTimer(SbtItem item) {
		long delTime = item.getPosixTime() + mSbtMax;
		long nowTime = RadioImpl.getNtpPosixMs();
		long delDelay = delTime - nowTime;

		Date delDate = new Date(delTime);
		Date nowDate = new Date(nowTime);
		if(BuildConfig.DEBUG)Log.d(TAG, "Adding DelTimer at NowDate: " + nowDate.toString());
		if(BuildConfig.DEBUG)Log.d(TAG, "Adding DelTimer at DelDate: " + delDate.toString());
		if(BuildConfig.DEBUG)Log.d(TAG, "Adding DelTimer at Delay: "  + delDelay + ", " + (delDelay/1000/60) + " Min, " + ((delDelay/1000)%60) + " sec");
		if(BuildConfig.DEBUG)Log.d(TAG, " ----- Adding DelTimer -----");

		if(delDelay >= 0) {
			Timer delTimer = new Timer();
			delTimer.schedule(new TimerTask() {
				@Override
				public void run() {
					if(BuildConfig.DEBUG)Log.d(TAG, "Executing DelTimer for: " + item.getDls().getText());

					for(SbtCallback cb : mSbtCallbacks) {
						cb.sbtItemInvalid(item);
					}

					if(BuildConfig.DEBUG)Log.d(TAG, "Executing DelTimer for: " + item.getDls().getText() + ", prevSize: " + mSbtItems.size());
					mSbtItems.remove(item.getPosixTime());

					if(BuildConfig.DEBUG)Log.d(TAG, "Executed DelTimer for: " + item.getDls().getText() + ", prevSize: " + mSbtItems.size());
					mDelItemsTasks.remove(delTimer);
				}
			}, delDelay);

			mDelItemsTasks.add(delTimer);
		} else {
			if(BuildConfig.DEBUG)Log.d(TAG, "NOT Adding DelTimer at: " +delDelay);
		}
	}

	void addOrUpdateItem(SbtItem item) {
		long delTime = item.getPosixTime() + mSbtMax;
		long nowTime = RadioImpl.getNtpPosixMs();

		if(delTime < nowTime) {
			if(BuildConfig.DEBUG)Log.e(TAG, "No DelTimer for past item, Now: " + new Date(nowTime).toString() + ", DelTime: " + new Date(delTime).toString());
		} else {
			if (!mSbtItems.containsKey(item.getPosixTime())) {
				addDelTimer(item);
			}

			mSbtItems.put(item.getPosixTime(), item);

			for (SbtCallback cb : mSbtCallbacks) {
				cb.sbtItemAdded(item);
			}
		}
	}

	void updateDabTime(long dabPosix) {
		for(SbtCallback cb : mSbtCallbacks) {
			cb.streamDabTime(dabPosix);
		}
	}

	/* callback to EDI tuner */
	void addSbtControlCallback(SbtControlCallback callback) {
		if (!mSbtCtrlCallbacks.contains(callback)) {
			mSbtCtrlCallbacks.add(callback);
		}
	}

	void removeSbtControlCallback(SbtControlCallback callback) {
		mSbtCtrlCallbacks.remove(callback);
	}

	interface SbtControlCallback {

		void pause(boolean pause);

		void seekMs(long ms);

		void setToggle(long toggleId);
	}
}
