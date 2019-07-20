import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import numpy as np
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import mplcursors


def plot(dependent):
    df = pd.read_csv('results.txt')
    print(df)

    tx_rate = df['tx_rate'].unique()
    rx_rate = df['rx_rate'].unique()

    X, Y = np.meshgrid(tx_rate, rx_rate)
    Z = df[dependent]
    Z = Z.values.reshape(len(X), len(Y))
    print(Z)

    fig = plt.figure()
    ax = fig.gca(projection='3d')

    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm, linewidth=0, antialiased=False)

    # ax.set_zlim(z_lower_bound, z_upper_bound)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%d'))
    plt.xlabel('tx rate')
    plt.ylabel('rx rate')
    plt.title(dependent)
    fig.colorbar(surf, shrink=0.5, aspect=5)
    fig.show()

    plt.show()

# plot('age')
plot('total_time')