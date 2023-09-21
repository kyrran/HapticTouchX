from turtle import position
import matplotlib.pyplot as plt
import math
import os
import pandas as pd
import numpy as np
import fnmatch

emptyMap = np.zeros([61,61])

# def get_dir_ball(scene, control_mode):
#     dir_list = []
#     for root, dirs, files in os.walk(".", topdown=False):
#         for name in files:
#             if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'ball':
#                 dir_list.append(os.path.join(root, name))   
#     return dir_list

# def get_dir_force(scene, control_mode):
#     dir_list = []
#     for root, dirs, files in os.walk(".", topdown=False):
#         for name in files:
#             if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '_force.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'HIP':
#                 dir_list.append(os.path.join(root, name))
#     return dir_list

def get_dir_force(scene, control_mode):
    dir_list = []
    dir_list2 = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '_force.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'CIP':
                dir_list.append(os.path.join(root, name))
                dir_list2.append(os.path.join(os.path.dirname(root),"ball\\"+name.replace('_force','')))
    return dir_list, dir_list2

for o in range(2,4):
    for p in range(1,4):
        emptyMap = np.zeros([61,61])
        plt.figure()
        dir1,dir2 = get_dir_force(p,o)
        for i in range(len(dir1)):
            force_test = dir1[i]
            ball_test = dir2[i]

            position_data = pd.read_csv(ball_test ,names =['timestamp', 'y', 'x', 'z'])
            position_data.dropna(axis=0, inplace=True, how='any')
            position_data['timestamp'] = position_data['timestamp'].astype('int64')
            force_data = pd.read_csv(force_test, names =['timestamp', 'y_f', 'x_f', 'z_f'])
            force_data['timestamp'] = force_data['timestamp'].astype('int64')
            total_data=position_data.merge(force_data , on='timestamp', how='inner')

            total_data['x'] = total_data['x'] + 0.3
            total_data['x'] = total_data['x'] * 100
            total_data['y'] = total_data['y'] + 0.3
            total_data['y'] = total_data['y'] * 100
            total_data = total_data.round({'x':0,'y':0})
            total_data = total_data.iloc[2000:,:]
            for index, row in total_data.iterrows():
                emptyMap[int(row['x'])][int(row['y'])] = emptyMap[int(row['x'])][int(row['y'])] + min(math.sqrt(row['x_f']**2 + row['y_f']**2),3)
            

        plt.contourf(emptyMap, cmap='inferno')
        plt.colorbar()
        ax = plt.gca()
        ax.set_yticklabels([])
        ax.set_xticklabels([])

      
plt.show()