class PS_Error(Exception):
    pass

class SendingFromTypeSUB(PS_Error):
    pass

class RecevingFromTypePUB(PS_Error):
    pass

class AsyncFromSyncType(PS_Error):
    pass

class MissingCmdLnArgs(PS_Error):
    pass

class OptionNotFound(PS_Error):
    pass

class BadConfig(PS_Error):
    pass