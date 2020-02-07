package org.omri.radio.impl;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.omri.radioservice.metadata.TextualDabDynamicLabel;
import org.omri.radioservice.metadata.TextualDabDynamicLabelPlusItem;
import org.omri.radioservice.metadata.TextualType;

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

public class TextualDabDynamicLabelImpl extends TextualImpl implements TextualDabDynamicLabel, Serializable {

	private static final long serialVersionUID = 342793136525104857L;

	private List<TextualDabDynamicLabelPlusItem> mDlpItemsList = new ArrayList<TextualDabDynamicLabelPlusItem>();
	private boolean mItemRunning = false;
	private boolean mItemToggled = false;
	
	@Override
	public TextualType getType() {
		return TextualType.METADATA_TEXTUAL_TYPE_DAB_DLS;
	}

	@Override
	public boolean hasTags() {
		return mDlpItemsList.size() > 0;
	}

	@Override
	public int getTagCount() {
		return mDlpItemsList.size();
	}

	@Override
	public List<TextualDabDynamicLabelPlusItem> getDlPlusItems() {
		return mDlpItemsList;
	}
	
	void addDlPlusItem(TextualDabDynamicLabelPlusItem dlpItem) {
		this.mDlpItemsList.add(dlpItem);
	}

	void addDlPlusItems(List<TextualDabDynamicLabelPlusItem> dlpItems) {
		this.mDlpItemsList.addAll(dlpItems);
	}

	@Override
	public boolean itemRunning() {
		return mItemRunning;
	}

	void setItemRunning(boolean itemRunning) {
		mItemRunning = itemRunning;
	}

	@Override
	public boolean itemToggled() {
		return mItemToggled;
	}

	void setItemToggled(boolean itemToggled) {
		mItemToggled = itemToggled;
	}
}
