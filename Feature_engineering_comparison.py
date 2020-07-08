from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from sklearn.decomposition import PCA
from sklearn.preprocessing import PolynomialFeatures
from sklearn.feature_selection import mutual_info_classif
from sklearn.feature_selection import SelectKBest
from sklearn.feature_selection import chi2
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import seaborn as sns
import csv
import numpy as np
import pandas as pd 

life=pd.read_csv('life.csv',encoding = 'ISO-8859-1')
world=pd.read_csv('world.csv',encoding = 'ISO-8859-1')
dataframe = pd.merge(world, life, how='inner', on='Country Code')
output=[]
country_count = len(dataframe['Country Code'])
# Create normalised feature dataframe
featureList=[]
for feature in world:
    if feature not in ["Country Name","Time","Country Code"]:
        featureList.append(feature)
data=dataframe[featureList]
data = data.replace('..', np.nan)
classlabel=dataframe['Life expectancy at birth (years)']
data = data.astype(float)

for feature in data:
    median = data[feature].median()
    data[feature] = data[feature].replace(np.nan, median)
#visual analysis
#sns_plot = sns.pairplot(data)
#sns_plot.savefig("output.png")

#Implement feature engineering using interaction term pairs and clustering labels.
poly = PolynomialFeatures(interaction_only=True) 
poly_data=poly.fit_transform(data)
mutual_info = mutual_info_classif(poly_data,classlabel, random_state=30)

KVALUESTESTED = 50
kmeans_mi = {}
for K in range(2, KVALUESTESTED):
    kmeans = KMeans(n_clusters=K, random_state=30).fit(data)
    clustering_values = kmeans.labels_.reshape(-1,1)
    kmeans_mi[K]=mutual_info_classif(clustering_values,classlabel, random_state=30)
x = range(2, KVALUESTESTED)
y = [kmeans_mi[k] for k in x]
plt.xticks(range(2,KVALUESTESTED+1, 2))
plt.scatter(x,y)
plt.title("Comparing kmeans information gain")
plt.xlabel('Kmeans K value')
plt.savefig('task2bOptimalKmeansPlot.png')
plt.close()
maximum = max(y)
for K in kmeans_mi:
    if(kmeans_mi[K] == maximum):
        optimal_K = K
        break

mutual_info[0] = maximum
poly_data =poly_data.T
poly_data[0] = clustering_values.T

plt.figure()
x = range(len(mutual_info))
y = [mutual_info[k] for k in x]
plt.title("Comparing feature information gain")
plt.xlabel('Feature number')
plot = plt.scatter(x,y)
plt.savefig('task2bInfoGainPlot.png')
plt.close()

top_4_indx = np.argsort(mutual_info)[-4:]
top_4_features = np.array([poly_data[i] for i in top_4_indx])
for feature in range(4):
    mean = top_4_features.T[feature].mean()
    stdev = np.sqrt(top_4_features.T[feature].var())
    top_4_features.T[feature] = (top_4_features.T[feature]-mean)/stdev

feature_pairs = poly.get_feature_names(featureList)
feature_pairs[0]="Kmeans clustering with a K = " + str(K)

print("\nList of interaction terms features, original features an\
d clustering feature with corresponding information gain:")
for i in range(0,len(feature_pairs)-1):
    print(feature_pairs[i], ":", mutual_info[i])

print("\nThe 4 selected features with the greatest mutual \
information with life expectancy:")
for i in top_4_indx:
    print(feature_pairs[i], ":", mutual_info[i])
# for feature in poly_data:
#     print(feature)
#print()

#normalise data for pca and sample
for feature in data:
    mean = data[feature].mean()
    stdev = np.sqrt(data[feature].var())
    data[feature] = (data[feature]-mean)/stdev

#Implement feature engineering and selection via PCA
pca = PCA(n_components=4)
pca_data = pca.fit_transform(data)
print("\nThe 4 selected principle component features:")
for i in range(4):
    print("Principle component {} explained variance ratio:".format(i+1),\
        pca.explained_variance_ratio_[i], "and singular value:",pca.singular_values_[i])
print("The total variance explained:", pca.singular_values_.sum(), "%")
        
#Take the first four features as a sample
sampleFeatures = []
counter=0
print("\nThe first 4 features:")
for f in data:
    print(f)
    sampleFeatures.append(f)
    counter+=1
    if counter>=4:
        break
sampleData = data[sampleFeatures]

print()

##train and test 
# Feature engineering
engX_train, engX_test, engY_train, engY_test = train_test_split(\
    top_4_features.T,classlabel, train_size=0.66, test_size=0.34, random_state=50)
K5N = KNeighborsClassifier(n_neighbors=5)
K5N.fit(engX_train, engY_train)
eng_pred=K5N.predict(engX_test)
 
# PCA
pcaX_train, pcaX_test, pcaY_train, pcaY_test = train_test_split(\
    pca_data,classlabel, train_size=0.66, test_size=0.34, random_state=100)
K5N = KNeighborsClassifier(n_neighbors=5)
K5N.fit(pcaX_train, pcaY_train)
pca_pred=K5N.predict(pcaX_test)

# First four samples
sampleX_train, sampleX_test, sampleY_train, sampleY_test = train_test_split(\
    sampleData,classlabel, train_size=0.66, test_size=0.34, random_state=100)
K5N = KNeighborsClassifier(n_neighbors=5)
K5N.fit(sampleX_train, sampleY_train)
sample_pred=K5N.predict(sampleX_test)

#
print("Accuracy of feature engineering:",\
     "{0:.3f}".format(accuracy_score(engY_test, eng_pred)))
print("Accuracy of PCA:", "{0:.3f}".format(accuracy_score(pcaY_test, pca_pred)))
print("Accuracy of first four features:",\
     "{0:.3f}".format(accuracy_score(sampleY_test, sample_pred)))
