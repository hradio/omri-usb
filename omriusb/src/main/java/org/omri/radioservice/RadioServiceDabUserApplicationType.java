package org.omri.radioservice;

public enum RadioServiceDabUserApplicationType {
	RFU(0, "RFU"),
	NOT_USED(1, "Not used"),
	MOT_SLIDESHOW(2, "MOT Slideshow"),
	MOT_BROADCAST_WEBSITE(3, "MOT Broadcast Website"),
	TPEG(4, "TPEG"),
	DGPS(5, "DGPS"),
	TMC(6, "TMC"),
	EPG(7, "Electronic Programme Guide"),
	DAB_JAVA(8, "Java Application"),
	DMB(9, "Digital Multimedia Broadcast (DMB)"),
	IPDC_SERVICES(10, "IPDC Services"),
	VOICE_APPLICATIONS(11, "Voice Applications"),
	MIDDLEWARE(12, "Middleware"),
	FILECASTING(13, "Filecasting"),

	JOURNALINE(1098, "Journaline");

	private final int mType;
	private final String mName;

	private RadioServiceDabUserApplicationType(int type, String name) {
		mType = type;
		mName = name;
	}

	public int getType() {
		return mType;
	}

	public String getName() {
		return mName;
	}

	public static RadioServiceDabUserApplicationType getUserApplicationTypeByType(int type) {
		for(RadioServiceDabUserApplicationType appType : RadioServiceDabUserApplicationType.values()) {
			if(appType.getType() == type) {
				return appType;
			}
		}

		return RadioServiceDabUserApplicationType.RFU;
	}
}
