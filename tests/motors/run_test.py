#!/usr/bin/env python

import argparse
import asyncio
import csv
import datetime
import matplotlib
import matplotlib.pyplot
import numpy
import os
import pathlib
import shutil

from pybricksdev.connections import PybricksHub
from pybricksdev.ble import find_device


async def run_pybricks_script(script_name):
    """Runs a script on the hub and awaits result."""

    # Connect to the hub.
    print("Searching for a hub.")
    hub = PybricksHub()
    address = await find_device("Pybricks Hub")
    await hub.connect(address)
    print("Connected!")

    # Run the script and disconnect.
    await hub.run(script_name)
    await hub.disconnect()

    return hub.output


def get_data(path):
    """Gets data columns from a comma separated file."""
    with open(path) as f:
        reader = csv.reader(f, delimiter=",")
        data = numpy.array([[int(x) for x in rec] for rec in reader])
    time = data[:, 0]
    return time, data


def gradient(data, time, smooth=8):
    """Computes a simple gradient from sampled data."""
    speed = []
    for i, t in enumerate(time):
        start = max(i - smooth, 0)
        end = min(i + smooth, len(time) - 1)
        speed.append((data[end] - data[start]) / (time[end] - time[start]))
    return numpy.array(speed)


def plot_servo_data(time, data, build_dir, subtitle=None):
    """Plots data for a servo motor."""
    battery = data[:, 1]
    count = data[:, 2]
    rate = data[:, 3]
    duty = data[:, 5]
    count_est = data[:, 6]
    rate_est = data[:, 7]
    torque_feedback = data[:, 8]
    torque_feedforward = data[:, 9]

    title = "servo" if subtitle is None else "servo_" + subtitle

    figure, axes = matplotlib.pyplot.subplots(nrows=4, ncols=1, figsize=(15, 12))
    figure.suptitle(title, fontsize=20)

    position_axis, speed_axis, torque_axis, duty_axis = axes

    position_axis.plot(time, count, drawstyle="steps-post", label="Reported count")
    position_axis.plot(time, count_est, drawstyle="steps-post", label="Observer")
    position_axis.set_ylabel("angle (deg)")

    speed_axis.plot(time, rate, drawstyle="steps-post", label="Reported rate")
    speed_axis.plot(time, rate_est, drawstyle="steps-post", label="Observer")
    speed_axis.plot(
        time, gradient(count, time / 1000), drawstyle="steps-post", label="Future count derivative"
    )
    speed_axis.set_ylabel("speed (deg/s)")

    torque_axis.plot(time, torque_feedback, label="Feedback", drawstyle="steps-post")
    torque_axis.plot(time, torque_feedforward, label="Feedforward", drawstyle="steps-post")
    torque_axis.set_ylabel("Torque")

    duty_axis.plot(time, duty, label="Duty", drawstyle="steps-post")
    duty_axis.set_ylabel("duty cycle")
    duty_axis.set_ylim([-20000, 20000])
    duty_axis.set_xlabel("time (s)")

    for axis in axes:
        axis.grid(True)
        axis.set_xlim([time[0], time[-1]])
        axis.legend()

    figure.savefig(build_dir / (title + ".png"))


