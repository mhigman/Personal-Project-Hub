import csv
import numpy as np
from datetime import date, datetime

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
    return '2'


def row_2(string: str):

    str_comp = string.split("-")
    str_comp[0] = months_to_num[str_comp[0]]
    componenets = list(map(int, str_comp))
    date_comp = date(componenets[-1], componenets[-3], componenets[-2])

    return (days_of_week[date_comp.weekday()], "/".join(map(str, [componenets[0], componenets[1], componenets[2]])))


def do_nothing(string: str):
    pass


def identity(arg):
    return arg


operations_to_row = [
    row_1,
    row_2,
    do_nothing,
    do_nothing,
    identity,
    identity
]

with open("site2.csv", "r+") as file:
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

    clean_data = [clean_data[0]] + sorted(clean_data[1:], key = lambda row: datetime.strptime(row[2], "%m/%d/%Y"))
    with open("cleansite2.csv", "w") as clean_file:
        writer = csv.writer(clean_file, lineterminator='\n')
        writer.writerows(clean_data)
