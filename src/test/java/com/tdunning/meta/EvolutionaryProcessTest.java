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

import org.junit.Test;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

import static org.junit.Assert.assertEquals;

public final class EvolutionaryProcessTest {
    public interface Evaluator {
        double eval(double[] x);
    }

    public class VectorState implements State {
        double[] state;
        private Evaluator fitness;

        VectorState(double[] state, Evaluator fitness) {
            this.state = Arrays.copyOf(state, state.length);
            this.fitness = fitness;
        }

        @Override
        public double eval() {
            return fitness.eval(state);
        }
    }

    public class VectorMutator implements Mutation<VectorState> {
        @Override
        public VectorState mutate(VectorState old, VectorState current, double scale, double omni, Random rand) {
            VectorState r = new VectorState(current.state, current.fitness);

            for (int i = 0; i < r.state.length; i++) {
                r.state[i] = current.state[i] + (current.state[i] - old.state[i]) * scale + omni * rand.nextGaussian();
            }
            return r;
        }
    }

    private double[] randomArray(int n, Random rand, int offset, double sd) {
        double[] r = new double[n];
        for (int i = 0; i < n; i++) {
            r[i] = sd *rand.nextGaussian() + offset;
        }
        return r;
    }

    @Test
    public void testNarrow() throws FileNotFoundException {
        Random rand = new Random();

        VectorState seed = new VectorState(randomArray(2, rand, 5, 1), (double[] sx) -> {
            double x, y;
            if (Math.abs(sx[0]) > 100 || Math.abs(sx[1]) > 100) {
                return 200;
            }
            x = sx[0] - 1 + sx[1] - 2;
            y = 100 * (-sx[0] + 1 + sx[1] - 2);
            return (x * x + y * y);
        });
        EvolutionaryProcess<VectorState, VectorMutator> ep =
                new EvolutionaryProcess<>(1, 100, seed, new VectorMutator(), 2);

        checkConvergence(ep, "narrow.csv", 20);
    }

    @Test
    public void testBowl() throws FileNotFoundException {
        Random rand = new Random();

        VectorState seed = new VectorState(randomArray(50, rand, 2, 1), (double[] x) -> {
            double r = 0;
            for (double v : x) {
                r += v * v;
            }
            return Math.sqrt(r);
        });
        EvolutionaryProcess<VectorState, VectorMutator> ep =
                new EvolutionaryProcess<>(1, 100, seed, new VectorMutator(), 0.01);

        checkConvergence(ep, "bowl.csv", 450);
    }

    @Test
    public void testSteps() throws FileNotFoundException {
        Random rand = new Random();

        VectorState seed = new VectorState(randomArray(5, rand, 3, 0.1), (double[] sx) -> {
            int i;
            double sum, x;
            sum = 0;
            for (i = 0; i < 5; i++) {
                x = sx[i];
                if (x < 0 || x > 5.1) return 1000;
                sum += Math.floor(x + 0.7);
            }
            return sum;
        });
        EvolutionaryProcess<VectorState, VectorMutator> ep =
                new EvolutionaryProcess<>(1, 100, seed, new VectorMutator(), 0.01);

        checkConvergence(ep, "steps.csv", 100);
    }

    private void checkConvergence(EvolutionaryProcess<VectorState, VectorMutator> ep, String filename, int steps) throws FileNotFoundException {
        PrintWriter out = new PrintWriter(filename);
        out.printf("i,j,fitness,omni");
        for (int i = 0; i < ep.getPopulation().get(0).current.state.length; i++) {
            out.printf(",x%d", i);
        }
        out.printf("\n");
        for (int i = 0; i < steps; i++) {
            ep.mutatePopulation(10);
            List<EvolutionaryProcess.Step<VectorState>> best = ep.getPopulation();
            for (int j = 0; j < 20; j++) {
                EvolutionaryProcess.Step<VectorState> x = best.get(j);
                out.printf("%d,%d,%.3g,%.3g", i, j, x.fitness, x.omni);
                for (int k = 0; k < x.current.state.length; k++) {
                    out.printf(",%.3f", x.current.state[k]);
                }
                out.printf("\n");
            }
        }
        ep.close();

        EvolutionaryProcess.Step<VectorState> best = ep.getPopulation().get(0);
        assertEquals(0, best.fitness, 0.02);
        assertEquals(0, best.omni, 0.02);
        out.close();
    }
}
