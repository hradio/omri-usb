package org.omri.radio.impl;

import org.omri.radioservice.metadata.ProgrammeInformationType;
import org.omri.radioservice.metadata.SpiProgrammeInformation;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

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

public class SpiProgrammeInformationImpl implements SpiProgrammeInformation {

	private Document mSpiDoc;

	SpiProgrammeInformationImpl(String spiPath) {
		try {
			DocumentBuilderFactory builderFac = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder = builderFac.newDocumentBuilder();
			mSpiDoc = docBuilder.parse(new File(spiPath));
		} catch(ParserConfigurationException parserExc) {
			if(DEBUG)parserExc.printStackTrace();
		} catch(IOException ioExc) {
			if(DEBUG)ioExc.printStackTrace();
		} catch(SAXException saxExc) {
			if(DEBUG)saxExc.printStackTrace();
		}
	}

	@Override
	public Document getSpiDocument() {
		return mSpiDoc;
	}

	@Override
	public ProgrammeInformationType getType() {
		return ProgrammeInformationType.METADATA_PROGRAMME_TYPE_SPI_EPG;
	}
}
