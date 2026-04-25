import numpy as np
import numpy.random as rng
from scipy.special import gammaln
from scipy.stats import binom

num_params = 1

# Binomial number of trials parameter for the example.
N = 100

def prior_transform(us):
    params = us.copy()
    return params


def log_likelihood(params, data):
    """
    Log likelihood as in standard NS,
    but with the data passed in, as there will be different datasets.
    """
    theta = params[0]
    x = data
    logl = 0.0
    logl += gammaln(N + 1) - gammaln(x + 1) - gammaln(N + x - 1)
    logl += x*np.log(params[0]) + (N - x)*np.log(1.0 - theta)
    return logl


def generate_data(params):
    """
    Generate a dataset.
    """
    theta = params[0]
    data = binom.rvs(N, theta)
    return data

def distance(params1, params2):
    """
    Distance to another particle in parameter space.
    """
    return np.abs(params1[0] - params2[0])

