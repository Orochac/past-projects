import csv
import numpy as np
import pandas as pd 
import textdistance 
from fuzzywuzzy import fuzz
import nltk
from nltk.stem.porter import *
from nltk.corpus import stopwords
# if running the first time with errors:
#nltk.download('punkt')
#nltk.download('stopwords')

NAME_WEIGHT = 80
DESC_NAME_WEIGHT =165
PRICE_NAME_WEIGHT = 60

google=pd.read_csv('google_small.csv',encoding = 'ISO-8859-1').astype(str)
amazon=pd.read_csv('amazon_small.csv',encoding = 'ISO-8859-1').astype(str)

output = []
ng = len(google)
na = len(amazon)

#clean the data
google = google.replace(np.nan, '')
amazon = amazon.replace(np.nan, '')

#match the data
for i in range(0,ng-1):
    for j in range(0,na-1):
        g = google.iloc[i]
        a = amazon.iloc[j]
        n_sim = fuzz.token_set_ratio(g[1],a[1])
        if g[2] and a[2]:
            d_sim = fuzz.token_set_ratio(g[2], a[2])
        else:
            d_sim=0
        if g[2] :
            m_d_sim = fuzz.token_set_ratio(g[2], a[3])
        else:
            m_d_sim=0
        m_n_sim = fuzz.ratio(g[1], a[3])
        price_diff = abs(float(g[4])-float(a[4]))
        if n_sim > NAME_WEIGHT:
            output.append((a[0],g[0]))
        elif n_sim > PRICE_NAME_WEIGHT and not price_diff:
            output.append((a[0],g[0]))
        elif n_sim + d_sim > DESC_NAME_WEIGHT:
            output.append((a[0],g[0]))

            

with open("task1a.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["idAmazon","idGoogleBase"])
    for i in output:
        wr.writerow(i)