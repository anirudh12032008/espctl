import json

def parseSch(line):
    if not line.startswith("$S"):
        return None
    return json.loads(line[2:])

def parseData(line):
    if not line.startswith("$D"):
        return None
    body = line[2:]
    c = body.rfind(",")
    if c == -1:
        return None
    wid = body[:c]
    value = float(body[c + 1:])
    return wid, value


def parsePin(line):
    if not line.startswith("$R"):
        return None
    body = line[2:]
    c = body.rfind(",")
    if c == -1:
        return None
    pin = int(body[:c])
    state = int(body[c+1:])
    return pin, state


def ispass(line):
    return not line.startswith("$")

def formatSet(id, val):
    return "!{},{}".format(id, val)
def formatPinW(pin,state):
    return ">{},{}".format(pin, 1 if state else 0)
def formatPinR(pin):
    return "<{}".format(pin)

REQ_SCH = "?"