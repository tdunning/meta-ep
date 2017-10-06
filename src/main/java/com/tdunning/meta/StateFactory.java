package com.tdunning.meta;

/**
 * Interface that describes how to create state variables.
 */
public interface StateFactory {
    Mutation create();

    Mutation clone(Mutation old);
}
