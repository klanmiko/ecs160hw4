import csv
import string
import random

def randomString(stringLength=10):
    """Generate a random string of fixed length """
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for i in range(stringLength))

with open("generated.csv", "w") as file:
    writer = csv.writer(file)
    writer.writerow(["name", "a"])
    for i in range(25008):
        writer.writerow(["a", "a", "a", "a", "a", "a", "a", "a", randomString()])
