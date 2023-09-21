from re import I
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

def get_fullcontrol_dir():
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'Full-Control*'):
                return os.path.join(root, name)

def get_sharedcontrol_dir():
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'Shared-Control*'):
                return os.path.join(root, name)

def get_variablecontrol_dir():
    for root, dirs, files in os.walk(".", topdown=False):
        for name in files:
            if fnmatch.fnmatch(name,'Variable-Control*'):
                return os.path.join(root, name)

fc =[]
fc_dir = get_fullcontrol_dir()
fc_data =  pd.read_excel(fc_dir)
fc.append(fc_data['Difficulty (Do you think the task is hard?)'].mean())
fc.append(fc_data['Commitment (I wanted to success on the task.)'].mean())
fc.append(fc_data['Worriness (I felt worry for touching the obstacles.)'].mean())
fc.append(fc_data['Performance (How do you think your performance with robotic guidance?)'].mean())


#  #   Column                                                                  Non-Null Count  Dtype         
# ---  ------                                                                  --------------  -----         
#  0   Timestamp                                                               11 non-null     datetime64[ns]
#  1   Subject Number                                                          11 non-null     float64       
#  2   Difficulty (Do you think the task is hard?)                             12 non-null     float64       
#  3   Commitment (I wanted to success on the task.)                           12 non-null     float64       
#  4   Worriness (I felt worry for touching the obstacles.)                    12 non-null     float64       
#  5   Performance (How do you think your performance with robotic guidance?)  12 non-null     float64       
#  6   Additional Comments (What you think can be improved for the game?)      4 non-null      object        
# dtypes: datetime64[ns](1), float64(5), object(1)
# memory usage: 800.0+ bytes
# None
sc =[]
sc_dir = get_sharedcontrol_dir()
sc_data =  pd.read_excel(sc_dir)
sc.append(sc_data['Difficulty (Do you think the task is hard)'].mean())
sc.append(sc_data['Commitment (I wanted to success on the task)'].mean())
sc.append(sc_data['Worriness (I felt worry for touching the obstacles.)'].mean())
sc.append(sc_data['Performance (How do you think your performance with robotic guidance?)'].mean())

print(sc_data['Difficulty (Do you think the task is hard)'].mean())

#  #   Column                                                                                  Non-Null Count  Dtype         
# ---  ------                                                                                  --------------  -----         
#  0   Timestamp                                                                               11 non-null     datetime64[ns]
#  1   Subject Number                                                                          11 non-null     float64       
#  2   Preference (Which control level mechanism do you prefer?)                               11 non-null     object        
#  3   Difficulty (Do you think the task is hard)                                              12 non-null     float64       
#  4   Commitment (I wanted to success on the task)                                            12 non-null     float64       
#  5   Worriness (I felt worry for touching the obstacles.)                                    12 non-null     float64       
#  6   Performance (How do you think your performance with robotic guidance?)                  12 non-null     float64       
#  7   Collaboration (Is the robotic guidance fighting against your will?)                     12 non-null     float64       
#  8   Trust (Do you believe the guidance force?)                                              12 non-null     float64       
#  9   Ease of use with robotic guidance                                                       12 non-null     float64       
#  10  Human-likeness (Is the guidance force like another human?)                              12 non-null     float64       
#  11  Visual Cue (Is the control bar visualisation helps you understand your control level?)  12 non-null     float64       
#  12  Additional Comments (What you think can be improved for this robotic guidance?)         5 non-null      object    
vc =[]
vc_dir = get_variablecontrol_dir()
vc_data =  pd.read_excel(vc_dir)
vc.append(vc_data['Difficulty (Do you think the task is hard?)'].mean())
vc.append(vc_data['Commitment (I wanted to success on the task.)'].mean())
vc.append(vc_data['Worriness (I felt worry for touching the obstacles.)'].mean())
vc.append(vc_data['Performance (How do you think your performance with robotic guidance?)'].mean())



#  0   Timestamp                                                                               11 non-null     datetime64[ns]
#  1   Subject Number                                                                          11 non-null     float64       
#  2   Preference (Which control level adjustment mechanism do you prefer?)                    11 non-null     object        
#  3   Difficulty (Do you think the task is hard?)                                             12 non-null     float64       
#  4   Commitment (I wanted to success on the task.)                                           12 non-null     float64       
#  5   Worriness (I felt worry for touching the obstacles.)                                    12 non-null     float64       
#  6   Performance (How do you think your performance with robotic guidance?)                  12 non-null     float64       
#  7   Collaboration (Is the robotic guidance fighting against your will?)                     12 non-null     float64       
#  8   Trust (Do you believe the guidance force?)                                              12 non-null     float64       
#  9   Ease of use with robotic guidance                                                       12 non-null     float64       
#  10  Human-likeness (Is the guidance force like another human?)                              12 non-null     float64       
#  11  Visual Cue (Is the control bar visualisation helps you understand your control level?)  12 non-null     float64       
#  12  Additional Comments (What you think can be improved for this robotic guidance?)         5 non-null      object        
categories = ['Difficulty', 'Commitment', 'Worries', 'Performance']

xpos = np.arange(len(categories))

plt.bar(xpos-0.2, fc, width =0.2, label='fC')
plt.bar(xpos, sc, width =0.2,label='SC')
plt.bar(xpos+0.2, vc, width =0.2,label='VC')
plt.ylim([0,9])
plt.legend()

