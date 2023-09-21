import matplotlib.pyplot as plt
import math
import os
import pandas as pd
import numpy as np
import fnmatch
from scipy.stats import f_oneway
from scipy.stats import ttest_ind
import statsmodels.api as sm
from statsmodels.formula.api import ols
import scipy.stats as stats

def get_dir(scene, control_mode):
    dir_list = []
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'*_'+str(scene) + '_' + str(control_mode) + '.csv') and os.path.basename(os.path.dirname(os.path.join(root, name))) == 'ball':
                dir_list.append(os.path.join(root, name))
    return dir_list




timetaken = []
for control_mode in range(1,4):
    control_mean = []
    for scene in range(1,4):
        dirs = get_dir(scene,control_mode)
        durations = []
        for dir in dirs:
            data = pd.read_csv(dir ,names =['timestamp', 'y', 'x', 'z'])
            data.dropna(axis = 0,how='any',inplace=True)
            duration = data['timestamp'].astype('int64').max() -data['timestamp'].astype('int64').min()
            durations.append(duration/1000)
        control_mean.append(sum(durations)/len(durations))
    timetaken.append(control_mean)


categories = ['Scene1', 'Scene2', 'Scene3']
xpos = np.arange(len(categories))

plt.bar(xpos-0.2, timetaken[0], width =0.2,label='FC')
plt.bar(xpos, timetaken[1], width =0.2,label='SC')
plt.bar(xpos+0.2, timetaken[2], width =0.2,label='VC')

plt.ylabel("Time Taken (Seconds)")
plt.legend()
plt.xticks(xpos, categories)
plt.show()
print(timetaken)

timetaken = []
for control_mode in range(1,4):
    control = []
    for scene in range(1,4):
        dirs = get_dir(scene,control_mode)
        durations = []
        for dir in dirs:
            data = pd.read_csv(dir ,names =['timestamp', 'y', 'x', 'z'])
            data.dropna(axis = 0,how='any',inplace=True)
            duration = data['timestamp'].astype('int64').max() -data['timestamp'].astype('int64').min()
            durations.append(duration/1000)
        control += durations
    timetaken.append(control)


data = pd.DataFrame({"timetaken":timetaken[0]+timetaken[1]+timetaken[2], "control_mode":["FC"]*len(timetaken[0])+["FS"]*len(timetaken[1])+ ["VC"]*len(timetaken[2])})
print(data)
model  = ols('timetaken ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(timetaken)
print(f_oneway(timetaken[0], timetaken[1], timetaken[2]))
print(ttest_ind(timetaken[0], timetaken[1]))
print(ttest_ind(timetaken[1], timetaken[0]))
print(ttest_ind(timetaken[0], timetaken[2]))