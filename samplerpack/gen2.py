import csv
import string
import random

def randomString(stringLength=10):
    """Generate a random string of fixed length """
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for i in range(stringLength))

with open("second.csv", "w") as file:
    writer = csv.writer(file)
    writer.writerow(["name", "a"])
    writer.writerow(["a", "a", "a", "a", "a", "a", "a", "a", "a"])
    writer.writerow(["a", "a", "a", "a", "a", "a", "a", "a", "a"])
    for i in range(25000):
        writer.writerow(["a", "a", "a", "a", "a", "a", "a", "a", randomString()])
