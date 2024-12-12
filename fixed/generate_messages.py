import random
n = 9
for i in range(n):
  f = open("./messages/message"+str(i+1)+".txt", "w")
  for j in range(random.randint(1, int(1e5))):
    f.write(chr(random.randint(60, 127)) + " ")
  f.close()
