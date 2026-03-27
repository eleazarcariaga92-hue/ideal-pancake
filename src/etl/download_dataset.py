import requests
import os

URL = 'https://example.com/ecommerce_dataset.csv'
FILE_PATH = 'data/ecommerce_dataset.csv'

if not os.path.exists('data'):
    os.makedirs('data')

response = requests.get(URL)
with open(FILE_PATH, 'wb') as f:
    f.write(response.content)  
print('Dataset downloaded to', FILE_PATH)