import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

#Change this to get the cleaned site data.
site_number = 1


error_data = pd.read_csv(f'cleansite{site_number}.csv', lineterminator='\n')

xdata = np.array(range(len(error_data)))
ydata = np.array(error_data.get('boxes\r'))

plt.plot(xdata, ydata, 'r--', label='Original Data')

plt.title("Sample Data Graph")
plt.legend()
plt.show()