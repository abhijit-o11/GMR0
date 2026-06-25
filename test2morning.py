import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from micromlgen import port

# load dataset
df = pd.read_csv("EEG_Scaled_data.csv", nrows=1000)

# -------- FEATURE EXTRACTION --------
def extract_features(row):
    return [
        np.mean(row),
        np.max(row),
        np.min(row),
        np.max(row) - np.min(row)
    ]

X = df.iloc[:, :-1].apply(extract_features, axis=1, result_type='expand')
Y = df.iloc[:, -1]

# split
X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2)

# train model
model = DecisionTreeClassifier()
model.fit(X_train, Y_train)

# accuracy
print("Accuracy:", model.score(X_test, Y_test))

# convert to C
c_code = port(model)

with open("seizure_model.h", "w") as f:
    f.write(c_code)

print("Model converted!")