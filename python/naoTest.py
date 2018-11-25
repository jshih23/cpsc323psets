#!/usr/bin/python
import naoqi
from naoqi import ALProxy
tts = ALProxy("ALTextToSpeech", "192.168.1.122", 9559)
tts.say("Nice weather we're having today.")

