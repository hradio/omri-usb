package org.omri.radioservice.metadata;

/**
 * Copyright (C) 2016 Open Mobile Radio Interface (OMRI) Group
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
 * Visual metadata types
 * @author Fabian Sattler, IRT GmbH
 */
public enum VisualMimeType {

	METADATA_VISUAL_MIMETYPE_UNKNOWN("image/unknown"),
	METADATA_VISUAL_MIMETYPE_JPEG("image/jpeg"),
	METADATA_VISUAL_MIMETYPE_PNG("image/png"),
	METADATA_VISUAL_MIMETYPE_TIFF("image/tiff"),
	METADATA_VISUAL_MIMETYPE_BMP("image/bmp"),
	METADATA_VISUAL_MIMETYPE_WEBP("image/webp"),
	METADATA_VISUAL_MIMETYPE_SVG("image/svg+xml"),
	METADATA_VISUAL_MIMETYPE_GIF("image/gif"),
	METADATA_VISUAL_MIMETYPE_ANIMATED_GIF("image/gif");

	private final String mMimeType;

	private VisualMimeType(String mimeType) {
		mMimeType = mimeType;
	}

	public String getMimeTypeString() {
		return mMimeType;
	}

	public static VisualMimeType getMimeFromString(String mimeString) {
		for(VisualMimeType mimeType : VisualMimeType.values()) {
			if(mimeType.getMimeTypeString().equals(mimeString)) {
				return mimeType;
			}
		}

		return METADATA_VISUAL_MIMETYPE_UNKNOWN;
	}
}
