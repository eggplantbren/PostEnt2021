radius = 1E-4
ndim = 1
nball_volume = pi^(0.5*ndim)/gamma(0.5*ndim + 1)*radius^ndim

# Load the output
output = read.csv("output.csv")

# Split the runs
runs = split(output, output$ns_run_id)

# Compute depth for a run
depth = function(run)
{
    subset = run[run$distance < radius, ]
    if(nrow(subset) == 0)
        return(NA)
    subset$depth[1]
}

# Compute crossing times/depths
depths = sapply(runs, depth)

# Warn if any NAs except in the final position
nas = is.na(depths)
num_nas = sum(as.integer(nas))
if(num_nas > 0)
{
    head = nas[-length(nas)]
    if(any(head))
        stop("Incomplete runs found that ARE NOT the final run. Quitting. You can try a bigger radius.")

    cat("Excluding final run as it has not reached the required radius.\n")
    depths = depths[!nas]
}
cat("Using", length(depths), "runs for calculation.\n")

# Mean -log(mass) inside ball. To convert to log(density) you need
# log(density) = log(prob) - log(volume). Then, the posterior entropy
# is minus the mean log(density).
mean_depth = mean(depths)
h = mean_depth + log(nball_volume)
sd_depths = sd(depths)
sem = sd_depths / sqrt(length(depths))

cat("mean(depth) =", mean_depth, "nats.\n")
cat("sd(depth) =", sd_depths, "nats.\n")
cat("H(theta | d) =", h, "+-", sem, "nats.\n")
