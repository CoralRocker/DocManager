class document:
    def __init__(self, path, references=None):
        if references != None :
            self.references = references

        self.filename = str()

        diridx = str(path).rfind('/')
        if diridx == -1:
            self.filename = str(path)
        else:
            print(path[diridx:])
