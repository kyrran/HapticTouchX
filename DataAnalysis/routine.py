
from re import I
from turtle import position
import matplotlib.pyplot as plt
import math
import os
import pandas as pd
import numpy as np
import fnmatch
import numpy.linalg as LA

def get_dir(scene, control_mode):
    dir_list = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'ball':
                dir_list.append(os.path.join(root, name))
    return dir_list

for k in range(1,4):
    for j in range(1,4):
        plt.figure()
        dir = get_dir(k,j)
        print(dir)
        for i in range(len(dir)):
            data = pd.read_csv(dir[i] ,names =['timestamp', 'y', 'x', 'z'])
            data['y'] = -data['y']
            ax = plt.gca()
            ax.set_xlim([-0.3, 0.3])
            ax.set_ylim([-0.3, 0.3])
            ax.set_yticklabels([])
            ax.set_xticklabels([])
            ax.set_aspect('equal')
            x_value = data.loc[:,['x']].values.tolist()
            y_value = data.loc[:,['y']].values.tolist()
            plt.plot(x_value, y_value, linewidth=0.8)

plt.show()