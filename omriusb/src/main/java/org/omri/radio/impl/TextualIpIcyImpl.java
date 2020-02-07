package org.omri.radio.impl;

import org.omri.radioservice.metadata.TextualIpIcy;
import org.omri.radioservice.metadata.TextualType;

import java.io.Serializable;

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

public class TextualIpIcyImpl implements TextualIpIcy, Serializable {

	private static final long serialVersionUID = 2243275121558548893L;
	
	private final String mText;

	TextualIpIcyImpl(String text) {
		mText = text;
	}

	@Override
	public TextualType getType() {
		return TextualType.METADATA_TEXTUAL_TYPE_ICY_TEXT;
	}

	@Override
	public String getText() {
		return mText;
	}
}
