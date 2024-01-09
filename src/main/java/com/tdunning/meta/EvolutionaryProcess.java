/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tdunning.meta;

import com.google.common.collect.Lists;
import com.google.common.collect.Ordering;

import java.io.Closeable;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

/**
 * Evolves a population using recorded-step meta-mutation.
 */
public class EvolutionaryProcess<S extends State, M extends Mutation<S>> implements Closeable {
    // used to execute operations on the population in thread parallel.
    private ExecutorService pool;
    private Random rand = new Random();

    private int threadCount;

    public static class Step<S extends State> implements Comparable<Step> {
        public double fitness;
        public double omni;
        S old, current;

        public Step(S old, S current, double omni) {
            this.old = old;
            this.current = current;
            fitness = current.eval();
            this.omni = omni;
        }

        @Override
        public int compareTo(Step o) {
            return Double.compare(o.fitness, fitness);
        }
    }

    // list of members of the population
    private List<Step<S>> population;
    private Mutation<S> mutator;

    // how big should the population be.  If this is changed, it will take effect
    // the next time the population is mutated.
    private int populationSize;

    /**
     * Creates an evolutionary optimization framework with specified threadiness,
     * population size and initial state.
     *
     * @param threadCount    How many threads to use in parallelDo
     * @param populationSize How large a population to use
     * @param seed           An initial population member
     * @param scale          The scale of the mutation to the seed for creating the initial population.
     */
    @SuppressWarnings("WeakerAccess")
    public EvolutionaryProcess(int threadCount, int populationSize, S seed, M mutator, double scale) {
        this.populationSize = populationSize;
        this.mutator = mutator;
        setThreadCount(threadCount);
        initializePopulation(populationSize, seed, mutator, scale);
    }

    private void initializePopulation(int populationSize, S seed, M change, double scale) {
        population = Lists.newArrayList();
        for (int i = 0; i < populationSize; i++) {
            population.add(new Step<>(seed, change.mutate(seed, seed, 0, scale, rand), 1));
        }
    }

    /**
     * Nuke all but a few of the current population and then repopulate with
     * variants of the survivors.
     *
     * @param survivors How many survivors we want to keep.
     */
    @SuppressWarnings("WeakerAccess")
    public void mutatePopulation(int survivors) {
        population.sort(Comparator.reverseOrder());
        List<Step<S>> parents = new ArrayList<>(population.subList(0, survivors));
        population.subList(survivors, population.size()).clear();

        // fill out the population with offspring from the survivors
        int i = 0;
        while (population.size() < populationSize) {
            Step<S> parent = parents.get(i % survivors);
            double linear = rand.nextGaussian();
            Step<S> p = new Step<>(
                    parent.current,
                    mutator.mutate(parent.old, parent.current, linear, parent.omni, rand),
                    -1.1 * parent.omni * Math.log(1 - rand.nextDouble()));

            population.add(p);
            i++;
        }
    }

    @SuppressWarnings("WeakerAccess")
    public void setThreadCount(int threadCount) {
        this.threadCount = threadCount;
        pool = Executors.newFixedThreadPool(threadCount);
    }

    public int getThreadCount() {
        return threadCount;
    }

    public int getPopulationSize() {
        return populationSize;
    }

    @SuppressWarnings("WeakerAccess")
    public List<Step<S>> getPopulation() {
        population.sort(Comparator.reverseOrder());
        List<S> r = Lists.newArrayList();
        for (Step<S> x : population) {
            r.add(x.current);
        }
        return population;
    }

    @Override
    public void close() {
        List<Runnable> remainingTasks = pool.shutdownNow();
        try {
            pool.awaitTermination(10, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            throw new IllegalStateException("Had to forcefully shut down " + remainingTasks.size() + " tasks");
        }
        if (!remainingTasks.isEmpty()) {
            throw new IllegalStateException("Had to forcefully shut down " + remainingTasks.size() + " tasks");
        }
    }
}
