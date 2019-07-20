import matplotlib.pyplot as plt

names = ['outdoor-a', 'outdoor-b', 'outdoor-c', 'shelter3-a', 'shelter3-b']

def plot(folder):
    values_list = []

    for name in names:
        file = open('%s/%s.txt' % (folder, name))
        values = []
        for line in file:
            values.append(int(line))
        file.close()

        values_list.append(values)

    plt.clf()
    plt.title(folder)
    plt.ylabel('packet age (ms)')
    plt.ylim(top=200)
    plt.boxplot(values_list, labels=names)
    plt.savefig('%s.png' % folder)

plot('simulation')
plot('deployment')
