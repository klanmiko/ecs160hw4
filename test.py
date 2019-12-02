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

    validPass, validFail = runTest(valid, "valid", "SUCCESS, CSV is valid", "FAILED, CSV is invalid")
    invalidPass, invalidFail = runTest(invalid, "invalid", "SUCCESS, CSV is invalid", "FAILED, CSV is valid")
    otherPass, otherFail = runTest(other, "other", "CSV is valid", "CSV is invalid")

    print("-----Results-----")
    printResults("valid", validPass, validFail, len(valid))
    printResults("invalid", invalidPass, invalidFail, len(invalid))
    printResults("other", otherPass, otherFail, len(other))

   
def runTest(files, validity, passMessage, failMessage):
    print("-------------Testing", validity, "CSVs-------------")
    passCount, failCount = 0, 0
    valid = (validity == "valid" or validity == "other")
    for file in files:
        print("Running", file, "...", end =" ")
        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)

        if (process.returncode == 0 and valid) or (process.returncode != 0 and not valid):
            print(passMessage)
            passCount += 1
        else: 
            print(failMessage)
            failCount += 1

        print(process.stdout)

        

if __name__ == '__main__':
    main()
