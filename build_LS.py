def xor_latin_square(N = 4):
  file = open('latin_square.in','w')
  file.write("D: " + str(2**N) + "\n")
  for i in range(2**N):
    line = ""
    for j in range(2**N):
      s = (i^j) + 1
      line += str(s) + " "
    file.write(line+"\n")
  file.close()

xor_latin_square()

def sum_latin_square(N = 2):
  file = open('latin_square.in','w')
  file.write("D: " + str(2*N+1) + "\n")
  for i in range(2*N+1):
    line = ""
    for j in range(2*N+1):
      s = (i+j)%(2*N+1) + 1
      line += str(s) + " "
    file.write(line + "\n")
  file.close()