plt.xticks(xpos, categories)
##########################################################################
#Stat
groups = [[],[],[]]
groups[0] = fc_data['Difficulty (Do you think the task is hard?)'].to_list()
groups[1] = sc_data['Difficulty (Do you think the task is hard)'].to_list()
groups[2] = vc_data['Difficulty (Do you think the task is hard?)'].to_list()
print(groups)
data = pd.DataFrame({"numCollisions":groups[0]+groups[1]+groups[2], "control_mode":["FC"]*len(groups[0])+["FS"]*len(groups[1])+ ["VC"]*len(groups[2])})
model  = ols('numCollisions ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(f_oneway(groups[0], groups[1], groups[2]))
print(ttest_ind(groups[0], groups[1]))
print(ttest_ind(groups[1], groups[0]))
print(ttest_ind(groups[0], groups[2]))


groups[0] = fc_data['Commitment (I wanted to success on the task.)'].to_list()
groups[1] = sc_data['Commitment (I wanted to success on the task)'].to_list()
groups[2] = vc_data['Commitment (I wanted to success on the task.)'].to_list()

data = pd.DataFrame({"numCollisions":groups[0]+groups[1]+groups[2], "control_mode":["FC"]*len(groups[0])+["FS"]*len(groups[1])+ ["VC"]*len(groups[2])})
model  = ols('numCollisions ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(f_oneway(groups[0], groups[1], groups[2]))
print(ttest_ind(groups[0], groups[1]))
print(ttest_ind(groups[1], groups[0]))
print(ttest_ind(groups[0], groups[2]))


groups[0] = fc_data['Worriness (I felt worry for touching the obstacles.)'].to_list()
groups[1] = sc_data['Worriness (I felt worry for touching the obstacles.)'].to_list()
groups[2] = vc_data['Worriness (I felt worry for touching the obstacles.)'].to_list()

data = pd.DataFrame({"numCollisions":groups[0]+groups[1]+groups[2], "control_mode":["FC"]*len(groups[0])+["FS"]*len(groups[1])+ ["VC"]*len(groups[2])})
model  = ols('numCollisions ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(f_oneway(groups[0], groups[1], groups[2]))
print(ttest_ind(groups[0], groups[1]))
print(ttest_ind(groups[1], groups[0]))
print(ttest_ind(groups[0], groups[2]))


groups[0] = fc_data['Performance (How do you think your performance with robotic guidance?)'].to_list()
groups[1] = sc_data['Performance (How do you think your performance with robotic guidance?)'].to_list()
groups[2] = vc_data['Performance (How do you think your performance with robotic guidance?)'].to_list()

data = pd.DataFrame({"numCollisions":groups[0]+groups[1]+groups[2], "control_mode":["FC"]*len(groups[0])+["FS"]*len(groups[1])+ ["VC"]*len(groups[2])})
model  = ols('numCollisions ~ control_mode', data =data).fit()
anova_result = sm.stats.anova_lm(model, typ=2)
print(anova_result['sum_sq'][0]/anova_result['sum_sq'][1])
print(anova_result)
print(f_oneway(groups[0], groups[1], groups[2]))
print(ttest_ind(groups[0], groups[1]))
print(ttest_ind(groups[1], groups[0]))
print(ttest_ind(groups[0], groups[2]))



#########################
#GRAPH
plt.figure()

sc = []
sc_dir = get_sharedcontrol_dir()
sc_data =  pd.read_excel(sc_dir)
sc.append(sc_data['Collaboration (Is the robotic guidance fighting against your will?)'].mean())
sc.append(sc_data['Trust (Do you believe the guidance force?)'].mean())
sc.append(sc_data['Ease of use with robotic guidance'].mean())
sc.append(sc_data['Visual Cue (Is the control bar visualisation helps you understand your control level?)'].mean())


vc = []
vc_dir = get_variablecontrol_dir()
vc_data =  pd.read_excel(vc_dir)
vc.append(vc_data['Collaboration (Is the robotic guidance fighting against your will?)'].mean())
vc.append(vc_data['Trust (Do you believe the guidance force?)'].mean())
vc.append(vc_data['Ease of use with robotic guidance'].mean())
vc.append(vc_data['Visual Cue (Is the control bar visualisation helps you understand your control level?)'].mean())


categories = ['Collaboration', 'Trust', 'Ease of use', 'Visual Cue']

plt.bar(xpos-0.1, sc, width =0.2,label='SC')
plt.bar(xpos+0.1, vc, width =0.2,label='VC')
plt.ylim([0,9])
plt.legend()
plt.xticks(xpos, categories)
plt.show()

print("The pairwise")
groups[0] = sc_data['Collaboration (Is the robotic guidance fighting against your will?)'].to_list()
groups[1] = vc_data['Collaboration (Is the robotic guidance fighting against your will?)'].to_list()
print(ttest_ind(groups[0], groups[1]))

groups[0] = sc_data['Trust (Do you believe the guidance force?)'].to_list()
groups[1] = vc_data['Trust (Do you believe the guidance force?)'].to_list()
print(ttest_ind(groups[0], groups[1]))

groups[0] = sc_data['Ease of use with robotic guidance'].to_list()
groups[1] = vc_data['Ease of use with robotic guidance'].to_list()
print(ttest_ind(groups[0], groups[1]))

groups[0] = sc_data['Visual Cue (Is the control bar visualisation helps you understand your control level?)'].to_list()
groups[1] = vc_data['Visual Cue (Is the control bar visualisation helps you understand your control level?)'].to_list()
print(ttest_ind(groups[0], groups[1]))
