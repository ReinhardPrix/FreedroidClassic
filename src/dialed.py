#!/usr/bin/python

import sys

####################################################################
#        ALL THE BLOODY FUNCTIONS GO HERE.
####################################################################

#I use ctrl+d anyway, but that one is for the other people.
def end():
  sys.exit()

def error(current):
  if current != None :
    print "  ?",
  else :
    print "  ?"

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
  return current

#Headers are a pain, I do not need them, so I just scroll past them.
def clip(current):
  line = ""
  while line != "Beginning of new chat dialog for character=\"XXXXX\"\n" :
    line = file.readline(current)
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
  file.seek(current, home)
  return nodecount

def scanall(current, count):
  everything = doid(current, count)
#  print everything
  return everything

def doid(current, count):
  everything = []
  for number in range(count):   # Replace range with a list of node numbers. Write a bit to gather the node names.
    node = []
    crude = ['']
    while crude[0] != "startup":
      crude = whatisit(current)
      list.append(node, crude)
    list.append(everything, node)
    nextnode(current)
  return everything

#Behold my mighty engine!
def whatisit(current) :
    back = file.tell(current)
    unknown = file.read(current, 8)
    file.seek(current, back)
    if unknown == "Subtitle" :  #Reply. We want that.
      what = "reply"
      meat = str(file.readline(current))[10:-2]
    elif unknown == "ReplySam" : #Trivial.
      what = "reply.s"
      meat = str(file.readline(current))[13:-2]
    elif unknown == "Position" : #Coordinates to maintain backwards compatibility with the GTK editor.
      what = "coords"
      meat = ["No damn clue for now." , "No damn clue for now."]
      meat[0] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      meat[1] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[1] = meat[1] + character
      file.readline(current)

    elif unknown == "ChangeOp" :  #Another harder one. 
      what = "switch"
      meat = ["No damn clue for now." , "No damn clue for now."]
      meat[0] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      meat[1] = file.readline(current)[-2:-1]

    elif unknown == "AlwaysEx" : # The last line.
      what = "startup"
      meat = str(file.readline(current))[43:-2]

    elif unknown == "New Opti" : #Tux speaking.
      what = "tuxtalk"
      meat=['' , '']

      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      character=''
      while character != "\"" :
        character = file.read(current, 1)
      character = ''
      while character != "\"" :
        character = file.read(current, 1)
        if character == "\"" :
          file.read(current, 1)
          break
        meat[1] = meat[1] + character

    elif unknown == "OptionSa" : #Tux voice. Easy.
      what = "tuxtalk.s"
      meat =  str(file.readline(current))[14:-2]

    elif unknown == "OnCondit" : #Might be a linked node or a goto...
#      if file.read(current, 30) == "OnCondition=\"GoldIsLessThan:1\"" : #Linked node as I do them... Will fix later.
#        file.seek(current, back)
#        what = "linked"
#        meat = str(file.readline(current))[-3:-1]
#      else :
#        file.seek(current, back)
        what = "goto"               #Welcome to hell my friends. This will hurt a bit. This is a goto.
        meat = ['', '', '']
        character=''
        while character != "\"" :
          character = file.read(current, 1)
        character = ''
        while character != "\"" :
          character = file.read(current, 1)
          if character == "\"" :
            file.read(current, 1)
            break
          meat[0] = meat[0] + character
        character=''
        while character != "=" :
          character = file.read(current, 1)
        while character != " " :
          character = file.read(current, 1)
          if character == " " :
            file.read(current, 1)
            break
          meat[1] = meat[1] + character
        character=''
        while character != "=" :
          character = file.read(current, 1)
        while character != "\n" :
          character = file.read(current, 1)
          if character == "\n" :
            break
          meat[2] = meat[2] + character
        if meat[1] == meat[2]:
          what = "linked"

    elif unknown == "DoSometh" :
      what = "extra"
      meat =  str(file.readline(current))[18:-2]

    else :                    
      print "PAAAANIIIIIIIIC!!!"                    
      hcf()

    output = [what, meat]
#    print output
    return output

  #  else !!!


