package com.tdunning.meta;

/**
 * State can be evaluated. The result should be bigger if better.
 */
public interface State {
    double eval();
}
