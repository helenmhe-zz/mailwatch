import requests
import json

api_key= "3b9de9783b5c46139d75a9de12678c3d"
news_titles=[]
news_source=""
news_result=""

def web_handler(request):
    news_result=""
    method= request['REQUEST_METHOD']
    if method=='GET':
        parameters=request['GET']
        if 'topic' in parameters and 'newssource' in parameters:
            news_source=parameters['newssource'].lower()
            to_send="https://newsapi.org/v1/articles?source={0}&sortBy=top&apiKey=".format(news_source)+api_key
            r=requests.get(to_send)
            data=r.json()
            for i in range(len(data['articles'])):
                news_titles.append(data['articles'][i]['title'])
            for item in news_titles:
                news_result+=item
                news_result+="|"
        return(news_result)

