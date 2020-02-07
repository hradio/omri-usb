package org.omri.radio.impl;

import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceDabEdi;
import org.omri.radioservice.RadioServiceIp;
import org.omri.radioservice.RadioServiceIpStream;
import org.omri.radioservice.RadioServiceMimeType;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.SbtItem;
import org.omri.radioservice.metadata.TextualDabDynamicLabelPlusItem;
import org.omri.radioservice.metadata.VisualMimeType;
import org.omri.tuner.Tuner;
import org.omri.tuner.TunerListener;
import org.omri.tuner.TunerStatus;
import org.omri.tuner.TunerType;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ConnectException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static org.omri.BuildConfig.DEBUG;
import static org.omri.tuner.TunerStatus.TUNER_STATUS_SCANNING;

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

public class TunerEdistream implements Tuner, IpServiceScanner.IpScannerListener, RadioServiceDabEdiImpl.SbtControlCallback {

	private final static String TAG = "TunerEdistream";

	private TunerStatus mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;

	private List<TunerListener> mTunerlisteners = Collections.synchronizedList(new ArrayList<TunerListener>());

	private RadioServiceDabEdi mRunningSrv = null;

	private boolean mIsInitializing = false;

	TunerEdistream() {
		IpServiceScanner.getInstance().addScanListener(this);
	}

	@Override
	public void initializeTuner() {
		if(DEBUG)Log.d(TAG, "Initializing Tuner");
		if(mTunerStatus == TunerStatus.TUNER_STATUS_NOT_INITIALIZED && !mIsInitializing) {
			mIsInitializing = true;
			new RestoreServicesTask().execute();
		}
	}

	@Override
	public void suspendTuner() {
		switch (mTunerStatus) {
			case TUNER_STATUS_SUSPENDED:
			case TUNER_STATUS_NOT_INITIALIZED:
				break;
			case TUNER_STATUS_SCANNING:
				stopRadioServiceScan();
			case TUNER_STATUS_INITIALIZED:
			case TUNER_STATUS_ERROR: {
				if(DEBUG)Log.d(TAG, "Suspending Tuner: " + this);
				stopHttpThread();

				mTunerStatus = TunerStatus.TUNER_STATUS_SUSPENDED;

				for (TunerListener listener : mTunerlisteners) {
					listener.tunerStatusChanged(this, mTunerStatus);
				}

				mIsInitializing = false;

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

			mIsInitializing = false;
		}
	}

	@Override
	public void deInitializeTuner() {
		switch (mTunerStatus) {
			case TUNER_STATUS_NOT_INITIALIZED:
				break;
			case TUNER_STATUS_SCANNING:
				stopRadioServiceScan();
			case TUNER_STATUS_ERROR:
			case TUNER_STATUS_INITIALIZED:
			case TUNER_STATUS_SUSPENDED: {
				stopHttpThread();

				if(UsbHelper.getInstance() != null) {
					UsbHelper.getInstance().ediStreamTunerDetached(this);
				}

				mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;
				for (TunerListener listener : mTunerlisteners) {
					listener.tunerStatusChanged(this, mTunerStatus);
				}

				break;
			}
		}
	}

	@Override
	public TunerType getTunerType() {
		return TunerType.TUNER_TYPE_IP_EDI;
	}

	@Override
	public TunerStatus getTunerStatus() {
		return mTunerStatus;
	}

	@Override
	public List<RadioService> getRadioServices() {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			return RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_EDI);
		}

