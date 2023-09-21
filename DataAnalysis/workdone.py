from turtle import position
import matplotlib.pyplot as plt
import math
import os
import pandas as pd
import numpy as np
import fnmatch
import numpy.linalg as LA

from scipy.stats import f_oneway
from scipy.stats import ttest_ind
import statsmodels.api as sm
from statsmodels.formula.api import ols
import scipy.stats as stats


### TAKE NOTE: The position of HIP is 10 times smaller than the game because it extract from the haptics device but not the game itself. The workspace ratio is 10.
### Therefore we need to make it 10 times larger before we process it.

def get_dirs_CIP(scene, control_mode):
    dir_list = []
    dir_list2 = []
    dir_list3 = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '_force.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'CIP':
                dir_list.append(os.path.join(root, name))
                dir_list2.append(os.path.join(root,name.replace('_force','')))
                dir_list3.append(os.path.join(os.path.dirname(root),"ball\\"+name.replace('_force','')))
    return dir_list, dir_list2, dir_list3

def get_dirs_HIP(scene, control_mode):
    dir_list = []
    dir_list2 = []
    dir_list3 = [] 
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '_force.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'HIP':
                dir_list.append(os.path.join(root, name))
                dir_list2.append(os.path.join(root,name.replace('_force','')))
                dir_list3.append(os.path.join(os.path.dirname(root),"ball\\"+name.replace('_force','')))
    return dir_list, dir_list2, dir_list3


def get_dirs_ALL(scene, control_mode):
    HIP_force_dirs =[]
    HIP_dirs = []
    CIP_force_dirs = []
    CIP_dirs = []
    ball_dirs = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '_force.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'HIP':
                HIP_force_dirs.append(os.path.join(root, name))
                HIP_dirs.append(os.path.join(root,name.replace('_force','')))
                CIP_force_dirs.append(os.path.join(os.path.dirname(root),"CIP\\"+name))
                CIP_dirs.append(os.path.join(os.path.dirname(root),"CIP\\"+name.replace('_force','')))
                ball_dirs.append(os.path.join(os.path.dirname(root),"ball\\"+name.replace('_force','')))
    return HIP_force_dirs, HIP_dirs, CIP_force_dirs, CIP_dirs, ball_dirs


def workovertime(position_dir, force_dir):
    position_data = pd.read_csv(position_dir ,names =['timestamp', 'y', 'x', 'z'])
    position_data.dropna(axis=0, inplace=True, how='any')
    position_data['timestamp'] = position_data['timestamp'].astype('int64')
    force_data = pd.read_csv(force_dir, names =['timestamp', 'y_f', 'x_f', 'z_f'])
    force_data['timestamp'] = force_data['timestamp'].astype('int64')
    total_data=position_data.merge(force_data , on='timestamp', how='inner')
    total_data['dx'] = total_data['x'].diff()
    total_data['dy'] = total_data['y'].diff()

    total_data.dropna(axis=0, inplace=True, how='any')

    work = 0

    for index, row in total_data.iterrows():
        a = np.array([row['x_f'], row['y_f']])
        b = np.array([row['dx'], row['dy']])
        inner = np.inner(a, b)
        norms = LA.norm(a) * LA.norm(b)

        if norms == 0.0:
            workdone = 0
        else:
            cos = inner / norms
            rad = np.arccos(np.clip(cos, -1.0, 1.0))
            workdone = norms * np.sin(rad)
        work+= workdone
    return work

def workovertime_HIP(position_dir, force_dir):
    position_data = pd.read_csv(position_dir ,names =['timestamp', 'y', 'x', 'z'])
    position_data.dropna(axis=0, inplace=True, how='any')
    position_data['timestamp'] = position_data['timestamp'].astype('int64')
    force_data = pd.read_csv(force_dir, names =['timestamp', 'y_f', 'x_f', 'z_f'])
    force_data['timestamp'] = force_data['timestamp'].astype('int64')
    total_data=position_data.merge(force_data , on='timestamp', how='inner')
    total_data['x'] = total_data['x'] *10
    total_data['y'] = total_data['y'] *10
    total_data['dx'] = total_data['x'].diff()
    total_data['dy'] = total_data['y'].diff()

    total_data.dropna(axis=0, inplace=True, how='any')

    work = 0

    for index, row in total_data.iterrows():
        a = np.array([row['x_f'], row['y_f']])
        b = np.array([row['dx'], row['dy']])
        inner = np.inner(a, b)
        norms = LA.norm(a) * LA.norm(b)

        if norms == 0.0:
            workdone = 0
        else:
            cos = inner / norms
            rad = np.arccos(np.clip(cos, -1.0, 1.0))
            workdone = norms * np.sin(rad)
        work+= workdone
    return work

