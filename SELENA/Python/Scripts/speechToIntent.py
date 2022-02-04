import argparse
import struct
import wave
from threading import Thread

import pvrhino
from pvrecorder import PvRecorder

import zmq

context = zmq.Context()

class RhinoDemo(Thread):
    """
    Microphone Demo for Rhino Speech-to-Intent engine. It creates an input audio stream from a microphone, monitors
    it, and extracts the intent from the speech command. It optionally saves the recorded audio into a file for further
    debugging.
    """

    def __init__(self, access_key, library_path, model_path, context_path, require_endpoint, audio_device_index=None,
                 output_path=None):
        """
        Constructor.
        :param access_key: AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
        :param library_path: Absolute path to Rhino's dynamic library.
        :param model_path: Absolute path to file containing model parameters.
        :param context_path: Absolute path to file containing context model (file with `.rhn` extension). A context
        represents the set of expressions (spoken commands), intents, and intent arguments (slots) within a domain of
        interest.
        :param require_endpoint If set to `False`, Rhino does not require an endpoint (chunk of silence) before
        finishing inference.
        :param audio_device_index: Optional argument. If provided, audio is recorded from this input device. Otherwise,
        the default audio input device is used.
        :param output_path: If provided recorded audio will be stored in this location at the end of the run.
        """

        super(RhinoDemo, self).__init__()

        self._access_key = access_key
        self._library_path = library_path
        self._model_path = model_path
        self._context_path = context_path
        self._require_endpoint = require_endpoint
        self._audio_device_index = audio_device_index

        self._output_path = output_path

    def connectToServer(self):
        #  Socket to talk to server
        print("Connecting to hello world server...")
        socket = context.socket(zmq.REP)
        socket.connect("tcp://localhost:50003")
        return socket

    def sendMsg(self, socket, command="wake"):
        print(f"Sending request: {command} ...")
        socket.send_string(command)
        #  Get the reply.

    def recvMsg(self, socket):
        print(f"waiting for reply ")
        message = socket.recv()
        message.decode("utf-8")
        print(f"Received reply {message}")

    def run(self):
        """
         Creates an input audio stream, instantiates an instance of Rhino object, and infers the intent from spoken
         commands.
         """

        rhino = None
        recorder = None
        wav_file = None

        try:

            socket = self.connectToServer()
            rhino = pvrhino.create(
                access_key=self._access_key,
                library_path=self._library_path,
                model_path=self._model_path,
                context_path=self._context_path,
                require_endpoint=self._require_endpoint)

            recorder = PvRecorder(device_index=self._audio_device_index, frame_length=rhino.frame_length)
            

            if self._output_path is not None:
                wav_file = wave.open(self._output_path, "w")
                wav_file.setparams((1, 2, 16000, 512, "NONE", "NONE"))

            print(rhino.context_info)
            print()

            print(f"Using device: {recorder.selected_device}")
            print("Listening...")
            print()

            while True:
                text = self.recvMsg(socket)
                recorder.start()
                listening = True
                while listening:
                    pcm = recorder.read()
                    if wav_file is not None:
                        wav_file.writeframes(struct.pack("h" * len(pcm), *pcm))

                    is_finalized = rhino.process(pcm)
                    if is_finalized:
                        inference = rhino.get_inference()
                        if inference.is_understood:
                            print('{', flush=True)
                            print("  intent : '%s'" % inference.intent, flush=True)
                            print('  slots : {', flush=True)
                            for slot, value in inference.slots.items():
                                print("    %s : '%s'" % (slot, value), flush=True)
                            print('  }', flush=True)
                            print('}\n', flush=True)
                        else:
                            print("Didn't understand the command.\n")
                            
                        self.sendMsg(socket, str(inference))
                        recorder.stop();
                        listening = False
                
        except pvrhino.RhinoInvalidArgumentError as e:
            print("One or more arguments provided to Rhino is invalid: {\n" +
                  f"\t{self._access_key=}\n" +
                  f"\t{self._library_path=}\n" +
                  f"\t{self._model_path=}\n" +
                  f"\t{self._context_path=}\n" +
                  f"\t{self._require_endpoint=}\n" +
                  "}")
            print(f"If all other arguments seem valid, ensure that '{self._access_key}' is a valid AccessKey")
            raise e
        except pvrhino.RhinoActivationError as e:
            print("AccessKey activation error")
            raise e
        except pvrhino.RhinoActivationLimitError as e:
            print(f"AccessKey '{self._access_key}' has reached it's temporary device limit")
            raise e
        except pvrhino.RhinoActivationRefusedError as e:
            print(f"AccessKey '{self._access_key}' refused")
            raise e
        except pvrhino.RhinoActivationThrottledError as e:
            print(f"AccessKey '{self._access_key}' has been throttled")
            raise e
        except pvrhino.RhinoError as e:
            print(f"Failed to initialize Rhino")
            raise e
        except KeyboardInterrupt:
            print('Stopping ...')

        finally:
            if recorder is not None:
                recorder.delete()

            if rhino is not None:
                rhino.delete()

            if wav_file is not None:
                wav_file.close()

    @classmethod
    def show_audio_devices(cls):
        devices = PvRecorder.get_audio_devices()

        for i in range(len(devices)):
            print(f'index: {i}, device name: {devices[i]}')


class Config:
    def __init__(self, access_key, library_path, model_path, context_path, require_endpoint, audio_device_index, output_path):
        self.access_key = access_key
        self.library_path = library_path
        self.model_path = model_path
        self.context_path = context_path
        self.require_endpoint = require_endpoint
        self.audio_device_index = audio_device_index
        self.output_path = output_path

def main():
   
    config = Config(
       "Xp0vYgF1t2p3Q8p+2JrkVKg/eJ/5tT8PNrra2y20NLXHeNpqRzWNEg==",
        None,
        r"C:\Users\world\source\repos\SELENA\SELENA\Python\Resources\rhino_params_es.pv",
        r"C:\Users\world\source\repos\SELENA\SELENA\Python\Resources\SELENA_es_windows_v2_1_0.rhn",
        None,
        0,
        None
        )
    RhinoDemo(
        access_key=config.access_key,
        library_path=config.library_path,
        model_path=config.model_path,
        context_path=config.context_path,
        require_endpoint=config.require_endpoint,
        audio_device_index=config.audio_device_index,
        output_path=config.output_path).run()


if __name__ == '__main__':
    main()