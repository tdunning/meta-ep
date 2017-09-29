# Meta evolution using the recorded step algorithm from
# Recorded Step Directional Mutation for Faster Convergence
# by Ted Dunning
# available from
# http://arxiv.org/abs/0803.3838

ep = function(x0, f, omni=norm(x0)/10, eps=f(x0)*1e-6, max.iterations=50, population=500, survivors=100, mode.coupling=0.05) {
    m = length(x0)
    kept = (diag(population) %*% x0) +
        matrix(rnorm(survivors * m, 0, omni), nrow=survivors)
    values = apply(kept, margin=1, f)
    old = kept
    omni = rep(omni, population)
    for (i in 1:max.iterations) {
        candidates = matrix(diag(survivors), nrow=population, ncol=survivors) %*% kept

        # mutate, recorded step
        step = (candidates-old)

        step = diag(rnorm(population) + 1) %*% step
        omniMutate = matrix(rnorm(population * m, 0, omni), nrow=population)
        old = x
        # mutate, omni-directional mutation
        x = x + step + omniMutate

        # now mutate omni rate according to itself and
        # somewhat according to how big the step was
        omni = omni * rexp(population, rate=1.05) +
            mode.coupling * sqrt(rowSums((x-old)^2))

        # evaluate function
        values = c(values, apply(candidates, margin=1, f))
        candidates = rbind(survivors, candidates)

        # retain best
        i = order(values)
        survivors = candidates[i[1:survivors],]
        values = values[i[1:survivors]]
    }
}
