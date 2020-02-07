package org.omri.radio.impl;

import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.util.Log;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.ExoPlaybackException;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.ExoPlayerFactory;
import com.google.android.exoplayer2.PlaybackParameters;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.Renderer;
import com.google.android.exoplayer2.Timeline;
import com.google.android.exoplayer2.audio.AudioAttributes;
import com.google.android.exoplayer2.audio.AudioSink;
import com.google.android.exoplayer2.audio.MediaCodecAudioRenderer;
import com.google.android.exoplayer2.extractor.mp3.Mp3Extractor;
import com.google.android.exoplayer2.extractor.ts.AdtsExtractor;
import com.google.android.exoplayer2.mediacodec.MediaCodecSelector;
import com.google.android.exoplayer2.source.ExtractorMediaSource;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.TrackGroupArray;
import com.google.android.exoplayer2.trackselection.DefaultTrackSelector;
import com.google.android.exoplayer2.trackselection.TrackSelectionArray;
import com.google.android.exoplayer2.trackselection.TrackSelector;
import com.google.android.exoplayer2.upstream.Allocator;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultAllocator;
import com.google.android.exoplayer2.util.Util;

import org.omri.radio.Radio;
import org.omri.radioservice.RadioService;
import org.omri.radioservice.RadioServiceIp;
import org.omri.radioservice.RadioServiceIpStream;
import org.omri.radioservice.RadioServiceMimeType;
import org.omri.radioservice.RadioServiceType;
import org.omri.radioservice.metadata.Textual;
import org.omri.tuner.Tuner;
import org.omri.tuner.TunerListener;
import org.omri.tuner.TunerStatus;
import org.omri.tuner.TunerType;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLSocketFactory;

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

public class TunerIpShoutcast implements Tuner, IcyStreamDataSource.IcyMetadataListener, Player.EventListener, AudioSink, IpServiceScanner.IpScannerListener {

	private final String TAG = "TunerIpShoutcast";

	private static final int BUFFER_SIZE = 1024*10*10;

	private final TunerType mTunertype = TunerType.TUNER_TYPE_IP_SHOUTCAST;
	private TunerStatus mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;
	private List<TunerListener> mTunerlisteners = Collections.synchronizedList(new ArrayList<TunerListener>());
	private List<RadioService> mServiceList = new ArrayList<>();
	private List<RadioService> mScannedServices = new ArrayList<>();

	private boolean mIsInitializing = false;

	private boolean mScanDeleteServices = false;

	private RadioServiceIp mCurrentRadioService = null;
	private RadioServiceIpStream mCurrentStream = null;

	/* Exo HandlerThread */
	Handler mHandler;

	private ExoPlayer mExoPlayer = null;

	TunerIpShoutcast() {
		if(DEBUG)Log.d(TAG, "Creating Tuner");

		TrackSelector trackSelector = new DefaultTrackSelector();
		Renderer[] renderers = new Renderer[1];
		renderers[0] = new MediaCodecAudioRenderer(MediaCodecSelector.DEFAULT, null, true, null, null, this);
		mExoPlayer = ExoPlayerFactory.newInstance(renderers, trackSelector);

		mExoPlayer.addListener(this);
		mHandler = new Handler();

		IpServiceScanner.getInstance().addScanListener(this);
	}

	/* Tuner implementation */
	@Override
	public void initializeTuner() {
		if(DEBUG)Log.d(TAG, "Initializing Tuner...");
		if(mTunerStatus == TunerStatus.TUNER_STATUS_NOT_INITIALIZED && !mIsInitializing) {
			//mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
			mIsInitializing = true;

			if(DEBUG)Log.d(TAG, "Restoring services...");
			new RestoreServicesTask().execute();
		}
	}

	@Override
	public void suspendTuner() {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			mTunerStatus = TunerStatus.TUNER_STATUS_SUSPENDED;
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerStatusChanged(this, mTunerStatus);
			}

			mIsInitializing = false;
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
		if(mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED || mTunerStatus == TunerStatus.TUNER_STATUS_SUSPENDED) {
			if(DEBUG)Log.d(TAG, "Deinitializing tuner...");
			mTunerStatus = TunerStatus.TUNER_STATUS_NOT_INITIALIZED;
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerStatusChanged(this, mTunerStatus);
			}

