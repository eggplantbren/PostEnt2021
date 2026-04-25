import numpy as np
import numpy.random as rng
from scipy.special import gammaln

num_params = 3

t = np.arange(1851, 1963)
t_min, t_max = np.min(t), np.max(t)
t_range = t_max - t_min

def prior_transform(us):
    params = us.copy()

    # lambda1, lambda2, change_year
    params[0] = 10.0*us[0]
    params[1] = 10.0*us[1]
    params[2] = t_min + t_range*us[2]

    return params


def calculate_mu(params):
    mu = params[0]*np.ones(len(t))
    mu[t > params[2]] = params[1]
    return mu

def log_likelihood(params, data):

    logl = 0.0

    mu = calculate_mu(params)

    # Poisson lambda^x exp(-lambda) / x!
    # Log of it = x*log(lambda) - lambda - gammaln(x+1)
    logl = np.sum(data*np.log(mu) - mu - gammaln(data + 1))

    return logl


def generate_data(params):
    """
    Generate a dataset.
    """
    return rng.poisson(calculate_mu(params))

def distance(params1, params2):
    """
    Distance to another particle in parameter space.
    """
    return np.abs(params1[2] - params2[2])

