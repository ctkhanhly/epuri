import seaborn as sns
import pandas as pd
import numpy as np

sns.set_theme(style="darkgrid")

sandboxed = []
net = []
with open("sandboxed_net_time.txt", "r") as f:
    for line in f.readlines():
        sandboxed.append(float(line.rstrip()))
with open("net_time.txt", "r") as f:
    for line in f.readlines():
        net.append(float(line.rstrip()))
iteration = list(range(1, 101))
data = {'sandboxed_net': np.array(sandboxed)*1000, 'net': np.array(net)*1000, 'Iteration': iteration}
df = pd.DataFrame(data)
             
lplot=sns.lineplot(x='Iteration', y='Connection Time (ms)', hue='Experiment Name', 
             data=pd.melt(df, ['Iteration'], var_name='Experiment Name', 
              value_name='Connection Time (ms)'))
fig = lplot.get_figure()
fig.savefig("out.png") 