def plot_control_data(time, data, build_dir, subtitle=None):
    """Plots data for the controller."""
    maneuver_time = data[:, 1]
    count = data[:, 2]
    rate = data[:, 3]
    actuation_type = data[:, 4]
    torque_total = data[:, 5]
    count_ref = data[:, 6]
    rate_ref = data[:, 7]
    count_est = data[:, 8]
    rate_est = data[:, 9]
    torque_p = data[:, 10]
    torque_i = data[:, 11]
    torque_d = data[:, 12]

    title = "control" if subtitle is None else "control_" + subtitle

    figure, axes = matplotlib.pyplot.subplots(nrows=4, ncols=1, figsize=(15, 12))
    figure.suptitle(title, fontsize=20)

    position_axis, error_axis, speed_axis, torque_axis = axes

    position_axis.plot(time, count, drawstyle="steps-post", label="Reported count")
    position_axis.plot(time, count_est, drawstyle="steps-post", label="Observer")
    position_axis.plot(time, count_ref, drawstyle="steps-post", label="Reference")
    position_axis.set_ylabel("angle (deg)")

    error_axis.plot(time, count_ref - count, drawstyle="steps-post", label="Reported error")
    error_axis.plot(time, count_ref - count_est, drawstyle="steps-post", label="Estimated error")
    error_axis.plot(time, count_est - count, drawstyle="steps-post", label="Estimation error")
    error_axis.set_ylabel("angle error (deg)")

    speed_axis.plot(time, rate, drawstyle="steps-post", label="Reported rate")
    speed_axis.plot(time, rate_est, drawstyle="steps-post", label="Observer")
    speed_axis.plot(
        time, gradient(count, time / 1000), drawstyle="steps-post", label="Future count derivative"
    )
    speed_axis.plot(time, rate_ref, drawstyle="steps-post", label="Reference")
    speed_axis.set_ylabel("speed (deg/s)")

    torque_axis.plot(time, torque_p, label="P", drawstyle="steps-post")
    torque_axis.plot(time, torque_i, label="I", drawstyle="steps-post")
    torque_axis.plot(time, torque_d, label="D", drawstyle="steps-post")
    torque_axis.plot(time, torque_total, label="Total", drawstyle="steps-post")
    torque_axis.set_ylabel("torque")
    torque_axis.set_xlabel("time (s)")

    for axis in axes:
        axis.grid(True)
        axis.set_xlim([time[0], time[-1]])
        axis.legend()

    figure.savefig(build_dir / (title + ".png"))


# Parse user argument.
parser = argparse.ArgumentParser(description="Run motor script and show log.")
parser.add_argument("file", help="Script to run")
parser.add_argument("--show", dest="show", default=False, action="store_true")
args = parser.parse_args()

# Local paths and data directories.
time_string = datetime.datetime.now().strftime("-%Y-%m-%d-%H%M-%S")
script_base_name, _ = os.path.splitext(os.path.split(args.file)[-1])
build_dir = pathlib.Path(__file__).parent / "build" / (script_base_name + time_string)
pathlib.Path(build_dir).mkdir(parents=True, exist_ok=True)

# Copy script to data directory to archive experiment.
script_archive = build_dir / (script_base_name + ".py")
shutil.copyfile(args.file, script_archive)

# Configure matplotlib.
matplotlib.use("TkAgg")
matplotlib.interactive(True)

# Run the script.
hub_output = asyncio.run(run_pybricks_script(script_archive))

# Save its standard output.
with open(build_dir / "hub_output.txt", "wb") as f:
    for line in hub_output:
        f.write(line + b"\n")

# Plot data.
try:
    # Single motor case
    servo_time, servo_data = get_data(build_dir / "servo.txt")
    plot_servo_data(servo_time, servo_data, build_dir)
    try:
        # Controlled case.
        control_time, control_data = get_data(build_dir / "control.txt")
        plot_control_data(control_time, control_data, build_dir)
    except IndexError:
        # Open loop case, no control data to parse.
        pass

except FileNotFoundError:
    # Drive base case
    servo_time, servo_data = get_data(build_dir / "servo_left.txt")
    plot_servo_data(servo_time, servo_data, build_dir, "left")

    servo_time, servo_data = get_data(build_dir / "servo_right.txt")
    plot_servo_data(servo_time, servo_data, build_dir, "right")

    control_time, control_data = get_data(build_dir / "control_distance.txt")
    plot_control_data(control_time, control_data, build_dir, "distance")

    control_time, control_data = get_data(build_dir / "control_heading.txt")
    plot_control_data(control_time, control_data, build_dir, "heading")

# If requested, show blocking windows with plots.
if args.show:
    matplotlib.pyplot.show(block=True)