#
# Copyright 2018-2021 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import argparse
from operator import mod
import os
import struct
import wave
from datetime import datetime
from threading import Thread

import pvporcupine
from pvrecorder import PvRecorder
import time

import zmq

context = zmq.Context()




class PorcupineDemo(Thread):
    """
    Microphone Demo for Porcupine wake word engine. It creates an input audio stream from a microphone, monitors it, and
    upon detecting the specified wake word(s) prints the detection time and wake word on console. It optionally saves
    the recorded audio into a file for further debugging.
    """

    def __init__(
            self,
            access_key,
            library_path,
            model_path,
            keyword_paths,
            sensitivities,
            input_device_index=None,
            output_path=None):

        """
        Constructor.
        :param library_path: Absolute path to Porcupine's dynamic library.
        :param model_path: Absolute path to the file containing model parameters.
        :param keyword_paths: Absolute paths to keyword model files.
        :param sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A
        higher sensitivity results in fewer misses at the cost of increasing the false alarm rate. If not set 0.5 will
        be used.
        :param input_device_index: Optional argument. If provided, audio is recorded from this input device. Otherwise,
        the default audio input device is used.
        :param output_path: If provided recorded audio will be stored in this location at the end of the run.
        """

        super(PorcupineDemo, self).__init__()

        self._access_key = access_key
        self._library_path = library_path
        self._model_path = model_path
        self._keyword_paths = keyword_paths
        self._sensitivities = sensitivities
        self._input_device_index = input_device_index

        self._output_path = output_path

    
    def connectToServer(self):
        #  Socket to talk to server
        print("Connecting to hello world server...")
        socket = context.socket(zmq.DEALER)
        socket.setsockopt_string(zmq.IDENTITY, "helper")
        socket.connect("tcp://localhost:50000")
        return socket

    def sendMsg(self, socket, command="wake"):
        print(f"Sending request: {command} ...")
        socket.send_string(command)
        #  Get the reply.

    def recvMsgNonBlocking(self, socket):
        try:
            message =socket.recv(flags=zmq.NOBLOCK)
        except zmq.Again as e:
            return False, ''
        message = message.decode()
        print(f"Received reply {message}")
        return True, message
    
    def recvMsgBlocking(self, socket):
        print(f"waiting for reply ")
        message =socket.recv()
        message = message.decode()
        print(f"Received reply {message}")
        return message

    def run(self):
        """
         Creates an input audio stream, instantiates an instance of Porcupine object, and monitors the audio stream for
         occurrences of the wake word(s). It prints the time of detection for each occurrence and the wake word.
         """

        keywords = list()
        for x in self._keyword_paths:
            keyword_phrase_part = os.path.basename(x).replace('.ppn', '').split('_')
            if len(keyword_phrase_part) > 6:
                keywords.append(' '.join(keyword_phrase_part[0:-6]))
            else:
                keywords.append(keyword_phrase_part[0])

        porcupine = None
        recorder = None
        wav_file = None
        try:

            socket = self.connectToServer()
                
            porcupine = pvporcupine.create(
                access_key=self._access_key,
                library_path=self._library_path,
                model_path=self._model_path,
                keyword_paths=self._keyword_paths,
                sensitivities=self._sensitivities)

            recorder = PvRecorder(device_index=self._input_device_index, frame_length=porcupine.frame_length)
            

            if self._output_path is not None:
                wav_file = wave.open(self._output_path, "w")
                wav_file.setparams((1, 2, 16000, 512, "NONE", "NONE"))

            print(f'Using device: {recorder.selected_device}')

            print('Listening {')
            for keyword, sensitivity in zip(keywords, self._sensitivities):
                print('  %s (%.2f)' % (keyword, sensitivity))
            print('}')
            infiniteLoop = True
            while infiniteLoop:

                loop = True
                                       
                recorder.start()
                while loop:
                    newMsg, msg = self.recvMsgNonBlocking(socket)

                    if(newMsg):
                        if(msg == "stop"):
                            infiniteLoop = False
                            loop = False
                            recorder.stop()
                            break
                        elif(msg == "pause"):
                            
                            recorder.stop()
                            while True:
                                msg = self.recvMsgBlocking(socket);
                                if(msg == "resume"):
                                    recorder.start()
                                    break;
                    pcm = recorder.read()

                    if wav_file is not None:
                        wav_file.writeframes(struct.pack("h" * len(pcm), *pcm))

                    result = porcupine.process(pcm)
                    if result >= 0:    
                        print('[%s] Detected %s' % (str(datetime.now()), keywords[result]))
                        self.sendMsg(socket)
                    
        except pvporcupine.PorcupineInvalidArgumentError as e:
            print("One or more arguments provided to Porcupine is invalid: {\n" +
                  f"\t{self._access_key=}\n" +
                  f"\t{self._library_path=}\n" +
                  f"\t{self._model_path=}\n" +
                  f"\t{self._keyword_paths=}\n" +
                  f"\t{self._sensitivities=}\n" +
                  "}")
            print(f"If all other arguments seem valid, ensure that '{self._access_key}' is a valid AccessKey")
            raise e
        except pvporcupine.PorcupineActivationError as e:
            print("AccessKey activation error")
            raise e
        except pvporcupine.PorcupineActivationLimitError as e:
            print(f"AccessKey '{self._access_key}' has reached it's temporary device limit")
            raise e
        except pvporcupine.PorcupineActivationRefusedError as e:
            print(f"AccessKey '{self._access_key}' refused")
            raise e
        except pvporcupine.PorcupineActivationThrottledError as e:
            print(f"AccessKey '{self._access_key}' has been throttled")
            raise e
        except pvporcupine.PorcupineError as e:
            print(f"Failed to initialize Porcupine")
            raise e
        except KeyboardInterrupt:
            print('Stopping ...')
        finally:
            if porcupine is not None:
                porcupine.delete()

            if recorder is not None:
                recorder.delete()

            if wav_file is not None:
                wav_file.close()

    @classmethod
    def show_audio_devices(cls):
        devices = PvRecorder.get_audio_devices()

        for i in range(len(devices)):
            print(f'index: {i}, device name: {devices[i]}')

class Config:
    def __init__(self, access_key, library_path, model_path, keyword_paths, sensitivities, output_path, audio_device_index):
        self.access_key = access_key
        self.library_path = library_path
        self.model_path = model_path
        self.keyword_paths = keyword_paths
        self.sensitivities = sensitivities
        self.output_path = output_path
        self.audio_device_index = audio_device_index

def main():
   
    config = Config(
        "Xp0vYgF1t2p3Q8p+2JrkVKg/eJ/5tT8PNrra2y20NLXHeNpqRzWNEg==",
        None,
        r".\Python\Resources\porcupine_params_es.pv",
        [
            r".\Python\Resources\selena_es_windows_v2_1_0.ppn", 
            r".\Python\Resources\oye-sel_es_windows_v2_1_0.ppn",
            r".\Python\Resources\hey-sel_es_windows_v2_1_0.ppn"
            ],
        [0.5, 0.5, 0.5],
        None,
        0
    )

    PorcupineDemo(
        access_key=config.access_key,
        library_path=config.library_path,
        model_path=config.model_path,
        keyword_paths=config.keyword_paths,
        sensitivities=config.sensitivities,
        output_path=config.output_path,
        input_device_index=config.audio_device_index).run()


if __name__ == '__main__':
    main()