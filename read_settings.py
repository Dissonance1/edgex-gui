#!/usr/bin/env python3
"""Read QSettings file and print saved profiles."""
import configparser, json, sys, os, socket, time, threading, subprocess

SETTINGS_FILE = "/home/aetina/.config/EdgeX-UI/EdgeX Qt Console.conf"

print(f"Reading: {SETTINGS_FILE}")
cp = configparser.RawConfigParser()
cp.read(SETTINGS_FILE, encoding="utf-8")
print(f"Sections: {cp.sections()}")
for s in cp.sections():
    print(f"\n[{s}]")
    for k, v in cp[s].items():
        print(f"  {k} = {v}")
