package org.omri.radio.impl;

import android.hardware.usb.UsbDevice;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import org.omri.radio.Radio;
import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceDab;
import org.omri.radioservice.RadioServiceDabComponent;
import org.omri.radioservice.RadioServiceDabUserApplication;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.TermId;
import org.omri.tuner.ReceptionQuality;
import org.omri.tuner.Tuner;
import org.omri.tuner.TunerListener;
import org.omri.tuner.TunerStatus;
import org.omri.tuner.TunerType;

import java.util.ArrayList;
import java.util.List;

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

public class TunerUsbImpl implements TunerUsb {

	private final String TAG = "TunerUsb";
	private final TunerType mTunertype = TunerType.TUNER_TYPE_DAB;

	private TunerStatus mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;
	private List<RadioService> mServices = new ArrayList<>();
	private List<RadioService> mScannedServices = new ArrayList<>();
	private boolean mIsScanning = false;
	private List<TunerListener> mTunerlisteners = new ArrayList<>();
	private RadioServiceDab mCurrentlyRunningService = null;

	private UsbDevice mUsbDevice = null;

	TunerUsbImpl(UsbDevice device) {
		mUsbDevice = device;
	}

	private boolean mRestoreServicesDone = false;
	private boolean mRestoreInProgress = false;
	private boolean mTunerInitDone = false;

	private boolean mHybridScanEnabled = false;

