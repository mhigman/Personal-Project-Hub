import csv
import numpy as np
from datetime import date

months_to_num = {
    "Jan": 1,
    "Feb": 2,
    "Mar": 3,
    "Apr": 4,
    "May": 5,
    "Jun": 6,
    "Jul": 7,
    "Aug": 8,
    "Sep": 9,
    "Oct": 10,
    "Nov": 11,
    "Dec": 12,
}

days_of_week = ["Monday", 
                "Tuesday", 
                "Wednesday", 
                "Thursday", 
                "Friday",
                "Saturday",
                "Sunday"]

def row_1(string: str):
    return '1'

def row_2(string: str):
    
    comps = string.split("/")
    date_comp = date(int(comps[-1]), int(comps[-3]), int(comps[-2]))



    return (days_of_week[date_comp.weekday()], string)


def do_nothing(string: str):
    pass
    
def identity(arg):
    return arg

def row_4(string: str):
    return string.split("-")[0]

operations_to_row = [
    row_1,
    row_2,
    do_nothing,
    do_nothing,
    row_4,
    identity
]

with open("site1.csv", "r+") as file:
    reader = list(csv.reader(file))
    data = reader[1:]
    headers = ["site", "weekday", "date", "week", "boxes"]
    data = list(zip(*data))
    clean_data = []
    for index, row in enumerate(list(data)):
        if index in [2, 3]:
            continue
        clean_row = [operations_to_row[index](i) for i in row]
        if type(clean_row[0]) == tuple:
            list1, list2 = zip(*clean_row)
            clean_data.append(list(list1))
            clean_data.append(list(list2))
            continue
        clean_data.append(clean_row)
    
    for index in range(len(clean_data)):
        clean_data[index].insert(0, headers[index])
    clean_data = list(zip(*clean_data))

    with open("cleansite1.csv", "w") as clean_file:
        writer = csv.writer(clean_file,lineterminator='\n')
        writer.writerows(clean_data)
  
