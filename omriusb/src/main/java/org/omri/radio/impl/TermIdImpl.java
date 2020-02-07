package org.omri.radio.impl;

import org.omri.radioservice.metadata.TermId;

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

public class TermIdImpl implements TermId, Serializable {

	private static final long serialVersionUID = -3003567186963940474L;

	private String mGenreHref = "";
	private String mTermId = "";
	private String mGenreText = "";

	@Override
	public String getGenreHref() {
		return mGenreHref;
	}

	void setGenreHref(String href) {
		mGenreHref = href;
	}

	@Override
	public String getTermId() {
		return mTermId;
	}

	void setTermId(String termId) {
		mTermId = termId;
	}

	@Override
	public String getText() {
		return mGenreText;
	}

	void setGenreText(String genre) {
		mGenreText = genre;
	}
}
