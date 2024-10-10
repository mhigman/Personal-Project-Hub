import csv
import numpy as np


months_to_num = {
    "January": 1,
    "February": 2,
    "March": 3,
    "April": 4,
    "May": 5,
    "June": 6,
    "July": 7,
    "August": 8,
    "September": 9,
    "October": 10,
    "November": 11,
    "December": 12,
}


def row_1(string: str):
    return string[len(string)-1]

def row_2(string: str):
    string = string.replace(",", " ")
    components = string.split()
    month = months_to_num[components[1]]

    return (components[0], "/".join(map(str, [month, components[2], components[3]])))


def do_nothing(string: str):
    pass
    
def identity(arg):
    return arg

def row_5(string: str):
    return string[1:]
operations_to_row = [
    row_1,
    row_2,
    do_nothing,
    do_nothing,
    identity,
    row_5
]

with open("site3.csv", "r+") as file:
    reader = list(csv.reader(file))
    data = reader[1:]
    headers = ["site", "weekday", "date", "week", "boxes"]
    data = list(map(list, zip(*data)))
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

    with open("cleansite3.csv", "w") as clean_file:
        writer = csv.writer(clean_file, lineterminator='\n')
        writer.writerows(clean_data)
  


