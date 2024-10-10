import csv
from datetime import date

days_of_week = ["Monday",
                "Tuesday",
                "Wednesday",
                "Thursday",
                "Friday",
                "Saturday",
                "Sunday"]


def site_number(string: str):
    return string[-1]


def compute_weekday(string: str):

    comps = string.split("/")
    date_comp = date(int(comps[-1]), int(comps[-3]), int(comps[-2]))
    return (days_of_week[date_comp.weekday()], string)


with open("projected.csv", "r") as file:
    reader = list(csv.reader(file))
    data = reader[1:]
    headers = ["site", "weekday", "date", "week", "boxes"]
    clean_data = [headers]
    prev_date = data[0][1]
    prev_site = site_number(data[0][0])
    clean_row = []
    printed = 0
    weekday, nowdate = "", ""
    prev_sites = {}
    for row in data:
        if len(prev_sites) == 0:
            weekday, nowdate = compute_weekday(row[1])
        if prev_date == row[1]:
            if not(site_number(row[0]) in prev_sites.keys()):
                prev_sites[site_number(row[0])] = 0
            prev_sites[site_number(row[0])] += float(row[-1])
            continue

        prev_date = row[1]
        clean_row = []
        for site, total_daily in prev_sites.items():
            clean_row.append(site)
            clean_row.append(weekday)
            clean_row.append(nowdate)
            clean_row.append(row[-2])
            clean_row.append(total_daily)
            clean_data.append(clean_row)
            clean_row = []
        prev_sites= {}        
        weekday, nowdate = "", ""
      

    clean_row.append(weekday)
    clean_row.append(nowdate)
    clean_row.append(row[-2])
    clean_row.append(total_daily)
    clean_data.append(clean_row)
    with open("cleanprojected.csv", "w") as clean_file:
        writer = csv.writer(clean_file, lineterminator='\n')
        writer.writerows(clean_data)
