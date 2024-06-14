import pandas as pd
import numpy as np
from sklearn.preprocessing import MinMaxScaler

site_number = 3
percent_training = 80

# Load your data
data = pd.read_csv(f'cleansite{site_number}.csv')  # Replace with your data file

# Convert date column to datetime and sort the data
data['date'] = pd.to_datetime(data['date'])
data.sort_values('date', inplace=True)

print(data)
# Select the target variable (e.g., number of boxes sold)
target = data['boxes'].values.reshape(-1, 1)

print(f"target= {target}")
# Normalize your target variable (important for LSTM models)
scaler = MinMaxScaler(feature_range=(0, 1))
scaled_target = scaler.fit_transform(target)

print(F'Scaled Target = {scaled_target}')
def create_dataset(dataset, look_back=1):
    X, Y = [], []
    for i in range(len(dataset) - look_back - 1):
        a = dataset[i:(i + look_back), 0]
        X.append(a)
        Y.append(dataset[i + look_back, 0])
    return np.array(X), np.array(Y)

look_back = 7  # Number of previous time steps to consider for predicting the next time step

# Define the ratio for splitting
train_size = int(len(scaled_target) * (percent_training/100))
test_size = len(scaled_target) - train_size

# Split the data into training and testing sets
train, test = scaled_target[0:train_size,:], scaled_target[train_size:len(scaled_target),:]

# Prepare the dataset for LSTM
trainX, trainY = create_dataset(train, look_back)
testX, testY = create_dataset(test, look_back)

# Reshape input to be [samples, time steps, features] for both training and testing data
trainX = np.reshape(trainX, (trainX.shape[0], trainX.shape[1], 1))
testX = np.reshape(testX, (testX.shape[0], testX.shape[1], 1))

print(f"Transformed x = {trainX}")
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense
# Create and fit the LSTM network
model = Sequential()

model.add(LSTM(175, return_sequences=True, input_shape=(look_back, 1)))  # 50 LSTM units

model.add(LSTM(90, return_sequences=True))

model.add(LSTM(45, return_sequences=True))

model.add(LSTM(25))

model.add(Dense(1))
model.compile(loss='mean_squared_error', optimizer='adam')

model.fit(trainX, trainY, epochs=250, batch_size=1, verbose=2)

# Make predictions
testPredict = model.predict(testX)

# Inverse transform predictions
predicted = scaler.inverse_transform(testPredict)

import matplotlib.pyplot as plt

# Inverse transform original Y
original_Y = scaler.inverse_transform([testY])


def mean_absolute_percentage_error(y_true, y_pred):
    y_true, y_pred = np.array(y_true), np.array(y_pred)
    return np.mean(np.abs((y_true - y_pred) / y_true)) * 100

mape = mean_absolute_percentage_error(testY[0], testPredict[:,0])
print(f'Mean Absolute Percentage Error (MAPE): {mape:.2f}%')

# Plotting the results
plt.plot(original_Y[0], label='Actual')
plt.plot(predicted[:,0], label='Predicted')
plt.legend()
plt.show()
