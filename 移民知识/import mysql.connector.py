import mysql.connector
import json

conn = mysql.connector.connect(user='root', password='',
                              host='localhost',database='company')

if conn:
    print ("Connected Successfully")
else:
    print ("Connection Not Established")

class create_dict(dict): 
  
    # __init__ function 
    def __init__(self): 
        self = dict() 
          
    # Function to add key:value 
    def add(self, key, value): 
        self[key] = value

mydict = create_dict()
select_employee = """SELECT * FROM employee"""
cursor = conn.cursor()
cursor.execute(select_employee)
result = cursor.fetchall()

for row in result:
    mydict.add(row[0],({"name":row[1],"email":row[2],"phone":row[3]}))

stud_json = json.dumps(mydict, indent=2, sort_keys=True)

print(stud_json)