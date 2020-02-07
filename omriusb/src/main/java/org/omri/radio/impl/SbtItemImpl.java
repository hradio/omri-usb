package org.omri.radio.impl;

import org.omri.radioservice.metadata.SbtItem;
import org.omri.radioservice.metadata.TextualDabDynamicLabel;
import org.omri.radioservice.metadata.VisualDabSlideShow;

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

public class SbtItemImpl implements SbtItem {

	private final long mItemToggleId;
	private final long mPosixTime;
	private final boolean mToggleState;
	private final boolean mRunningState;
	private final TextualDabDynamicLabel mDls;
	private final VisualDabSlideShow mSls;

	public SbtItemImpl(long toggleId, long posixTime, boolean toggleState, boolean runningState, TextualDabDynamicLabel dls, VisualDabSlideShow sls) {
		mItemToggleId = toggleId;
		mPosixTime = posixTime;
		mToggleState = toggleState;
		mRunningState = runningState;
		mDls = dls;
		mSls = sls;
	}

	@Override
	public long getId() {
		return mItemToggleId;
	}

	@Override
	public long getPosixTime() {
		return mPosixTime;
	}

	@Override
	public boolean getItemToggleState() {
		return mToggleState;
	}

	@Override
	public boolean getItemRunningState() {
		return mRunningState;
	}

	@Override
	public TextualDabDynamicLabel getDls() {
		return mDls;
	}

	@Override
	public VisualDabSlideShow getSls() {
		return mSls;
	}
}
