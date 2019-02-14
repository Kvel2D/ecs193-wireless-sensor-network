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

def different_tx_rx_rate(differing_rates):
    tx_rate = differing_rates['tx_rate'].unique()
    rx_rate = differing_rates['rx_rate'].unique()
    X, Y = np.meshgrid(tx_rate, rx_rate)

    tx_time = differing_rates['tx_time']
    Z = tx_time.values.reshape(len(X), len(Y))
    print(Z)

    fig = plt.figure(2)
    ax = fig.gca(projection='3d')

    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.set_zlim(-1, 2)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    plt.xlabel('Avg time in between tx (ms)')
    plt.ylabel('Avg time in between rx (ms)')
    plt.title('Tx_time (percentage)')

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    fig.show()

def rx_rate3D(differing_rates):
    tx_rate = differing_rates['tx_rate'].unique()
    rx_rate = differing_rates['rx_rate'].unique()
    #print(differing_rates['tx_rate'])
    #print(differing_rates['rx_rate'])

    X, Y = np.meshgrid(tx_rate, rx_rate)

    # R = X + Y
    # print(R)

    rx_time = differing_rates['rx_time']
    Z = rx_time.values.reshape(len(X), len(Y))
    print(Z)

    fig = plt.figure(3)
    ax = fig.gca(projection='3d')


    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.set_zlim(-1, 10)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    plt.xlabel('Avg time in between tx (ms)')
    plt.ylabel('Avg time in between rx (ms)')
    plt.title('Rx_time (percentage)')

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    fig.show()

def avg_time_pkt3D(differing_rates):
    tx_rate = differing_rates['tx_rate'].unique()
    rx_rate = differing_rates['rx_rate'].unique()

    X, Y = np.meshgrid(tx_rate, rx_rate)

    avg_pkt_time = differing_rates['avg_transmission_time_per_pkt']
    Z = avg_pkt_time.values.reshape(len(X), len(Y))
    print(Z)

    fig = plt.figure(4)
    ax = fig.gca(projection='3d')

    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.set_zlim(0, 11000000)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    plt.xlabel('Avg time in between tx (ms)')
    plt.ylabel('Avg time in between rx (ms)')
    plt.title('Avg time to transmit a packet (ms)')

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    plt.show()

if __name__ == '__main__':
    varying_rates = read_data("varying_rate.csv")
    same_tx_rx_rate(varying_rates)

    differing_rates = read_data("../Dmitry/02-10-results.txt")
    different_tx_rx_rate(differing_rates)
    rx_rate3D(differing_rates)
    avg_time_pkt3D(differing_rates)

