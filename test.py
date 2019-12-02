import sys
import glob, os
import subprocess as sp

def main():
    invalid, valid, other = [], [], []
    for file in os.listdir("csvs"):
        if file.startswith("invalid") and file.endswith(".csv"):
            invalid.append(file)
        elif file.startswith("valid") and file.endswith(".csv"):
            valid.append(file)
        elif file.endswith(".csv"):
            other.append(file)

    sp.run(["make", "clean"])
    sp.run(["make"])

    print("-----Testing valid CSVs-----")

    for file in valid:
        print("Running", file, end = " ")
        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)

        if process.returncode == 0:
            print("PASSED, CSV is valid")
        else:
            print("FAILED, CSV is invalid")
        
        print(process.stdout)

    print("-----Testing invalid CSVs-----")

    for file in invalid:
        print("Running", file, end=" ")
        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)

        if process.returncode != 0:
            print("PASSED, CSV is not invalid")
        else:
            print("FAILED, CSV is valid")

        print(process.stdout)

    print("-----Testing other CSVs-----")

    for file in other:
        print("Running", file, end=" ")
        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)
        
        if process.returncode == 0:
            print("Valid CSV")
        else: 
            print("Invalid CSV")

        print(process.stdout)
        

if __name__ == '__main__':
    main()
