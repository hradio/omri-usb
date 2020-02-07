package org.omri.radio.impl;

import org.omri.radioservice.metadata.TextualDabDynamicLabelPlusContentType;
import org.omri.radioservice.metadata.TextualDabDynamicLabelPlusItem;

import java.io.Serializable;
import java.io.UnsupportedEncodingException;

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

public class TextualDabDynamicLabelPlusItemImpl implements TextualDabDynamicLabelPlusItem, Serializable {

	private static final long serialVersionUID = 4395469276512935565L;

	private TextualDabDynamicLabelPlusContentType mContentType = TextualDabDynamicLabelPlusContentType.DUMMY;
	private String mDlpItemText = "";
	
	@Override
	public TextualDabDynamicLabelPlusContentType getDynamicLabelPlusContentType() {
		return mContentType;
	}
	
	void setDlPlusContentType(int contType) {
		if(contType <= TextualDabDynamicLabelPlusContentType.values().length) {
			mContentType = TextualDabDynamicLabelPlusContentType.values()[contType];
		}
	}

	@Override
	public String getDlPlusContentTypeDescription() {
		return mContentType.getContentTypeString();
	}

	@Override
	public String getDlPlusContentCategory() {
		return mContentType.toString();
	}

	@Override
	public String getDlPlusContentText() {
		return mDlpItemText;
	}

	void setDlPlusContentText(String tagText) {
		mDlpItemText = tagText;
	}

	void setDlPlusContentText(byte[] dlpTextBytes, int charset) {
		try {
			switch(charset) {
				case 0: {
					for(byte temp : dlpTextBytes) {
						mDlpItemText += TextualImpl.EBUChar.EBU_SET[TextualImpl.EBUChar.getRow(temp)][TextualImpl.EBUChar.getCel(temp)];
					}
					break;
				}
				case 4: {
					mDlpItemText = new String(dlpTextBytes, 0, dlpTextBytes.length, "ISO-8859-1");
					break;
				}
				default: {
					mDlpItemText = new String(dlpTextBytes);
					break;
				}
			}
		} catch(UnsupportedEncodingException unsupEnc) {
			//
		}
	}
}