def dumpnodes(everything):
  node = []
  subnode = []
  print ""
  for node in everything:
    for subnode in node:
      if subnode[0] == "tuxtalk":
        if len(subnode[1][0]) == 1:
          print "  " + subnode[1][0] + "  Tux: " + subnode[1][1]
        else:
          print "  " + subnode[1][0] + " Tux: " + subnode[1][1]
#      elif subnode[0] == "reply":
#        print "A: " + subnode[1]

def dumpnode(node):
  count = 0
  print ""
  print " #" + node[0][1][0]
  for subnode in node:
    if subnode[0] == "tuxtalk":
      print " Tux: " + subnode[1][1]
    elif subnode[0] == "reply":
      if count == 0:
        print " A: " + subnode[1]
        count = count + 1
      else:
        print "    " + subnode[1]

def rebuild(everything):
  print """% -*- mode: flyspell; mode: fill -*-
----------------------------------------------------------------------
 *
 *   Copyright (c) 1994, 2003, 2004, 2005, 2006  Team Freedroid
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
----------------------------------------------------------------------

This file was generated using dialed.
If you have questions concerning FreedroidRPG, please send an email to:

freedroid-discussion@lists.sourceforge.net

BEGIN OF AUTORS NOTES """  
  print " "
  print """END OF AUTORS NOTES


Beginning of new chat dialog for character="XXXXX" """

  for nodo in range(count):
    print ""
    write(everything[nodo])
    print "\n----------------------------------------------------------------------"
  print "\n" + 'End of chat dialog for character="XXXXX"' + "\n"

def write(node):
  for something in node:
    if something[0] == "tuxtalk" :
      print "New Option Nr=" + something[1][0] + '  OptionText="' + something[1][1] + "\""
    elif something[0] == "tuxtalk.s" :
      print "OptionSample=\"" + something[1] + "\""
    elif something[0] == "reply" :
      print "Subtitle=\"" + something[1] + "\""
    elif something[0] == "reply.s" :
      print "ReplySample=\"" + something[1] + "\""
    elif something[0] == "coords" :
      print "PositionX=" + something[1][0] + "  PositionY=" + something[1][1] + "  "
    elif something[0] == "switch" :
      print "ChangeOption=" + something[1][0] + " ChangeToValue=" + something[1][1]
    elif something[0] == "startup" :
      print "AlwaysExecuteThisOptionPriorToDialogStart=\"" + something[1] + "\""
    elif something[0] == "goto" :
      print 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][2]
    elif something[0] == "linked" :
      print 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][1]
    elif something[0] == "extra" :
      print 'DoSomethingExtra="' + something[1] + '"'



####################################################################
#    HERE STARTS THE PROGRAM.
####################################################################
#Preparing the needed varialbles.
current = None
node = None
home = None
count = None
subnode = None
everything = None


# current = load(current)
# home = clip(current)
# count = nodecount(current, home)
# everything = scanall(current, count)
# rebuild(everything)


while True :
  if current != None :
    print ""
    a = str(current)
    print "f: " + a[12:-26],
  if node != None :
    print "n:" + str(noda),
  if subnode != None :
    print "s:" + str(subnode),
  print " >>",
  command = raw_input()
  if command == "q":
    end()
  elif command == "s":
    node = None
    subnode = None
    current = load(current)
    home = clip(current)
    count = nodecount(current, home)
    everything = scanall(current, count)
    dumpnodes(everything)
  elif command == "n":
    if everything == None:
      error(current)
    else:
      subnode = None
      print "What node?",
      noda = int(raw_input())
      node = everything[noda]
      dumpnode(node)
  elif command == "p":
    if node == None:
      error(current)
    else:
      dumpnode(node)
  elif command == "d":
    if everything == None:
      error(current)
    else:
      dumpnodes(everything)
  elif command == "dd":
    if everything == None:
      error(current)
    else:
      for x in everything:
        dumpnode(x)
#  elif command == "u":
#    subnode = int(raw_input("What subnode? "))
#    gosub(current, home, desired, subnode)
#  elif command == "e":
#    gosub(current, home, desired, subnode)
  else:
    error(current)
