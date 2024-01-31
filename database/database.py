import csv
from pymongo import MongoClient

def initDatabaseClient(app):
  client = MongoClient(app.config['MONGO_URI'])
  return client

def checkDatabase(client):
  dbname= 'WaterBnb'
  dbnames = client.list_database_names()
  if dbname in dbnames: 
      print(f"{dbname} is there!")
  else:
      print("YOU HAVE to CREATE the db !\n")

def checkThatCollectionExist(client, collectionName):
  db = client.WaterBnb
  collname = collectionName
  collnames = db.list_collection_names()
  if collname in collnames: 
      print(f"{collname} is there!")
  else:
      print(f"YOU HAVE to CREATE the {collname} collection !\n")

def setupUsersCollection(client):
  userscollection = client.WaterBnb.users
  userscollection.delete_many({})
  excel = csv.reader(open("../usersM1_2023.csv")) 
  for l in excel:
      ls = (l[0].split(';'))
      if userscollection.find_one({"name" : ls[0]}) ==  None:
          userscollection.insert_one({"name": ls[0], "num": ls[1]})
