package com.tdunning.meta.com.tdunning;

import com.tdunning.meta.EvolutionaryProcess;
import com.tdunning.meta.Mutation;
import com.tdunning.meta.State;

import java.util.Random;

/**
 * Finds a piece-wise rational approximation for asin.
 */
public class Asin {
    public static void main(String[] args) {
        Approx seed = new Approx();

        EvolutionaryProcess<Approx, ApproxMutator> ep =
                new EvolutionaryProcess<>(1, 100, seed, new ApproxMutator(), 0.1);

        for (int i = 0; i < 100; i++) {
            ep.mutatePopulation(10);
            EvolutionaryProcess.Step<Approx> best = ep.getPopulation().get(0);
            System.out.printf("%.4f, %.3f\n", best.fitness, best.omni);
        }
    }

    private static class ApproxMutator implements Mutation<Approx> {

        @Override
        public Approx mutate(Approx old, Approx current, double scale, double omni, Random rand) {
            omni = omni * 1e-3;
            Approx nextState = new Approx();
            for (int i = 0; i < current.params.length; i++) {
                nextState.params[i] = current.params[i] + (current.params[i] - old.params[i]) * scale + rand.nextGaussian() * omni;
            }
            // bounds
            for (int i = 0; i < 6; i++) {
                if (nextState.params[i] < 0) {
                    nextState.params[i] = 0;
                }
                if (nextState.params[i] > 1) {
                    nextState.params[i] = 1;
                }
            }
            // make sure that regions butt up cleanly (if not overlap)
            for (int i = 1; i < 6; i += 2) {
                if (nextState.params[i] < nextState.params[i + 1]) {
                    nextState.params[i] = nextState.params[i + 1];
                }
            }
            return nextState;
        }
    }

    private static class Approx implements State {
        // start at previous known solution
        double[] params = new double[]{
                // cutoffs
                0.1, 0.55, 0.5, 0.8, 0.75, 0.9, 0.87,
                // models
                0.2955302411, 1.2221903614, 0.1488583743, 0.2422015816, -0.3688700895, 0.0733398445,
                -0.0430991920, 0.9594035750, -0.0362312299, 0.1204623351, 0.0457029620, -0.0026025285,
                -0.034873933724, 1.054796752703, -0.194127063385, 0.283963735636, 0.023800124916, -0.000872727381,
                -0.37588391875, 2.61991859025, -2.48835406886, 1.48605387425, 0.00857627492, -0.00015802871
        };

        private static double c0High;
        private static double c1High;
        private static double c2Low;
        private static double c2High;
        private static double c3Low;
        private static double c3High;
        private static double c4Low;
        private static double[] m0 = new double[6];
        private static double[] m1 = new double[6];
        private static double[] m2 = new double[6];
        private static double[] m3 = new double[6];

        @Override
        public double eval() {
            int i = 0;
            c0High = params[i++];
            c1High = params[i++];
            c2Low = params[i++];
            c2High = params[i++];
            c3Low = params[i++];
            c3High = params[i++];
            c4Low = params[i++];
            for (int j = 0; j < 6; j++) {
                m0[j] = params[i++];
            }
            for (int j = 0; j < 6; j++) {
                m1[j] = params[i++];
            }

            for (int j = 0; j < 6; j++) {
                m2[j] = params[i++];
            }
            for (int j = 0; j < 6; j++) {
                m3[j] = params[i++];
            }

            double error = 0;
            int n = 0;
            for (double x = 0; x < 1; x += 1e-4) {
                error += Math.abs(fastAsin(x) - Math.asin(x));
                n++;
            }
            return (error / n * 1e6 + c0High + 1 - c3High);
        }


        static double fastAsin(double x) {
            if (x < 0) {
                return -fastAsin(-x);
            } else if (x > 1) {
                return Double.NaN;
            } else {
                // Cutoffs for models. Note that the ranges overlap. In the
                // overlap we do linear interpolation to guarantee the overall
                // result is "nice"
                if (x > c3High) {
                    return Math.asin(x);
                } else {
                    // the models

                    // the parameters for all of the models
                    double[] vars = {1, x, x * x, x * x * x, 1 / (1 - x), 1 / (1 - x) / (1 - x)};

                    // raw grist for interpolation coefficients
                    double x0 = bound((c0High - x) / c0High);
                    double x1 = bound((c1High - x) / (c1High - c2Low));
                    double x2 = bound((c2High - x) / (c2High - c3Low));
                    double x3 = bound((c3High - x) / (c3High - c4Low));

                    // interpolation coefficients
                    //noinspection UnnecessaryLocalVariable
                    double mix0 = x0;
                    double mix1 = (1 - x0) * x1;
                    double mix2 = (1 - x1) * x2;
                    double mix3 = (1 - x2) * x3;
                    double mix4 = 1 - x3;

                    // now mix all the results together, avoiding extra evaluations
                    double r = 0;
                    if (mix0 > 0) {
                        r += mix0 * eval(m0, vars);
                    }
                    if (mix1 > 0) {
                        r += mix1 * eval(m1, vars);
                    }
                    if (mix2 > 0) {
                        r += mix2 * eval(m2, vars);
                    }
                    if (mix3 > 0) {
                        r += mix3 * eval(m3, vars);
                    }
                    if (mix4 > 0) {
                        // model 4 is just the real deal
                        r += mix4 * Math.asin(x);
                    }
                    return r;
                }
            }
        }

        private static double eval(double[] model, double[] vars) {
            double r = 0;
            for (int i = 0; i < model.length; i++) {
                r += model[i] * vars[i];
            }
            return r;
        }

        private static double bound(double v) {
            if (v <= 0) {
                return 0;
            } else if (v >= 1) {
                return 1;
            } else {
                return v;
            }
        }

    }
}
