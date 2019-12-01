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

    validPass, validFail = runTest(valid, "valid", "OK, CSV is valid", "FAIL, CSV is invalid")
    invalidPass, invalidFail = runTest(invalid, "invalid", "OK, CSV is invalid", "FAIL, CSV is valid")
    otherPass, otherFail = runTest(other, "other", "CSV is valid", "CSV is invalid")

    print("-----Results-----")
    printResults("valid", validPass, validFail, len(valid))
    printResults("invalid", invalidPass, invalidFail, len(invalid))
    printResults("other", otherPass, otherFail, len(other))

#    invalidPass, invalidFail, validPass, validFail, otherPass, otherFail = 0, 0, 0, 0, 0, 0
#
#    print("-----Testing valid CSVs-----")
#    for file in valid:
#        print("Running", file, end =" ")
#        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)
#
#        if process.returncode == 0:
#            print("PASSED, CSV is valid")
#            validPass += 1
#        else:
#            print("FAILED, CSV is invalid")
#            validFail += 1
#        
#        print(process.stdout)
#
#        print("\n")
#
#    print("-----Testing invalid CSVs-----")
#
#    for file in invalid:
#        print("Running", file, end =" ")
#        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)
#
#        if process.returncode != 0:
#            print("PASSED, CSV is not invalid")
#            invalidPass += 1
#        else:
#            print("FAILED, CSV is valid")
#            invalidFail += 1
#
#        print(process.stdout)
#
#        print("\n")
#
#    print("-----Testing other CSVs-----")
#
#    for file in other:
#        print("Running", file, end =" ")
#        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)
#        
#        if process.returncode == 0:
#            print("Valid CSV")
#            otherPass += 1
#        else: 
#            print("Invalid CSV")
#            otherFail += 1
#
#        print(process.stdout)
#        
#        print("\n")
#
    
def runTest(files, validity, passMessage, failMessage):
    print("-------------Testing", validity, "CSVs-------------")
    passCount, failCount = 0, 0
    valid = (validity == "valid" or validity == "other")
    for file in files:
        print("Running", file, "...", end =" ")
        process = sp.run(["main", "csvs/" + file], stdout=sp.PIPE)
        
        if process.returncode == 0 or (process.returncode != 0 and not valid):
            print(passMessage)
            passCount += 1
        else: 
            print(failMessage)
            failCount += 1

        print(process.stdout)
        print("\n")

    return passCount, failCount

def printResults(validity, passCount, failCount, total):
    print("For", total, validity, "files tested:")
    print(passCount, "out of", total, "passed")
    print(failCount, "out of", total, "failed")
    print("\n")

if __name__ == '__main__':
    main()

