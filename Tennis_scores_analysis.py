#Some code was used from the jupyter notebook worksheets
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin
import re
import matplotlib.pyplot as plt
import csv
import json
import numpy as np
from numpy.lib.function_base import average
from adjustText import adjust_text

# Function to both find the score difference given an input string
# as well as identigy the vality of the score.
def calculate_score_diff(score):
    score = re.split(" |-|/",score)
    total = 0
    i=1
    while(i<len(score)):
        if score[i][0] != '(':
            player1 = int(score[i])
            player2 = int(score[i+1])

            # 6 and 7 are required in every score for no retirement
            if(player1 in [6,7] or player2 in [6,7]):
                total += player1-player2
            else:

                # Exception to the previous rule if 
                # there is a final set tie breaker
                if(player1>6 and player2>6 and abs(player1-player2)==2):
                    total += player1-player2
                else:
                    return "invalid"
        i+=2
    return abs(total)

# Initiating list and dictionaries for the program
output = []
game_difference = {}
namecount={}
names = []
visited = {}
to_visit = []
top5_name = []
top5_freq = []
win_percentage=[]

# Defining key variables
page_limit = 200
pages_visited = 1

# Create the list of valid tennis players from a json file
with open('tennis.json') as f:
    Data = json.load(f)
    for i in Data:
        names.append(i["name"])

# Parse and scrape the seed url, initiating the visited and to_visit lists
seed_url = 'http://comp20008-jh.eng.unimelb.edu.au:9889/main/'
page = requests.get(seed_url)
soup = BeautifulSoup(page.text, 'html.parser')
visited[seed_url] = True
links = soup.findAll('a')
seed_link = soup.findAll('a', href=re.compile("^index.html"))
to_visit_relative = [l for l in links if l not in seed_link]
for link in to_visit_relative:
    to_visit.append(urljoin(seed_url, link['href']))   

# Begin crawling throught the to_visit links adding all new links found 
while (to_visit):

    # Test for previously set page limit
    if pages_visited == page_limit :
        break      

    # Remove the first url from the list and scrape
    link = to_visit.pop(0)
    page = requests.get(link)
    soup = BeautifulSoup(page.text, 'html.parser')

    # Create list of words from the body of text
    text = soup.findAll('div')[1].text
    splitText = text.split()

    # mark the item as visited, i.e., add to visited list, remove from to_visit
    visited[link] = True
    to_visit
    new_links = soup.findAll('a')
    for new_link in new_links :
        new_item = new_link['href']
        new_url = urljoin(link, new_item)
        if new_url not in visited and new_url not in to_visit:
            to_visit.append(new_url)
    pages_visited = pages_visited + 1

    # Identify who the article is about by first player mentioned
    i=0
    while i < len(splitText)-2:
        name =  str(splitText[i]+ ' '+ splitText[i+1])
        if name.upper() in names:
            if name in namecount:
                namecount[name] +=1
            else:
                namecount[name] = 1
            break
        i += 1
    if name.upper() not in names:
        continue

    # Identify the score associated to the player with the first mentioned score
    score = "no score found"
    score = re.search("( [0-9]+-[0-9]+(( \([0-9]+(-|/)[0-9]+\))|)){2,5}", text)
    if(score):
        score = score.group()[0:]
        scorediff = calculate_score_diff(score)
        if scorediff == "invalid":
            continue
        if name in game_difference:
            game_difference[name].append(scorediff)
        else:
            game_difference[name]=[scorediff]
    else:
        namecount[name] -=1
        continue
    
    # Get the page headline and store all previous values in the output array
    title = soup.findAll('h1')
    output.append([link, title[0].text, name, score])

# Calculate and plot the frequency of player mentions in the articles
namecount = sorted(namecount.items(), key=lambda x: x[1], reverse=True)
for i in range(5):
    top5_name.append(namecount[i][0])
    top5_freq.append(namecount[i][1])
plt.bar(np.arange(0, 10, step = 2),top5_freq,width = 0.8)
plt.xticks(np.arange(0, 10, step = 2),top5_name, rotation=0)
plt.xlabel("Players")
plt.ylabel("Frequency")
plt.title("Frequency of articles writen on tennis players")
plt.savefig("task4.png")

# Calculate and plot the average game diff and win pct by player
avg_diff=[average(game_difference[player]) for player in game_difference]
player_names=[player for player in game_difference]
fig=plt.figure()
with open('tennis.json') as f:
    Data = json.load(f)
    for name in player_names:
        for entry in Data:
            if entry["name"]==name.upper():
                win_percentage.append(float(entry["wonPct"][:-1]))
                break
plt.scatter(avg_diff, win_percentage)
plt.xlabel('Average Game Difference')
plt.ylabel('Win Percenctage')
plt.title('Scatter of Average Game Gifference by Win Percentage')
texts = []
for i in range(0,len(avg_diff)):
    texts.append(plt.text(avg_diff[i], win_percentage[i], player_names[i]))
adjust_text(texts, arrowprops=dict(arrowstyle="->", color='r', lw=0.5))
plt.savefig("task5.png")

# Calculate average game difference for each player and save to csv file
with open("task3.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["player", "avg game difference "])
    for i in game_difference:
        wr.writerow([i, average(game_difference[i])])

# Save url, headline, player and score for each page to csv file
with open("task2.csv", 'w', newline='') as myfile:
    wr = csv.writer(myfile, quoting=csv.QUOTE_ALL)
    wr.writerow(["url", "headline", "player", "score"])
    for i in output:
        wr.writerow(i)



