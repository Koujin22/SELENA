"""Synthesizes speech from the input string of text or ssml.
Make sure to be working in a virtual environment.

Note: ssml must be well-formed according to:
    https://www.w3.org/TR/speech-synthesis/
"""
from google.cloud import texttospeech
from pydub import AudioSegment
from pydub.playback import play
import os
import io

import zmq
import time

context = zmq.Context()

os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = r".\Python\Resources\helical-sol-339819-39563c1d52d4.json"
def main():
    
    client, voice, audio_config = init()

    socket = connectToServer()
    # Set the text input to be synthesized
    
    # Perform the text-to-speech request on the text input with the selected
    # voice parameters and audio file type
    
    while True:
        text = recvMsg(socket)
        text = textToSsml(text)
        response = synthesize(client, voice, audio_config, text)
        
        playSpeech(response)

def textToSsml(text):
    print(text,flush=True);
    text = text.decode('latin_1')
    ssml = "<speak><break time=\'250ms\'/>"
    for i in range(len(text)):
        ssml = ssml + parseChar(text[i]);
    ssml = ssml + "<break time=\'250ms\'/></speak>"
    return ssml

def parseChar(char):
    match char:
        case '.':
            return "<break time=\'500ms\'/>"
        case ',':
            return "<break time=\'250ms\'/>"
        case _:
            return char

def init():
    # Instantiates a client
    client = texttospeech.TextToSpeechClient()

    # Build the voice request, select the language code ("en-US") and the ssml
    # voice gender ("neutral")
    voice = texttospeech.VoiceSelectionParams(
        language_code="es-US", name="es-US-Wavenet-A"
    )

    # Select the type of audio file you want returned
    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.MP3
    )
    return client, voice, audio_config

def synthesize(client, voice, audio_config, text):
    synthesis_input = texttospeech.SynthesisInput(ssml=text)    
    return client.synthesize_speech(
        input=synthesis_input, voice=voice, audio_config=audio_config
    )
    
def playSpeech(response):
    fp = io.BytesIO(response.audio_content)

    song = AudioSegment.from_file(fp, format="mp3")
    time.sleep(0.5)
    play(song)

def connectToServer():
    #  Socket to talk to server
    socket = context.socket(zmq.SUB)
    socket.connect("tcp://localhost:50002")
    socket.subscribe("")
    return socket

def recvMsg(socket):
    print(f"waiting for reply ")
    message = socket.recv()
    print(f"Received reply {message}")
    return message
if __name__ == "__main__":
    main();