		return new ArrayList<>();
	}

	private ConcurrentLinkedQueue<RadioServiceDabEdiImpl> mScanSrvQ = new ConcurrentLinkedQueue<>();

	@Override
	public void startRadioServiceScan() {
		startRadioServiceScan(null);
	}

	@Override
	public void startRadioServiceScan(Object scanOptions) {
		if (DEBUG) Log.d(TAG, "Starting Service scan " + (scanOptions != null ? "with options bundle": "without options bundle") + " at Tunerstate: " + mTunerStatus.toString());
		if (mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			mTunerStatus = TunerStatus.TUNER_STATUS_SCANNING;
			mScanSrvQ.clear();

			boolean useHradioSearch = false;
			Bundle optBundle = null;

			if (scanOptions instanceof Bundle) {
				optBundle = (Bundle) scanOptions;
				useHradioSearch = optBundle.getBoolean(RadioImpl.SERVICE_SEARCH_OPT_USE_HRADIO, false);

				if(optBundle.getBoolean(RadioImpl.SERVICE_SEARCH_OPT_DELETE_SERVICES, false)) {
					if(DEBUG)Log.d(TAG, "Clearing existing services before new scan");
					RadioServiceManager.getInstance().clearServiceList(RadioServiceType.RADIOSERVICE_TYPE_EDI);
				}
			}

			if(!IpServiceScanner.getInstance().isScanning()) {
				if(useHradioSearch) {
					IpServiceScanner.getInstance().scanServices(optBundle);
				} else {
					IpServiceScanner.getInstance().scanServices(null);
				}
			} else {
				if(DEBUG)Log.d(TAG, "Scan is already running from another tuner");
				this.scanStarted();
			}
		}
	}


	@Override
	public void scanStarted() {
		if (DEBUG) Log.d(TAG, "Service scan started at Tunerstate: " + mTunerStatus.toString());
		if(mTunerStatus == TUNER_STATUS_SCANNING) {
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerStatusChanged(this, mTunerStatus);
				listener.tunerScanStarted(this);
			}
		}
	}

	@Override
	public void scanProgress(final int percent) {
		if (DEBUG) Log.d(TAG, "Service scan progress: " + percent + " at Tunerstate: " + mTunerStatus.toString());
		if(mTunerStatus == TUNER_STATUS_SCANNING) {
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerScanProgress(this, percent);
			}
		}
	}

	@Override
	public void foundStreamingServices(final List<RadioService> ipStreamservices) {
		if (DEBUG) Log.d(TAG, "Scan found " + ipStreamservices.size() + " services, at Tunerstate: " + mTunerStatus.toString());
		if(mTunerStatus == TUNER_STATUS_SCANNING) {
			for(RadioService foundSrv : ipStreamservices) {
				if(foundSrv.getRadioServiceType() == RadioServiceType.RADIOSERVICE_TYPE_IP) {
					if (DEBUG) Log.d(TAG, "Scan searching EDI streams at Service: " + foundSrv.getServiceLabel());
					RadioServiceIp ipSrv = (RadioServiceIp)foundSrv;
					for(RadioServiceIpStream stream : ipSrv.getIpStreams()) {
						if(stream.getMimeType() == RadioServiceMimeType.AUDIO_EDI) {
							if(DEBUG)Log.d(TAG, "Found EdiStream Service: " + ipSrv.getServiceLabel() + " at: " + stream.getUrl());
							RadioServiceDabEdiImpl scanSrv = new RadioServiceDabEdiImpl(stream.getUrl());
							scanSrv.setServiceLabel(ipSrv.getServiceLabel());
							scanSrv.addLogo(ipSrv.getLogos());
							scanSrv.addGenre(ipSrv.getGenres());
							scanSrv.addKeyword(ipSrv.getKeywords());
							scanSrv.addLink(ipSrv.getLinks());
							scanSrv.addLocation(ipSrv.getLocations());
							mScanSrvQ.offer(scanSrv);
							break;
						}
					}
				} else {
					if (DEBUG) Log.d(TAG, "Scan found Service: " + foundSrv.getServiceLabel() + " is no IP service");
				}
			}
		}
	}

	@Override
	public void scanFinished() {
		if (DEBUG) Log.d(TAG, "Service scan finished: " + mTunerStatus.toString());
		if (mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {

			push(new Runnable() {
				@Override
				public void run() {
					if(DEBUG) {
						RadioServiceDabEdiImpl nextSrv = mScanSrvQ.peek();
						if (nextSrv != null) {
							Log.d(TAG, "EdiScan nextSrv: " + nextSrv.getServiceLabel() + " : " + nextSrv.getUrl());
						} else {
							Log.d(TAG, "EdiScan nextSrv is null");
						}
					}
					if(DEBUG)Log.d(TAG, "Adding UpdateServiceTask Q: " + mScanSrvQ.size());
					startRadioService(mScanSrvQ.poll());
				}
			});
		}
	}

	private ExecutorService executor = Executors.newSingleThreadExecutor();

	private void push(Runnable task) {
		executor.execute(task);
	}

	private void updateScanServices() {
		if(DEBUG)Log.d(TAG, "Updating services remaining: " + mScanSrvQ.size());
		if(!mScanSrvQ.isEmpty()) {
			push(new Runnable() {
				@Override
				public void run() {
					if(DEBUG) {
						RadioServiceDabEdiImpl nextSrv = mScanSrvQ.peek();
						if (nextSrv != null) {
							Log.d(TAG, "EdiScan nextSrv: " + nextSrv.getServiceLabel() + " : " + nextSrv.getUrl());
						} else {
							Log.d(TAG, "EdiScan nextSrv is null");
						}
					}
					startRadioService(mScanSrvQ.poll());
				}
			});
		}
	}

	private void updateTunerListenerScanServiceFound(RadioService srv) {
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanServiceFound(this, srv);
		}
	}

	private void updateTunerListenerScanStatus(int left) {
		if(DEBUG)Log.d(TAG, "updateTunerListenerScanStatus: " + left);
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanProgress(this, left);
		}
	}

	@Override
	public void stopRadioServiceScan() {
		if(DEBUG)Log.d(TAG, "Stopping ServiceScan, remaining Q: " + mScanSrvQ.size() + ", TunerState: " + mTunerStatus.toString());

		if(!mScanSrvQ.isEmpty()) {
			if(DEBUG)Log.d(TAG, "stopRadioServiceScan called but scanQ is not empty");
			return;
		}

		if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			RadioServiceManager.getInstance().serializeServices(RadioServiceType.RADIOSERVICE_TYPE_EDI);

			mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerScanProgress(this, 100);
				listener.tunerScanFinished(this);
				listener.tunerStatusChanged(this, mTunerStatus);
			}

			IpServiceScanner.getInstance().stopScan();
		}
	}

	@Override
	public void startRadioService(RadioService radioService) {
		if(radioService != null) {
			if (radioService.getRadioServiceType() == RadioServiceType.RADIOSERVICE_TYPE_EDI) {
				if (DEBUG) Log.d(TAG + "_" + this, "Starting EdiService from: " + ((RadioServiceDabEdi) radioService).getUrl() + " and Tunerstatus is: " + mTunerStatus.toString());

				mConRetries = 5;

				if (mRunningSrv != null) {
					if (DEBUG) Log.d(TAG + "_" + this, "Starting EdiService but stopping old service before");
					stopHttpThread();
				}

				mRunningSrv = (RadioServiceDabEdi) radioService;
				UsbHelper.getInstance().startEdiStreamService(this, mRunningSrv);
				startHttphread();
			}
		} else {
			if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
				if(DEBUG)Log.d(TAG, "Stopping ServiceScan at null startService");
				stopRadioServiceScan();
			}
		}
	}

	@Override
	public void stopRadioService() {
		if(DEBUG)Log.d(TAG, "stopRadioService called");
		mTimeshiftToken = null;
		mMaxTimeshift = 0;
		mSbtAvailable = false;
		mIsReconnecting = false;
		stopHttpThread();
	}

	/* native callbacks */
	private void serviceStarted(RadioServiceDabEdi startedService) {
		if(DEBUG)Log.d(TAG + "_" + this, "DabEdiService started: " + startedService.getServiceLabel() + " TunerStatus: " + mTunerStatus.toString());
		if(mTunerStatus != TUNER_STATUS_SCANNING) {
			mConRetries = 5;
			if(DEBUG)Log.d(TAG + "_" + this, "Calling " + mTunerlisteners + " Subscribed listeners for serviceStarted");
			for (TunerListener listener : mTunerlisteners) {
				if(DEBUG)Log.d(TAG + "_" + this, "StartedServiceCallback: " + listener);
				listener.radioServiceStarted(this, startedService);
			}
		}
	}

	private void serviceStopped(RadioServiceDabEdi stoppedService) {
		if(DEBUG)Log.d(TAG + "_" + this, "DabEdiService stopped: " + stoppedService.getServiceLabel());
		if(stoppedService != null) {
			if(!mIsReconnecting) {
				((RadioServiceDabEdiImpl) stoppedService).removeSbtControlCallback(this);
				if (mTunerStatus != TUNER_STATUS_SCANNING) {
					for (TunerListener listener : mTunerlisteners) {
						listener.radioServiceStopped(this, stoppedService);
						((RadioServiceImpl) stoppedService).serviceStopped();
					}
				}
			}
		}
	}

	//called from native
	private void serviceUpdated(RadioServiceDabEdi srv) {
		if(mTunerStatus == TUNER_STATUS_SCANNING) {
			stopHttpThread();

			if(srv != null) {
				if(DEBUG)Log.d(TAG, "DabEdiService updated: " + srv.getServiceLabel() + " at URL: " + srv.getUrl());
				RadioServiceManager.getInstance().addService(srv);
				updateTunerListenerScanServiceFound(srv);
			} else {
				if(DEBUG)Log.d(TAG, "Updated service is null");
			}

			if(!mScanSrvQ.isEmpty()) {
				push(new Runnable() {
					@Override
					public void run() {
						if(DEBUG)Log.d(TAG, "Adding UpdateServiceTask");
						if(DEBUG) {
							RadioServiceDabEdiImpl nextSrv = mScanSrvQ.peek();
							if (nextSrv != null) {
								Log.d(TAG, "EdiScan nextSrv: " + nextSrv.getServiceLabel() + " : " + nextSrv.getUrl());
							} else {
								Log.d(TAG, "EdiScan nextSrv is null");
							}
						}
						startRadioService(mScanSrvQ.poll());
					}
				});
			} else {
				stopRadioServiceScan();
			}
		}
	}

	private boolean isTunerScanning() {
		if(DEBUG)Log.d(TAG, "Getting TunerScanStatus: " + mTunerStatus.toString());
		if(mTunerStatus == TUNER_STATUS_SCANNING) {
			return true;
		}

		return false;
	}

	@Override
	public RadioService getCurrentRunningRadioService() {
		return mRunningSrv;
	}

	@Override
	public void subscribe(TunerListener tunerListener) {
		if(!mTunerlisteners.contains(tunerListener)) {
			boolean added = mTunerlisteners.add(tunerListener);
			if(DEBUG)Log.d(TAG + "_" + this, "Subscribe: " + tunerListener + " : " + added);
		}
	}

	@Override
	public void unsubscribe(TunerListener tunerListener) {
		boolean removed = mTunerlisteners.remove(tunerListener);
		if(DEBUG)Log.d(TAG + "_" + this, "UnSubscribe: " + tunerListener + " : " + removed);
	}

	private int mConRetries = 5;
	private void restartStream() {
		if(mRunningSrv != null) {
			--mConRetries;
			stopHttpThread();
			startHttphread();
		}
	}

	private Thread mHttpThread = null;
	private volatile boolean mHttpThreadRunning = false;
	private void startHttphread() {
		if(!mHttpThreadRunning) {
			mHttpThread = new Thread(HttpRunnable);
			mHttpThread.start();
		}

	}

	private void stopHttpThread() {
		if(DEBUG)Log.d(TAG + "_" + this, "Stopping HttpThread");
		if(mHttpThreadRunning) {
			mHttpThreadRunning = false;

			if(mHttpThread != null) {
				if(mHttpThread.isAlive()) {
					mHttpThread.interrupt();
					try {
						if(DEBUG)Log.d(TAG, "Joining httpThread...");
						mHttpThread.join(2000);
						if(DEBUG)Log.d(TAG, "Joining httpThread done");
						serviceStopped(mRunningSrv);
					} catch(InterruptedException iErr) {
						if(DEBUG)iErr.printStackTrace();
						if(mTunerStatus != TUNER_STATUS_SCANNING) {
							serviceStopped(mRunningSrv);
						} else {
							if(DEBUG)Log.d(TAG,"calling serviceUpdated()");
							serviceUpdated(null);
						}
					}
				}
			}
			mHttpThread = null;
		} else {
			if(DEBUG) Log.d(TAG, "httpThread already stopped");
		}
	}

	private Runnable HttpRunnable = new Runnable() {
		@Override
		public void run() {
			mHttpThreadRunning = true;
			getEdistream();
		}
	};

	private String mTimeshiftToken;
	private long mMaxTimeshift;
	private boolean mSbtAvailable = false;
	private boolean mIsReconnecting = false;

	private void getEdistream() {
		mSbtAvailable = false;

		InputStream mInput = null;
		int httpResponseCode = -1;

		HttpURLConnection mHttpStreamConnection = null;

		try {
			URL ediUrl;
			if( (mIsReconnecting && mTimeshiftToken != null && mTimeshiftToken.length() > 0)) {
				String tokenUrl = mRunningSrv.getUrl().endsWith("/") ? (mRunningSrv.getUrl().substring(0, mRunningSrv.getUrl().length()-1) + "?timeshiftToken=" + mTimeshiftToken) : (mRunningSrv.getUrl() + "?timeshiftToken=" + mTimeshiftToken);
				if(DEBUG)Log.d(TAG, "Reconnecting with TimeshiftToken: " + mTimeshiftToken + ", at URL: " + tokenUrl);
				ediUrl = new URL(tokenUrl);
			} else if(((RadioServiceDabEdiImpl)mRunningSrv).getInitialTimeshiftToken() != null) {
				String tokenUrl = mRunningSrv.getUrl().endsWith("/") ? (mRunningSrv.getUrl().substring(0, mRunningSrv.getUrl().length()-1) + "?timeshiftToken=" + ((RadioServiceDabEdiImpl)mRunningSrv).getInitialTimeshiftToken()) : (mRunningSrv.getUrl() + "?timeshiftToken=" + ((RadioServiceDabEdiImpl)mRunningSrv).getInitialTimeshiftToken());
				if(DEBUG)Log.d(TAG, "Connecting with InitSbt TimeshiftToken: " + mTimeshiftToken + ", at URL: " + tokenUrl);
				ediUrl = new URL(tokenUrl);
			} else if(((RadioServiceDabEdiImpl)mRunningSrv).getInitialTimePosix() > 0) {
				long wantedUts = ((RadioServiceDabEdiImpl)mRunningSrv).getInitialTimePosix();

				if(DEBUG)Log.d(TAG, "Connecting with InitUts: " + wantedUts);

				String utsUrl = mRunningSrv.getUrl().endsWith("/") ? (mRunningSrv.getUrl().substring(0, mRunningSrv.getUrl().length()-1) + "?wantedUts=" + wantedUts) : (mRunningSrv.getUrl() + "?wantedUts=" + wantedUts);
				ediUrl = new URL(utsUrl);
			} else {
				ediUrl = new URL(mRunningSrv.getUrl());
			}

			mHttpStreamConnection = (HttpURLConnection)ediUrl.openConnection();
			mHttpStreamConnection.setConnectTimeout(3000);
			mHttpStreamConnection.setReadTimeout(5000);
			mHttpStreamConnection.connect();

			httpResponseCode = mHttpStreamConnection.getResponseCode();

			//TODO initial timeshiftToken and time Init values

			if (DEBUG) Log.d(TAG + "_" + this, "EdiStream HttpResponseCode: " + httpResponseCode + " ConnectionRetries left: " + mConRetries);

			if(httpResponseCode == HttpURLConnection.HTTP_OK) {
				//TODO will never be zero on SocketTimeoutException
				mConRetries = 5;
				if(mTunerStatus != TUNER_STATUS_SCANNING) {
					String timeshiftToken = mHttpStreamConnection.getHeaderField("timeshift-token");
					if (timeshiftToken != null) {
						if(mIsReconnecting) {
							if(DEBUG)Log.d(TAG, "Reconnecting TimeshiftToken, New: " + timeshiftToken + ", Old: " + mTimeshiftToken);
							if(mTimeshiftToken != null && !mTimeshiftToken.equals(timeshiftToken)) {
								if(DEBUG)Log.d(TAG, "Reconnecting TimeshiftToken doesn't match!");
								//Received new timeshifttoken or token not valid anymore
								mIsReconnecting = false;
							}
						} else if(mRunningSrv.getSbtToken() != null) {
							if(DEBUG)Log.d(TAG, "InitSbt TimeshiftToken given: " + mRunningSrv.getSbtToken() + ", Returned Token is: " + timeshiftToken + ", Equal: " + mRunningSrv.getSbtToken().equals(timeshiftToken));						}

						mTimeshiftToken = timeshiftToken;

						String timeshiftMax = mHttpStreamConnection.getHeaderField("timeshift-max");
						if (DEBUG) Log.d(TAG, "SBT TimeshiftToken is: " + mTimeshiftToken);
						if (timeshiftMax != null) {
							try {
								mMaxTimeshift = Long.parseLong(timeshiftMax);
								if (DEBUG) Log.d(TAG, "SBT Timeshift Max is: " + mMaxTimeshift);
								if(!mIsReconnecting) {
									mSbtAvailable = true;
									((RadioServiceDabEdiImpl) mRunningSrv).addSbtControlCallback(this);
									((RadioServiceDabEdiImpl) mRunningSrv).setIsSbt(true);
									((RadioServiceDabEdiImpl) mRunningSrv).setSbtMax(mMaxTimeshift);
									((RadioServiceDabEdiImpl) mRunningSrv).setSbtToken(mTimeshiftToken);

									push(new Runnable() {
										@Override
										public void run() {
											getSbtItems();
										}
									});
								}
							} catch (NumberFormatException numExc) {
								if (DEBUG) numExc.printStackTrace();
							}
						} else {
							if (DEBUG) Log.d(TAG, "SBT Timeshift Max is null");
						}
					} else {
						if (DEBUG) Log.d(TAG, "SBT TimeshiftToken is null");
					}
				}

				mInput = mHttpStreamConnection.getInputStream();

				byte[] downBuff = new byte[2048];

				int len = 0;
				while (mHttpThreadRunning && (len = mInput.read(downBuff)) != -1) {
					UsbHelper.getInstance().ediStream(downBuff, len);
				}

				mInput.close();
			} else {
				if(DEBUG)Log.d(TAG, "EdiStream error HttpResponseCode: " + mHttpStreamConnection.getResponseCode());
			}
		} catch (SocketTimeoutException sockExc) {
			if(DEBUG)sockExc.printStackTrace();
			if(DEBUG)Log.w(TAG + "_" + this, "SocketTimeout... " + (mTunerStatus == TUNER_STATUS_SCANNING ? "while scanning" : "restarting stream"));
			if (mConRetries == 0 || mTunerStatus == TUNER_STATUS_SCANNING) {
				mIsReconnecting = false;
				stopRadioService();
			} else {
				if(DEBUG)Log.d(TAG,  "Restarting stream ConnectionRetries left: " + mConRetries);
				mIsReconnecting = true;
				restartStream();
			}
		} catch(ConnectException conExc) {
			if(mConRetries > 0) {
				mIsReconnecting = true;
				restartStream();
			} else {
				mIsReconnecting = false;
				stopRadioService();
			}
		} catch(EOFException eofExc) {
			if(mConRetries > 0) {
				mIsReconnecting = true;
				restartStream();
			} else {
				mIsReconnecting = false;
				stopRadioService();
			}
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} finally {
			if(mHttpStreamConnection != null) {
				mHttpStreamConnection.disconnect();
			}
		}
	}

	void servicesUpdated() {
		if(DEBUG)Log.d(TAG, "Serializing EDIServices");
		RadioServiceManager.getInstance().serializeServices(RadioServiceType.RADIOSERVICE_TYPE_EDI);
	}

	/* De-/Serializing*/
	private class RestoreServicesTask extends AsyncTask<Void, Void, Void> {

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected Void doInBackground(Void... params) {
			//restoreServices();
			while (!RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_EDI) || !VisualLogoManager.getInstance().isReady()) {
				try {
					Thread.sleep(10);
				} catch(InterruptedException interExc) {
					if(DEBUG)interExc.printStackTrace();
				}
			}

			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			super.onPostExecute(aVoid);
			if(DEBUG)Log.d(TAG, "Restore services finished #Services: " + RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_EDI).size());

			mIsInitializing = false;
			mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;

			sayReady();
		}
	}

	private void sayReady() {
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerStatusChanged(this, mTunerStatus);
		}
	}

	/* SBT */
	void getSbtItems() {
		try {
			URL postUrl = new URL(mRunningSrv.getUrl());
			HttpURLConnection postConn = (HttpURLConnection)postUrl.openConnection();
			postConn.setRequestMethod("POST");
			postConn.setRequestProperty("Content-Type", "application/json;charset=UTF-8");
			postConn.setRequestProperty("Accept","application/json");
			postConn.setDoOutput(true);
			postConn.setDoInput(true);

			JSONObject itemsPostObj = new JSONObject();
			itemsPostObj.put("action", "items");

			DataOutputStream itemsOutStream = new DataOutputStream(postConn.getOutputStream());
			itemsOutStream.writeBytes(itemsPostObj.toString());
			itemsOutStream.flush();
			itemsOutStream.close();

			if(DEBUG)Log.d(TAG, "SBT Items responseCode: " + postConn.getResponseCode());

			if(postConn.getResponseCode() == HttpURLConnection.HTTP_OK) {
				BufferedReader responseReader = new BufferedReader(new InputStreamReader(postConn.getInputStream()));
				StringBuilder jsonStringBuilder = new StringBuilder();

				String line;
				while ((line = responseReader.readLine()) != null) {
					jsonStringBuilder.append(line);
				}
				responseReader.close();

				String itemsString = jsonStringBuilder.toString();

				JSONArray responseArr = new JSONArray(itemsString);
				if(DEBUG)Log.d(TAG, "SBT Items: " + responseArr.length());
				ArrayList<SbtItem> sbtItems = new ArrayList<>();
				for(int i = 0; i < responseArr.length(); i++) {
					JSONObject item = responseArr.getJSONObject(i);

					String itemLabel = item.getString("label");
					TextualDabDynamicLabelImpl dls = new TextualDabDynamicLabelImpl();
					dls.setText(itemLabel);

					JSONArray tagsArr = item.optJSONArray("tags");
					ArrayList<TextualDabDynamicLabelPlusItem> tagsList = new ArrayList<>();
					if(tagsArr != null) {
						for (int j = 0; j < tagsArr.length(); j++) {
							JSONObject tag = tagsArr.getJSONObject(j);
							String tagLabel = tag.getString("text");
							int tagType = tag.getInt("type");

							TextualDabDynamicLabelPlusItemImpl dlsTag = new TextualDabDynamicLabelPlusItemImpl();

							dlsTag.setDlPlusContentText(tagLabel);
							dlsTag.setDlPlusContentType(tagType);

							tagsList.add(dlsTag);
						}
					}

					long toggleId = item.getLong("id");
					long toggleTime = item.getLong("time");

					boolean runningState = item.getBoolean("runningState");
					boolean toggleState = item.getBoolean("toggleState");

					dls.addDlPlusItems(tagsList);
					dls.setItemRunning(runningState);
					dls.setItemToggled(toggleState);

					String slideMime = item.getString("slideMime");
					String slidePath = item.getString("slidePath");

					VisualDabSlideShowImpl sls = null;
					if(slidePath.length() > 0) {
						String itemSlideFullPath = mRunningSrv.getUrl().endsWith("/") ? (mRunningSrv.getUrl().substring(0, mRunningSrv.getUrl().length()-1) + slidePath) : (mRunningSrv.getUrl() + slidePath);
						sls = new VisualDabSlideShowImpl();
						sls.setVisualMimeType(VisualMimeType.getMimeFromString(slideMime));
						sls.setVisualData(downloadItemSlide(itemSlideFullPath));
					}

					if(DEBUG)Log.d(TAG, "SBT Item: " + itemLabel + ", ID: " + toggleId + ", Time: " + toggleTime);

					SbtItemImpl sbtItem = new SbtItemImpl(toggleId, toggleTime, toggleState, runningState, dls, sls);
					sbtItems.add(sbtItem);
				}

				Collections.sort(sbtItems, new Comparator<SbtItem>() {
					@Override
					public int compare(SbtItem o1, SbtItem o2) {
						return o1.getPosixTime() < o2.getPosixTime() ? -1
								: o1.getPosixTime() > o2.getPosixTime() ? 1
								: 0;
					}
				});

				for(SbtItem sortedItem : sbtItems) {
					((RadioServiceDabEdiImpl)mRunningSrv).addOrUpdateItem(sortedItem);
				}
			}

			postConn.disconnect();

		} catch(MalformedURLException malUrl) {
			if(DEBUG)malUrl.printStackTrace();
		} catch(ProtocolException protExc) {
			if(DEBUG)protExc.printStackTrace();
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} catch(JSONException jsonExc) {
			if(DEBUG)jsonExc.printStackTrace();
		}
	}

	void setSbtToggle(long toggleId) {
		try {
			URL postUrl = new URL(mRunningSrv.getUrl());
			HttpURLConnection postConn = (HttpURLConnection)postUrl.openConnection();
			postConn.setRequestMethod("POST");
			postConn.setRequestProperty("Content-Type", "application/json;charset=UTF-8");
			postConn.setRequestProperty("Accept","application/json");
			postConn.setDoOutput(true);
			postConn.setDoInput(true);

			JSONObject togglePostObj = new JSONObject();
			togglePostObj.put("timeshiftToken", mTimeshiftToken);
			togglePostObj.put("action", "toggle");
			togglePostObj.put("wantedPos", toggleId);

			DataOutputStream itemsOutStream = new DataOutputStream(postConn.getOutputStream());
			itemsOutStream.writeBytes(togglePostObj.toString());
			itemsOutStream.flush();
			itemsOutStream.close();

			int responseCode = postConn.getResponseCode();
			if(responseCode == HttpURLConnection.HTTP_OK) {
				if(DEBUG)Log.d(TAG, "Flushing all buffers on toggleset");
				UsbHelper.getInstance().flushEdiData();
			}

			postConn.disconnect();
		} catch(MalformedURLException malUrl) {
			if(DEBUG)malUrl.printStackTrace();
		} catch(ProtocolException protExc) {
			if(DEBUG)protExc.printStackTrace();
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} catch(JSONException jsonExc) {
			if(DEBUG)jsonExc.printStackTrace();
		}
	}

	void seekSbt(long seekMs) {
		try {
			URL postUrl = new URL(mRunningSrv.getUrl());
			HttpURLConnection postConn = (HttpURLConnection)postUrl.openConnection();
			postConn.setRequestMethod("POST");
			postConn.setRequestProperty("Content-Type", "application/json;charset=UTF-8");
			postConn.setRequestProperty("Accept","application/json");
			postConn.setDoOutput(true);
			postConn.setDoInput(true);

			JSONObject togglePostObj = new JSONObject();
			togglePostObj.put("timeshiftToken", mTimeshiftToken);
			togglePostObj.put("action", "seek");
			togglePostObj.put("wantedPos", seekMs);

			DataOutputStream itemsOutStream = new DataOutputStream(postConn.getOutputStream());
			itemsOutStream.writeBytes(togglePostObj.toString());
			itemsOutStream.flush();
			itemsOutStream.close();

			int responseCode = postConn.getResponseCode();
			if(responseCode == HttpURLConnection.HTTP_OK) {
				if(DEBUG)Log.d(TAG, "Flushing all buffers on seek");
				UsbHelper.getInstance().flushEdiData();
			}

			postConn.disconnect();
		} catch(MalformedURLException malUrl) {
			if(DEBUG)malUrl.printStackTrace();
		} catch(ProtocolException protExc) {
			if(DEBUG)protExc.printStackTrace();
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} catch(JSONException jsonExc) {
			if(DEBUG)jsonExc.printStackTrace();
		}
	}

	private void pauseSbt(boolean pause) {
		try {
			URL postUrl = new URL(mRunningSrv.getUrl());
			HttpURLConnection postConn = (HttpURLConnection)postUrl.openConnection();
			postConn.setRequestMethod("POST");
			postConn.setRequestProperty("Content-Type", "application/json;charset=UTF-8");
			postConn.setRequestProperty("Accept","application/json");
			postConn.setDoOutput(true);
			postConn.setDoInput(true);

			JSONObject togglePostObj = new JSONObject();
			togglePostObj.put("timeshiftToken", mTimeshiftToken);
			if(pause) {
				togglePostObj.put("action", "pause");
			} else {
				togglePostObj.put("action", "play");
			}

			DataOutputStream itemsOutStream = new DataOutputStream(postConn.getOutputStream());
			itemsOutStream.writeBytes(togglePostObj.toString());
			itemsOutStream.flush();
			itemsOutStream.close();

			if(DEBUG)Log.d(TAG, "SBT pause responseCode: " + postConn.getResponseCode());
			if(postConn.getResponseCode() == HttpURLConnection.HTTP_OK) {
				if(DEBUG)Log.d(TAG, "Flushing all buffers on pause");
				UsbHelper.getInstance().flushEdiData();

			}

			postConn.disconnect();
		} catch(MalformedURLException malUrl) {
			if(DEBUG)malUrl.printStackTrace();
		} catch(ProtocolException protExc) {
			if(DEBUG)protExc.printStackTrace();
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} catch(JSONException jsonExc) {
			if(DEBUG)jsonExc.printStackTrace();
		}
	}

	private byte[] downloadItemSlide(String logoUrl) {
		if (DEBUG) Log.d(TAG, "SBT Item SLS download URL: " + (logoUrl != null ? logoUrl : "null"));
		InputStream mInput = null;
		ByteArrayOutputStream mOutput = new ByteArrayOutputStream();
		HttpURLConnection mHttpConnection = null;
		int httpResponseCode = -1;

		try {
			URL logoDlUrl = new URL(logoUrl);

			mHttpConnection = (HttpURLConnection)logoDlUrl.openConnection();
			mHttpConnection.setConnectTimeout(2000);
			mHttpConnection.setReadTimeout(2000);
			mHttpConnection.setInstanceFollowRedirects(true);
			mHttpConnection.connect();

			httpResponseCode = mHttpConnection.getResponseCode();

			if (DEBUG) Log.d(TAG, "SBT Item SLS download HttpResponseCode: " + httpResponseCode);

			if(httpResponseCode == HttpURLConnection.HTTP_OK) {
				String contentType = mHttpConnection.getContentType();

				if(VisualMimeType.getMimeFromString(contentType) != VisualMimeType.METADATA_VISUAL_MIMETYPE_UNKNOWN) {
					if (DEBUG) Log.d(TAG, "SBT Item SLS download Size: " + mHttpConnection.getContentLength());
					mInput = mHttpConnection.getInputStream();

					byte[] downBuff = new byte[4096];

					int len = 0;
					while ((len = mInput.read(downBuff)) != -1) {
						mOutput.write(downBuff, 0, len);
					}
				} else {
					if (DEBUG) Log.d(TAG, "SBT Item SLS unknown MIME: " + contentType);
				}
			}
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} finally {
			if(mHttpConnection != null) {
				mHttpConnection.disconnect();
			}
		}

		return mOutput.toByteArray();
	}

	/* Native callbacks */
	private void dabTimeUpdate(DabTime dabTime) {
		if(DEBUG)Log.d(TAG, "SBT dabTime: " + dabTime.getPosixMillis() + " : " + new Date(dabTime.getPosixMillis()).toString());
		((RadioServiceDabEdiImpl)mRunningSrv).updateDabTime(dabTime.getPosixMillis());
	}

	private void newLiveItem(byte[] itemJson) {
		if(DEBUG)Log.d(TAG, "SBT live item added: " + itemJson.length);

		if(itemJson != null) {
			try {
				JSONObject item = new JSONObject(new String(itemJson));

				String itemLabel = item.getString("label");
				TextualDabDynamicLabelImpl dls = new TextualDabDynamicLabelImpl();
				dls.setText(itemLabel);

				JSONArray tagsArr = item.getJSONArray("tags");
				ArrayList<TextualDabDynamicLabelPlusItem> tagsList = new ArrayList<>();
				for(int j = 0; j < tagsArr.length(); j++) {
					JSONObject tag = tagsArr.getJSONObject(j);
					String tagLabel = tag.getString("text");
					int tagType = tag.getInt("type");

					TextualDabDynamicLabelPlusItemImpl dlsTag = new TextualDabDynamicLabelPlusItemImpl();

					dlsTag.setDlPlusContentText(tagLabel);
					dlsTag.setDlPlusContentType(tagType);

					tagsList.add(dlsTag);
				}

				long toggleId = item.getLong("id");
				long toggleTime = item.getLong("time");

				boolean runningState = item.getBoolean("runningState");
				boolean toggleState = item.getBoolean("toggleState");

				dls.addDlPlusItems(tagsList);
				dls.setItemRunning(runningState);
				dls.setItemToggled(toggleState);

				String slideMime = item.getString("slideMime");
				String slidePath = item.getString("slidePath");

				VisualDabSlideShowImpl sls = null;
				if(slidePath.length() > 0) {
					String itemSlideFullPath = mRunningSrv.getUrl().endsWith("/") ? (mRunningSrv.getUrl().substring(0, mRunningSrv.getUrl().length()-1) + slidePath) : (mRunningSrv.getUrl() + slidePath);
					sls = new VisualDabSlideShowImpl();
					sls.setVisualMimeType(VisualMimeType.getMimeFromString(slideMime));
					sls.setVisualData(downloadItemSlide(itemSlideFullPath));
				}

				if(DEBUG)Log.d(TAG, "SBT live Item: " + itemLabel + ", ID: " + toggleId + ", Time: " + toggleTime);

				SbtItemImpl sbtItem = new SbtItemImpl(toggleId, toggleTime, toggleState, runningState, dls, sls);

				((RadioServiceDabEdiImpl)mRunningSrv).addOrUpdateItem(sbtItem);

			} catch(JSONException jsonExc) {
				if(DEBUG)jsonExc.printStackTrace();
			}
		}
	}

	/* SBT control callback */
	@Override
	public void seekMs(long ms) {
		if(DEBUG)Log.d(TAG, "SBT seeking to: " + ms);
		push(new Runnable() {
			@Override
			public void run() {
				seekSbt(ms);
			}
		});
	}

	@Override
	public void setToggle(long toggleId) {
		if(DEBUG)Log.d(TAG, "SBT setting toggle: " + toggleId);
		push(new Runnable() {
			@Override
			public void run() {
				setSbtToggle(toggleId);
			}
		});
	}

	@Override
	public void pause(boolean pause) {
		if(DEBUG)Log.d(TAG, "SBT pausing: " + pause);
		push(new Runnable() {
			@Override
			public void run() {
				pauseSbt(pause);
			}
		});
	}
}
