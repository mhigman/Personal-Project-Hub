import csv


for i in range(3):
    with open(f'cleansite{i+1}.csv') as actual, \
        open(f'cleanprojected{i+1}.csv') as expected, \
        open(f'cleanerror{i+1}.csv', 'w') as destination:
        reader_actual = list(csv.reader(actual))
        reader_expected = list(csv.reader(expected))
        writer = csv.writer(destination, lineterminator='\n')
        
        writer.writerow(reader_actual[0])

        for row_actual, row_expected in zip(reader_actual[1:], reader_expected[1:]):
            writer.writerow([row_actual[0], \
                            row_actual[1], \
                            row_actual[2], \
                            row_actual[3], \
                            (float(row_actual[-1]) - float(row_expected[-1]))])
    