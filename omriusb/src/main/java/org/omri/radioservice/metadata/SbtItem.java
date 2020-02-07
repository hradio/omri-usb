package org.omri.radioservice.metadata;

public interface SbtItem {

	public long getId();

	public long getPosixTime();

	public boolean getItemToggleState();

	public boolean getItemRunningState();

	public TextualDabDynamicLabel getDls();

	public VisualDabSlideShow getSls();
}
