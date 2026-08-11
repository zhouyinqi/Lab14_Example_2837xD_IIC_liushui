#!/usr/bin/env python3
"""Non-invasive smoke test for the DSP TCP host protocol.

Only PING and GET_STATUS are sent. The script does not start a test or
control any output channel.
"""

import argparse
import socket
import struct
import sys


PROTOCOL_VERSION = 1
REQUEST_SIZE = 12
RESPONSE_SIZE = 40
OPCODE_PING = 0x01
OPCODE_GET_STATUS = 0x20


def crc16_ccitt(data):
    crc = 0xFFFF
    for value in data:
        crc ^= value << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def build_request(opcode, sequence):
    frame = struct.pack(
        ">2sBBHBBH",
        b"BT",
        PROTOCOL_VERSION,
        opcode,
        sequence,
        0,
        0,
        0,
    )
    assert len(frame) == REQUEST_SIZE - 2
    return frame + struct.pack(">H", crc16_ccitt(frame))


def recv_exact(sock, length):
    data = bytearray()
    while len(data) < length:
        block = sock.recv(length - len(data))
        if not block:
            raise ConnectionError("DSP closed the TCP connection")
        data.extend(block)
    return bytes(data)


def parse_response(frame):
    if len(frame) != RESPONSE_SIZE:
        raise ValueError("unexpected response length: {}".format(len(frame)))

    expected_crc = crc16_ccitt(frame[:-2])
    actual_crc = struct.unpack(">H", frame[-2:])[0]
    if expected_crc != actual_crc:
        raise ValueError(
            "response CRC mismatch: expected 0x{:04X}, received 0x{:04X}".format(
                expected_crc, actual_crc
            )
        )

    values = struct.unpack(">2sBB" + "H" * 9 + "IfffH", frame)
    if values[0] != b"BR":
        raise ValueError("unexpected response magic: {!r}".format(values[0]))
    if values[1] != PROTOCOL_VERSION:
        raise ValueError("unsupported response version: {}".format(values[1]))

    return {
        "opcode": values[2],
        "sequence": values[3],
        "protocolStatus": values[4],
        "lastCommandResult": values[5],
        "managerMode": values[6],
        "activeStage": values[7],
        "activeTestId": values[8],
        "recordId": values[9],
        "recordResult": values[10],
        "recordError": values[11],
        "rawValue": values[12],
        "measuredValue": values[13],
        "expectedMin": values[14],
        "expectedMax": values[15],
    }


def print_response(name, response):
    print(name)
    for key in (
        "protocolStatus",
        "lastCommandResult",
        "managerMode",
        "activeStage",
        "activeTestId",
        "recordId",
        "recordResult",
        "recordError",
        "rawValue",
        "measuredValue",
    ):
        print("  {:<18} {}".format(key + ":", response[key]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default="192.168.1.111", help="DSP IP address")
    parser.add_argument("--port", default=12001, type=int, help="DSP TCP port")
    parser.add_argument("--timeout", default=5.0, type=float, help="socket timeout in seconds")
    args = parser.parse_args()

    with socket.create_connection((args.host, args.port), args.timeout) as sock:
        sock.settimeout(args.timeout)
        for sequence, (name, opcode) in enumerate(
            (("PING response", OPCODE_PING), ("GET_STATUS response", OPCODE_GET_STATUS)),
            start=1,
        ):
            sock.sendall(build_request(opcode, sequence))
            response = parse_response(recv_exact(sock, RESPONSE_SIZE))
            print_response(name, response)
            if response["protocolStatus"] != 0:
                return 2

    return 0


if __name__ == "__main__":
    sys.exit(main())