def total_work(position_dir, HIP_dir, CIP_dir):
    position_data = pd.read_csv(position_dir ,names =['timestamp', 'y', 'x', 'z'])
    position_data.dropna(axis=0, inplace=True, how='any')
    position_data['timestamp'] = position_data['timestamp'].astype('int64')

    force_data = pd.read_csv(HIP_dir, names =['timestamp', 'y_f1', 'x_f1', 'z_f1'])
    force_data['timestamp'] = force_data['timestamp'].astype('int64')
    force_data2 = pd.read_csv(CIP_dir, names =['timestamp', 'y_f2', 'x_f2', 'z_f2'])
    force_data2['timestamp'] = force_data['timestamp'].astype('int64')

    force_data = force_data.merge(force_data2, on='timestamp', how='inner')
    force_data['x_f'] = force_data['x_f1'] + force_data['x_f2']
    force_data['y_f'] = force_data['y_f1'] + force_data['y_f2']

    total_data=position_data.merge(force_data , on='timestamp', how='inner')
    total_data['dx'] = total_data['x'].diff()
    total_data['dy'] = total_data['y'].diff()
    total_data.dropna(axis=0, inplace=True, how='any')
    work = 0
    for index, row in total_data.iterrows():
        a = np.array([row['x_f'], row['y_f']])
        b = np.array([row['dx'], row['dy']])
        inner = np.inner(a, b)
        norms = LA.norm(a) * LA.norm(b)

        if norms == 0.0:
            workdone = 0
        else:
            cos = inner / norms
            rad = np.arccos(np.clip(cos, -1.0, 1.0))
            workdone = norms * np.sin(rad)
        work+= workdone 
    return work
#####################################################

# eff = []
# energy = []
# energy_anova = []
# for control in range(1,4):
#     control_eff = []
#     control_energy = []
#     control_energy_anova = []
#     for scene in range(1,4):
#         force_dirs, HIP_position_dirs, ball_posision_dirs = get_dirs_CIP(scene,control)
#         eff_list_scene = []
#         energy_list_scene = []
#         for i in range(len(force_dirs)):
#             energy_consumed_human = workovertime_HIP(HIP_position_dirs[i],force_dirs[i])
#             work_done_by_human = workovertime(ball_posision_dirs[i], force_dirs[i])
#             print(work_done_by_human/energy_consumed_human)
#             energy_list_scene.append(energy_consumed_human)
#             eff_list_scene.append(work_done_by_human/energy_consumed_human)
#         control_eff.append(sum(eff_list_scene)/len(eff_list_scene))
#         control_energy.append(sum(energy_list_scene)/len(energy_list_scene))
#         control_energy_anova += energy_list_scene
#     eff.append(control_eff)
#     energy.append(control_energy)
#     energy_anova.append(control_energy_anova)

# data = pd.DataFrame({"numCollisions":energy_anova[0]+energy_anova[1]+energy_anova[2], "control_mode":["FC"]*len(energy_anova[0])+["FS"]*len(energy_anova[1])+ ["VC"]*len(energy_anova[2])})
# model  = ols('numCollisions ~ control_mode', data =data).fit()
# anova_result = sm.stats.anova_lm(model, typ=2)
# print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
# print(anova_result)
# print(f_oneway(energy_anova[0], energy_anova[1], energy_anova[2]))
# print(ttest_ind(energy_anova[0], energy_anova[1]))
# print(ttest_ind(energy_anova[1], energy_anova[0]))
# print(ttest_ind(energy_anova[0], energy_anova[2]))



# categories = ['Scene1', 'Scene2', 'Scene3']
# xpos = np.arange(len(categories))

# plt.bar(xpos-0.2, eff[0], width =0.2,label='FC')
# plt.bar(xpos, eff[1], width =0.2,label='SC')
# plt.bar(xpos+0.2, eff[2], width =0.2,label='VC')

# plt.ylabel("Efficiency of Human")
# plt.legend()

# plt.xticks(xpos, categories)
# plt.figure()
# plt.bar(xpos-0.2, energy[0], width =0.2,label='FC')
# plt.bar(xpos, energy[1], width =0.2,label='SC')
# plt.bar(xpos+0.2, energy[2], width =0.2,label='VC')

# plt.ylabel("Energy consumed by human")
# plt.legend()
# plt.xticks(xpos, categories)


# plt.show()
all_control_anova = []
all_control_bar = []
for control in range(2,4):
    control_bar = []
    control_anova = []
    for scene in range(1,4):
        HF_dirs, H_dirs, CF_dirs, C_dirs, ball_dirs = get_dirs_ALL(scene, control)
        scene_total = 0
        for i in range(len(HF_dirs)):
            t_work = total_work( ball_dirs[i],HF_dirs[i], CF_dirs[i])
            h_energy = workovertime_HIP(H_dirs[i],HF_dirs[i])
            c_energy = workovertime(C_dirs[i],CF_dirs[i])
            efficient = t_work/(h_energy + c_energy)
            scene_total += efficient
            control_anova.append(efficient)
        control_bar.append(scene_total/len(H_dirs))
    all_control_bar.append(control_bar)
    all_control_anova.append(control_anova)


categories = ['Scene1', 'Scene2', 'Scene3']
xpos = np.arange(len(categories))

plt.bar(xpos-0.2, all_control_bar[0], width =0.2,label='SC')
plt.bar(xpos, all_control_bar[1], width =0.2,label='VC')
plt.ylim([0,1])
plt.ylabel("Total Efficiency")
plt.legend()
plt.xticks(xpos, categories)
plt.show()

data = pd.DataFrame({"timetaken":all_control_anova[0]+all_control_anova[1], "control_mode":["SC"]*len(all_control_anova[0])+ ["VC"]*len(all_control_anova[1])})
print(data)
model  = ols('timetaken ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(all_control_anova)
print(ttest_ind(all_control_anova[0], all_control_anova[1]))
