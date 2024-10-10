import csv
with open('cleanprojected.csv') as home\
    , open('cleanprojected1.csv', 'w') as one\
    , open('cleanprojected2.csv', 'w') as two\
    , open('cleanprojected3.csv', 'w') as three:
    reader = list(csv.reader(home))
    one_w = csv.writer(one, lineterminator='\n')
    two_w = csv.writer(two, lineterminator='\n')
    three_w = csv.writer(three, lineterminator='\n')
    one_w.writerow(reader[0])
    two_w.writerow(reader[0])
    three_w.writerow(reader[0])
    for row in reader[1:]:
        if row[0] == '1':
            one_w.writerow(row)
        elif row[0] == '2':
            two_w.writerow(row)
        else:
            three_w.writerow(row)