package org.omri.radioservice;

public interface RadioServiceRawAudiodataListener extends RadioServiceListener {

	public void rawAudioData(byte[] rawData, boolean sbr, boolean ps, RadioServiceMimeType type, int numChannels, int samplingRate);
}
