import matplotlib.pyplot as plt
import math
import os
import pandas as pd
import numpy as np
import fnmatch

dirname = os.path.dirname(__file__)
filename = os.path.join(os.path.dirname(dirname), 'Data\\RawData\\roboticguidance_eda\\S7\\ball\\S7_M20_3_3.csv')

def get_dir(scene, control_mode):
    dir_list = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'ball':
                dir_list.append(os.path.join(root, name))
    return dir_list

for i in range(1,4):
    for j in range(1,4):
        plt.figure()
        dirs = get_dir(i,j)
        total_x = []
        total_y = []
        total_collision = 0
        ax = plt.gca()
        ax.set_xlim([-0.3, 0.3])
        ax.set_ylim([-0.3, 0.3])
        ax.set_yticklabels([])
        ax.set_xticklabels([])
        ax.set_aspect('equal')
        for dir in dirs:
            data = pd.read_csv(dir ,names =['timestamp', 'y', 'x', 'z'])
            collision_index = data[data['timestamp'] == 'Collision'].index
            collision_index = collision_index -1
            total_collision += len(collision_index)
            collision_data = data.iloc[collision_index]
            collision_pos_data = collision_data.loc[:,['y','x']]
            total_x += collision_data.loc[:,['x']].values.tolist()
            total_y += collision_data.loc[:,['y']].values.tolist()
        title = "Average number of collsion frames = " + str(int(total_collision/len(dirs)))
        ax.set_title(title)
        plt.scatter(total_x, total_y, s=1, marker=',')
plt.show()



# gamefilename = "S" + subject_num + "/ball" + "/S" + subject_num + "_" + subject_sex + subject_age + "_" + game_scene + "_" + control_mode + ".csv"
# best_ball_file_name = "S0/ball/S0_R0_" + game_scene + "_4.csv"

# game_data = np.array([])
# data = pd.read_csv(gamefilename)
# before_data = data.to_numpy()
# data.dropna(inplace=True)
# data = data.to_numpy()
# distance = 0
# distances = []

# num_collision = before_data.shape[0] - data.shape[0]
# print(num_collision)
# for i in range(1,data.shape[0]):
#     p1 = data[i-1][1:]
#     p2 = data[i][1:]
#     squared_dist = np.sum((p1-p2)**2, axis=0)
#     dist = np.sqrt(squared_dist)
#     distance += dist
#     distances.append(distance)

# game_data = np.array([])
# data = pd.read_csv(best_ball_file_name)
# before_data = data.to_numpy()
# data.dropna(inplace=True)
# data = data.to_numpy()
# best_distance = 0
# best_distances = []

# num_collision = before_data.shape[0] - data.shape[0]
# print(num_collision)
# for i in range(1,data.shape[0]):
#     p1 = data[i-1][1:]
#     p2 = data[i][1:]
#     squared_dist = np.sum((p1-p2)**2, axis=0)
#     dist = np.sqrt(squared_dist)
#     best_distance += dist
#     best_distances.append(best_distance)

# plt.plot(distances,'g',best_distances,'r')
# plt.show()
# print(distance)