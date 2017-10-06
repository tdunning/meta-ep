package com.tdunning.meta;

import java.util.Random;

/**
 * A mutation changes old state into new.
 */
public interface Mutation<S extends State> {
    /**
     * Extrapolate from old through current state. If scale == 0, we
     * recreate the current state, if scale = -1, we get the old state.
     *
     * In addition, we add omni-directional noise with size specified by omni.
     *
     * @param old      The previous location
     * @param current  The current location
     * @param scale    The scale of directional mutation
     * @param omni     The scale of omni-directional mutation
     * @return A new state based on old and current.
     */
    S mutate(S old, S current, double scale, double omni, Random rand);
}
