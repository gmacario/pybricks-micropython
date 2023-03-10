# Copyright (c) 2021 The Pybricks Authors

"""
Hardware Module: Technic Hub (ble2_technic.py) and Prime Hub (ble2_prime.py)

Description: Tests broadcast receive and transmit.
"""

from pybricks.tools import wait
from pybricks.experimental import Broadcast


# Initialize broadcast with one topic.
radio = Broadcast(["data"])

for i in range(100):

    # Get the data topic.
    data = radio.receive("data")
    print(data)

    wait(1000)
