import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.fft import rfft
import sympy




error_data = pd.read_csv('cleansite3.csv', lineterminator='\n')

print(error_data['boxes\r'])

xdata = np.array(range(len(error_data)))[:450]
ydata = np.array(error_data.get('boxes\r'))[:450]
ydata_actual = np.array(error_data.get('boxes\r'))[450:]
mean = np.mean(ydata)
yf = rfft(ydata)
PERIOD = len(xdata)
def fourier_series(x, f):
    # Make the parameter objects for all the terms
    print(len(f))
    amplitudes = np.abs(f)
    phases = np.angle(f)
    dc_component = amplitudes[0] / (2*len(f))
    expr = (dc_component + sum([\
        (amplitudes[n] * 0.7) / len(f) * \
        sympy.cos(2 * sympy.pi * n/(PERIOD) * (x) + phases[n]) \
        for n in range(1, len(amplitudes))if n < 100]) * (1/(x/3000 + 1)) + (1-1/(x/3000 + 1)) * (mean-dc_component))  + 2*x
    return expr
    # return 2 * x + dc_component
# Adjust the time vector to match the length of the signal
t = np.linspace(0, 1000, len(error_data))
x = sympy.Symbol('x')
# Evaluate the Fourier series function
curve = fourier_series(x, yf)


series = sympy.lambdify(x, curve, 'numpy')

errors = []
for index, value in enumerate(ydata_actual, start=200):
    percent_error = (np.abs((-series(index)) + value)/value) * 100
    errors.append(percent_error)
print(errors)
print(np.mean(errors))


# # Plot the results
plt.plot(t, series(t), "b", label='Fourier Series')
plt.plot(np.array(range(len(error_data)))[:450], np.array(error_data.get('boxes\r')[:450]), 'r--', label='Training Data')
plt.plot(np.array(range(len(error_data)))[450:], np.array(error_data.get('boxes\r')[450:]), 'g--', label='Testing Data')

plt.title("Fourier Series and Original Signal")
plt.legend()
plt.show()
