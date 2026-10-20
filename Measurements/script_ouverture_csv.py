P1FRF_data = []
with open('Measurements/P1FRF.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=';')
    line_count = 0
    for row in csv_reader:
        P1FRF_data.append([float(row[0]), float(row[1])])
    print("out")
    
print(P1FRF_data)