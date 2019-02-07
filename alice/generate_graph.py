import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import mplcursors

def read_data(file):
    ret = pd.read_csv(file)
    print(ret)
    return ret

def same_tx_rx_rate(varying_rates):
    plt.figure()
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
    plt.show()

def different_tx_rx_rate(differing_rates):
    fig = plt.figure()
    ax = fig.gca(projection='3d')


    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.set_zlim(-1.01, 1.01)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    plt.show()


if __name__ == '__main__':
    varying_rates = read_data("varying_rate.csv")
    same_tx_rx_rate(varying_rates)

    #differing_rates = read_data("differing_rates.csv")