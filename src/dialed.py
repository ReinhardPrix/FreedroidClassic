#!/usr/bin/python

import sys

####################################################################
#        ALL THE BLOODY FUNCTIONS GO HERE.
####################################################################

#Recursive, works on simple dialogues for now.
def dumpall(current, home, count):
  print ""
  if count == 0:
    return
  count = count - 1
#Tux speaks first.
  #print count
  print '#' + getnodenumber()
  character=''
  while character != "\"" :
    character = file.read(current, 1)  
  character=''
  firstsay=''
  while character != "\"" :
    character = file.read(current, 1)
    if character == "\"" :
      break
    firstsay = firstsay + character
  print "Tux:  " + firstsay
  print "A: ",
                           #Moving forward for now...
  file.readline(current)   # To kill the \n in the what Tux said.
  file.readline(current)   # Reply sound.
  file.readline(current)   # The other dialogue editor's coordinates.
  while True :
    back = file.tell(current)
    if file.read(current, 8) != "Subtitle" :
      file.seek(current, back)
      print ""
      nextnode(current)
      dumpall(current, home, count)
      return
    else :
     file.seek(current, back)
#    mark = "# ",
    character=''
    while character != "\"" :
      character = file.read(current, 1)  
    character=''
    firstsay=''
    while character != "\"" :
      character = file.read(current, 1)
      if character == "\"" :
        file.readline(current)  # To the end of the line.
        file.readline(current)  # Need to skip sounds for now.
        break
      firstsay = firstsay + character
    if firstsay == "no" :
      print "\n"
#      count = count - 1
      dumpall(current, home, count)
      break
    elif firstsay == "yes" :
      print "\n"
#      count = count - 1
      dumpall(current, home, count)
      break
    else :
      print firstsay
  print "",

#Prints the node that I want printed.
def dumpknown(current, home, desired):
  print ""
  character=''
  while character != "\"" :
    character = file.read(current, 1)  
  character=''
  firstsay=''
  while character != "\"" :
    character = file.read(current, 1)
    if character == "\"" :
      break
    firstsay = firstsay + character
  print "Tux:  " + firstsay
  #Moving forward for now...
  file.readline(current)   # To kill the \n in the what Tux said.
  file.readline(current)   # Reply sound.
  file.readline(current)   # The other dialogue editor's coordinates.
  while True :
    character=''
    while character != "\"" :
      character = file.read(current, 1)  
    character=''
    firstsay=''
    while character != "\"" :
      character = file.read(current, 1)
      if character == "\"" :
        file.readline(current)
        file.readline(current)
        break
      firstsay = firstsay + character
    if firstsay == "no" :
      gotoknown(current, home, desired)
      break
    elif firstsay == "yes" :
      break
      gotoknown(current, home, desired)
    else :
      print "A: " + firstsay

#Chooses a node.
def gotonode(current, home):
  print "What node?",
  desired = raw_input()
  #Here we go.
  file.seek(current, home) #Going to the start of the whole mess.
  while getnodenumber() != desired :    #A "while" loop to handle the search.
    nextnode(current)
  return desired

#Goes to a known node.
def gotoknown(current, home, desired):
  file.seek(current, home) #Going to the start of the whole mess.
  while getnodenumber() != desired :    #A "while" loop to handle the search.
    nextnode(current)
  return desired

#I use ctrl+d anyway, but that one is for the other people.
def end():
  print "Quit."
  sys.exit()

#Brief look at the nodes.
def dumpnodes(current):
  print ""
  while file.read(current,1) != '' :
    file.tell(current)

#         FIXME.


    if file.read(current,1) == '' :
      break

    character=''
    while character != "=" :
      character = file.read(current, 1)
    character=''
    nodenum=''
    while character != "\n" :
      character = file.read(current, 1)
      nodenum = nodenum + character
    print nodenum,
    nextnode(current)

#Get the node number. Broken, I think.
def getnodenumber():
  back = file.tell(current)
  character=''
  while character != "=" :
    character = file.read(current, 1)
    if character == '':
      break
  character=''
  nodenum=''
  while character != " " :
    character = file.read(current, 1)
    if character == " ":
      break
    elif character == '':
      break
    nodenum = nodenum + character
  file.seek(current, back)
  return nodenum

#Goes to the next node.
def nextnode(current):
  line = ''
  while line != "----------------------------------------------------------------------\n" :
    line = file.readline(current)
    if line == '' :
      break
    elif line == "End of chat dialog for character=\"XXXXX\"\n" :
      break
  file.readline(current)

