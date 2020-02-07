package org.omri.radio;

import org.omri.tuner.Tuner;

public interface RadioStatusListener extends RadioListener {

	/**
	 * Signals that a new {@link Tuner} device was attached
	 * @param attachedTuner the new attached {@link Tuner}
	 */
	void tunerAttached(Tuner attachedTuner);

	/**
	 * Signals that a {@link Tuner} device was detached and is not available anymore
	 * @param detachedTuner the detached {@link Tuner} device
	 */
	void tunerDetached(Tuner detachedTuner);
}
