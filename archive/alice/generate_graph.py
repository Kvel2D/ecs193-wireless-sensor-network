import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import numpy as np
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import mplcursors

def read_data(file):
    ret = pd.read_csv(file)
    #print(ret)
    return ret

def same_tx_rx_rate(varying_rates):
    fig1 = plt.figure(1)
    plt.subplot(1, 2, 1)
    # fig, ax = plt.subplots()

    plt.plot(varying_rates['tx_rate'], varying_rates['n0_awake_time'])
    plt.plot(varying_rates['rx_rate'], varying_rates['n1_awake_time'])
    plt.xlabel('Avg Time in between Transmission / Receiving')
    plt.ylabel('% Awake Time')
    plt.title('Varying Tranmission / Receiving Rate')
    plt.legend(loc="lower right")

    # mplcursors.cursor()

    plt.subplot(1, 2, 2)
    plt.plot(varying_rates['tx_rate'], varying_rates['avg_transmission_time_per_pkt'] / 1000)
    plt.xlabel('Avg Time in between Transmission')
    plt.ylabel('seconds')
    plt.title('Avg Transmission Time per Pkt')
    fig1.show()


def generate_3d(df, dependent, z_lower_bound, z_upper_bound, fig_id, title):
    tx_rate = df['tx_rate'].unique()
    rx_rate = df['rx_rate'].unique()
    X, Y = np.meshgrid(tx_rate, rx_rate)

    Z = df[dependent]
    Z = Z.values.reshape(len(X), len(Y))
    print(Z)

    fig = plt.figure(fig_id)
    ax = fig.gca(projection='3d')

    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.set_zlim(z_lower_bound, z_upper_bound)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    plt.xlabel('Avg time in between tx (ms)')
    plt.ylabel('Avg time in between rx (ms)')
    plt.title(title)

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    fig.show()



if __name__ == '__main__':
    varying_rates = read_data("varying_rate.csv")
    same_tx_rx_rate(varying_rates)

    differing_rates = read_data("../Dmitry/02-10-results.txt")
    generate_3d(differing_rates, 'tx_time', -1, 3, 2, 'tx_time (percentage)')
    generate_3d(differing_rates, 'rx_time', -1, 10, 3, 'rx_time (percentage)')
    generate_3d(differing_rates, 'avg_transmission_time_per_pkt', 0, 11000000, 4, 'avg_transmission_time_per_pkt (ms)')
    # plt.show()

    fixed_slot = read_data("../Jiahui Dai/fixSlot_varying_rate.csv")
    generate_3d(fixed_slot, 'n0_awake_time', -1, 3, 5, 'n0_awake_time (percentage)')
    generate_3d(fixed_slot, 'n1_awake_time', -1, 3, 6, 'n1_awake_time (percentage)')
    generate_3d(fixed_slot, 'avg_transmission_time_per_pkt', 0, 950000, 7, 'avg_transmission_time_per_pkt (ms)')
    plt.show()