#Loads a file.
def load(current):
  if current != None:
    file.close(current)
  print "What character?",
  character = "../dialogs/" + raw_input() + ".dialog"
  current = file(character)
  print "Loaded " + character
  return current
  
#Headers are a pain, I do not need them, so I just scroll past them.
def clip(current):
  line = ""
  while line != "Beginning of new chat dialog for character=\"XXXXX\"\n" :
    line = file.readline(current)
  print "Discarded the headers."
  home = file.tell(current) + 1
  return home

#How many nodes do we have here?
def nodecount(current, home):
  nodecount = 0
  line2 = "a"
  while line2 != '' :
    line = line2 = file.readline(current)
    if line == "----------------------------------------------------------------------\n" :
      nodecount = nodecount + 1
  print nodecount,
  print "nodes found."
  file.seek(current, home)
  return nodecount

#Not used for now.
def switch(current):
  current = load(current)
  home = clip(current)
  nodecount(current, home)
  dumpnodes(current)
  return home


####################################################################
#    HERE STARTS THE PROGRAM.
####################################################################

#Preparing the needed varialbles.
current=None
desired=None
home=None
count=None

while True :
  
  if current != None :
    print ""
    a = str(current)
    print "f: " + a[12:-33],
  if desired != None :
    print "n:" + desired,
  print " >>",
  command = raw_input()
  if command == "q":
    end()
  elif command == "s":
    desired = None
    current = load(current)
    home = clip(current)
    count = nodecount(current, home)
    dumpnodes(current)
  elif command == "n":
    desired = gotonode(current, home)
    dumpknown(current, home, desired)
  elif command == "p":
    dumpknown(current, home, desired)
  elif command == "d":
    gotoknown(current, home, "0")
    dumpnodes(current)
    if desired != None:
      gotoknown(current, home, desired)
  elif command == "dd":
    gotoknown(current, home, "0")
    dumpall(current, home, count)
    if desired != None:
      gotoknown(current, home, desired)



####################################################################
#      THE ATTIC
####################################################################

#switch(current)
#switch(current)
#home = clip(current)
#nodecount(current, home)
#dumpnodes(current)
#gotonode(current, home)

#current = load(current)
#home = clip(current)
#nodecount(current, home)
#dumpnodes(current)

#Ok, time to get a node that I want.


#Get the first thing tux says in the node.
#character=''
#while character != "\"" :
#  character = file.read(current, 1)  
#character=''
#firstsay=''
#while character != "\"" :
#  character = file.read(current, 1)
#  if character == "\"" :
#    break
#  firstsay = firstsay + character
#print "Tux: " + firstsay

#Moving forward for now...
#file.readline(current)   # To kill the \n in the what Tux said.
#file.readline(current)   # Reply sound.
#file.readline(current)   # The other dialogue editor's coordinates.

#Get the first reply.
#character=''
#while character != "\"" :
#  character = file.read(current, 1)  
#character=''
#reply1=''
#while character != "\"" :
#  character = file.read(current, 1)
#  if character == "\"" :
#    break
#  reply1 = reply1 + character
#print reply1


  
#Print the first node.
#line = ""
#count = 0
#while line != "----------------------------------------------------------------------\n" :
#  line = file.readline(current)
#  count = count + 1
#a = file.tell(current) + 1
#file.seek(current, home)
#g = 0
#while g < count :
#  x = file.readline(current)
#  print x,
#  g = g + 1


#def dumpone(current, home):
#  #Get the first thing tux says in the node.
#  character=''
#  while character != "\"" :
#    character = file.read(current, 1)  
#  character=''
#  firstsay=''
#  while character != "\"" :
#    character = file.read(current, 1)
#    if character == "\"" :
#      break
#    firstsay = firstsay + character
#  print "Tux: " + firstsay
#  #Moving forward for now...
#  file.readline(current)   # To kill the \n in the what Tux said.
#  file.readline(current)   # Reply sound.
#  file.readline(current)   # The other dialogue editor's coordinates.
#  #Get the first reply.
#  character=''
#  while character != "\"" :
#    character = file.read(current, 1)  
#  character=''
#  reply1=''
#  while character != "\"" :
#    character = file.read(current, 1)
#    if character == "\"" :
#      break
#    reply1 = reply1 + character
#  print reply1
#  gotoknown(current, home, desired)
