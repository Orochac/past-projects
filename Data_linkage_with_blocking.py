import csv
import numpy as np
import pandas as pd 

def block_func(row):
    mnfct = row[3]
    if not mnfct:
        if row[1][0] in list('0123456789') and row[2]:
            mnfct =row[2].split()[0]
        else:
            mnfct =row[1].split()[0]      
    else:
        mnfct = mnfct.split()[0]
    key = ''.join([str(mnfct)[0:2]])
    return key

#clean the data

#assign data to blocks

google=pd.read_csv('google.csv',encoding = 'ISO-8859-1')
amazon=pd.read_csv('amazon.csv',encoding = 'ISO-8859-1')
test=pd.read_csv('amazon_google_truth.csv',encoding = 'ISO-8859-1')

google = google.replace(np.nan, '')
amazon = amazon.replace(np.nan, '')
google["name"] = google["name"].str.replace('\(', ' ').str.replace('-', '')
amazon["manufacturer"] = amazon["manufacturer"].str.replace('!', '')

#assign data to blocks
a_output=[]
g_output=[]
ng = len(google)
na = len(amazon)
for i in range(0,ng):
    g = google.iloc[i]
    block_key= block_func(g)
    g_output.append((block_key,g[0]))
for j in range(0,na):
    a = amazon.iloc[j]
    block_key= block_func(a)
    a_output.append((block_key,a[0]))



with open("amazon_blocks.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["block_key","product_id"])
    for i in a_output:
        wr.writerow(i)
with open("google_blocks.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["block_key","product_id"])
    for i in g_output:
        wr.writerow(i)

