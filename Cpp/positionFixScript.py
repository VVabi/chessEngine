file = open("chesspositions.txt", "r")


output = []

for line in file:
    print len(line)
    if len(line) == 66:
        line = line[:-1]
        line = line+"0000\n"
    output.append(line)
    
        






file = open("chesspositionsfixed.txt", "w")

for x in output:
   file.write(x)


file.close()
