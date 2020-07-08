from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import csv
import numpy as np
import pandas as pd 

life=pd.read_csv('life.csv',encoding = 'ISO-8859-1')
world=pd.read_csv('world.csv',encoding = 'ISO-8859-1')
dataframe = pd.merge(world, life, how='inner', on='Country Code')
output=[]


##get just the features
featureList=[]
for feature in world:
    if feature not in ["Country Name","Time","Country Code"]:
        featureList.append(feature)
data=dataframe[featureList]
    
data = data.replace('..', np.nan)
##get just the class labels
classlabel=dataframe['Life expectancy at birth (years)']

data = data.astype(float)

for feature in data:
    median = data[feature].median()
    data[feature] = data[feature].replace(np.nan, median)
    mean = data[feature].mean()
    stdev = np.sqrt(data[feature].var())
    data[feature] = (data[feature]-mean)/stdev
    output.append((feature,"{0:.3f}".format(median),\
        "{0:.3f}".format(mean),"{0:.3f}".format(stdev)))
##randomly select 66% of the instances to be training and the rest to be testing
X_train, X_test, y_train, y_test = train_test_split(\
    data,classlabel, train_size=0.66, test_size=0.34, random_state=100)


K5N = KNeighborsClassifier(n_neighbors=5)
K10N = KNeighborsClassifier(n_neighbors=10)
DecTree = DecisionTreeClassifier(max_depth=4, random_state=100)

K5N = K5N.fit(X_train, y_train)
K10N = K10N.fit(X_train, y_train)
DecTree = DecTree.fit(X_train, y_train)

K5N_pred=K5N.predict(X_test)
K10N_pred=K10N.predict(X_test)
DecTree_pred=DecTree.predict(X_test)

#
print("Accuracy of decision tree:", "{0:.3f}".format(accuracy_score(y_test, DecTree_pred)))
print("Accuracy of k-nn (k=5):", "{0:.3f}".format(accuracy_score(y_test, K5N_pred)))
print("Accuracy of k-nn (k=10):", "{0:.3f}".format(accuracy_score(y_test, K10N_pred)))

with open("task2a.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["feature", "median", "mean", "variance"])
    for i in output:
        wr.writerow(i)