			if(mCurrentRadioService != null) {
				stopStream();
			}
			if(mExoPlayer != null) {
				mExoPlayer.release();
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
		if(DEBUG)Log.d(TAG, "Returning servicelist containing " + RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_IP).size() + " services");
		if(mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			return RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_IP);
		} else {
			return new ArrayList<>();
		}
	}

	@Override
	public void startRadioServiceScan() {
		startRadioServiceScan(null);
	}

	@Override
	public void startRadioServiceScan(Object scanOptions) {
		if (DEBUG) Log.d(TAG, "Starting Service scan: " + mTunerStatus.toString());

		if (mTunerStatus == TunerStatus.TUNER_STATUS_INITIALIZED) {
			mTunerStatus = TunerStatus.TUNER_STATUS_SCANNING;
			mScannedServicesWithoutStream.clear();

			Bundle optBundle = null;
			boolean useHradioSearch = false;
			if (scanOptions instanceof Bundle) {
				optBundle = (Bundle) scanOptions;
				useHradioSearch = optBundle.getBoolean(RadioImpl.SERVICE_SEARCH_OPT_USE_HRADIO, false);
				if(optBundle.getBoolean(RadioImpl.SERVICE_SEARCH_OPT_DELETE_SERVICES, false)) {
					if(DEBUG)Log.d(TAG, "Clearing existing services before new scan");
					RadioServiceManager.getInstance().clearServiceList(RadioServiceType.RADIOSERVICE_TYPE_IP);
				}
			}

			if (useHradioSearch) {
				IpServiceScanner.getInstance().scanServices(optBundle);
			} else {
				IpServiceScanner.getInstance().scanServices(null);
			}
		}
	}

	/* ScanListener */
	@Override
	public void scanStarted() {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerStatusChanged(this, mTunerStatus);
				listener.tunerScanStarted(this);
			}
		}
	}

	@Override
	public void scanProgress(int percent) {
		if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerScanProgress(this, percent);
			}
		}
	}

	private List<RadioService> mScannedServicesWithoutStream = new ArrayList<>();
	@Override
	public void foundStreamingServices(List<RadioService> ipStreamservices) {
		if(DEBUG)Log.d(TAG, "foundStreamingServices at tunerstate: " + mTunerStatus.toString());
		if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			for (RadioService foundSrv : ipStreamservices) {
				ArrayList<RadioServiceIpStream> streams = new ArrayList<>();
				RadioServiceIpImpl fipSrv = (RadioServiceIpImpl) foundSrv;
				for (RadioServiceIpStream stream : fipSrv.getIpStreams()) {
					if (stream.getMimeType() != RadioServiceMimeType.AUDIO_EDI) {
						streams.add(stream);
					}
				}

				//TODO
				if (!streams.isEmpty()) {
					RadioServiceManager.getInstance().addService(foundSrv);
					updateTunerListenerScanServiceFound(foundSrv);
				} else {
					mScannedServicesWithoutStream.add(fipSrv);
				}
			}
		}
	}

	@Override
	public void scanFinished() {
		if (mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			if(DEBUG)Log.d(TAG, "scanFinished serializing services");
			new SerializeServicesTask().execute();

			mTunerStatus = TunerStatus.TUNER_STATUS_INITIALIZED;
			for (TunerListener listener : mTunerlisteners) {
				listener.tunerScanFinished(this);
				listener.tunerStatusChanged(this, mTunerStatus);
			}
		}
	}

	private void updateTunerListenerScanServiceFound(RadioService srv) {
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanServiceFound(this, srv);
		}
	}

	private void updateTunerListenerScanStatus(int left) {
		for (TunerListener listener : mTunerlisteners) {
			listener.tunerScanProgress(this, left);
		}
	}

	@Override
	public void stopRadioServiceScan() {
		if(DEBUG)Log.d(TAG, "Stopping Scan: " + mTunerStatus.toString());
		if(mTunerStatus == TunerStatus.TUNER_STATUS_SCANNING) {
			if(IpServiceScanner.getInstance().isScanning()) {
				IpServiceScanner.getInstance().stopScan();
			} else {
				this.scanFinished();
			}
		}
	}

	@Override
	public void startRadioService(RadioService radioService) {
		if(radioService != null && radioService.getRadioServiceType() == RadioServiceType.RADIOSERVICE_TYPE_IP) {
			RadioServiceIp srv = (RadioServiceIp)radioService;
			if(DEBUG) {
				Log.d(TAG, "Starting Service: " + srv.getServiceLabel());
				for (RadioServiceIpStream stream : srv.getIpStreams()) {
						Log.d(TAG, "Available Stream: " + stream.getUrl() + " : " + stream.getMimeType().toString());
				}
			}

			if(srv.getIpStreams().size() > 0) {
				if(mCurrentRadioService != null) {
					stopStream();
				}

				mCurrentRadioService = srv;

				Thread parseThread = new Thread(new Runnable() {
					@Override
					public void run() {
						//TODO choosable bitrate, alternative stream urls
						parseStreamUrls(mCurrentRadioService.getIpStreams());
					}
				});
				parseThread.start();
			}
		}
	}

	@Override
	public void stopRadioService() {
		if(mCurrentRadioService != null) {
			stopStream();
		}
	}

	@Override
	public RadioService getCurrentRunningRadioService() {
		return mCurrentRadioService;
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

	private void parseStreamUrls(List<RadioServiceIpStream> streams) {
		ArrayList<String> parsedUrls = new ArrayList<>();

		for(RadioServiceIpStream ipStream : streams) {
			String streamUrlString = ipStream.getUrl();
			if(streamUrlString != null && !streamUrlString.isEmpty()) {
				streamUrlString = streamUrlString.trim();

				parsedUrls.addAll(parseStreamUrls(streamUrlString));
			}
		}

		if(!parsedUrls.isEmpty()) {
			if(DEBUG)Log.d(TAG, "Starting stream with URL: " + parsedUrls.get(0));
			startStream(parsedUrls.get(0));
		} else {
			if(DEBUG)Log.w(TAG, "No valid URL for this IpService");
			stopStream();
		}
	}

	private ArrayList<String> parseStreamUrls(String streamsUrl) {
		ArrayList<String> parsedUrls = new ArrayList<>();
			if (streamsUrl.endsWith("m3u")) {
				HttpURLConnection m3uConnection = null;
			try {
				URL m3uUrl = new URL(streamsUrl);
				if(streamsUrl.startsWith("https")) {
					if(DEBUG)Log.d(TAG, "Using HTTPS connection");
					SSLSocketFactory socketFactory = (SSLSocketFactory) SSLSocketFactory.getDefault();
					m3uConnection = (HttpsURLConnection)m3uUrl.openConnection();
					((HttpsURLConnection) m3uConnection).setSSLSocketFactory(socketFactory);
				} else {
					if(DEBUG)Log.d(TAG, "Using HTTP connection");
					m3uConnection = (HttpURLConnection) m3uUrl.openConnection();
				}
				m3uConnection.setConnectTimeout(3000);
				m3uConnection.setReadTimeout(1000);
				m3uConnection.setInstanceFollowRedirects(true);
				m3uConnection.connect();

				int responseCode = m3uConnection.getResponseCode();
				//if(DEBUG)Log.d(TAG, "ResponseCode: " + responseCode);
				if(responseCode == 200) {
					String contentLengthHeader = m3uConnection.getHeaderField("Content-Length");
					//if(DEBUG)Log.d(TAG, "ContentLength: " + contentLengthHeader);

					BufferedReader m3uInBuf = new BufferedReader(new InputStreamReader(m3uConnection.getInputStream()));

					String m3uLine;
					while( (m3uLine = m3uInBuf.readLine()) != null) {
						if(DEBUG)Log.d(TAG, "M3U Line: " + m3uLine);
						if(!m3uLine.startsWith("#") && !m3uLine.isEmpty()) {
							parsedUrls.add(m3uLine);
						}
					}

					m3uInBuf.close();
				} else if(responseCode == HttpURLConnection.HTTP_MOVED_PERM || responseCode == HttpURLConnection.HTTP_MOVED_TEMP) {
					String redirectUrl = m3uConnection.getHeaderField("Location");
					if(redirectUrl != null && !redirectUrl.isEmpty()) {
						if(DEBUG)Log.d(TAG, "Following redirect to: " + redirectUrl);
						parsedUrls.addAll(parseStreamUrls(redirectUrl));
					}
				}

				m3uConnection.disconnect();

			} catch(IOException ioExc) {
				if(DEBUG)ioExc.printStackTrace();
			} finally {
				if(m3uConnection != null) {
					m3uConnection.disconnect();
				}
			}

		} else {
			parsedUrls.add(streamsUrl);
		}

		return parsedUrls;
	}

	private void startStream(String url) {
		if (DEBUG) Log.d(TAG, "Starting Stream: " + mCurrentRadioService.getIpStreams().get(0).getUrl() + " : " + mCurrentRadioService.getIpStreams().get(0).getMimeType().toString() + " : " + mCurrentRadioService.getIpStreams().get(0).getBitrate());

		Uri streamUri = Uri.parse(url);
		mCurrentStream = mCurrentRadioService.getIpStreams().get(0);

		Allocator allocator = new DefaultAllocator(false, BUFFER_SIZE);
		String userAgent = Util.getUserAgent(((RadioImpl) Radio.getInstance()).mContext, "OMRI");

		if (DEBUG) Log.d(TAG, "UserAgent: " + userAgent);

		DataSource.Factory dataSourceFactory = new IcyHttpDataSourceFactory(userAgent, this, mRawListener, null);
		MediaSource mediaSource = new ExtractorMediaSource(streamUri, dataSourceFactory, mCurrentRadioService.getIpStreams().get(0).getMimeType() == RadioServiceMimeType.AUDIO_MPEG ? Mp3Extractor.FACTORY : AdtsExtractor.FACTORY, mHandler, null);

		mExoPlayer.prepare(mediaSource);
		mExoPlayer.setPlayWhenReady(true);
	}

	IcyStreamDataSource.RawStreamDataListener mRawListener = new IcyStreamDataSource.RawStreamDataListener() {
		@Override
		public void rawAudioData(byte[] rawData) {
			if(mCurrentRadioService != null) {
				((RadioServiceIpImpl) mCurrentRadioService).rawStreamData(rawData, mCurrentStream.getMimeType());
			} else if(DEBUG) {
				if(DEBUG)Log.d(TAG, "CurrentRadioService is null at rawAudioData");
			}
		}
	};

	private void stopStream() {
		if(DEBUG)Log.d(TAG, "Stopping Stream");

		for(TunerListener listener : mTunerlisteners) {
			listener.radioServiceStopped(this, mCurrentRadioService);
		}

		mCurrentRadioService = null;

		mPlayerState = 0;

		if(mExoPlayer != null) {
			mExoPlayer.stop();
			mExoPlayer.seekTo(0);
		}


	}

	private boolean islaying() {
		if(mExoPlayer != null) {
			switch (mExoPlayer.getPlaybackState()) {
				case Player.STATE_IDLE:
					return false;
				case Player.STATE_READY:
					return true;
				case Player.STATE_BUFFERING:
					return true;
				case Player.STATE_ENDED:
					return false;
				default:
					return false;
			}
		}
		return false;

	}

	@Override
	public void onNewIcyStreamTitle(String icyTag) {
		if(DEBUG)Log.d(TAG, "onNewIcyStreamTitle: " + icyTag);

		Textual textual = new TextualIpIcyImpl(icyTag);
		((RadioServiceIpImpl)mCurrentRadioService).labeReceived(textual);
	}

	@Override
	public void onNewIcyStreamUrl(String icyUrl) {
		if(DEBUG)Log.d(TAG, "onNewIcyStreamUr: " + icyUrl);
	}


	private int mPlayerState = 0;
	/* Player callbacks */
	@Override
	public void onPlayerStateChanged(boolean playWhenReady, int playbackState) {
		if(DEBUG)Log.d(TAG, "onPlayerStateChanged: playWhenready: " + playWhenReady + ", PlayState: " + playbackState);
		switch (playbackState) {
			case Player.STATE_IDLE: {
				if(DEBUG)Log.d(TAG, "onPlayerStateChanged: Player.STATE_IDLE");
				if(mCurrentRadioService != null) {
					for (TunerListener listener : mTunerlisteners) {
						listener.radioServiceStopped(this, mCurrentRadioService);
					}
				}

				mCurrentRadioService = null;
				break;
			}
			case Player.STATE_READY: {
				if(DEBUG)Log.d(TAG, "onPlayerStateChanged: Player.STATE_READY");
				if(mPlayerState != playbackState) {
					if(mCurrentRadioService != null) {
						for (TunerListener listener : mTunerlisteners) {
							listener.radioServiceStarted(this, mCurrentRadioService);
						}
					}

					mPlayerState = playbackState;
				}
				break;
			}
			case Player.STATE_ENDED: {
				if(DEBUG)Log.d(TAG, "onPlayerStateChanged: Player.STATE_ENDED");
				if(mCurrentRadioService != null) {
					for (TunerListener listener : mTunerlisteners) {
						listener.radioServiceStopped(this, mCurrentRadioService);
					}
				}

				mCurrentRadioService = null;
				mPlayerState = playbackState;
				break;
			}
		}
	}

	@Override
	public void onTimelineChanged(Timeline timeline, Object manifest, int reason) {

	}

	@Override
	public void onTracksChanged(TrackGroupArray trackGroups, TrackSelectionArray trackSelections) {

	}

	@Override
	public void onLoadingChanged(boolean isLoading) {

	}

	@Override
	public void onRepeatModeChanged(int repeatMode) {

	}

	@Override
	public void onShuffleModeEnabledChanged(boolean shuffleModeEnabled) {

	}

	@Override
	public void onPlayerError(ExoPlaybackException error) {

	}

	@Override
	public void onPositionDiscontinuity(int reason) {

	}

	@Override
	public void onPlaybackParametersChanged(PlaybackParameters playbackParameters) {

	}

	@Override
	public void onSeekProcessed() {

	}

	/* Exoplayer Audiosink impl */
	@Override
	public void setListener(Listener listener) {
		if(DEBUG)Log.d(TAG, "AudioSink setListener");
	}

	@Override
	public boolean isEncodingSupported(int encoding) {
		if(DEBUG)Log.d(TAG, "AudioSink isEncodingSupported: " + encoding);
		if (isEncodingPcm(encoding)) {
			return encoding != C.ENCODING_PCM_FLOAT || Util.SDK_INT >= 21;
		}

		return false;
	}

	private static boolean isEncodingPcm(@C.Encoding int encoding) {
		return encoding == C.ENCODING_PCM_8BIT || encoding == C.ENCODING_PCM_16BIT
				|| encoding == C.ENCODING_PCM_24BIT || encoding == C.ENCODING_PCM_32BIT
				|| encoding == C.ENCODING_PCM_FLOAT;
	}

	@Override
	public long getCurrentPositionUs(boolean sourceEnded) {
		//if(DEBUG)Log.d(TAG, "AudioSink getCurrentPositionUs sourceEnded: " + sourceEnded);
		return mCurrentPosUs;
	}

	private int mSamplingRate = 0;
	private int mChannelCnt = 0;
	@Override
	public void configure(int inputEncoding, int inputChannelCount, int inputSampleRate, int specifiedBufferSize, @Nullable int[] outputChannels, int trimStartSamples, int trimEndSamples) throws ConfigurationException {
		if(DEBUG)Log.d(TAG, "AudioSink configure inputEncoding: " + inputEncoding +
				", inputChannelCount: " + inputChannelCount +
				", inputSampleRate: " + inputSampleRate +
				", outputChannels: " + (outputChannels != null ? outputChannels[0] : "null") +
				", trimStartSamples: " + trimStartSamples +
				", trimEndSamples: " + trimEndSamples
		);

		mSamplingRate = inputSampleRate;
		mChannelCnt = inputChannelCount;
	}

	@Override
	public void play() {
		//if(DEBUG)Log.d(TAG, "AudioSink play");
	}

	@Override
	public void handleDiscontinuity() {
		//if(DEBUG)Log.d(TAG, "AudioSink handleDiscontinuity");
	}

	private long mCurrentPosUs = 0;
	private byte[] mOutputBuffer = null;
	@Override
	public boolean handleBuffer(ByteBuffer buffer, long presentationTimeUs) throws InitializationException, WriteException {
		if(!buffer.hasRemaining()) {
			return true;
		}

		int bytesRemaining = buffer.remaining();
		if(mOutputBuffer == null || mOutputBuffer.length != bytesRemaining) {
			if(DEBUG)Log.d(TAG, "AudioSink reconfiguring buffer: " + bytesRemaining);
			mOutputBuffer = new byte[bytesRemaining];
		}

		int originalPosition = buffer.position();
		buffer.get(mOutputBuffer, 0, bytesRemaining);
		buffer.position(originalPosition);

		mCurrentPosUs = presentationTimeUs;
		if(mCurrentRadioService != null) {
			((RadioServiceIpImpl) mCurrentRadioService).decodedAudioData(mOutputBuffer, mChannelCnt, mSamplingRate);
		} else if(DEBUG) {
			Log.d(TAG, "CurrentRadioService is null at handleBuffer");
		}
		return true;
	}

	@Override
	public void playToEndOfStream() throws WriteException {
		//if(DEBUG)Log.d(TAG, "AudioSink playToEndOfStream");
	}

	@Override
	public boolean isEnded() {
		//if(DEBUG)Log.d(TAG, "AudioSink isEnded");
		return false;
	}

	@Override
	public boolean hasPendingData() {
		//if(DEBUG)Log.d(TAG, "AudioSink hasPendingData");
		return false;
	}

	@Override
	public PlaybackParameters setPlaybackParameters(PlaybackParameters playbackParameters) {
		//if(DEBUG)Log.d(TAG, "AudioSink setPlaybackParameters");

		mPlaybackParameters = playbackParameters;

		return mPlaybackParameters;
	}

	PlaybackParameters mPlaybackParameters = PlaybackParameters.DEFAULT;
	@Override
	public PlaybackParameters getPlaybackParameters() {
		//if(DEBUG)Log.d(TAG, "AudioSink getPlaybackParameters");
		return mPlaybackParameters;
	}

	@Override
	public void setAudioAttributes(AudioAttributes audioAttributes) {
		//if(DEBUG)Log.d(TAG, "AudioSink setAudioAttributes: " + audioAttributes.contentType);
	}

	@Override
	public void setAudioSessionId(int audioSessionId) {
		//if(DEBUG)Log.d(TAG, "AudioSink setAudioSessionId");
	}

	@Override
	public void enableTunnelingV21(int tunnelingAudioSessionId) {
		//if(DEBUG)Log.d(TAG, "AudioSink enableTunnelingV21");
	}

	@Override
	public void disableTunneling() {
		//if(DEBUG)Log.d(TAG, "AudioSink disableTunneling");
	}

	@Override
	public void setVolume(float volume) {
		//if(DEBUG)Log.d(TAG, "AudioSink setVolume");
	}

	@Override
	public void pause() {
		//if(DEBUG)Log.d(TAG, "AudioSink pause");
	}

	@Override
	public void reset() {
		//if(DEBUG)Log.d(TAG, "AudioSink reset");
	}

	@Override
	public void release() {
		if(DEBUG)Log.d(TAG, "AudioSink release");
	}

	/**/
	private class SerializeServicesTask extends AsyncTask<Void, Void, Void> {

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected Void doInBackground(Void... params) {
			RadioServiceManager.getInstance().serializeServices(RadioServiceType.RADIOSERVICE_TYPE_IP);
			if(DEBUG)Log.d(TAG, "Serializing Services: " + RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_IP));
			while (!RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_IP)) {
				try {
					Thread.sleep(10);
					if(DEBUG)Log.d(TAG, "Waiting for serialized servicelist to be ready");
				} catch(InterruptedException interExc) {
					if(DEBUG)interExc.printStackTrace();
				}
			}
			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			super.onPostExecute(aVoid);
		}
	}

	private class RestoreServicesTask extends AsyncTask<Void, Void, Void> {

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected Void doInBackground(Void... params) {
			if(DEBUG)Log.d(TAG, "Restoring Servicelist in background, ready: " + RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_IP));
			while (!RadioServiceManager.getInstance().isServiceListReady(RadioServiceType.RADIOSERVICE_TYPE_IP) || !VisualLogoManager.getInstance().isReady()) {
				try {
					if(DEBUG)Log.d(TAG, "Waiting for servicelist and VisualLogoManager to be ready");
					Thread.sleep(10);
				} catch(InterruptedException interExc) {
					if(DEBUG)interExc.printStackTrace();
				}
			}

			mServiceList = RadioServiceManager.getInstance().getRadioServices(RadioServiceType.RADIOSERVICE_TYPE_IP);
			return null;
		}

		@Override
		protected void onPostExecute(Void aVoid) {
			super.onPostExecute(aVoid);
			if(DEBUG)Log.d(TAG, "Restore services finished with " + mServiceList.size() + " services");

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
}
