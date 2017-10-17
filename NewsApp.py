import requests
import json

api_key= "3b9de9783b5c46139d75a9de12678c3d"
news_titles=[]
news_descriptions=[]
topics = ["tech","sports","business","entertainment", "breaking news", "music"]
sources = ["ars-technica","espn","bloomberg","entertainment-weekly","mtv-news"]

def web_handler(request):
    method= request['REQUEST_METHOD']
    if method=='GET':
        parameters=request['GET']
        if 'topic' in parameters
            if parameters['topic'] != "breaking news":
                to_send="https://newsapi.org/v1/articles?source={}&sortBy=top&apiKey=".format(sources[topics.index(parameters['topic'])])+api_key
            else if parameters['topic'] == "breaking news":
                to_send="https://newsapi.org/v1/articles?source=associated-press&sortBy=latest&apiKey=3b9de9783b5c46139d75a9de12678c3d"+api_key
            r=requests.get(to_send)
            data=r.json()
            for i in range(5):
                news_titles.append(data['articles'][i]['title'])
                news_descriptions.append(data['articles'][i]['description'])
            return ({'titles':news_titles, 'descriptions':news_descriptions})
  
