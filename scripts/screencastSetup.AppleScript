#!/usr/bin/osascript

# this script set up apps needed to make a screencast on mac OSX 
# using 3 apps:
# Google Chrome, Arduino IDE, QuickTime Player
# It opens Chrome and Arduino IDE and position both windows inside rectangle 1300x720 px 
# 
# Such rectangle is selected, and then recorded using QuickTime Player 
# File -> New Screen Recording
# Once recorded I export it from QuickTime via
# Export -> Ipad, Iphone ... -> (select option 720p) 
#
# Video exported in such way works nice on youtube - NO BLACK STRIPES ON SIDES ;-) 

# stop Chrome
tell application "Google Chrome"
    quit
    delay 1
end tell

# start Chrome app
tell application "Google Chrome"
    launch
    # use open location if you want to open specyfic page
    #open location "http://jokeduino.com"
    activate
end tell

# set Chrome window position and dimentsions
tell application "System Events" to tell process "Google Chrome"
    set position of window 1 to {40,60}
    set size of window 1 to {800, 720}
end tell

# close Arduno IDE
tell application "Arduino"
    quit
    delay 1
end tell

# start Arduno IDE
tell application "Arduino"
    launch
    delay 1
end tell

# set Arduino IDE window position and dimentions
tell application "System Events" to tell process "Arduino"
    set position of window 1 to {840,60}
    set size of window 1 to {500, 720}
end tell

# start QuickTime
tell application "QuickTime Player"
    launch
    activate
end tell