	@Override
	public void initializeTuner() {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_NOT_INITIALIZED) {
			if (DEBUG) Log.d(TAG, "Initializing Tuner");

			UsbHelper.getInstance().attachDevice(this);
			if(!mRestoreInProgress) {
				new RestoreServicesTask().execute();
				mRestoreInProgress = true;
			}
		}
	}

	@Override
	public UsbDevice getUsbDevice() {
		return mUsbDevice;
	}

	@Override
	public void suspendTuner() {
		UsbHelper.getInstance().stopService(mUsbDevice.getDeviceName());

		switch (mTunerStatus) {
			case TUNER_STATUS_SUSPENDED:
			case TUNER_STATUS_NOT_INITIALIZED:
				break;
			case TUNER_STATUS_SCANNING:
				stopRadioServiceScan();
			case TUNER_STATUS_INITIALIZED:
			case TUNER_STATUS_ERROR: {
				if(DEBUG)Log.d(TAG, "Suspending Tuner: " + this);
				mTunerStatus = TunerStatus.TUNER_STATUS_SUSPENDED;

				for (TunerListener listener : mTunerlisteners) {
					listener.tunerStatusChanged(this, mTunerStatus);
				}
				break;
			}

		}
	}

	@Override
	public void resumeTuner() {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_SUSPENDED) {
			mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;

			for (TunerListener listener : mTunerlisteners) {
				listener.tunerStatusChanged(this, mTunerStatus);
			}
		}
	}

	@Override
	public void deInitializeTuner() {

		switch (mTunerStatus) {
			case TUNER_STATUS_NOT_INITIALIZED:
				break;
			case TUNER_STATUS_SCANNING:
				stopRadioServiceScan();
			case TUNER_STATUS_SUSPENDED:
			case TUNER_STATUS_ERROR:
			case TUNER_STATUS_INITIALIZED: {
				if(UsbHelper.getInstance() != null) {
					UsbHelper.getInstance().stopService(mUsbDevice.getDeviceName());
					UsbHelper.getInstance().removeDevice(mUsbDevice);
				}

				mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;
				for (TunerListener listener : mTunerlisteners) {
					listener.tunerStatusChanged(this, mTunerStatus);
				}
			}
		}
	}

	@Override
	public TunerType getTunerType() {
		return mTunertype;
	}

	@Override
	public TunerStatus getTunerStatus() {
		return mTunerStatus;
	}

	@Override
	public List<RadioService> getRadioServices() {
		if(DEBUG)Log.d(TAG, "getting Services at TunerStatus: " + mTunerStatus.toString());
		if(mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			return RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_DAB);
		} else {
			return new ArrayList<>();
		}
	}

	@Override
	public void startRadioServiceScan() {
		UsbHelper.getInstance().startEnsembleScan(mUsbDevice.getDeviceName());
		//TODO scanning without deleting old services
		//mServices.clear();
		mScannedServices.clear();
	}

	@Override
	public void startRadioServiceScan(Object scanOptions) {
		if(scanOptions != null) {
			if(scanOptions instanceof Bundle) {
				if(((Bundle)scanOptions).getBoolean(RadioImpl.SERVICE_SEARCH_OPT_DELETE_SERVICES, false)) {
					if(DEBUG)Log.d(TAG, "Clearing existing services before new scan");
					RadioServiceManager.getInstance().clearServiceList(RadioServiceType.RADIOSERVICE_TYPE_DAB);
				}
				if(((Bundle)scanOptions).getBoolean(RadioImpl.SERVICE_SEARCH_OPT_HYBRID_SCAN, false)) {
					mHybridScanEnabled = true;
				}
			}
		}

		startRadioServiceScan();
	}

	@Override
	public void stopRadioServiceScan() {
		UsbHelper.getInstance().stopEnsembleScan(mUsbDevice.getDeviceName());
	}

	@Override
	public void startRadioService(RadioService radioService) {
		if(DEBUG)Log.d(TAG, "Starting Service: " + radioService.getServiceLabel());

		if(radioService.getRadioServiceType() == RadioServiceType.RADIOSERVICE_TYPE_DAB) {
			UsbHelper.getInstance().startService(mUsbDevice.getDeviceName(), (RadioServiceDab) radioService);
		}
	}

	@Override
	public void stopRadioService() {
		UsbHelper.getInstance().stopService(mUsbDevice.getDeviceName());
	}

	@Override
	public RadioService getCurrentRunningRadioService() {
		return mCurrentlyRunningService;
	}

	@Override
	public void subscribe(TunerListener tunerListener) {
		if(!mTunerlisteners.contains(tunerListener)) {
			mTunerlisteners.add(tunerListener);
		}
	}

	@Override
	public void unsubscribe(TunerListener tunerListener) {
		mTunerlisteners.remove(tunerListener);
	}

	@Override
	public void callBack(int callbackType) {

		if(callbackType == 5) {
			if(mTunerStatus != TunerStatus.TUNER_STATUS_INITIALIZED) {
				mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
				for (TunerListener listener : mTunerlisteners) {
					listener.tunerStatusChanged(this, mTunerStatus);
				}
			}
		} else {
			TunerUsbCallbackTypes type = TunerUsbCallbackTypes.getTypeByValue(callbackType);
			if(DEBUG)Log.d(TAG, "Native callback for device: " + mUsbDevice.getDeviceName() + " with CallbackType: " + type.toString());
			switch(type) {
				case TUNER_READY: {
					if (!mIsScanning) {
						mTunerInitDone = true;
						if(mRestoreServicesDone) {
							mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
							for (TunerListener listener : mTunerlisteners) {
								listener.tunerStatusChanged(this, mTunerStatus);
							}
						}
					} else {
						mIsScanning = false;
						mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;

						new EnrichServicesData().execute();
					}
					break;
				}
				case TUNER_FAILED: {
					break;
				}
				case TUNER_FREQUENCY_LOCKED: {
					break;
				}
				case TUNER_FREQUENCY_NOT_LOCKED: {
					break;
				}
				case TUNER_SCAN_IN_PROGRESS: {
					mIsScanning = true;
					mTunerStatus = TunerStatus.TUNER_STATUS_SCANNING;
					for (TunerListener listener : mTunerlisteners) {
						listener.tunerScanStarted(this);
					}
					break;
				}
				default: {
					break;
				}
			}
		}
	}

	@Override
	public void scanProgressCallback(int percentDone) {
		if(DEBUG)Log.d(TAG, "Scan Progress: " + percentDone);
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanProgress(this, percentDone);
		}
	}

	@Override
	public void serviceFound(RadioServiceDab service) {
		if(DEBUG) {
			Log.d(TAG, "Scan New Service found Ensemble Freq: " + service.getEnsembleFrequency());
			Log.d(TAG, "Scan New Service found EnsembleId: " + Integer.toHexString(service.getEnsembleId()));
			Log.d(TAG, "Scan New Service found Ensemblelabel: " + service.getEnsembleLabel() + " : " + service.getEnsembleShortLabel());
			Log.d(TAG, "Scan New Service found EnsembleECC: " + ("0x" + Integer.toHexString(service.getEnsembleEcc()).toUpperCase()));
			Log.d(TAG, "Scan New Service found ServiceID: " + ("0x" + Integer.toHexString(service.getServiceId())));
			Log.d(TAG, "Scan New Service found ServiceLabel: " + service.getServiceLabel() + " : " + service.getShortLabel());
			Log.d(TAG, "Scan New Service found isProgramme: " + service.isProgrammeService());
			Log.d(TAG, "Scan New Service Num Components: " + service.getServiceComponents().size());
			for (RadioServiceDabComponent comp : service.getServiceComponents()) {
				Log.d(TAG, "Scan New Service Component SubChanID: " + ("0x" + Integer.toHexString(comp.getSubchannelId())));
				Log.d(TAG, "Scan New Service Component Type: " + comp.getServiceComponentType());
				Log.d(TAG, "Scan New Service Component Bitrate: " + comp.getBitrate());
				Log.d(TAG, "Scan New Service Component TMID: " + comp.getTmId());
				Log.d(TAG, "Scan New Service Component PacketAddress: " + comp.getPacketAddress());

				for (RadioServiceDabUserApplication uApp : comp.getUserApplications()) {
					Log.d(TAG, "Scan New Service Component UApp Type: " + uApp.getType().getType() + " : " + uApp.getType().getName());
					Log.d(TAG, "Scan New Service Component UApp DSCTy: " + uApp.getDataServiceComponentType().getType() + " : " + uApp.getDataServiceComponentType().getName());
					Log.d(TAG, "Scan New Service Component UApp isXPAD: " + uApp.isXpadApptype() + ", DGUsed: " + uApp.isDatagroupTransportUsed());
				}
			}
			for (TermId tid : service.getGenres()) {
				Log.d(TAG, "Scan New Service Genre: " + tid.getText());
			}
			Log.d(TAG, "----------------- Scan New Service found -----------------");
		}

		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanServiceFound(this, service);
		}

		RadioServiceManager.getInstance().addService(service);
	}

	@Override
	public void serviceStarted(RadioServiceDab startedService) {
		if(DEBUG)Log.d(TAG, "DabService started: " + startedService.getServiceLabel());
		mCurrentlyRunningService = startedService;
		if(mCurrentlyRunningService != null) {
			for (TunerListener listener : mTunerlisteners) {
				listener.radioServiceStarted(this, mCurrentlyRunningService);
			}
		}
	}

	@Override
	public void serviceStopped(RadioServiceDab stoppedService) {
		if(DEBUG)Log.d(TAG, "DabService stopped: " + stoppedService.getServiceLabel());
		for(TunerListener listener : mTunerlisteners) {
			listener.radioServiceStopped(this, stoppedService);
			((RadioServiceImpl) stoppedService).serviceStopped();
		}
		mCurrentlyRunningService = null;
	}

	@Override
	public void receptionStatistics(boolean rfLocked, int qualLevel) {
		//if(DEBUG)Log.d(TAG, "Reception Statistics RFLock: " + rfLocked + " Qual: " + qualLevel);

		if(qualLevel > 5) {
			qualLevel = 5;
		}
		for(TunerListener cb : mTunerlisteners) {
			cb.tunerReceptionStatistics(this, rfLocked, ReceptionQuality.values()[qualLevel]);
		}
	}

	private class SerializeServicesTask extends AsyncTask<Void, Void, Void> {

		private final Tuner mInstance;

		SerializeServicesTask(Tuner instance) {
			mInstance = instance;
		}

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected Void doInBackground(Void... voids) {
			RadioServiceManager.getInstance().serializeServices(RadioServiceType.RADIOSERVICE_TYPE_DAB);

			if(mHybridScanEnabled) {
				if(DEBUG)Log.d(TAG, "Enrich Services");
				IpServiceScanner.getInstance().enrichServices(RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_DAB));
			}

			mHybridScanEnabled = false;
			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			if(mInstance != null) {
				for (TunerListener listener : mTunerlisteners) {
					mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
					listener.tunerStatusChanged(mInstance, mTunerStatus);
					listener.tunerScanFinished(mInstance);
				}
			}
		}
	}

	private class RestoreServicesTask extends AsyncTask<Void, Void, Void> {

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected Void doInBackground(Void... params) {
			if(DEBUG)Log.d(TAG, "Restoring services....");
			while (!RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_DAB) || !VisualLogoManager.getInstance().isReady()) {
				try {
					Thread.sleep(10);
					if(DEBUG)Log.d(TAG, "Waiting for servicelist or VisualLogoManager to be ready");
				} catch(InterruptedException interExc) {
					if(DEBUG)interExc.printStackTrace();
				}
			}

			mServices = RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_DAB);
			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			super.onPostExecute(aVoid);
			if(DEBUG)Log.d(TAG, "Restore services finished");

			mRestoreServicesDone = true;
			mRestoreInProgress = false;
			if(mTunerInitDone) {
				callBack(5);
			}
		}
	}

	void servicesUpdated() {
		new SerializeServicesTask(this).execute();
	}

	private class EnrichServicesData extends AsyncTask<Void, Void, Void> {

		@Override
		protected Void doInBackground(Void... voids) {
			for(Tuner ipTuner : Radio.getInstance().getAvailableTuners()) {
				if(ipTuner.getTunerType() == TunerType.TUNER_TYPE_IP_SHOUTCAST) {
					for(RadioService ipSrv : ipTuner.getRadioServices()) {
						for (RadioService dabSrv : mServices) {
							if (ipSrv.equals(dabSrv)) {
								if (!ipSrv.getLogos().isEmpty()) {
									((RadioServiceDabImpl) dabSrv).addLogo(ipSrv.getLogos());
								}
							}
						}
					}

					break;
				}
			}
			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			afterEnrich();
		}
	}

	private void afterEnrich() {
		new SerializeServicesTask(this).execute();
	}
}
