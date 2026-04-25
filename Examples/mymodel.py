import numpy as np
import numpy.random as rng
from scipy.special import gammaln
from scipy.stats import binom
import celerite2
from celerite2 import terms
import matplotlib.pyplot as plt

rng.seed(1234)

num_params = 4

days = 365*100
window = np.arange(15000, 15000+20*365)
err = 0.015
n = 250
t = np.arange(days)
choice = rng.choice(window, size=n, replace=False)
choice = np.sort(choice)
t_obs = t[choice]

def generate_light_curve(params):
    mu = params[0]
    beta, tau, jitter = 10.0**params[1:]
    alpha = np.exp(-1.0/tau)
    sigma = beta*np.sqrt(0.5*tau)
    eps = sigma*np.sqrt(1.0 - alpha**2)

    y = np.empty(days)
    y[0] = mu + sigma*rng.randn()
    for i in range(1, days):
        y[i] = mu + alpha*(y[i-1] - mu) + eps*rng.randn()

    y_obs = y[choice] + np.sqrt(jitter**2 + err**2)*rng.randn(n)
    err_obs = err*np.ones(n)

    # Save the data
    data = np.column_stack((t_obs, y_obs, err_obs))

    return data



def prior_transform(us):
    params = us.copy()

    # mu
    params[0] = 15.0 + 10.0*us[0]

    # log10_beta
    params[1] = -1.5 + 1.0*us[1]

    # log10_tau
    params[2] = 2.0 + 5.0*us[2]

    # log10_jitter
    params[3] = -3.0 + 3.0*us[1]

    return params


def log_likelihood(params, data):

    logl = 0.0

    mu = params[0]
    beta, tau, jitter = 10.0**params[1:4]
    sigma = beta*np.sqrt(0.5*tau)

    #mu = np.mean(data[:,1])

    try:
        term = terms.RealTerm(a=sigma**2, c=1.0/tau)
        kernel = term
        gp = celerite2.GaussianProcess(kernel, mean=mu)
        gp.compute(data[:,0], yerr=np.sqrt(data[:,2]**2 + jitter**2))
        logl = gp.log_likelihood(data[:,1])
    except Exception:
        logl = -1.0E300

    return logl


def generate_data(params):
    """
    Generate a dataset.
    """
    return generate_light_curve(params)

def distance(params1, params2):
    """
    Distance to another particle in parameter space.
    """
    return np.abs(params1[2] - params2[2])

