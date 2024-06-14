import numpy as np
import matplotlib.pyplot as plt




# Function to compute Fourier series coefficients
def compute_fourier_coefficients(ydata):
    N = len(ydata)
    yf = np.fft.fft(ydata)
    amplitudes = 2.0 / N * np.abs(yf[:N//2])
    phases = np.angle(yf[:N//2])
    return amplitudes, phases

# Function to construct Fourier series from coefficients
def fourier_series(t, amplitudes, phases):
    dc_component = amplitudes[0] / 2.0
    series_terms = [amplitudes[n] * np.cos(2 * np.pi * n * t + phases[n]) for n in range(1, len(amplitudes))]
    return dc_component + sum(series_terms)

# Sample data
t = np.linspace(0, 2 * np.pi, 1000)
ydata = 1.0 * np.sin(2 * np.pi * 3 * t) + 0.5 * np.sin(2 * np.pi * 5 * t)

# Compute Fourier series coefficients
amplitudes, phases = compute_fourier_coefficients(ydata)

# Construct Fourier series
series = fourier_series(t, amplitudes, phases)

# Plot the results
plt.plot(t, ydata, label='Original Signal')
plt.plot(t, series, label='Fourier Series')
plt.legend()
plt.show()
