import sys

if __name__ == "__main__":
    path = sys.argv[1]
    text = None
    with open(path, "r") as f:
        text = f.read()
    
    text = text.replace("{Current_Dir}", "templatedir")

    with open(path, "w") as f:
        f.write(text)